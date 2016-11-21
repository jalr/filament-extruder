// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "can.h"
#include "uart.h"

#include "encoder.h"

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
const uint8_t can_filter[] PROGMEM = {
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

const uint16_t const exp10_table [] PROGMEM = {
	1ul, // 10^0
	10ul, // 10^1
	100ul, // 10^2
	1000ul, // 10^3
	10000ul, // 10^4
};


void displayNumber(uint16_t number) {
	// calculate digits
	uint16_t multiplier;
	uint8_t digit[NUMBER_OF_DIGITS];
	for (uint8_t digitCounter=NUMBER_OF_DIGITS; digitCounter>0; digitCounter--) {
		switch (digitCounter-1) {
			case 3:
				multiplier = 1000;
				break;
			case 2:
				multiplier = 100;
				break;
			case 1:
				multiplier = 10;
				break;
			case 0:
				multiplier = 1;
				break;
		}
		digit[digitCounter-1] = 0;
		while (number >= multiplier) {
			digit[digitCounter-1]++;
			number -= multiplier;
		}
	}
	
	// send digits to display
	#if (DIGIT_ORDER == 1)
		for (int8_t segmentPos=0; segmentPos<NUMBER_OF_DIGITS; segmentPos++) {
	#else
		for (int8_t segmentPos=NUMBER_OF_DIGITS-1; segmentPos>=0; segmentPos--) {
	#endif
		uint8_t segments = pgm_read_byte(&sevensegment_table[digit[segmentPos]]);
		for (uint8_t segmentBit=0; segmentBit<8; segmentBit++) {
			if (segments & (1<<segmentBit)) {
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

// -----------------------------------------------------------------------------
// Main loop for receiving and sending messages.

int main(void)
{
	uint16_t no;
	int8_t enc;
	char buf[10];
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU) );

	encoder_init();

	STEPPER_EN_PORT |= (1<<STEPPER_EN); // Disable Stepper Driver

	DDR(STEPPER_STEP_PORT) |= (1<<STEPPER_STEP);
	DDR(STEPPER_EN_PORT) |= (1<<STEPPER_EN);
	DDR(DPY_PORT) |= (1<<DPY_STROBE) | (1<<DPY_DATA) | (1<<DPY_CLOCK);

	TCCR1A = (1<<COM1A0); // Toggle OC1A on Compare Match

	TCCR1A |= (1<<WGM11) | (1<<WGM10); // Fast PWM, TOP=OCR1A
	TCCR1B |= (1<<WGM13) | (1<<WGM12);


	TCCR1B |= (1<<CS10); // Prescaler 1
	//TCCR1B |= (1<<CS11); // Prescaler 8
	//TCCR1B |= (1<<CS11) | (1<<CS10); // Prescaler 64
	//TCCR1B |= (1<<CS12); // Prescaler 256
	//TCCR1B |= (1<<CS12) | (1<<CS10); // Prescaler 1024

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

	OCR1A = 32768;

	no = 2000;

	for(;;) {
		enc = encoder_read2();
		no += enc;
		OCR1A = no;
		displayNumber(no);

		if (encoder_read_button()) {
			uart_puts_P("button pressed\r\n");
			STEPPER_EN_PORT ^= (1<<STEPPER_EN);
		}

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
					no = msg.data[0]<<8;
					no += msg.data[1];
					displayNumber(no);
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
