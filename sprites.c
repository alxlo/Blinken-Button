/*
 * sprites.c
 *
 *  http://interactive-matter.org/
 *
 *  This file is part of Blinken Button.
 *
 *  Blinken Button is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Blinken Button is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  Created on: 26.01.2010
 */
#include <avr/pgmspace.h>

const prog_uint8_t PROGMEM predefined_sprites[][8] = {
  {
    0x18,    // ___XX___ 0
    0x3C,    // __XXXX__
    0x7E,    // _XXXXXX_
    0xDB,    // X_XXXX_X
    0xFF,    // XXXXXXXX
    0x24,    // __X__X__
    0x5A,    // _X_XX_X_
    0xA5     // X_X__X_X
  },
  {
    0x18,    // ___XX___  1
    0x3C,    // __XXXX__
    0x7E,    // _XXXXXX_
    0xDB,    // X_XXXX_X
    0xFF,    // XXXXXXXX
    0x24,    // __X__X__
    0x42,    // _X____X_
    0x24     // __X__X__
  },
  {
    0x24,    // __X__X__  2
    0x7E,    // _XXXXXX_
    0xDB,    // XX_XX_XX
    0xFF,    // XXXXXXXX
    0xA5,    // X_X__X_X
    0x99,    // X__XX__X
    0x81,    // X______X
    0xC3     // XX____XX
  },
  {
    0x24,    // __X__X__  3
    0x18,    // ___XX___
    0x7E,    // X_XXXX_X
    0xDB,    // XX_XX_XX
    0xFF,    // XXXXXXXX
    0xDB,    // X_XXXX_X
    0x99,    // X__XX__X
    0xC3     // XX____XX
  },
    {
      0x00,    // ________ 4
      0x00,    // ________
      0x14,    // ___X_X__
      0x3E,    // __XXXXX_
      0x3E,    // __XXXXX_
      0x1C,    // ___XXX__
      0x08,    // ____X___
      0x00     // ________
    },
    {
      0x00,    // ________  5
      0x66,    // _XX__XX_
      0xFF,    // XXXXXXXX
      0xFF,    // XXXXXXXX
      0xFF,    // XXXXXXXX
      0x7E,    // _XXXXXX_
      0x3C,    // __XXXX__
      0x18     // ___XX___
    },
  {
    0xCC,    // XX__XX__  6
    0xCC,    // XX__XX__
    0x33,    // __XX__XX
    0x33,    // __XX__XX
    0xCC,    // XX__XX__
    0xCC,    // XX__XX__
    0x33,    // __XX__XX
    0x33     // __XX__XX
  },
  {
    0x33,    // __XX__XX  7
    0x33,    // __XX__XX
    0xCC,    // XX__XX__
    0xCC,    // XX__XX__
    0x33,    // __XX__XX
    0x33,    // __XX__XX
    0xCC,    // XX__XX__
    0xCC     // XX__XX__
  },
  {
    0x00,    // ________  8
    0x00,    // ________
    0x00,    // ________
    0x18,    // ___XX___
    0x18,    // ___XX___
    0x00,    // ________
    0x00,    // ________
    0x00     // ________
  },
  {
    0x00,    // ________  9
    0x00,    // ________
    0x3C,    // __XXXX__
    0x3C,    // __XXXX__
    0x3C,    // __XXXX__
    0x3C,    // __XXXX__
    0x00,    // ________
    0x00     // ________
  },
  {
    0x00,    // ________  10
    0x7E,    // _XXXXXX_
    0x7E,    // _XXXXXX_
    0x7E,    // _XXXXXX_
    0x7E,    // _XXXXXX_
    0x7E,    // _XXXXXX_
    0x7E,    // _XXXXXX_
    0x00     // ________
  },
  {
    0xFF,    // XXXXXXXX  11
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF     // XXXXXXXX
  },
  {
    0x0F,    // ____XXXX  12
    0x0F,    // ____XXXX
    0x0F,    // ____XXXX
    0x0F,    // ____XXXX
    0xF0,    // XXXX____
    0xF0,    // XXXX____
    0xF0,    // XXXX____
    0xF0,    // XXXX____
  },
  {
    0xF0,    // XXXX____  13
    0xF0,    // XXXX____
    0xF0,    // XXXX____
    0xF0,    // XXXX____
    0x0F,    // ____XXXX
    0x0F,    // ____XXXX
    0x0F,    // ____XXXX
    0x0F,    // ____XXXX
  },
  {
    0xFF,    // XXXXXXXX  14
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0xFF,    // XXXXXXXX
  },
  {
    0xFF,    // XXXXXXXX  15
    0xFF,    // XXXXXXXX
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0x00,    // ________
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
  },
  {
    0xFF,    // XXXXXXXX  16
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0x00,    // ________
    0x00,    // ________
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
    0xFF,    // XXXXXXXX
  },
  {
    0x10,    // ___X____  17
    0x10,    // ___X____
    0x10,    // ___X____
    0x1F,    // ___XXXXX
    0xF8,    // XXXXX___
    0x08,    // ____X___
    0x08,    // ____X___
    0x08,    // ____X___
  },
  {
    0x70,    // _XXX____  18
    0x31,    // __XX___X
    0x13,    // ___X__XX
    0x1F,    // ___XXXXX
    0xF8,    // XXXXX___
    0xC8,    // XX__X___
    0x8C,    // X___XX__
    0x0E,    // ____XXX_
  },
  {
    0xF1,    // XXXX___X  19
    0x73,    // _XXX__XX
    0x37,    // __XX_XXX
    0x1F,    // ___XXXXX
    0xF8,    // XXXXX___
    0xEC,    // XXX_XX__
    0xCE,    // XX__XXX_
    0x8F,    // X___XXXX
  },
  {
    0x04,    // _____X__  20
    0x00,    // ________
    0x80,    // X_______
    0x00,    // ________
    0x00,    // ________
    0x01,    // _______X
    0x00,    // ________
    0x20,    // __X_____
  },
  {
    0x0E,    // ____XXX_  21
    0x84,    // X____X__
    0xC0,    // XX______
    0x80,    // X_______
    0x01,    // _______X
    0x03,    // ______XX
    0x21,    // __X____X
    0x70,    // _XXX____
  },
  {
    0x9F,    // X__XXXXX  22
    0xCE,    // XX__XXX_
    0xE4,    // XXX__X__
    0xC1,    // XX_____X
    0x83,    // X_____XX
    0x27,    // __X__XXX
    0x73,    // _XXX__XX
    0xF9,    // XXXXX__X
  }
};

/*
 * copy_to_buffer
 * Copies the given sprite from PROGMEM to RAM.
 */
void copy_to_buffer(const prog_uint8_t sprite[8], uint8_t* buffer) {
  memcpy_P(buffer, sprite, 8);
}

