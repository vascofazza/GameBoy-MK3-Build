/*
* 
* This sketch handles button keys emulation using direct port manipulation, battery measurement and screen brightness control.
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
#define KEY_LEFT_CTRL  0x80
#define KEY_LEFT_SHIFT 0x81
#define KEY_LEFT_ALT   0x82
#define KEY_LEFT_GUI   0x83
#define KEY_RIGHT_CTRL 0x84
#define KEY_RIGHT_SHIFT    0x85
#define KEY_RIGHT_ALT  0x86
#define KEY_RIGHT_GUI  0x87

#define KEY_UP_ARROW   0xDA
#define KEY_DOWN_ARROW 0xD9
#define KEY_LEFT_ARROW 0xD8
#define KEY_RIGHT_ARROW    0xD7
#define KEY_BACKSPACE  0xB2
#define KEY_TAB        0xB3
#define KEY_RETURN 0xB0
#define KEY_ESC        0xB1
#define KEY_INSERT 0xD1
#define KEY_DELETE 0xD4
#define KEY_PAGE_UP    0xD3
#define KEY_PAGE_DOWN  0xD6
#define KEY_HOME   0xD2
#define KEY_END        0xD5
#define KEY_CAPS_LOCK  0xC1
#define KEY_F1     0xC2
#define KEY_F2     0xC3
#define KEY_F3     0xC4
#define KEY_F4     0xC5
#define KEY_F5     0xC6
#define KEY_F6     0xC7
#define KEY_F7     0xC8
#define KEY_F8     0xC9
#define KEY_F9     0xCA
#define KEY_F10        0xCB
#define KEY_F11        0xCC
#define KEY_F12        0xCD

#include <Keyboard.h>
#include <EEPROM.h>

#define BatteryLevelPin A0
#define BrightnessAddress 0x00
#define OFFSET 20

#define WINDOW 200
int analogread[WINDOW];
long total;
int readIndex = 0;

// buttons A B X Y L R U D L R ST SEL MOD
static char keys[] = {KEY_LEFT_CTRL, KEY_LEFT_ALT, 'z', 'x', KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT, KEY_UP_ARROW, KEY_DOWN_ARROW,
                      KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_RETURN, ' '
                     };
static bool keyPressed[13];
static byte level = 0;
void setup() {
  //init battery readings
  for (int i = 0; i < WINDOW; i++)
    analogread[i] = 0;
  // open the serial port:
  Serial.begin(9600);
  // initialize control over the keyboard:
  Keyboard.begin();
  DDRE &= B10111111; // MOD
  PORTE |= ~B10111111;
  //      TXLD TR
  DDRD &= B01000000; // SEL - TXLED - START - R - L - D - U
  PORTD |= ~B01100000; //pullup
  //    not connected - rx led - SPI
  DDRB &= B10001110; // X - B - A - RXLED
  DDRB |= B00001110;
  PORTB |= ~B10000001; //pullup
  DDRF &= B10001111; // L - R - Y
  PORTF |= ~B10001111; //pullup
  pinMode(BatteryLevelPin, INPUT);
  //init
  for (int i = 0; i < sizeof(keyPressed); i++)
    keyPressed[i] = false;

  level = EEPROM.read(BrightnessAddress);
  if (level < 0 || level > 7) {
    EEPROM.write(BrightnessAddress, 1);
    level = 1;
  }
  delay(1000);
  PORTB = (PORTB & 0xF1) | ((7 - (level & 7)) << 1);
}
void loop() {

  // X - B - A
  auto portB = PINB;
  auto portF = PINF;
  auto portD = PIND;
  auto portE = PINE;

  //battery read, using 1ms from debouncing
  readBattery();

  //debouncing
  _delay_ms(4);

  int counter = 0;

  if (portB == PINB)
  {
    bool A = portB & B00010000;
    if (A != keyPressed[counter])
    {
      keyPressed[counter] = A;
      if (A)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
    counter++;
    bool B = portB & B00100000;
    if (B != keyPressed[counter])
    {
      keyPressed[counter] = B;
      if (B)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
    counter++;
    bool X = portB & B01000000;
    if (X != keyPressed[counter])
    {
      keyPressed[counter] = X;
      if (X)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
  }
  else
    counter += 2;

  if (portF == PINF)
  {
    counter++;
    bool Y = portF & B00010000;
    if (Y != keyPressed[counter])
    {
      keyPressed[counter] = Y;
      if (Y)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
    counter++;
    bool LL = portF & B00100000;
    if (LL != keyPressed[counter])
    {
      keyPressed[counter] = LL;
      if (LL)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
    counter++;
    bool RR = portF & B01000000;
    if (RR != keyPressed[counter])
    {
      keyPressed[counter] = RR;
      if (RR)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
  }
  else
    counter += 3;

  // SEL - START - R - L - D - U
  //01100000
  if (portD != PIND || portE != PINE)
    return;

  bool MOD = !(portE & B01000000);
  if (MOD != keyPressed[12])
  {
    keyPressed[12] = MOD;
    if (MOD)
    {
      Serial.print("mod_on\n");
    }
    else
    {
      Serial.print("mod_off\n");
      updateEEPROM();
    }
  }

  //UP
  counter++;
  bool UP = portD & B00000001;
  if (UP != keyPressed[counter])
  {
    keyPressed[counter] = UP;
    if (MOD && !UP) {
      adjustLevel(true);
    }
    else
    {
      if (UP)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
  }
  counter++;

  bool DOWN = portD & B00000010;
  if (DOWN != keyPressed[counter])
  {
    keyPressed[counter] = DOWN;
    if (MOD && !DOWN) {
      adjustLevel(false);
    }
    else
    {
      if (DOWN)
        Keyboard.release(keys[counter]);
      else
        Keyboard.press(keys[counter]);
    }
  }
  counter++;

  bool LEFT = portD & B00000100;
  if (LEFT != keyPressed[counter])
  {
    keyPressed[counter] = LEFT;
    if (LEFT)
      Keyboard.release(keys[counter]);
    else
      Keyboard.press(keys[counter]);
  }
  counter++;
  bool RIGHT = portD & B00001000;
  if (RIGHT != keyPressed[counter])
  {
    keyPressed[counter] = RIGHT;
    if (RIGHT)
      Keyboard.release(keys[counter]);
    else
      Keyboard.press(keys[counter]);
  }

  counter++;
  bool START = portD & B10000000;
  if (START != keyPressed[counter])
  {
    keyPressed[counter] = START;
    if (MOD && !START) {
      sendCommand(0);
    }
    else if (START)
      Keyboard.release(keys[counter]);
    else
      Keyboard.press(keys[counter]);
  }

  counter++;
  bool SELECT = portD & B00010000;
  if (SELECT != keyPressed[counter])
  {
    keyPressed[counter] = SELECT;
    if (MOD && !SELECT) {
      //sendCommand(1);
      Keyboard.press(KEY_ESC);
      delay(150);
      Keyboard.release(KEY_ESC);
    }
    else if (SELECT)
      Keyboard.release(keys[counter]);
    else
      Keyboard.press(keys[counter]);
  }

  if (Serial.available())
    processSerial();
}

void processSerial()
{
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == 'b')
    {
      int bat = total / WINDOW;
      Serial.print("b");
      Serial.print(bat);
      Serial.print("\n");
    }
    else if (c == 'l')
    {
      Serial.print("l");
      Serial.print(level);
      Serial.print("\n");
    }
    else if (c == 's')
    {
      level = Serial.parseInt();
      PORTB = (PORTB & 0xF1) | ((7 - (level & 7)) << 1);
    }
  }
}

void updateEEPROM()
{
  EEPROM.update(BrightnessAddress, level);
}

void adjustLevel(bool up)
{
  if (up && level < 7)
    level++;
  else if (!up && level > 0)
    level--;
  else
    return;
  PORTB = (PORTB & 0xF1) | ((7 - (level & 7)) << 1);
  Serial.print("l");
  Serial.print(level);
  Serial.print("\n");
}

void sendCommand(int cmd)
{
  Serial.print("c");
  Serial.print(cmd);
  delay(250);
  Serial.print("\n");
}

void readBattery()
{
  total -= analogread[readIndex];
  analogread[readIndex] = analogRead(BatteryLevelPin)+OFFSET;
  total += analogread[readIndex];
  readIndex++;
  if (readIndex >= WINDOW)
    readIndex = 0;
}

