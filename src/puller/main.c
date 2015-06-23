// coding: utf-8

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/signal.h>

#include "can.h"
#include "uart.h"

#define UART_BAUD_RATE 115200

#define F_CPU 8000000UL

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
	
	while (1)
	{
		// Check if a new messag was received
		if (can_check_message())
		{
			uart_puts_P("message received");
			can_t msg;
			
			// Try to read the message
			uart_puts_P("reading message");
			if (can_get_message(&msg))
			{
				// If we received a message resend it with a different id
				msg.id += 10;
				
				// Send the new message
				uart_puts_P("re-sendig message");
				can_send_message(&msg);
				uart_puts_P("message re-sent");
			}
		}
	}
	
	return 0;
}
