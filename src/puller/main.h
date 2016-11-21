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
#define ENC_BUTTON_DELAY  10

#define SEG_A             0x80
#define SEG_B             0x40
#define SEG_C             0x20
#define SEG_D             0x10
#define SEG_E             0x08
#define SEG_F             0x04
#define SEG_G             0x02

#define NUMBER_OF_DIGITS 4
#define DIGIT_ORDER 0
