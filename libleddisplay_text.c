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

void ldisplay_scroll_char(ldisplay_animq_t *queue, ldisplay_buffer_t init_buffer, const char c, int fixed_font, int dir, uint16_t duration) {
  ldisplay_buffer_t cur_buffer;
  int            charwidth = fixed_font ? 6 /* fixed font char width */        :   font_std_var_ascii[(unsigned)c].width ;
  ldisplay_buffer_t *glyph = fixed_font ? &(font_std_fixed_ascii[(unsigned)c]) : &(font_std_var_ascii[(unsigned)c].glyph);

  ldisplay_buffer_copy(cur_buffer, init_buffer);

  int i;
  for (i=0; i<charwidth; ++i) {
    ldisplay_buffer_scroll(cur_buffer, dir, 1);
    _overlay(*glyph, cur_buffer, charwidth-i-1, 0);
    ldisplay_set(duration, cur_buffer, LDISPLAY_NOCHANGE, queue);
  }
}

void ldisplay_scroll_text(ldisplay_animq_t *queue, ldisplay_buffer_t init_buffer, const char *text, int fixed_font, int dir, uint16_t duration) {
  ldisplay_buffer_t cur_buffer;
  char *tmp = (char*)text;

  ldisplay_buffer_copy(cur_buffer, init_buffer);

  while (*tmp) {
    ldisplay_scroll_char(queue, cur_buffer, *tmp, fixed_font, dir, duration);
    ldisplay_buffer_copy(cur_buffer, queue->last->data.buffer);
    ++tmp;
  }
}

void ldisplay_scroll_to_blank(ldisplay_animq_t *queue, ldisplay_buffer_t init_buffer, int dir, uint16_t duration) {
  if (ldisplay_buffer_blank(init_buffer)) {
    return;
  }

  ldisplay_buffer_t cur_buffer;
  ldisplay_buffer_copy(cur_buffer, init_buffer);

  do {
    ldisplay_buffer_scroll(cur_buffer, dir, 1);
    ldisplay_set(duration, cur_buffer, LDISPLAY_NOCHANGE, queue);
  } while (!ldisplay_buffer_blank(cur_buffer));
}

