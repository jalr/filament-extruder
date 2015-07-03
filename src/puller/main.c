// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "can.h"
#include "uart.h"

#define UART_BAUD_RATE 115200

#define F_CPU 8000000UL

#define PIN(x) (*(&x - 2)) // Address Of Data Direction Register Of Port x 
#define DDR(x) (*(&x - 1)) // Address Of Input Register Of Port x

/*
 *
 * PIN MAPPING:
 *  2	INT0		MCP2515 INT
 *  5	PD5			Display STROBE
 *  6	PD6			Display DATA
 *  7	PD7			Display CLOCK
 *  9	PB1 (OC1A)  A4988 STEP
 * 10	SS			MCP2515 CS
 * 11	MOSI		MCP2515 SI
 * 12	MISO		MCP2515 SO
 * 13	SCK			MCP2515 SCK
 * A0	PC0			Encoder Button
 * A1	PC1			Encoder PH A
 * A2	PC2			Encoder PH B
 * A3   PC3         A4988 ENABLE
 * 
 */

#define DPY_PORT          PORTD
#define DPY_STROBE        PD5
#define DPY_DATA          PD6
#define DPY_CLOCK         PD7

// If you change STEPPER_STEP, you have to change Timer/Counter code as well
#define STEPPER_STEP_PORT PORTB
#define STEPPER_STEP      PB1 

#define STEPPER_EN_PORT    PORTC
#define STEPPER_EN         PC3

#define ENC_PORT          PORTC
#define ENC_BUTTON        PC0
#define ENC_PHASEA        PC1
#define ENC_PHASEB        PC2

#define SEG_A             0x80
#define SEG_B             0x40
#define SEG_C             0x20
#define SEG_D             0x10
#define SEG_E             0x08
#define SEG_F             0x04
#define SEG_G             0x02
#define SEG_DP            0x01

static const uint8_t PROGMEM sevensegment_table[] = {
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, // 0
	SEG_B | SEG_C, // 1
	SEG_A | SEG_B | SEG_D | SEG_E | SEG_G, // 2
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_G, // 3
	SEG_B | SEG_C | SEG_F | SEG_G, // 4
	SEG_A | SEG_C | SEG_D | SEG_F | SEG_G, // 5
	SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 6
	SEG_A | SEG_B | SEG_C | SEG_F, // 7
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, // 8
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G // 9
};

//pgm_read_byte(&sevensegment_table[7])



// -----------------------------------------------------------------------------
/** Set filters and masks.
 *
 * The filters are divided in two groups:
 *
 * Group 0: Filter 0 and 1 with corresponding mask 0.
 * Group 1: Filter 2, 3, 4 and 5 with corresponding mask 1.
 *
 * If a group mask is set to 0, the group will receive all messages.
 *
 * If you want to receive ONLY 11 bit identifiers, set your filters
 * and masks as follows:
 *
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER(0),				// Filter 0
 *		MCP2515_FILTER(0),				// Filter 1
 *		
 *		// Group 1
 *		MCP2515_FILTER(0),				// Filter 2
 *		MCP2515_FILTER(0),				// Filter 3
 *		MCP2515_FILTER(0),				// Filter 4
 *		MCP2515_FILTER(0),				// Filter 5
 *		
 *		MCP2515_FILTER(0),				// Mask 0 (for group 0)
 *		MCP2515_FILTER(0),				// Mask 1 (for group 1)
 *	};
 *
 *
 * If you want to receive ONLY 29 bit identifiers, set your filters
 * and masks as follows:
 *
 * \code
 *	uint8_t can_filter[] PROGMEM = {
 *		// Group 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 0
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 1
 *		
 *		// Group 1
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 2
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 3
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 4
 *		MCP2515_FILTER_EXTENDED(0),		// Filter 5
 *		
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 0 (for group 0)
 *		MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
 *	};
 * \endcode
 *
 * If you want to receive both 11 and 29 bit identifiers, set your filters
 * and masks as follows:
 */
const uint8_t can_filter[] PROGMEM = 
{
	// Group 0
	MCP2515_FILTER(0),				// Filter 0
	MCP2515_FILTER(0),				// Filter 1
	
	// Group 1
	MCP2515_FILTER_EXTENDED(0),		// Filter 2
	MCP2515_FILTER_EXTENDED(0),		// Filter 3
	MCP2515_FILTER_EXTENDED(0),		// Filter 4
	MCP2515_FILTER_EXTENDED(0),		// Filter 5
	
	MCP2515_FILTER(0),				// Mask 0 (for group 0)
	MCP2515_FILTER_EXTENDED(0),		// Mask 1 (for group 1)
};
// You can receive 11 bit identifiers with either group 0 or 1.


