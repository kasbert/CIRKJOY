
/*
  c64key.h - C64/VIC20 keyboard matrix codes

  Copyright (c) 2022 Jarkko Sonninen

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

#ifndef c64key_h
#define c64key_h

// Commodore key matrix codes
// https://sta.c64.org/cbm64kbdcode2.html
// https://ist.uwaterloo.ca/~schepers/MJK/keyboard_int.html
#define CKM_A 0x0a
#define CKM_B 0x1c
#define CKM_C 0x14
#define CKM_D 0x12
#define CKM_E 0x0e
#define CKM_F 0x15
#define CKM_G 0x1a
#define CKM_H 0x1d
#define CKM_I 0x21
#define CKM_J 0x22
#define CKM_K 0x25
#define CKM_L 0x2a
#define CKM_M 0x24
#define CKM_N 0x27
#define CKM_O 0x26
#define CKM_P 0x29
#define CKM_Q 0x3e
#define CKM_R 0x11
#define CKM_S 0x0d
#define CKM_T 0x16
#define CKM_U 0x1e
#define CKM_V 0x1f
#define CKM_W 0x09
#define CKM_X 0x17
#define CKM_Y 0x19
#define CKM_Z 0x0c
#define CKM_0 0x23
#define CKM_1 0x38
#define CKM_2 0x3b
#define CKM_3 0x08
#define CKM_4 0x0b
#define CKM_5 0x10
#define CKM_6 0x13
#define CKM_7 0x18
#define CKM_8 0x1b
#define CKM_9 0x20

#define CKM_ASTERISK    0x31
#define CKM_AT          0x2e
#define CKM_COLON       0x2d
#define CKM_COMMA       0x2f
#define CKM_EQUAL       0x35
#define CKM_LEFT_ARROW  0x39
#define CKM_MINUS       0x2b
#define CKM_PERIOD      0x2c
#define CKM_POUND       0x30
#define CKM_PLUS        0x28
#define CKM_SEMICOLON   0x32
#define CKM_SLASH       0x37
#define CKM_UP_ARROW    0x36
#define CKM_F1          0x04 // F1/F2
#define CKM_F3          0x05 // F3/F4
#define CKM_F5          0x06 // F5/F6
#define CKM_F7          0x03 // F7/F8
#define CKM_HOME        0x33 // HOME/CLR
#define CKM_CRSR_RIGHT  0x02 // RIGHT/LEFT
#define CKM_CRSR_DOWN   0x07 // DOWN/UP
#define CKM_DEL         0x00 // DEL/INST
#define CKM_RETURN      0x01
#define CKM_STOP        0x3f // STOP/RUN
#define CKM_SPACE       0x3c
#define CKM_CBM         0x3d
#define CKM_CTRL        0x3a
#define CKM_L_SHIFT     0x0f
#define CKM_R_SHIFT     0x34

#endif
