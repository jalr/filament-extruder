/*
 
 HAMAMATSU S9226 CMOS linear image sensor

 |S9226 Pin | Arduino | Port
 |1 - GND   | GND     |
 |2 - CLK   | 5       | PD5 / OC0B
 |3 - Trig  | 2       | PD2 / INT0 
 |4 - ST    | 6       | PD6 / OC0A
 |5 - Vg    | VCC     |
 |6 - Video | A3      | PC3 / ADC3
 |7 - EOS   | 3       | PD3 / INT1
 |8 - Vdd   | VCC     |

*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include <uart.h>

#define F_CPU 8000000UL

#define UART_BAUD_RATE 115200 

#define TIMER0_MAX 160
//#define TIMER0_MAX 256

volatile uint16_t cell;

ISR(TIMER0_OVF_vect) {
	TCNT0 = 256-TIMER0_MAX;
	static uint16_t clk_cnt;
	if (clk_cnt==0) {
		cli();
		cell = 0;
		sei();
		PORTD &= ~(1<<PD6); // ST
	} else {
		PORTD |= (1<<PD6); // ST
	}
	clk_cnt++;
	if (clk_cnt > 1024*4 + 16) clk_cnt = 0;
}

ISR(INT0_vect) {
	cli();
	cell++;
	sei();
}

ISR(ADC_vect) {
	uint16_t video;
	char mystr[16];
	uint16_t cell_cpy;
	cli();
	cell_cpy = cell;
	sei();

	//video = (ADCH<<8) + ADCL;
	video = ADCL;
	video += (ADCH<<8);

	/*
	sprintf(mystr, "%d:", cell_cpy);
	uart_puts(mystr);

	sprintf(mystr, "%d\n", video);
	uart_puts(mystr);
	*/
	if (cell_cpy == 1) {
		uart_puts("\n\n");
	}
	if (video<255) {
		uart_puts("X");
	} else {
		uart_puts("_");
	}
}

int main(void) {
    unsigned int c;
	//uart_init(UART_BAUD_SELECT_DOUBLE_SPEED(115200,8000000L));
    uart_init( UART_BAUD_SELECT_DOUBLE_SPEED(UART_BAUD_RATE,F_CPU) ); 

	DDRB = (1<<PB5);
	PORTB |= (1<<PB5);
	_delay_ms(1000);
	
	DDRD = (1<<PD6) | (1<<PD5);

	//TCCR0B = (1<<CS02) | (1<<CS00);  // clk I/O /1024 (From prescaler)
	//TCCR0B = (1<<CS02);              // clk I/O /256 (From prescaler)
	////TCCR0B = (1<<CS01) | (1<<CS00);  // clk I/O /64 (From prescaler)
	//TCCR0B = (1<<CS01);              // clk I/O /8 (From prescaler)
	TCCR0B = (1<<CS00);              // clk I/O /(No prescaling)

	TCCR0A = 0;
	TCCR0A |= (1<<COM0B1); //Clear OC0B on Compare Match, set OC0B at BOTTOM
	TCCR0A |= (1<<WGM02) | (1<<WGM01) | (1<<WGM00); // Fast PWM 
	OCR0B = 256 - TIMER0_MAX + TIMER0_MAX/2;
	TIMSK0 = (1<<TOIE0);

	EICRA = (1<<ISC01); //The falling edge of INT0 generates an interrupt request.
	EIMSK = (1<<INT0);

	ADMUX = (1<<REFS0); // AREF = AVCC with external capacitor at AREF pin
	ADMUX |= (1<<MUX1) | (1<<MUX0); // ADC3
	//ADMUX |= (1<<ADLAR);
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADIE); // enable interrupt
	ADCSRA |= (1<<ADATE); // enable auto trigger

	ADCSRB |= (1<<ADTS1);  // External Interrupt Request 0
	
	sei();

	PORTB &= ~(1<<PB5);

	char text[] = "init done\n";
	uart_puts(text);

	for(;;) {
		/*
         * Get received character from ringbuffer
         * uart_getc() returns in the lower byte the received character and 
         * in the higher byte (bitmask) the last receive error
         * UART_NO_DATA is returned when no data is available.
         *
         */
        c = uart_getc();
        if ( c & UART_NO_DATA )
        {
            /* 
             * no data available from UART 
             */
        }
        else
        {
            /*
             * new data available from UART
             * check for Frame or Overrun error
             */
            if ( c & UART_FRAME_ERROR )
            {
                /* Framing Error detected, i.e no stop bit detected */
                uart_puts_P("UART Frame Error: ");
            }
            if ( c & UART_OVERRUN_ERROR )
            {
                /* 
                 * Overrun, a character already present in the UART UDR register was 
                 * not read by the interrupt handler before the next character arrived,
                 * one or more received characters have been dropped
                 */
                uart_puts_P("UART Overrun Error: ");
            }
            if ( c & UART_BUFFER_OVERFLOW )
            {
                /* 
                 * We are not reading the receive buffer fast enough,
                 * one or more received character have been dropped 
                 */
                uart_puts_P("Buffer overflow error: ");
            }
            /* 
             * send received character back
             */
            uart_putc( (unsigned char)c );
		}
	}
}

