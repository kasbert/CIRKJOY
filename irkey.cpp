
/*
  irkey.cpp - Infrared Keyboard Library

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

#include <Arduino.h>

#include <IRremote.h>
#include "irkey.h"

//#define DEBUG 0

// IR Receiver (TSOP4838)
const int IR_RECEIVE_PIN = A5;

IRrecv irrecv(IR_RECEIVE_PIN);

#define MY_DECODE_KEYBOARD 250
#define MY_DECODE_JOYSTICK 251

#define KEYB_HZ            38
#define KEYB_BITS          28
#define KEYB_SUM_BITS    2
#define KEYB_HDR_MARK    1000
#define KEYB_HDR_SPACE   500
#define KEYB_BIT_MARK     500
#define KEYB_00_SPACE  450
#define KEYB_01_SPACE  650
#define KEYB_10_SPACE  900
#define KEYB_11_SPACE  1150

void IR_setup(void) {
  irrecv.enableIRIn();  // Start the receiver
}

bool decodeKeyb(decode_results *results) {
  uint32_t data = 0;
  unsigned int offset = 1;
  IR_DEBUG_PRINTLN("Attempting keyboard decode");

  // Check SIZE
  if (irparams.rawlen < KEYB_BITS / 2 + 3) {
    return false;
  }

  // Check HDR Mark/Space
  if (!MATCH_MARK(results->rawbuf[offset], KEYB_HDR_MARK)) {
    return false;
  }
  offset++;

  if (!MATCH_SPACE(results->rawbuf[offset], KEYB_HDR_SPACE)) {
    return false;
  }
  offset++;

  for (int i = 0; i < KEYB_BITS; i += 2) {
    int us = results->rawbuf[offset] * MICROS_PER_TICK;
    if (!MATCH_MARK(results->rawbuf[offset], KEYB_BIT_MARK)) {
      IR_DEBUG_PRINT("ERROR ");
      IR_DEBUG_PRINT(offset);
      IR_DEBUG_PRINT(",");
      IR_DEBUG_PRINTLN(us);
      return false;
    }
    offset++;
    us = results->rawbuf[offset] * MICROS_PER_TICK;
    if (us >= KEYB_00_SPACE - 100 && us <= KEYB_00_SPACE + 100) {
      data = (data >> 2);
      //data = (data << 2);
    } else if (us >= KEYB_01_SPACE - 100 && us <= KEYB_01_SPACE + 100) {
      data = (data >> 2) | 0x40000000;
      //  data = (data << 2) | 1;
    } else if (us >= KEYB_10_SPACE - 100 && us <= KEYB_10_SPACE + 100) {
      data = (data >> 2) | 0x80000000;
      // data = (data << 2) | 2;
    } else if (us >= KEYB_11_SPACE - 100 && us <= KEYB_11_SPACE + 100) {
      data = (data >> 2) | 0xC0000000;
      //data = (data << 2) | 3;
    } else {
      IR_DEBUG_PRINT("ERROR ");
      IR_DEBUG_PRINT(offset);
      IR_DEBUG_PRINT(",");
      IR_DEBUG_PRINTLN(us);
      return false;
    }
    offset++;
  }

  results->bits = (offset - 1) / 2;
  results->bits = KEYB_BITS;
  data >>= 4;

  int chksum = 2;
  for (uint32_t mask = 0x1, i = 0; i < 24; i++, mask <<= 1) {
    if (data & mask) chksum++;
  }
  if (chksum != data >> 24) {
    IR_DEBUG_PRINTLN("CHECKSUM ERROR");
    return false;
  }

  results->value = data;
  results->decode_type = (decode_type_t)MY_DECODE_KEYBOARD;
  return true;
}


#define JOY_BITS          16  // The number of bits in the command
#define JOY_HDR_MARK     1200  // The length of the Header:Mark
#define JOY_T1          600  // Manchester 600us - 1200us

bool decodeJoy(decode_results *results) {
  unsigned long data = 0;  // Somewhere to build our code
  int offset = 1;  // Skip the Gap reading
  IR_DEBUG_PRINTLN("Attempting joystick decode");

  if (irparams.rawlen < JOY_BITS + 1) {
    return false;
  }

  // Check initial Mark match
  if (!MATCH_MARK(results->rawbuf[offset], JOY_HDR_MARK)) {
    return false;
  }
  offset++;

  // Read the bits in
  bool skip = true;
  for (int i = 0; i < JOY_BITS; ) {
    int us = results->rawbuf[offset] * MICROS_PER_TICK;
    bool mark = (offset & 1);
    offset++;
    if (offset > irparams.rawlen) {
      IR_DEBUG_PRINT("ERROR ");
      IR_DEBUG_PRINT(offset);
      IR_DEBUG_PRINT(",");
      IR_DEBUG_PRINT(us);
      IR_DEBUG_PRINT(",");
      IR_DEBUG_PRINTLN(irparams.rawlen);
      return false;
    }
    if (us >= JOY_T1 - 100 && us < JOY_T1 + 100) {
      if (skip) {
        skip = false;
      } else {
        data = (data << 1);
        if (mark) {
          data = data | 1;
        }
        i++;
        skip = true;
      }

    } else if (us >= JOY_T1 * 2 - 100 && us < JOY_T1 * 2 + 100) {
      data = (data << 1);
      if (mark) {
        data = data | 1;
      }
      i++;
      skip = true;

    } else {
      IR_DEBUG_PRINT("ERROR ");
      IR_DEBUG_PRINT(offset);
      IR_DEBUG_PRINT(",");
      IR_DEBUG_PRINTLN(us);
      return false;
    }
  }

  // Success
  results->bits = JOY_BITS;
  results->value = data;
  results->decode_type = (decode_type_t)MY_DECODE_JOYSTICK;
  return true;
}


//+=============================================================================

static int calcKeybChecksum(uint32_t data) {
  int chksum = 2;
  for (uint32_t mask = 0x1, i = 0; i < 24; i++, mask <<= 1) {
    if (data & mask) chksum++;
  }
  return chksum;
}

static void dumpInfo(decode_results *results);
static void dumpCode(decode_results *results);
static void dumpKeyb(uint32_t data);
static void dumpJoy(uint32_t data);

uint32_t read_IR () {
  decode_results results;        // Somewhere to store the results
  results.decode_type = UNKNOWN;

  if (irrecv.decode(&results)) {  // Grab an IR code
    decodeKeyb(&results);
    decodeJoy(&results);
#ifdef DEBUG
    Serial.println("");           // Blank line between entries
    dumpInfo(&results);           // Output the results
    dumpCode(&results);           // Output the results as source code
#endif

    irrecv.resume();              // Prepare for the next value
  }

  if (results.decode_type == MY_DECODE_KEYBOARD) {
    int chksum = calcKeybChecksum(results.value);
    if (chksum != IR_GET_CHKSUM(results.value)) {
      return 0;
    }
    return results.value;
  } else if (results.decode_type == MY_DECODE_JOYSTICK) {
    // Convert joystick (remote) event to keyboard event
    uint32_t data = 0;
    ((struct keyb_event*)&data)->header |= (results.value & 0x80) ? 0x20 : 0;
    ((struct keyb_event*)&data)->header |= (results.value & 0x8000) ? 0x40 : 0;
    (((struct keyb_event*)&data)->u.joy_ev.x = results.value >> 8) & 0x3f;
    (((struct keyb_event*)&data)->u.joy_ev.y = results.value >> 0) & 0x3f;
    if (data == 0) {
      // When joy is returned to exact center, x and y may be zero
      data = 1;
    }
    return data;
  }

  return 0;
}

const char *key2sym(uint8_t key) {
  switch (key) {
      // sed -e 's,^,case 0x,' -e 's/=/: return "/' -e 's/$/";/' < irkeys.txt > irkeys_names.h
#include "irkeys_names.h"
  }
}

#ifdef DEBUG


//+=============================================================================
// Display IR code
//
void ircode(decode_results *results) {
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }
  // Print Code
  Serial.print(results->value, HEX);
}

//+=============================================================================
// Display encoding type
//
static void encoding(decode_results *results) {
  switch (results->decode_type) {
    default:
    case UNKNOWN:
      Serial.print("UNKNOWN");
      break;
    case NEC:
      Serial.print("NEC");
      break;
    case SONY:
      Serial.print("SONY");
      break;
    case RC5:
      Serial.print("RC5");
      break;
    case RC6:
      Serial.print("RC6");
      break;
    case DISH:
      Serial.print("DISH");
      break;
    case SHARP:
      Serial.print("SHARP");
      break;
    case SHARP_ALT:
      Serial.print("SHARP_ALT");
      break;
    case JVC:
      Serial.print("JVC");
      break;
    case SANYO:
      Serial.print("SANYO");
      break;
#ifdef MITSUBISHI
    case MITSUBISHI:
      Serial.print("MITSUBISHI");
      break;
#endif
    case SAMSUNG:
      Serial.print("SAMSUNG");
      break;
    case LG:
      Serial.print("LG");
      break;
    case WHYNTER:
      Serial.print("WHYNTER");
      break;
#ifdef AIWA_RC_T501
    case AIWA_RC_T501:
      Serial.print("AIWA_RC_T501");
      break;
#endif
    case PANASONIC:
      Serial.print("PANASONIC");
      break;
    case DENON:
      Serial.print("Denon");
      break;
    case BOSEWAVE:
      Serial.print("BOSEWAVE");
      break;
    case MY_DECODE_KEYBOARD:
      Serial.print("KEYBOARD");
      break;
    case MY_DECODE_JOYSTICK:
      Serial.print("JOYSTICK");
      break;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
static void dumpInfo(decode_results *results) {
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAW_BUFFER_LENGTH");
    return;
  }

  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : 0x");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}


//+=============================================================================
// Dump out the decode_results structure.
//
static void dumpCode(decode_results *results) {
  // Start declaration
  /*
    Serial.print("unsigned int  ");          // variable type
    Serial.print("rawData[");                // array name
    Serial.print(results->rawlen - 1, DEC);  // array size
    Serial.print("] = {");                   // Start declaration

    // Dump data
    for (unsigned int i = 1; i < results->rawlen; i++) {
      Serial.print(results->rawbuf[i] * MICROS_PER_TICK, DEC);
      if (i < results->rawlen - 1)
        Serial.print(","); // ',' not needed on last one
      if (!(i & 1))
        Serial.print(" ");
    }

    // End declaration
    Serial.print("};");  //

    // Comment
    Serial.print("  // ");
    encoding(results);
    Serial.print(" ");
    ircode(results);

    // Newline
    Serial.println("");

    // Now dump "known" codes
    if (results->decode_type != UNKNOWN) {

      // Some protocols have an address
      if (results->decode_type == PANASONIC) {
        Serial.print("unsigned int  addr = 0x");
        Serial.print(results->address, HEX);
        Serial.println(";");
      }

      // All protocols have data
      Serial.print("unsigned int  data = 0x");
      Serial.print(results->value, HEX);
      Serial.println(";");
    }
  */


  if (results->decode_type == MY_DECODE_KEYBOARD) {
    Serial.print(" Keyboard: ");
    dumpKeyb(results->value);
    Serial.println(";");
  }
  if (results->decode_type == MY_DECODE_JOYSTICK) {
    Serial.print("Joystick: ");
    dumpJoy(results->value);
    Serial.println(";");
  }

}

