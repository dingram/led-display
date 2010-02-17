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
#include <pthread.h>
#include <errno.h>
#define LDISPLAY_NO_ANIM_DEFS
#include "libleddisplay_priv.h"

// animation thread
static pthread_t anim_thread;

// flag to kill animation
int die_anim_thread = 0;

ldisplay_animq_t animq;
pthread_mutex_t  animq_mutex;

int             end_of_anim_queue = 0;
pthread_mutex_t eoq_mutex;
pthread_cond_t  eoq_cond;

static pthread_once_t thread_init_once = PTHREAD_ONCE_INIT;


static ldisplay_frame_t *_ldisplay_dequeue(void) {
  ldisplay_frame_t *cur;
  (void)pthread_mutex_lock(&animq_mutex);
  if (!animq.first) {
    cur = NULL;
  } else {
    cur = animq.first;
    if (animq.first == animq.last) {
      animq.first = animq.last = NULL;
    } else {
      animq.first = animq.first->next;
    }
  }
  (void)pthread_mutex_unlock(&animq_mutex);
  return cur;
}

static void _animsleep(uint16_t ms, pthread_mutex_t *mutex, pthread_cond_t *cond) {
  struct timespec ts;
  int created_mutex = 0;
  int created_cond = 0;
  int rc = 0;

  if (!mutex) {
    created_mutex=1;
    mutex = calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);
    (void)pthread_mutex_lock(mutex);
  }
  if (!cond) {
    created_cond=1;
    cond = calloc(1, sizeof(pthread_cond_t));
    pthread_cond_init(cond, NULL);
  }

  do {
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += ms * 1e6; // ms -> ns
    if (ts.tv_nsec >= 1000000000L) {
      ++ts.tv_sec;
      ts.tv_nsec -= 1000000000L;
    }
    while (rc == 0) {
      rc = pthread_cond_timedwait(cond, mutex, &ts);
    }
    if (rc == ETIMEDOUT) {
      break;
    }
  } while (0/*rc == EINVAL*/);

  if (created_cond) {
    created_cond=0;
    pthread_cond_destroy(cond);
    free(cond);
  }
  if (created_mutex) {
    created_mutex=0;
    (void)pthread_mutex_unlock(mutex);
    pthread_mutex_destroy(mutex);
    free(mutex);
  }
}

static void _anim_frame_dispatch(ldisplay_frame_t *frame) {
  switch (frame->type) {
    case LDISPLAY_INVERT:
      // invert the internal buffer
      _ldisplay_setBrightness(frame->brightness);
      _ldisplay_invert();
      break;
    case LDISPLAY_CLEAR:
      // clear the internal buffer
      _ldisplay_setBrightness(frame->brightness);
      _ldisplay_reset();
      break;
    case LDISPLAY_SET:
      // copy to internal buffer
      _ldisplay_setBrightness(frame->brightness);
      _ldisplay_set(frame->data.buffer);
      break;
    case LDISPLAY_LOOP:
    case LDISPLAY_BRK_IF_LAST:
    case LDISPLAY_NOOP:
    default:
      // do nothing
      break;
  }
}

static void *anim_thread_func(void *arg) {
  pthread_mutex_t    mutex;
  pthread_cond_t     cond;
  ldisplay_frame_t  *cur_frame;

  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);

  uint16_t delay = MAX_FRAME_LENGTH_MS;
  while (!die_anim_thread) {
    delay = MAX_FRAME_LENGTH_MS;

    (void)pthread_mutex_lock(&mutex);

    cur_frame = _ldisplay_dequeue();
    if (cur_frame) {
      if (cur_frame->duration > MAX_FRAME_LENGTH_MS) {
        ldisplay_frame_t *tmp = ldisplay_framedup(cur_frame);
        tmp->duration -= MAX_FRAME_LENGTH_MS;
        ldisplay_queue_prepend(tmp, NULL);
      } else {
        delay = cur_frame->duration;
      }

      // handle frame
      _anim_frame_dispatch(cur_frame);

      // free the current frame
      free(cur_frame);
      cur_frame = NULL;

    } else {
      // end of animation queue
      pthread_mutex_lock(&eoq_mutex);
      if (!end_of_anim_queue) {
        end_of_anim_queue = 1;
        pthread_cond_broadcast(&eoq_cond);
      }
      pthread_mutex_unlock(&eoq_mutex);
    }

    // update hardware from buffer
    _ldisplay_update();

    // wait
    _animsleep(delay, &mutex, &cond);

    (void)pthread_mutex_unlock(&mutex);
  }

  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&mutex);

  return 0;
}

static void _anim_thread_init(void) {
  pthread_mutex_init(&eoq_mutex, NULL);
  pthread_cond_init(&eoq_cond, NULL);
  pthread_create(&anim_thread, NULL, &anim_thread_func, NULL);
}

void anim_thread_init(void) {
  pthread_once(&thread_init_once, _anim_thread_init);
}

void ldisplay_wait_for_anim() {
  pthread_mutex_lock(&eoq_mutex);
  while (!end_of_anim_queue) {
    pthread_cond_wait(&eoq_cond, &eoq_mutex);
  }
  pthread_mutex_unlock(&eoq_mutex);
}

void anim_thread_join(void) {
  die_anim_thread = 1;
  pthread_join(anim_thread, NULL);
}

