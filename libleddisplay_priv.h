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
#ifndef LIBLEDDISPLAY_PRIV_H
#define LIBLEDDISPLAY_PRIV_H

#include <pthread.h>
#include "libleddisplay.h"

#ifndef LDISPLAY_NO_ANIM_DEFS
extern int die_anim_thread;

extern ldisplay_animq_t animq;
extern pthread_mutex_t  animq_mutex;

extern int             end_of_anim_queue;
extern pthread_mutex_t eoq_mutex;
extern pthread_cond_t  eoq_cond;
#endif

void _ldisplay_setBrightness(unsigned char brightness);
void _ldisplay_reset(void);
void _ldisplay_invert(void);
void _ldisplay_set(ldisplay_buffer_t data);

void anim_thread_init(void);
void anim_thread_join(void);

int _ldisplay_update(void);

#endif /* defined LIBLEDDISPLAY_PRIV_H */
