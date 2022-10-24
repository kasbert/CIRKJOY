/*
  C64keyboard - Commodore Keyboard library

  Copyright (c) 2022  Jarkko Sonninen

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

#ifndef mapping_h
#define mapping_h

// sed -e 's,\(.*\)=\(.*\),#define IR_KC_\2 0x\1,' < irkeys.txt > irkeys.h
#include "irkeys.h"
#include "c64key.h"

/* By default both shifted and unshifted are allowed
 *  Shift in input will press shift in output
 */
#define DEF_FLAGS (IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT)

typedef struct {
  uint8_t irKey; // Input IR key code
  uint8_t ckmKey; // Output C64 key matrix code
  uint8_t flags;
} C64Keymap_t;

const PROGMEM C64Keymap_t C64Keymap_main[] = {
{ IR_KC_L_SHIFT, CKM_L_SHIFT, DEF_FLAGS },
{ IR_KC_L_CTRL, CKM_CBM, DEF_FLAGS },
{ IR_KC_R_SHIFT, CKM_R_SHIFT, DEF_FLAGS },
{ IR_KC_RETURN, CKM_RETURN, DEF_FLAGS },

{ IR_KC_A, CKM_A, DEF_FLAGS },
{ IR_KC_B, CKM_B, DEF_FLAGS },
{ IR_KC_C, CKM_C, DEF_FLAGS },
{ IR_KC_D, CKM_D, DEF_FLAGS },
{ IR_KC_E, CKM_E, DEF_FLAGS },
{ IR_KC_F, CKM_F, DEF_FLAGS },
{ IR_KC_G, CKM_G, DEF_FLAGS },
{ IR_KC_H, CKM_H, DEF_FLAGS },
{ IR_KC_I, CKM_I, DEF_FLAGS },
{ IR_KC_J, CKM_J, DEF_FLAGS },
{ IR_KC_K, CKM_K, DEF_FLAGS },
{ IR_KC_L, CKM_L, DEF_FLAGS },
{ IR_KC_M, CKM_M, DEF_FLAGS },
{ IR_KC_N, CKM_N, DEF_FLAGS },
{ IR_KC_O, CKM_O, DEF_FLAGS },
{ IR_KC_P, CKM_P, DEF_FLAGS },
{ IR_KC_Q, CKM_Q, DEF_FLAGS },
{ IR_KC_R, CKM_R, DEF_FLAGS },
{ IR_KC_S, CKM_S, DEF_FLAGS },
{ IR_KC_T, CKM_T, DEF_FLAGS },
{ IR_KC_U, CKM_U, DEF_FLAGS },
{ IR_KC_V, CKM_V, DEF_FLAGS },
{ IR_KC_W, CKM_W, DEF_FLAGS },
{ IR_KC_X, CKM_X, DEF_FLAGS },
{ IR_KC_Y, CKM_Y, DEF_FLAGS },
{ IR_KC_Z, CKM_Z, DEF_FLAGS },
{ IR_KC_1, CKM_1, DEF_FLAGS },
{ IR_KC_2, CKM_2, DEF_FLAGS },
{ IR_KC_3, CKM_3, DEF_FLAGS },
{ IR_KC_4, CKM_4, DEF_FLAGS },
{ IR_KC_5, CKM_5, DEF_FLAGS },
{ IR_KC_6, CKM_6, DEF_FLAGS },
{ IR_KC_7, CKM_7, DEF_FLAGS },
{ IR_KC_8, CKM_8, DEF_FLAGS },
{ IR_KC_9, CKM_9, DEF_FLAGS },
{ IR_KC_0, CKM_0, DEF_FLAGS },
{ IR_KC_F1, CKM_F1, DEF_FLAGS },
{ IR_KC_F2, CKM_F1, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT },
{ IR_KC_F3, CKM_F3, DEF_FLAGS },
{ IR_KC_F4, CKM_F3, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT },
{ IR_KC_F5, CKM_F5, DEF_FLAGS },
{ IR_KC_F6, CKM_F5, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT },
{ IR_KC_F7, CKM_F7, DEF_FLAGS },
{ IR_KC_F8, CKM_F7, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT },
//{ IR_KC_F9, CKM_F9 },
//{ IR_KC_F10, CKM_F10 },
//{ IR_KC_NUM, CKM_NUM },
//{ IR_KC_SCROLL, CKM_SCROLL },
{ IR_KC_PRTSCR, CKM_UP_ARROW, DEF_FLAGS },
{ IR_KC_PAUSE, CKM_POUND, DEF_FLAGS },
{ IR_KC_HOME, CKM_HOME, DEF_FLAGS },
{ IR_KC_END, CK_RESTORE, DEF_FLAGS },
//{ IR_KC_L_GUI, CKM_L_GUI },
//{ IR_KC_L_ALT, CKM_L_ALT },
//{ IR_KC_R_GUI, CKM_R_GUI },
{ IR_KC_MINUS, CKM_PLUS, DEF_FLAGS },
{ IR_KC_EQUAL, CKM_MINUS, DEF_FLAGS },
{ IR_KC_ESC, CKM_STOP, DEF_FLAGS },
{ IR_KC_TAB, CKM_CTRL, DEF_FLAGS },
//{ IR_KC_MENU, CKM_MENU },
{ IR_KC_CAPS, CKM_STOP, DEF_FLAGS },
//{ IR_KC_INS, CKM_INS },
//{ IR_KC_DEL, CKM_DEL },
//{ IR_KC_PGUP, CKM_PGUP },
//{ IR_KC_PGDN, CKM_PGDN },
{ IR_KC_UP_ARROW, CKM_CRSR_DOWN, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT},
{ IR_KC_DN_ARROW, CKM_CRSR_DOWN, DEF_FLAGS },
{ IR_KC_L_ARROW, CKM_CRSR_RIGHT, IR_NO_SHIFT | IR_SHIFT | CKM_SHIFT | CKM_NO_SHIFT},
{ IR_KC_R_ARROW, CKM_CRSR_RIGHT, DEF_FLAGS },
{ IR_KC_HASH, CKM_EQUAL, DEF_FLAGS },
{ IR_KC_LBRACKET, CKM_AT, DEF_FLAGS },
{ IR_KC_RBRACKET, CKM_ASTERISK, DEF_FLAGS },
{ IR_KC_SEMICOLON, CKM_COLON, DEF_FLAGS },
{ IR_KC_AT, CKM_SEMICOLON, DEF_FLAGS },
{ IR_KC_PERIOD, CKM_PERIOD, DEF_FLAGS },
{ IR_KC_COMMA, CKM_COMMA, DEF_FLAGS },
{ IR_KC_BACKSPACE, CKM_DEL, DEF_FLAGS },
{ IR_KC_DIV, CKM_SLASH, DEF_FLAGS },
{ IR_KC_BACKSLASH, CK_CAPSLOCK, IR_NO_SHIFT | IR_SHIFT },
{ IR_KC_SPACE, CKM_SPACE, DEF_FLAGS },
{ IR_KC_GRAVE, CKM_LEFT_ARROW, DEF_FLAGS },
{ IR_KC_ENTER, CKM_RETURN, DEF_FLAGS },
//{ IR_KC_F11, CKM_F11 },
//{ IR_KC_F12, CKM_F12 },
//{ IR_KC_KP0, CKM_KP0 },
//{ IR_KC_KP1, CKM_KP1 },
//{ IR_KC_KP2, CKM_KP2 },
//{ IR_KC_KP3, CKM_KP3 },
//{ IR_KC_KP4, CKM_KP4 },
//{ IR_KC_KP5, CKM_KP5 },
//{ IR_KC_KP6, CKM_KP6 },
//{ IR_KC_KP7, CKM_KP7 },
//{ IR_KC_KP8, CKM_KP8 },
//{ IR_KC_KP9, CKM_KP9 },
//{ IR_KC_KP_DOT, CKM_KP_DOT },
//{ IR_KC_KP_DIV, CKM_KP_DIV },
//{ IR_KC_KP_PLUS, CKM_KP_PLUS },
//{ IR_KC_KP_MINUS, CKM_KP_MINUS },
//{ IR_KC_KP_TIMES, CKM_KP_TIMES },
{ IR_KC_CLOSE, CK_RESET, DEF_FLAGS },
/*
{ IR_KC_FULLSCREEN, CKM_FULLSCREEN },
{ IR_KC_WINDOW, CKM_WINDOW },
{ IR_KC_HELP, CKM_HELP },
{ IR_KC_CALC, CKM_CALC },
{ IR_KC_WEB, CKM_WEB },
{ IR_KC_EJECT, CKM_EJECT },
{ IR_KC_PREV_TR, CKM_PREV_TR },
{ IR_KC_PLAY, CKM_PLAY },
{ IR_KC_STOP, CKM_STOP },
{ IR_KC_NEXT_TR, CKM_NEXT_TR },
{ IR_KC_RECORD, CKM_RECORD },
{ IR_KC_VOL_DN, CKM_VOL_DN },
{ IR_KC_VOL_UP, CKM_VOL_UP },
{ IR_KC_MUTE, CKM_MUTE },
{ IR_KC_SLEEP, CKM_SLEEP },
*/
{ IR_KC_POWER, CK_RESET, DEF_FLAGS }

};

#endif