// -----------------------------------------------------------------------------
// Main loop for receiving and sending messages.

int main(void)
{
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU) );
	sei();
	uart_puts_P("CAN init");

	// Initialize MCP2515
	can_init(BITRATE_125_KBPS);
	
	uart_puts_P("load filters");
	// Load filters and masks
	can_static_filter(can_filter);
	uart_puts_P("filters loaded");
	
	// Create a test messsage
	can_t msg;
	
	msg.id = 0x123456;
	msg.flags.rtr = 0;
	msg.flags.extended = 1;
	
	msg.length = 4;
	msg.data[0] = 0xde;
	msg.data[1] = 0xad;
	msg.data[2] = 0xbe;
	msg.data[3] = 0xef;
	
	// Send the message
	uart_puts_P("sending message");
	can_send_message(&msg);
	uart_puts_P("message sent");

	STEPPER_EN_PORT |= (1<<STEPPER_EN); // Disable Stepper Driver

	DDR(STEPPER_STEP_PORT) |= (1<<STEPPER_STEP);
	DDR(STEPPER_EN_PORT) |= (1<<STEPPER_EN);
	DDR(DPY_PORT) |= (1<<DPY_STROBE) | (1<<DPY_DATA) | (1<<DPY_CLOCK);

	/*
	TCCR1A = (1<<COM1A0); //Toggle OC1A on Compare Match.
	TCCR1A |= (1<<WGM12); // CTC Mode
	*/
	//TCCR1A = (1<<COM1A1) | (1<<COM1A0); // Set OC1A on Compare Match, clear OC1A at BOTTOM (inverting mode)
	TCCR1A = (1<<COM1A0); // Toggle OC1A on Compare Match

	TCCR1A |= (1<<WGM11) | (1<<WGM10); // Fast PWM, TOP=OCR1A
	TCCR1B |= (1<<WGM13) | (1<<WGM12);


	TCCR1B |= (1<<CS10); // Prescaler 1
	//TCCR1B |= (1<<CS11); // Prescaler 8
	//TCCR1B |= (1<<CS11) | (1<<CS10); // Prescaler 64
	//TCCR1B |= (1<<CS12); // Prescaler 256
	//TCCR1B |= (1<<CS12) | (1<<CS10); // Prescaler 1024

	OCR1A = 32768;

	for(;;) {
		// Check if a new messag was received
		if (can_check_message())
		{
			uart_puts_P("message received");
			can_t msg;
			
			// Try to read the message
			uart_puts_P("reading message");
			if (can_get_message(&msg))
			{
				if (msg.id == 42) {
					OCR1A = msg.data[0]<<8;
					OCR1A += msg.data[1];
				}
				else if (msg.id == 43) {
					if (msg.data[0] == 1) {
						STEPPER_EN_PORT &= ~(1<<STEPPER_EN);
					} else if (msg.data[0] == 0) {
						STEPPER_EN_PORT |= (1<<STEPPER_EN);
					}
				}
				else if (msg.id == 44) {
					for (uint8_t by=0; by<4; by++) {
						uint8_t digit = pgm_read_byte(&sevensegment_table[msg.data[by]]);
						for (uint8_t bi=0; bi<8; bi++) {
							if (digit & (1<<bi)) {
								DPY_PORT |= (1<<DPY_DATA);	
							} else {
								DPY_PORT &= ~(1<<DPY_DATA);
							}
							_delay_us(2);
							DPY_PORT |= (1<<DPY_CLOCK);
							_delay_us(2);
							DPY_PORT &= ~(1<<DPY_CLOCK);
						}
					}
					DPY_PORT |= (1<<DPY_STROBE);
					_delay_us(2);
					DPY_PORT &= ~(1<<DPY_STROBE);
					_delay_us(2);
				}
				// If we received a message resend it with a different id
				msg.id += 10;
				
				// Send the new message
				uart_puts_P("re-sendig message");
				can_send_message(&msg);
				uart_puts_P("message re-sent");
			}
		}
	}
}
