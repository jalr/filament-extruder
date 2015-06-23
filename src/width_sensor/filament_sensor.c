/*
  HAMAMATSU S9226 CMOS linear image sensor
 1 - GND   -> GND
 2 - CLK   -> 5
 3 - Trig  -> 2
 4 - ST    -> 6
 5 - Vg    -> VCC
 6 - Video -> A3
 7 - EOS   -> 3
 8 - Vdd   -> VCC
 
 */

// Definition of interrupt names
//#include <avr/io.h>
// ISR interrupt service routine
//#include <avr/interrupt.h>

#include "TimerOne.h"


const int clockPin = 5;
const int startPin = 6;
const int triggerPin = 2;
const int videoPin = 2;

long previousMillis = 0;

long interval = 4; // clock (milliseconds)

volatile int cell;
volatile int clockState;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Timer1.initialize(2000);
  Timer1.attachInterrupt(timer_tick);
  pinMode(clockPin, OUTPUT);
  pinMode(startPin, OUTPUT);
  /*
  GICR |= ( 1 << INT0);
   // Signal change triggers interrupt
   MCUCR |= ( 1 << ISC01); // The falling edge of INT0 generates an interrupt request.
   */
}

/*
ISR(INT0_vect) {
 video = analogRead(A3);
 }
 */

void timer_tick() {
  if (clockState == LOW) {
    if (cell == 0) {
      digitalWrite(startPin, LOW);
    }
    else if (cell == 1) {
      digitalWrite(startPin, HIGH);
    }
    clockState = HIGH;
  } else {
    clockState = LOW;
    cell++;
  }
  digitalWrite(clockPin, clockState);
}

void loop() {
  static int trigger_last;
  int video;
  int cell_cpy;
  
  cli();
  cell_cpy = cell;
  sei();

  if (digitalRead(triggerPin) == LOW) {
    if (trigger_last == HIGH) {
      video = analogRead(videoPin);
      Serial.print(cell_cpy);
      Serial.print(": ");
      Serial.println(video);
    }
    trigger_last = LOW;
  } else {
     trigger_last = HIGH;
  }
  if (cell_cpy > 1024) {
    cli();
    cell = 0;
    clockState = LOW;
    sei();
  }
}
