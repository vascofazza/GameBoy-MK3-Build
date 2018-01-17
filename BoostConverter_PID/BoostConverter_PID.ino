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
#include <PID_v1.h>
#define F_CPU 8000000
#define PWM 1
#define feedbackinput A3
int pinb = 0;

#define FIRST 2
#define SECOND 4
#define THIRD 0

double Setpoint, Input, Output;
double Kp=.001, Ki=.07, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

void setup() {
  
  //no downscaling on timer -> fast PWM
  TCCR0A = 2 << COM0A0 | 2 << COM0B0 | 3 << WGM00;
  TCCR0B = 0 << WGM02 | 1 << CS00;
  TCCR1 = 0 << PWM1A | 0 << COM1A0 | 1 << CS10;
  GTCCR = 1 << PWM1B | 2 << COM1B0;
  pinMode(PWM, OUTPUT);
  pinMode(feedbackinput, INPUT);
  digitalWrite(PWM, LOW);
  pinMode(FIRST, INPUT_PULLUP);
  pinMode(SECOND, INPUT_PULLUP);
  pinMode(THIRD, INPUT_PULLUP);
  analogReference(INTERNAL2V56);

  Input = analogRead(feedbackinput);
  Setpoint = 600;

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0,180);
}

void loop() {
  if (PINB != pinb)
  {
    pinb = PINB;
    int dim = 7 - (((PINB & 1) << 2) | ((PINB & 16) >> 3) | ((PINB & 4) >> 2));
    switch (dim) {
      case 0: Setpoint = 675; break; //18.4
      case 1: Setpoint = 685; break; //18.4
      case 2: Setpoint = 695; break; //
      case 3: Setpoint = 700; break; //
      case 4: Setpoint = 715; break; //19.5
      case 5: Setpoint = 725; break; //
      case 6: Setpoint = 740; break; //20.2
      case 7: Setpoint = 0; break; //
    }
  }
  Input = analogRead(feedbackinput);
  myPID.Compute();
  analogWrite(PWM, Output);
}
