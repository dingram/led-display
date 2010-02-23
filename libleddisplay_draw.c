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

int ldisplay_buffer_blank(const ldisplay_buffer_t buffer) {
  int i=0;
  for (i=0; i<LDISPLAY_HEIGHT; ++i) {
    if (((uint32_t*)buffer)[i]) {
      return 0;
    }
  }
  return 1;
}

void ldisplay_buffer_combine(const ldisplay_buffer_t foreground, ldisplay_buffer_t background, int xOff, int yOff, int mode) {
  int i;
  // index bounds checking
  if (yOff<-6 || yOff>6 || xOff<-20 || xOff>20) return;

  // easy case
  if (mode == LDISPLAY_COMBINE_REPLACE && xOff==0) {
    for (i= (yOff>0) ? yOff : 0; i<7; ++i) {
      if (i-yOff < 0 || i-yOff>6) continue;
      ((uint32_t*)background)[i] = ((uint32_t*)foreground)[i-yOff];
    }
    return;
  }

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

void ldisplay_buffer_scroll(ldisplay_buffer_t buffer, int direction, unsigned int distance) {
  if (!distance) {
    return;
  }

  int i=0;

  switch (direction) {
    case LDISPLAY_DIRECTION_LEFT:
      for (i=0; i<LDISPLAY_HEIGHT; ++i) {
        ((uint32_t*)buffer)[i] <<= distance;
      }
      break;
    case LDISPLAY_DIRECTION_RIGHT:
      // right shift for unsigned int is logical
      for (i=0; i<LDISPLAY_HEIGHT; ++i) {
        ((uint32_t*)buffer)[i] >>= distance;
      }
      break;
    case LDISPLAY_DIRECTION_UP:
      for (i=0; i<LDISPLAY_HEIGHT; ++i) {
        UINT_BUFFER_ROW(buffer, i) = (i+distance < LDISPLAY_HEIGHT) ? UINT_BUFFER_ROW(buffer, i+distance) : 0;
      }
      break;
    case LDISPLAY_DIRECTION_DOWN:
      for (i=LDISPLAY_HEIGHT-1; i>=0; --i) {
        UINT_BUFFER_ROW(buffer, i) = (i-(signed int)distance >= 0) ? UINT_BUFFER_ROW(buffer, i-distance) : 0;
      }
      break;
    default:
      break;
  }
}

void ldisplay_buffer_clear(ldisplay_buffer_t buffer) {
  memset(buffer, 0, sizeof(ldisplay_buffer_t));
}
void ldisplay_buffer_copy(ldisplay_buffer_t dest, const ldisplay_buffer_t src) {
  memcpy(dest, src, sizeof(ldisplay_buffer_t));
}
