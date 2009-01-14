/*
 * Copyright (C) 2009 David Ingram
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef LED_FONT_TIME_H
#define LED_FONT_TIME_H

#include "binconst.h"

static const ldisplay_fixed_char_t time_font_colon = {
  B24(00000, 00000000, 00000000),
  B24(00000, 00000000, 00000000),
  B24(00000, 00000100, 00000000),
  B24(00000, 00000000, 00000000),
  B24(00000, 00000100, 00000000),
  B24(00000, 00000000, 00000000),
  B24(00000, 00000000, 00000000)
};

static const ldisplay_fixed_char_t time_font_digits[10] = {
  { // zero
    B8(0110),
    B8(1001),
    B8(1001),
    B8(1001),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // one
    B8(0010),
    B8(0110),
    B8(0010),
    B8(0010),
    B8(0010),
    B8(0010),
    B8(0010)
  },
  { // two
    B8(0110),
    B8(1001),
    B8(0001),
    B8(0010),
    B8(0010),
    B8(0100),
    B8(1111)
  },
  { // three
    B8(1110),
    B8(0001),
    B8(0001),
    B8(0110),
    B8(0001),
    B8(0001),
    B8(1110)
  },
  { // four
    B8(1010),
    B8(1010),
    B8(1010),
    B8(1111),
    B8(0010),
    B8(0010),
    B8(0010)
  },
  { // five
    B8(1111),
    B8(1000),
    B8(1000),
    B8(1110),
    B8(0001),
    B8(0001),
    B8(1110)
  },
  { // six
    B8(0110),
    B8(1000),
    B8(1000),
    B8(1110),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // seven
    B8(1111),
    B8(0001),
    B8(0010),
    B8(0010),
    B8(0100),
    B8(0100),
    B8(1000)
  },
  { // eight
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // nine
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0111),
    B8(0001),
    B8(0001),
    B8(0110)
  },
};

static const uint32_t time_segment_font_digits[10][7] = {
  { // zero
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0000),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // one
    B8(0000),
    B8(0001),
    B8(0001),
    B8(0000),
    B8(0001),
    B8(0001),
    B8(0000)
  },
  { // two
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0110),
    B8(1000),
    B8(1000),
    B8(0110)
  },
  { // three
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0110)
  },
  { // four
    B8(0000),
    B8(1001),
    B8(1001),
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0000)
  },
  { // five
    B8(0110),
    B8(1000),
    B8(1000),
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0110)
  },
  { // six
    B8(0110),
    B8(1000),
    B8(1000),
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // seven
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0000),
    B8(0001),
    B8(0001),
    B8(0000)
  },
  { // eight
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110)
  },
  { // nine
    B8(0110),
    B8(1001),
    B8(1001),
    B8(0110),
    B8(0001),
    B8(0001),
    B8(0000)
  },
};

#endif /* defined LED_FONT_TIME_H */
