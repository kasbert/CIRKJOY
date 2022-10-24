/*
  C64keyboard.cpp - Commodore Keyboard library

  Copyright (c) 2019 Hartland PC LLC
  Written by Robert VanHazinga

  Modified by Jarkko Sonninen 2020,2022

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "C64keyboard.hpp"
#include "c64key.h"

// Map codes to pins
/*
 * https://ist.uwaterloo.ca/~schepers/MJK/keyboard_int.html
  AAABBB
  PORT A = Y in keyboard matrix picture
  PORT B = X in keyboard matrix picture

  X0..X13 and Y0..Y7 refers to MT swich matrix

  PORT A
  PA0 = X6
  PA1 = X4
  PA2 = X3
  PA3 = X2
  PA4 = X1
  PA5 = X0
  PA6 = X7
  PA7 = X5

  PORT B
  PB0 = Y4
  PB1 = Y5
  PB2 = Y6
  PB3 = Y3
  PB4 = Y4
  PB5 = Y1
  PB6 = Y2
  PB7 = Y7

*/
static const uint8_t am[] = {6, 4, 3, 2, 1, 0, 7, 5};
static const uint8_t bm[] = {4, 5, 6, 3, 0, 1, 2, 7};

void C64keyboard::resetSwitch(void) {
  pinMode( nmiPin, INPUT_PULLUP); // C64 NMI
  digitalWrite(ANALOG_SW_RESET, HIGH);
  digitalWrite(ANALOG_SW_DATA , LOW);
  //digitalWrite(ANALOG_SW_STROBE, HIGH);
  digitalWrite(ANALOG_SW_STROBE, LOW);
  //digitalWrite(ANALOG_SW_DATA , HIGH);
  digitalWrite(ANALOG_SW_RESET, LOW);
  capslock = false;
  lshift = false;
  rshift = false;
  switchState[0] = switchState[1] = switchState[2] = 0;
  switchState[3] = switchState[4] = switchState[5] = 0;
  switchState[6] = switchState[7] = 0;
  for (int i = 0; i < 0x40; i++) {
    setSwitch(i, 0);
  }
}

void C64keyboard::setSwitch(uint8_t swCode, uint8_t data) {
  int bitr;
  bool state;
  uint8_t a = (swCode >> 3) & 7;
  uint8_t b = (swCode & 7);

  if (data) {
    switchState[b] |= 1 << a;
  } else {
    switchState[b] &= ~(1 << a);
  }

  a = am[a];
  b = bm[b];

#ifdef ANALOG_SW_AX3
  // Fix logic table hole in MT8812/16. See datasheet table
  // Convert x12 & x13 to x6 & x7. AX3 line control
  if ((a & 6) == 6) {
    a += 2;
  }
#endif

  digitalWrite(ANALOG_SW_DATA, data);

  digitalWrite(ANALOG_SW_AY0, bitRead(b,0));
  digitalWrite(ANALOG_SW_AY1, bitRead(b,1));
  digitalWrite(ANALOG_SW_AY2, bitRead(b,2));

  digitalWrite(ANALOG_SW_AX0, bitRead(a,0));
  digitalWrite(ANALOG_SW_AX1, bitRead(a,1));
  digitalWrite(ANALOG_SW_AX2, bitRead(a,2));
#ifdef ANALOG_SW_AX3
  digitalWrite(ANALOG_SW_AX3, bitRead(a,3));
#endif

  digitalWrite(ANALOG_SW_STROBE, HIGH);

  digitalWrite(ANALOG_SW_STROBE, LOW);
}

void C64keyboard::c64key(uint16_t code) {
  uint8_t c = code & 0xff;
  uint8_t autoShift = !!(code & FLAG_AUTOSHIFT);
  uint8_t keyDown = !!(code & FLAG_KEYDOWN);

  switch (c) {
    case CK_IGNORE_KEYCODE:
      break;

    case CK_RESET:
      resetSwitch();
      break;

    case CK_RESTORE:
      if (keyDown) {
        pinMode (nmiPin, OUTPUT);
        digitalWrite (nmiPin, LOW);
      } else {
        //pinMode (nmiPin, INPUT);
        pinMode (nmiPin, INPUT_PULLUP);
      }
      break;

    case CK_CAPSLOCK:
      if (keyDown) {
        capslock = !capslock;
        setSwitch(CKM_L_SHIFT, capslock | lshift);
      }
      break;

    /*
        case CK_KEYMAP1:
          if (keyDown) {
            currkeymap = 1;
          }
          break;

        case CK_KEYMAP2:
          if (keyDown) {
            currkeymap = 2;
          }
          break;
    */

    case CKM_R_SHIFT:
      rshift = keyDown;
      setSwitch(c, keyDown);
      break;

    case CKM_L_SHIFT:
      lshift = keyDown;
      setSwitch(c, keyDown | capslock);
      break;

    default:
      /*
      if (capslock && (rshift || lshift)) {
        // Differential shift conversion during key press
        if (keyDown) {
          setSwitch(CKM_R_SHIFT, LOW);
          setSwitch(CKM_L_SHIFT, LOW);
        } else {
          setSwitch(CKM_L_SHIFT, lshift | capslock);
          setSwitch(CKM_R_SHIFT, rshift);
        }
      } else {
      }
      */

      // Auto shift for arrows etc.
      if (autoShift) {
        if (keyDown) {
          setSwitch(CKM_R_SHIFT, HIGH);
        } else {
          setSwitch(CKM_R_SHIFT, rshift);
        }
      }

      setSwitch(c, keyDown);
      break;
  }

  //  debug output
  if (debug) {
    debugkey(c, lshift << 4 | rshift << 3 | capslock << 2 | autoShift << 1 | keyDown);
  }

}

