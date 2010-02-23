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

#include <string.h>
#include "libleddisplay_priv.h"

// fonts
#include "led_font_time.h"
#include "led_font_std.h"

static inline void _overlay(const ldisplay_buffer_t foreground, ldisplay_buffer_t background, int xOff, int yOff) {
  int i;
  // index bounds checking
  if (yOff<-6 || yOff>6 || xOff<-20 || xOff>20) return;

  if (xOff<0) {
    xOff = -xOff;
    for (i= (yOff>0) ? yOff : 0; i<7; ++i) {
      if (i-yOff < 0 || i-yOff>6) continue;
      background[i] |= (uint32_t)(foreground[i-yOff] << xOff);
    }
  } else {
    for (i= (yOff>0) ? yOff : 0; i<7; ++i) {
      if (i-yOff < 0 || i-yOff>6) continue;
      background[i] |= (uint32_t)(foreground[i-yOff] >> xOff);
    }
  }
}

int ldisplay_drawTime(ldisplay_buffer_t buffer, unsigned int time, int style) {
  if (time>9999 || style<0 || style>1) {
    return ERR_BAD_ARGS;
  }

  CLEAR_BUFFER(buffer);

  _overlay(time_font_colon, buffer, 0, 0);

  if (style) {
    _overlay(time_segment_font_digits[(time     )%10], buffer,   0, 0);
    _overlay(time_segment_font_digits[(time/10  )%10], buffer, - 5, 0);
    _overlay(time_segment_font_digits[(time/100 )%10], buffer, -12, 0);
    _overlay(time_segment_font_digits[(time/1000)%10], buffer, -17, 0);
  } else {
    _overlay(time_font_digits[(time     )%10], buffer,   0, 0);
    _overlay(time_font_digits[(time/10  )%10], buffer, - 5, 0);
    _overlay(time_font_digits[(time/100 )%10], buffer, -12, 0);
    _overlay(time_font_digits[(time/1000)%10], buffer, -17, 0);
  }

  return SUCCESS;
}

int ldisplay_drawChars(ldisplay_buffer_t buffer, const char chars[4], char offset) {
  CLEAR_BUFFER(buffer);

  _overlay(font_std_fixed_ascii[(unsigned)chars[0]], buffer, offset - 21, 0);
  _overlay(font_std_fixed_ascii[(unsigned)chars[1]], buffer, offset - 16, 0);
  _overlay(font_std_fixed_ascii[(unsigned)chars[2]], buffer, offset - 11, 0);
  _overlay(font_std_fixed_ascii[(unsigned)chars[3]], buffer, offset -  6, 0);
  _overlay(font_std_fixed_ascii[(unsigned)chars[4]], buffer, offset -  1, 0);

  return SUCCESS;
}
