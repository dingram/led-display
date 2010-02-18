/*
 * Copyright (C) 2006 David Ingram
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
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "libleddisplay.h"

#include "binconst.h"

int main(int argc, char *argv[]) {
  ldisplay_buffer_t buf;
  ldisplay_animq_t *myq = ldisplay_queue_new();

  // initialise device
	if (ldisplay_init() != SUCCESS) {
    fprintf(stderr, "\033[1;31mDevice failed to initialise!\033[0m\n");
    return 1;
  }

  {
    ldisplay_buffer_clear(buf);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    ldisplay_drawTime(buf, 1234, 0);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    int i=0;
    for (i=0; i<=LDISPLAY_WIDTH; ++i) {
      ldisplay_buffer_scroll(buf, LDISPLAY_DIRECTION_LEFT, 1);
      ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    }
  }

  {
    ldisplay_buffer_clear(buf);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    ldisplay_drawTime(buf, 1234, 0);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    int i=0;
    for (i=0; i<=LDISPLAY_WIDTH; ++i) {
      ldisplay_buffer_scroll(buf, LDISPLAY_DIRECTION_RIGHT, 1);
      ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    }
  }

  {
    ldisplay_buffer_clear(buf);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    ldisplay_drawTime(buf, 1234, 0);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    int i=0;
    for (i=0; i<=LDISPLAY_HEIGHT; ++i) {
      ldisplay_buffer_scroll(buf, LDISPLAY_DIRECTION_UP, 1);
      ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    }
  }

  {
    ldisplay_buffer_clear(buf);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    ldisplay_drawTime(buf, 1234, 0);
    ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    int i=0;
    for (i=0; i<=LDISPLAY_HEIGHT; ++i) {
      ldisplay_buffer_scroll(buf, LDISPLAY_DIRECTION_DOWN, 1);
      ldisplay_set(100, buf, LDISPLAY_NOCHANGE, myq);
    }
  }

  //ldisplay_dump_queue(myq);
  ldisplay_queue_concat(NULL, myq);

  ldisplay_queue_free(myq);

  ldisplay_wait_for_anim();
	ldisplay_cleanup();

	return 0;
}
