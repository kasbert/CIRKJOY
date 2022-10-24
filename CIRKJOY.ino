/*
  C64keyboard - Commodore Keyboard library

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

#include <avr/wdt.h>

#include "C64keyboard.hpp"
#include "mapping.h"
#include "irkey.h"

// IR Receiver (TSOP4838)
//const int IR_RECEIVE_PIN = A5;

const int JOY_UP_PIN = A0;
const int JOY_DOWN_PIN = A1;
const int JOY_LEFT_PIN = A2;
const int JOY_RIGHT_PIN = A3;
const int JOY_BUTTON_PIN = A4;

// C64 Restore key
const int NMI_PIN = 2;

//PS2 communication pins
//const int PS2_DATA_PIN=        A0; // Data pin for PS2 keyboard
//const int PS2_IRQ_PIN  =       2;  // Interrupt (clk) pin for PS2 keyboard

//IRrecv irrecv(IR_RECEIVE_PIN);

C64keyboard ckey;

static uint16_t mapKey(uint32_t irData);
static void handleButtons(uint32_t k);
static uint8_t handleJoyMode(uint32_t k);
static void handleJoystick(uint32_t k);
static void debugIRCode(uint32_t data);

static uint32_t joyTimeout;
static uint8_t joyStatus;
static uint8_t joyMoveLimit = 16;
static uint8_t keyboardJoyMode;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(JOY_UP_PIN, INPUT_PULLUP);
  pinMode(JOY_DOWN_PIN, INPUT_PULLUP);
  pinMode(JOY_LEFT_PIN, INPUT_PULLUP);
  pinMode(JOY_RIGHT_PIN, INPUT_PULLUP);
  pinMode(JOY_BUTTON_PIN, INPUT_PULLUP);
  digitalWrite (JOY_RIGHT_PIN, HIGH);
  digitalWrite (JOY_LEFT_PIN, HIGH);
  digitalWrite (JOY_UP_PIN, HIGH);
  digitalWrite (JOY_DOWN_PIN, HIGH);
  digitalWrite (JOY_BUTTON_PIN, HIGH);

  IR_setup();

  ckey.debug = false;
  ckey.begin(NMI_PIN);

  Serial.begin( 115200 );
  if (ckey.debug) {
    Serial.println("C64 IR keyboard");
    Serial.println(F("Build date " __DATE__));
  }
}

void loop() {
  wdt_enable(WDTO_1S);     // enable the watchdog

  uint32_t irData = read_IR();
  if (irData) {
    digitalWrite(LED_BUILTIN, HIGH);
    if (ckey.debug) {
      debugIRCode(irData);
    }
    if (IR_GET_KEYBOARD(irData)) {
      if (!IR_GET_RELEASE(irData)) {
        switch (IR_GET_CODE(irData)) {
          case IR_KC_VOL_DN:
            joyMoveLimit = 8;
            break;
          case IR_KC_VOL_UP:
            joyMoveLimit = 16;
            break;
          case IR_KC_MUTE:
            joyMoveLimit = 24;
            break;
          case IR_KC_SLEEP:
            keyboardJoyMode = 1;
            break;
          case IR_KC_HELP:
            ckey.debug = 1;
            Serial.println("C64 IR keyboard");
            Serial.println(F("Build date " __DATE__));
            break;
          case IR_KC_CLOSE:
          case IR_KC_POWER:
            //if ((ck & 0xff) == CK_RESET) {
            handleButtons(0);
            handleJoystick(0);
            keyboardJoyMode = 0;
            ckey.debug = false;
            break;
        }
      }
      if (!keyboardJoyMode || !handleJoyMode(irData)) {
        // Normal key processing
        uint16_t ck = mapKey(irData);
        ckey.c64key(ck);
      }

    } else {
      handleButtons(irData);
      handleJoystick(irData);
    }
    if (ckey.debug) {
      for (uint32_t t = 0x10; t; t >>= 1) {
        Serial.write(joyStatus & t ? '1' : '0');
      }
      Serial.println();
    }
  }
  if (joyTimeout && (long)(joyTimeout - millis()) < 0) {
    handleJoystick(0);
  }

  int c = Serial.read();
  if (c >= 32) {
    // FIXME. needs mapping
    Serial.print("Serial: 0x");
    Serial.println(c, HEX);
    ckey.c64key(c | FLAG_KEYDOWN);
    delay(200);
    ckey.c64key(c);
  }
  digitalWrite(LED_BUILTIN, LOW);
  wdt_reset();
}

#define SET_TIMEOUT() joyTimeout= millis() + 200
#define CLEAR_TIMEOUT() joyTimeout= 0;

// value==1 means button pushed
static inline void setPin_(int pin, int value) {
  if (value) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  } else {
#define EXTENAL_PULLUP 1
#if EXTENAL_PULLUP
    pinMode(pin, INPUT_PULLUP);
#else
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
#endif
  }
}

static uint8_t handleJoyMode(uint32_t irData) {
  uint8_t kc = IR_GET_CODE(irData);
  uint8_t keyDown = !IR_GET_RELEASE(irData);
  switch (kc) {
    case IR_KC_UP_ARROW:
    case IR_KC_W:
      setPin_(JOY_UP_PIN, keyDown);
      joyStatus &= ~1;
      joyStatus |= keyDown << 0;
      return 1;
    case IR_KC_DN_ARROW:
    case IR_KC_S:
      setPin_(JOY_DOWN_PIN, keyDown);
      joyStatus &= ~2;
      joyStatus |= keyDown << 1;
      return 1;
    case IR_KC_L_ARROW:
    case IR_KC_A:
      setPin_(JOY_LEFT_PIN, keyDown);
      joyStatus &= ~4;
      joyStatus |= keyDown << 2;
      return 1;
    case IR_KC_R_ARROW:
    case IR_KC_D:
      setPin_(JOY_RIGHT_PIN, keyDown);
      joyStatus &= ~8;
      joyStatus |= keyDown << 3;
      return 1;
    case IR_KC_SPACE:
    case IR_KC_L_SHIFT:
    case IR_KC_R_SHIFT:
    case IR_KC_L_CTRL:
      setPin_(JOY_BUTTON_PIN, keyDown);
      joyStatus &= ~0x10;
      joyStatus |= keyDown << 4;
      return 1;
  }
  return 0;
}

static void handleButtons(uint32_t k) {
  int button = !!(IR_GET_BUTTON1(k) || IR_GET_BUTTON2(k));
  setPin_(JOY_BUTTON_PIN, button);
  joyStatus &= 0xf;
  joyStatus |= button << 4;
}

static void handleJoystick(uint32_t k) {
  int8_t x = IR_GET_JOY_X(k);
  int8_t y = IR_GET_JOY_Y(k);
  CLEAR_TIMEOUT();
  joyStatus &= 0xf0;
  if (y >= joyMoveLimit) {
    setPin_(JOY_UP_PIN, 1);
    setPin_(JOY_DOWN_PIN, 0);
    SET_TIMEOUT();
    joyStatus |= 1;
    joyStatus &= ~2;
  } else if (y <= -joyMoveLimit) {
    setPin_(JOY_UP_PIN, 0);
    setPin_(JOY_DOWN_PIN, 1);
    joyStatus |= 2;
    joyStatus &= ~1;
    SET_TIMEOUT();
  } else {
    setPin_(JOY_UP_PIN, 0);
    setPin_(JOY_DOWN_PIN, 0);
    joyStatus &= ~3;
  }
  if (x <= -joyMoveLimit) {
    setPin_(JOY_LEFT_PIN, 1);
    setPin_(JOY_RIGHT_PIN, 0);
    joyStatus |= 4;
    joyStatus &= ~8;
    SET_TIMEOUT();
  } else if (x >= joyMoveLimit) {
    setPin_(JOY_LEFT_PIN, 0);
    setPin_(JOY_RIGHT_PIN, 1);
    joyStatus |= 8;
    joyStatus &= ~4;
    SET_TIMEOUT();
  } else {
    setPin_(JOY_LEFT_PIN, 0);
    setPin_(JOY_RIGHT_PIN, 0);
    joyStatus &= ~0x0c;
  }
}

static uint16_t mapKey(uint32_t irData) {
  uint8_t kc = IR_GET_CODE(irData);
  uint16_t c = CK_IGNORE_KEYCODE;
  int len = sizeof(C64Keymap_main) / sizeof(C64Keymap_main[0]);
  for (int i = 0; i < len; i++) {
    uint8_t irKey = pgm_read_byte(&C64Keymap_main[i].irKey);
    uint8_t ckmKey = pgm_read_byte(&C64Keymap_main[i].ckmKey);
    uint8_t flags = pgm_read_byte(&C64Keymap_main[i].flags);
    if (irKey == kc) {
      if (IR_GET_SHIFT(irData) && (flags & IR_SHIFT)) {
        c = ckmKey;
        if (flags & CKM_SHIFT) {
          c |= FLAG_AUTOSHIFT;
        }
        break;
      } else if (!IR_GET_SHIFT(irData) && (flags & IR_NO_SHIFT)) {
        c = ckmKey;
        if (flags & CKM_NO_SHIFT) {
          c |= FLAG_AUTOSHIFT;
        }
        break;
      }
    }
  }
  if (!IR_GET_RELEASE(irData)) {
    c |= FLAG_KEYDOWN;
  }
  return c;
}

static void debugIRCode(uint32_t data) {
  Serial.print("IR: 0x");
  Serial.print(data, HEX);
  if (IR_GET_KEYBOARD(data)) {
    Serial.print(" key: 0x");
    Serial.print(IR_GET_CODE(data), HEX);
    Serial.print(" '");
    Serial.print(key2sym(IR_GET_CODE(data)));
    Serial.print("' ");

    Serial.print("+KEY");
    if (IR_GET_RELEASE(data)) {
      Serial.print("+RELEASE");
    }
    if (IR_GET_REPEAT(data)) {
      Serial.print("+REPEAT");
    }
    if (IR_GET_SHIFT(data)) {
      Serial.print("+SHIFT");
    }
    if (IR_GET_ALT(data)) {
      Serial.print("+ALT");
    }
    if (IR_GET_CTRL(data)) {
      Serial.print("+CTRL");
    }
    if (IR_GET_GUI(data)) {
      Serial.print("+GUI");
    }
  } else {
    Serial.print(" joy ");
    Serial.print("x:");
    Serial.print(IR_GET_JOY_X(data), DEC);
    Serial.print(" y:");
    Serial.print(IR_GET_JOY_Y(data), DEC);
    Serial.print(" ");
    if (IR_GET_BUTTON1(data)) {
      Serial.print("+BUTTON1");
    }
    if (IR_GET_BUTTON2(data)) {
      Serial.print("+BUTTON2");
    }
  }
  Serial.println();
}
