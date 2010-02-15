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
#ifndef LIBLEDDISPLAY_H
#define LIBLEDDISPLAY_H

#define NODEV

#include <stdint.h>

// Generic return codes
#define SUCCESS         0
#define ERR_INIT_NODEV  1
#define ERR_BAD_ARGS    2

#define LDISPLAY_DIM       0
#define LDISPLAY_MEDIUM    1
#define LDISPLAY_BRIGHT    2
#define LDISPLAY_NOCHANGE  3

typedef uint32_t ldisplay_buffer_t[7];

typedef struct { uint32_t glyph[7]; unsigned char width; } ldisplay_var_char_t;
typedef uint32_t ldisplay_fixed_char_t[7];
typedef union { ldisplay_var_char_t var; ldisplay_fixed_char_t fixed; } ldisplay_char_t;

typedef struct { ldisplay_fixed_char_t *glyphs; unsigned char width; } ldisplay_fixed_font_t;
typedef struct { ldisplay_var_char_t   *glyphs; unsigned char width; } ldisplay_var_font_t;
typedef union { ldisplay_var_font_t var; ldisplay_fixed_font_t fixed; } ldisplay_font_t;

struct ldisplay_animq;
struct ldisplay_frame {
  uint16_t duration; // ms
  unsigned char type;
  unsigned char brightness;
  union {
    ldisplay_buffer_t buffer;
    struct {
      struct ldisplay_animq *queue;
      uint8_t repeat_count;
    } loop;
  } data;
  struct ldisplay_frame *next;
};

struct ldisplay_animq {
  struct ldisplay_frame *first;
  struct ldisplay_frame *last;
};

typedef struct ldisplay_animq ldisplay_animq_t;
typedef struct ldisplay_frame ldisplay_frame_t;

#define LDISPLAY_NOOP         0
#define LDISPLAY_INVERT       1
#define LDISPLAY_CLEAR        2
#define LDISPLAY_SET          3
#define LDISPLAY_LOOP         4
#define LDISPLAY_BRK_IF_LAST  5

// Initialisation function to set up the connection to the device.
// MUST be called before any others.
// Returns 0 on success, 1 if device not found.
int ldisplay_init();

void ldisplay_reset(uint16_t duration);

void ldisplay_invert(uint16_t duration);

void ldisplay_set(uint16_t duration, ldisplay_buffer_t buffer, unsigned char brightness);


//void ldisplay_setBrightness(unsigned char brightness);

int ldisplay_drawTime(ldisplay_buffer_t buffer, unsigned int time, int style);
//int ldisplay_showTime(unsigned int time, int style);

int ldisplay_drawChars(ldisplay_buffer_t buffer, const char chars[4], char offset);
//int ldisplay_showChars(const char chars[4], char offset);

// Cleanup function to release the interface.
// MUST be called before exiting
void ldisplay_cleanup();

void ldisplay_dumpBuffer(uint32_t data[7]);


#define CLEAR_BUFFER(b) memset((b), 0, 7*sizeof(uint32_t));

// 300ms max frame length
#define MAX_FRAME_LENGTH_MS 300

#endif /* defined LIBLEDDISPLAY_H */
