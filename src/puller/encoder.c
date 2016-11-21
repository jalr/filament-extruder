#include <avr/io.h>
#include <avr/interrupt.h>
#include "main.h"

#include <avr/pgmspace.h>

volatile int8_t enc_delta; // -128 ... 127
volatile uint8_t timer=0;
volatile uint8_t enc_btn=0;

void encoder_init(void) {
	TCCR0A |= (1<<WGM01); // CTC
	TCCR0B |= (1<<CS01) | (1<<CS00); // /64
	TIMSK0 |= (1<<OCIE0A);
	ENC_PORT |= (1<<ENC_BUTTON) | (1<<ENC_PHASEA) | (1<<ENC_PHASEB); // Pullup
	OCR0A = (uint8_t)(F_CPU / 64.0 * 1e-3 - 0.5); // 1ms
}

ISR(TIMER0_COMPA_vect) {
	static int8_t last;
	int8_t new, diff;

	if (timer<255) timer++;

	new = 0;

	if(!(PIN(ENC_PORT) & (1<<ENC_BUTTON))) {
		if (enc_btn == 0) {
			enc_btn = ENC_BUTTON_DELAY;
		}
	} else {
		if (enc_btn > 0) {
			enc_btn--;
		}
		if(PIN(ENC_PORT) & (1<<ENC_PHASEA))
			new = 3;
		if(PIN(ENC_PORT) & (1<<ENC_PHASEB))
			new ^= 1; // convert gray to binary
		diff = last - new; // difference last - new
		if(diff & 1) { // bit 0 = value (1)
			//if (timer>16) timer -= 16; else timer=0;
			last = new; // store new as next last
			enc_delta += (diff & 2) - 1; // bit 1 = direction (+/-)
		}
	}
}


int8_t encoder_read1(void) { // read single step encoders
	int8_t val;
	cli();
	val = enc_delta;
	enc_delta = 0;
	sei();
	return val; // counts since last call
}


int8_t encoder_read2(void) { // read two step encoders
	int8_t val;
	cli();
	val = enc_delta;
	enc_delta &= 1;
	sei();
	return val>>1;
}


int8_t encoder_read4(void) { // read four step encoders
	int8_t val;
	cli();
	val = enc_delta;
	enc_delta &= 3;
	sei();
	return val>>2;
}

uint8_t encoder_read_button(void) {
	if (enc_btn == ENC_BUTTON_DELAY) {
		enc_btn--;
		return 1; // return true
	} else {
		return 0; // return false
	}
}


