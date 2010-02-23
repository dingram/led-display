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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "libleddisplay_priv.h"

void ldisplay_dump_frame(ldisplay_frame_t *frame) {
  printf("Frame type: ");
  switch (frame->type) {
    case LDISPLAY_INVERT:
      // invert the internal buffer
      printf("INVERT\n");
      printf("  duration:   %d\n", frame->duration);
      printf("  brightness: %d\n", frame->brightness);
      break;
    case LDISPLAY_CLEAR:
      // clear the internal buffer
      printf("CLEAR\n");
      printf("  duration:   %d\n", frame->duration);
      printf("  brightness: %d\n", frame->brightness);
      break;
    case LDISPLAY_SET:
      // copy to internal buffer
      printf("SET\n");
      printf("  duration:   %d\n", frame->duration);
      printf("  brightness: %d\n", frame->brightness);
      printf("  buffer:\n");

      int i, j;
      printf("    +");
      for (j=21; j>=0; --j) {
        printf("-");
      }
      printf("+\n");
      for (i=0; i<7; ++i) {
        printf("    |");
        for (j=21; j>=0; --j) {
          printf( "%c", ((frame->data.buffer[i] >> j) & 0x1) ? '#' : ' ' );
        }
        printf("|\n");
      }
      printf("    +");
      for (j=21; j>=0; --j) {
        printf("-");
      }
      printf("+\n");
      break;
    case LDISPLAY_LOOP:
      printf("LOOP\n");
      printf("  iterations: %d\n", frame->data.loop.repeat_count);
      break;
    case LDISPLAY_BRK_IF_LAST:
      printf("BRK_IF_LAST\n");
      break;
    case LDISPLAY_NOOP:
      printf("NOOP\n");
      printf("  duration: %d\n", frame->duration);
    default:
      printf("UNKNOWN (%d)\n", frame->type);
      break;
  }
  if (frame->next) {
    ldisplay_dump_frame(frame->next);
  }
}


ldisplay_frame_t *ldisplay_framedup(ldisplay_frame_t *frame) {
  if (!frame) {
    return NULL;
  }

  ldisplay_frame_t *ret;
  ret = malloc(sizeof(ldisplay_frame_t));
  if (!ret) {
    return NULL;
  }

  memcpy(ret, frame, sizeof(ldisplay_frame_t));

  return ret;
}

// free the given frame
void ldisplay_frame_free(ldisplay_frame_t *frame) {
  if (!frame) {
    return;
  }
  ldisplay_frame_t *next = frame->next;
  if (frame->type == LDISPLAY_LOOP) {
    if (frame->data.loop.queue) {
      ldisplay_queue_free(frame->data.loop.queue);
    }
  }
  free(frame);
  ldisplay_frame_free(next);
}

ldisplay_frame_t *ldisplay_frame_new(unsigned char type, uint16_t duration, unsigned char brightness) {
  ldisplay_frame_t *tmp = calloc(1, sizeof(ldisplay_frame_t));

  tmp->duration = duration;
  tmp->type = type;
  tmp->brightness = brightness;

  return tmp;
}

void ldisplay_frame_setBuffer(ldisplay_frame_t *frame, ldisplay_buffer_t buffer) {
  memcpy(frame->data.buffer, buffer, sizeof(ldisplay_buffer_t));
}


void ldisplay_noop(uint16_t duration, ldisplay_animq_t *queue) {
  ldisplay_frame_t *frame = ldisplay_frame_new(LDISPLAY_NOOP, duration, LDISPLAY_NOCHANGE);
  ldisplay_enqueue(frame, queue);
}

void ldisplay_setBrightness(unsigned char brightness, ldisplay_animq_t *queue) {
  ldisplay_frame_t *frame = ldisplay_frame_new(LDISPLAY_NOOP, 0, brightness);
  ldisplay_enqueue(frame, queue);
}

void ldisplay_reset(uint16_t duration, ldisplay_animq_t *queue) {
  ldisplay_frame_t *frame = ldisplay_frame_new(LDISPLAY_CLEAR, duration, LDISPLAY_NOCHANGE);
  ldisplay_enqueue(frame, queue);
}

void ldisplay_invert(uint16_t duration, ldisplay_animq_t *queue) {
  ldisplay_frame_t *frame = ldisplay_frame_new(LDISPLAY_INVERT, duration, LDISPLAY_NOCHANGE);
  ldisplay_enqueue(frame, queue);
}

void ldisplay_set(uint16_t duration, ldisplay_buffer_t buffer, unsigned char brightness, ldisplay_animq_t *queue) {
  ldisplay_frame_t *frame = ldisplay_frame_new(LDISPLAY_SET, duration, brightness);
  ldisplay_frame_setBuffer(frame, buffer);
  ldisplay_enqueue(frame, queue);
}
