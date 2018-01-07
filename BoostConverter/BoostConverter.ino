/*
* 
* This sketch drives a boost converter circuit using an attiny85 - used to drive the backlight led strip.
* This file originates from vascofazza's GameBoy MK3 raspberry project.
* Author: Federico Scozzafava
* 
* THIS HEADER MUST REMAIN WITH THIS FILE AT ALL TIMES
*
* This firmware is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This firmware is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this repo. If not, see <http://www.gnu.org/licenses/>.
*
*/

#define F_CPU 8000000
int pwm = 1;
int feedbackinput = A3;
int potinputval;
int potinputval1;

int pwmval;
int pinb = 0;
int counter = 0;
int readIndex = 0;
unsigned long total = 0;
unsigned int analogval = 0;

#define FIRST 2
#define SECOND 4
#define THIRD 0
#define WINDOW 8
int feedbackinputval[WINDOW];

void setup() {
  //no downscaling on timer -> fast PWM
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = 0 << WGM02 | 1 << CS00;
  TCCR1 = 0 << PWM1A | 0 << COM1A0 | 1 << CS10;
  GTCCR = 1 << PWM1B | 2 << COM1B0;
  pinMode(pwm, OUTPUT);
  pinMode(feedbackinput, INPUT);
  digitalWrite(pwm, LOW);
  pinMode(FIRST, INPUT_PULLUP);
  pinMode(SECOND, INPUT_PULLUP);
  pinMode(THIRD, INPUT_PULLUP);
  pwmval = 0;
  potinputval = 0;
  for (int i = 0; i < WINDOW; i++)
    feedbackinputval[i] = 0;
}

void loop() {
  if (PINB != pinb)
  {
    pinb = PINB;
    int dim = 7 - (((PINB & 1) << 2) | ((PINB & 16) >> 3) | ((PINB & 4) >> 2));
    switch (dim) {
      case 0: potinputval1 = 515; break; //18.4
      case 1: potinputval1 = 530; break; //18.4
      case 2: potinputval1 = 540; break; //
      case 3: potinputval1 = 553; break; //
      case 4: potinputval1 = 581; break; //19.5
      case 5: potinputval1 = 601; break; //
      case 6: potinputval1 = 621; break; //20.2
      case 7: potinputval1 = 0; break; //
    }
    potinputval1 /= 12;
    potinputval = potinputval1;
  }
//  if (potinputval != potinputval1 && (counter++ % 30 == 0))
//  {
//    if (potinputval < potinputval1)
//      potinputval++;
//    else
//      potinputval--;
//  }
  total -= feedbackinputval[readIndex];
  feedbackinputval[readIndex] = (analogRead(feedbackinput) /12); //baad hack
  total += feedbackinputval[readIndex];
  readIndex++;
  if (readIndex >= WINDOW)
    readIndex = 0;
  analogval = total / WINDOW;
  if (potinputval > analogval) {
    if (pwmval < 230) {
      pwmval++;
      analogWrite(pwm, pwmval);
    }
  }
  if (potinputval < analogval) {
    if (pwmval > 0) {
      pwmval--;
      analogWrite(pwm, pwmval);
    }
  }
}
