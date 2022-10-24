/*
  irkey.h - Infrared Keyboard Library

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

#ifndef irkey_h
#define irkey_h

void IR_setup(void);
uint32_t read_IR ();
const char *key2sym(uint8_t key);

struct keyb_event {
  uint8_t header;
  union {
    struct {
      uint8_t modifier;
      uint8_t code;
    } key_ev;
    struct {
      uint8_t x: 6;
      uint8_t y: 6;
      uint8_t pad: 4;
    } joy_ev;
  } u;
  uint8_t chksum: 4;
  uint8_t pad: 4;
};

#define IR_GET_CHKSUM(data) (((struct keyb_event*)&data)->chksum)
#define IR_GET_HEADER(data) (((struct keyb_event*)&data)->header)

#define IR_GET_KEYBOARD(data) (((struct keyb_event*)&data)->header & 0x02)
#define IR_GET_RELEASE(data) (((struct keyb_event*)&data)->header & 0x80)
#define IR_GET_REPEAT(data) (((struct keyb_event*)&data)->header & 0x40)

#define IR_GET_CODE(data) (((struct keyb_event*)&data)->u.key_ev.code)
#define IR_GET_MODIFIER(data) (((struct keyb_event*)&data)->u.key_ev.modifier)

#define IR_GET_SHIFT(data) (((struct keyb_event*)&data)->u.key_ev.modifier & 0x01)
#define IR_GET_ALT(data) (((struct keyb_event*)&data)->u.key_ev.modifier & 0x02)
#define IR_GET_CTRL(data) (((struct keyb_event*)&data)->u.key_ev.modifier & 0x04)
#define IR_GET_GUI(data) (((struct keyb_event*)&data)->u.key_ev.modifier & 0x08)

#define IR_GET_JOY_X(data) ((int8_t)(((struct keyb_event*)&data)->u.joy_ev.x<<2))
#define IR_GET_JOY_Y(data) ((int8_t)(((struct keyb_event*)&data)->u.joy_ev.y<<2))

#define IR_GET_BUTTON1(data) (((struct keyb_event*)&data)->header & 0x20)
#define IR_GET_BUTTON2(data) (((struct keyb_event*)&data)->header & 0x40)

#endif
