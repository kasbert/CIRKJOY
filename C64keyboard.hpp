/*
  C64keyboard.hpp - Commodore Keyboard library

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


#ifndef C64keyboard_h
#define C64keyboard_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//#define MT8808  0    // Set true if using MT8808 and false if using MT8812 or MT8816

//MT88xx control pins
#define ANALOG_SW_DATA    3  // Sets selected cross switch on/off
#define ANALOG_SW_STROBE  12 // Strobe timing pulse
#define ANALOG_SW_RESET   5  // Reset all MT88XX cross switches to off
#define ANALOG_SW_AY0     9  // Pin that controls AY0
#define ANALOG_SW_AY1     8  // Pin that controls AY1
#define ANALOG_SW_AY2     4  // Pin that controls AY2
#define ANALOG_SW_AX0     7  // Pin that controls AX0
#define ANALOG_SW_AX1     11 // Pin that controls AX1
#define ANALOG_SW_AX2     10 // Pin that controls AX2
#if !MT8808
#define ANALOG_SW_AX3     6  // Pin that controls AX3 (MT8812 & MT8816)
#endif

// Flags for c64key()
#define FLAG_KEYDOWN 0x100
#define FLAG_AUTOSHIFT 0x200

// Special mappings in addition to CKM_ codes
#define CK_RESET 0xab
#define CK_RESTORE 0xac
#define CK_CAPSLOCK 0xAD
#define CK_KEYMAP1 0xae
#define CK_KEYMAP2 0xaf

#define CK_IGNORE_KEYCODE 0xb0

// Flags for keymap entries
// Input shift flags. If set, the key rule is used
#define IR_NO_SHIFT 0x01
#define IR_SHIFT 0x02
// Output shift state
#define CKM_NO_SHIFT 0x10
#define CKM_SHIFT 0x20


class C64keyboard {
  public:

    C64keyboard();

    void begin(int nmiPin);

    void resetSwitch(void);
    void setSwitch(uint8_t c, uint8_t data);
    void c64key(uint16_t k);

    // Set true for serial monitor of C64 keycodes and IR keycodes
    bool debug = true;
    void debugkey (uint8_t c, uint8_t flags);

    int nmiPin;
    volatile bool lshift = false, rshift = false, capslock = false ;
    uint8_t switchState[8];
};
#endif
