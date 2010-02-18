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

#undef NODEV

#include <stdint.h>

#define LDISPLAY_HEIGHT 7
#define LDISPLAY_WIDTH  21

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

#define LDISPLAY_COMBINE_REPLACE 0
#define LDISPLAY_COMBINE_OR      1
#define LDISPLAY_COMBINE_AND     2
#define LDISPLAY_COMBINE_XOR     3

#define LDISPLAY_DIRECTION_UP    0
#define LDISPLAY_DIRECTION_DOWN  1
#define LDISPLAY_DIRECTION_LEFT  2
#define LDISPLAY_DIRECTION_RIGHT 3

// Initialisation function to set up the connection to the device.
// MUST be called before any others.
// Returns 0 on success, 1 if device not found.
int ldisplay_init();

void ldisplay_reset(uint16_t duration, ldisplay_animq_t *queue);
void ldisplay_invert(uint16_t duration, ldisplay_animq_t *queue);
void ldisplay_set(uint16_t duration, ldisplay_buffer_t buffer, unsigned char brightness, ldisplay_animq_t *queue);

// wait until animation queue ends
void ldisplay_wait_for_anim();


void ldisplay_dump_queue(ldisplay_animq_t *queue);
void ldisplay_dump_frame(ldisplay_frame_t *frame);


ldisplay_frame_t *ldisplay_frame_new(unsigned char type, uint16_t duration, unsigned char brightness);
ldisplay_frame_t *ldisplay_framedup(ldisplay_frame_t *frame);
// free the given frame
void ldisplay_frame_free(ldisplay_frame_t *frame);

ldisplay_animq_t *ldisplay_queue_new();
// free the given queue and all of its frames
void ldisplay_queue_free(ldisplay_animq_t *queue);


void ldisplay_enqueue(ldisplay_frame_t *frame, ldisplay_animq_t *queue);
void ldisplay_queue_prepend(ldisplay_frame_t *frame, ldisplay_animq_t *queue);

// move frames from "additional" onto end of "main"
void ldisplay_queue_concat(ldisplay_animq_t *main, ldisplay_animq_t *additional);

// copy frames from "additional" onto end of "main"
void ldisplay_queue_dupconcat(ldisplay_animq_t *main, ldisplay_animq_t *additional);

void ldisplay_queue_reverse(ldisplay_animq_t *queue);


int ldisplay_drawTime(ldisplay_buffer_t buffer, unsigned int time, int style);
int ldisplay_drawChars(ldisplay_buffer_t buffer, const char chars[4], char offset);


// Cleanup function to release the interface.
// MUST be called before exiting
void ldisplay_cleanup();

void ldisplay_dumpBuffer(uint32_t data[7]);

void ldisplay_buffer_combine(const ldisplay_buffer_t foreground, ldisplay_buffer_t background, int xOff, int yOff, int mode);
void ldisplay_buffer_scroll(ldisplay_buffer_t buffer, int direction, unsigned int distance);
void ldisplay_buffer_clear(ldisplay_buffer_t buffer);

#define CLEAR_BUFFER(b) memset((b), 0, sizeof(ldisplay_buffer_t));

// 300ms max frame length
#define MAX_FRAME_LENGTH_MS 300

#endif /* defined LIBLEDDISPLAY_H */