void C64keyboard::begin(int nmiPin) {
  this->nmiPin = nmiPin;

  // initialize the pins

  pinMode( ANALOG_SW_AY0, OUTPUT);
  pinMode( ANALOG_SW_AY1, OUTPUT);
  pinMode( ANALOG_SW_AY2, OUTPUT);
  pinMode( ANALOG_SW_AX0, OUTPUT);
  pinMode( ANALOG_SW_AX1, OUTPUT);
  pinMode( ANALOG_SW_AX2, OUTPUT);
#ifdef ANALOG_SW_AX3
  pinMode( ANALOG_SW_AX3, OUTPUT);  // ANALOG_SW_AX3 (AX3) is separate as it is used only for conversion of X12/X13 into X6/X7
  digitalWrite(ANALOG_SW_AX3, LOW);
#endif
  pinMode( ANALOG_SW_STROBE, OUTPUT);  // MT88XX strobe
  pinMode( ANALOG_SW_DATA, OUTPUT);   // MT88XX data
  pinMode( ANALOG_SW_RESET, OUTPUT); // MT88XX reset
  pinMode( ANALOG_SW_DATA, OUTPUT);  //MT88XX data
  pinMode( nmiPin, INPUT_PULLUP); // C64 NMI

  resetSwitch();
}

C64keyboard::C64keyboard() {
  // nothing to do here, begin() does it all
}

char *c64key2str(uint8_t c) {
  switch (c) {
case CKM_A: return "A";
case CKM_B: return "B";
case CKM_C: return "C";
case CKM_D: return "D";
case CKM_E: return "E";
case CKM_F: return "F";
case CKM_G: return "G";
case CKM_H: return "H";
case CKM_I: return "I";
case CKM_J: return "J";
case CKM_K: return "K";
case CKM_L: return "L";
case CKM_M: return "M";
case CKM_N: return "N";
case CKM_O: return "O";
case CKM_P: return "P";
case CKM_Q: return "Q";
case CKM_R: return "R";
case CKM_S: return "S";
case CKM_T: return "T";
case CKM_U: return "U";
case CKM_V: return "V";
case CKM_W: return "W";
case CKM_X: return "X";
case CKM_Y: return "Y";
case CKM_Z: return "Z";
case CKM_0: return "0";
case CKM_1: return "1";
case CKM_2: return "2";
case CKM_3: return "3";
case CKM_4: return "4";
case CKM_5: return "5";
case CKM_6: return "6";
case CKM_7: return "7";
case CKM_8: return "8";
case CKM_9: return "9";
case CKM_ASTERISK: return "*";
case CKM_AT: return "@";
case CKM_COLON: return ":";
case CKM_SEMICOLON: return ";";
case CKM_COMMA: return ",";
case CKM_PERIOD: return ".";
case CKM_EQUAL: return "=";
case CKM_LEFT_ARROW: return "LEFT_ARROW";
case CKM_MINUS: return "-";
case CKM_PLUS: return "+";
case CKM_POUND: return "£";
case CKM_SLASH: return "/";
case CKM_UP_ARROW: return "UP_ARROW";
case CKM_F1: return "F1";
case CKM_F3: return "F3";
case CKM_F5: return "F5";
case CKM_F7: return "F7";
case CKM_HOME: return "HOME";
case CKM_CRSR_RIGHT: return "CRSR_RIGHT";
case CKM_CRSR_DOWN: return "CRSR_DOWN";
case CKM_DEL: return "DEL";
case CKM_RETURN: return "RETURN";
case CKM_STOP: return "STOP";
case CKM_SPACE: return " ";
case CKM_CBM: return "CBM";
case CKM_CTRL: return "CTRL";
case CKM_L_SHIFT: return "L_SHIFT";
case CKM_R_SHIFT: return "R_SHIFT";
  }
static char b[10];
b[0]='0';
b[1]='x';
return itoa(c, b+2, 16);
}

void C64keyboard::debugkey (uint8_t c, uint8_t flags) {
  Serial.print ("C64: 0x");
  Serial.print (c, 16);

  switch (c) {
    case CK_IGNORE_KEYCODE:
      Serial.print(" IGNORE_KEYCODE");
      break;

    case CK_RESET:
      Serial.print(" RESET");
      break;

    case CK_RESTORE:
      Serial.print(" RESTORE");
      break;

    case CK_CAPSLOCK:
      Serial.print(" CAPS LOCK");
      Serial.print(capslock);
      break;

    case CK_KEYMAP1:
      Serial.print(" KEY_MAP_1");
      break;

    case CK_KEYMAP2:
      Serial.print(" KEY_MAP_2");
      break;

    case CKM_R_SHIFT:
      Serial.print(" RSHIFT");
      Serial.print(rshift);
      break;

    case CKM_L_SHIFT:
      Serial.print(" LSHIFT");
      Serial.print(lshift);
      break;

    default:
      Serial.print(" '");
      Serial.print(c64key2str(c));
      Serial.print("'");
  }

  Serial.print (" flags: ");
  Serial.print(flags, 2);
  Serial.println();
#define PRINTBIN(Num) for (uint8_t t = 1; t; t <<= 1) Serial.write(Num  & t ? '1' : '0'); // Prints a binary number with leading zeros (Automatic Handling)
  PRINTBIN(switchState[0]);
  Serial.println (" ");
  PRINTBIN(switchState[1]);
  Serial.println (" ");
  PRINTBIN(switchState[2]);
  Serial.println (" ");
  PRINTBIN(switchState[3]);
  Serial.println (" ");
  PRINTBIN(switchState[4]);
  Serial.println (" ");
  PRINTBIN(switchState[5]);
  Serial.println (" ");
  PRINTBIN(switchState[6]);
  Serial.println (" ");
  PRINTBIN(switchState[7]);
  Serial.println();
}
