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
#include <pthread.h>
#include "libleddisplay_priv.h"

void ldisplay_dump_queue(ldisplay_animq_t *queue) {
  if (queue->first) {
    ldisplay_dump_frame(queue->first);
  } else {
    printf("Empty queue\n");
  }
}

ldisplay_animq_t *ldisplay_queue_new() {
  ldisplay_animq_t *tmp = calloc(1, sizeof(ldisplay_animq_t));

  return tmp;
}

// free the given queue and all of its frames
void ldisplay_queue_free(ldisplay_animq_t *queue) {
  if (!queue->first) {
    // empty queue; nothing to free
    return;
  }
  // free the frame data
  ldisplay_frame_free(queue->first);
  queue->first = NULL;
  queue->last = NULL;
  // free the queue pointer itself
  free(queue);
}

// copy frames from "additional" onto end of "main"
void ldisplay_queue_dupconcat(ldisplay_animq_t *main, ldisplay_animq_t *additional) {
  if (!main) {
    (void)pthread_mutex_lock(&animq_mutex);
    ldisplay_queue_concat(&animq, additional);
    (void)pthread_mutex_unlock(&animq_mutex);
  } else {
    ldisplay_frame_t *curframe = additional->first;
    while (curframe) {
      ldisplay_frame_t *tmpframe = ldisplay_framedup(curframe);
      tmpframe->next = NULL;
      ldisplay_enqueue(tmpframe, main);
      curframe = curframe->next;
    }
    if (main->first && additional->first) {
      main->last->next = additional->first;
      main->last = additional->last;
    } else if (additional->first) {
      main->first = additional->first;
      main->last = additional->last;
    }
    additional->first = NULL;
    additional->last  = NULL;
  }
}

// move frames from "additional" onto end of "main"
void ldisplay_queue_concat(ldisplay_animq_t *main, ldisplay_animq_t *additional) {
  if (!main) {
    (void)pthread_mutex_lock(&animq_mutex);
    pthread_mutex_lock(&eoq_mutex);
    ldisplay_queue_concat(&animq, additional);
    end_of_anim_queue = 0;
    pthread_mutex_unlock(&eoq_mutex);
    (void)pthread_mutex_unlock(&animq_mutex);
  } else {
    if (main->first && additional->first) {
      main->last->next = additional->first;
      main->last = additional->last;
    } else if (additional->first) {
      main->first = additional->first;
      main->last = additional->last;
    }
    additional->first = NULL;
    additional->last  = NULL;
  }
}

void ldisplay_enqueue(ldisplay_frame_t *frame, ldisplay_animq_t *queue) {
  if (!queue) {
    (void)pthread_mutex_lock(&animq_mutex);
    pthread_mutex_lock(&eoq_mutex);
    ldisplay_enqueue(frame, &animq);
    end_of_anim_queue = 0;
    pthread_mutex_unlock(&eoq_mutex);
    (void)pthread_mutex_unlock(&animq_mutex);
  } else {
    if (queue->last) {
      queue->last->next = frame;
      queue->last = frame;
    } else {
      queue->first = frame;
      queue->last = frame;
    }
  }
}

void ldisplay_queue_prepend(ldisplay_frame_t *frame, ldisplay_animq_t *queue) {
  if (!queue) {
    (void)pthread_mutex_lock(&animq_mutex);
    pthread_mutex_lock(&eoq_mutex);
    ldisplay_queue_prepend(frame, &animq);
    end_of_anim_queue = 0;
    pthread_mutex_unlock(&eoq_mutex);
    (void)pthread_mutex_unlock(&animq_mutex);
  } else {
    if (queue->first) {
      frame->next = queue->first;
      queue->first = frame;
    } else {
      queue->first = frame;
      queue->last = frame;
    }
  }
}

void ldisplay_queue_reverse(ldisplay_animq_t *queue) {
  ldisplay_frame_t *last = NULL;
  ldisplay_frame_t *cur  = queue->first;
  while (cur) {
    ldisplay_frame_t *next = cur->next;
    cur->next = last;
    last = cur;
    cur = next;
  }
  queue->last = queue->first;
  queue->first = last;
}