static void dumpKeyb(uint32_t data) {
  int chksum = calcKeybChecksum(data);
  struct keyb_event p;
  *((uint32_t*)(&p)) = data;
  Serial.print("chk:");
  Serial.print(p.chksum, HEX);
  Serial.print(chksum == p.chksum ? " OK" : " ERROR");
  Serial.print(",header:");
  Serial.print(p.header, HEX);
  if (p.header & 0x02) {
    Serial.print("+KEY");
    if (p.header & 0x80) {
      Serial.print("+RELEASE");
    }
    if (p.header & 0x40) {
      Serial.print("+REPEAT");
    }
    Serial.print(",mods:");
    Serial.print(p.u.key_ev.modifier, HEX);
    if (p.u.key_ev.modifier & 0x01) {
      Serial.print("+SHIFT");
    }
    if (p.u.key_ev.modifier & 0x02) {
      Serial.print("+ALT");
    }
    if (p.u.key_ev.modifier & 0x04) {
      Serial.print("+CTRL");
    }
    if (p.u.key_ev.modifier & 0x08) {
      Serial.print("+WIN");
    }
    Serial.print(",code:");
    Serial.print(p.u.key_ev.code, HEX);
    Serial.print(" ");
    Serial.print(key2sym(p.u.key_ev.code));
  } else {
    if (p.header & 0x20) {
      Serial.print("+BUTTON1");
    }
    if (p.header & 0x40) {
      Serial.print("+BUTTON2");
    }
    Serial.print(" x:");
    Serial.print((short)(char)(p.u.joy_ev.x<<2), DEC);
    Serial.print(",y:");
    Serial.print((short)(char)(p.u.joy_ev.y<<2), DEC);
  }
}

static void dumpJoy(uint32_t data) {
  Serial.print(" x:");
  Serial.print((data >> 8) & 0x7f, HEX);
  Serial.print(",y:");
  Serial.print(data & 0x7f, HEX);
  if (data & 0x80) {
    Serial.print(" +BUTTON1");
  }
  if (data & 0x8000) {
    Serial.print(" +BUTTON2");
  }
}


//+=============================================================================
// Dump out the raw data as Pronto Hex.
//
void dumpPronto(decode_results *results) {
  Serial.print("Pronto Hex: ");
  irrecv.dumpPronto(&Serial, results);
  Serial.println();
}
#endif
