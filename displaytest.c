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
#include "libleddisplay.h"

#include "binconst.h"

// ugh; ugly solution
int canprint=0;

void rprintf(char *msg) {
  if (canprint) printf(msg);
}

// TODO: fix to use getopt library

int main(int argc, char *argv[]) {
  // no output buffering please
	setbuf(stdout,0);

  // no output if we are just performing an action
  canprint=(argc<2);

  rprintf("libleddisplay: LED Display Linux Showcase program\n");
  rprintf("Written by Dave Ingram\n");
  rprintf("---------------------------------------------------\n");
  rprintf("  http://www.dmi.me.uk/code/linux/leddisplay/      \n\n");

  // initialise device
	if (init() != SUCCESS) {
    fprintf(stderr, "\033[1;31mDevice failed to initialise!\033[0m\n");
    return 1;
  }

  rprintf("Connected to the display\n");
  rprintf("Resetting...\n");

  // only delay if required
  if (canprint)
    usleep(10000);

  // reset it to a known initial state
  reset();

  if (argc<2) {
    uint32_t i;

    uint32_t pattern[7] = {
      B32(0, 00000, 00000000, 00000000), // 0x000000, // 000000000000000000000
      B32(0, 00000, 00001110, 01111101), // 0x0007ba, // 000000000000000000000
      B32(0, 00001, 10000001, 00001000), // 0x018108, // 000000000000000000000
      B32(0, 00110, 01000110, 00001000), // 0x064308, // 000000000000000000000
      B32(0, 01000, 00100001, 00010000), // 0x082090, // 000000000000000000000
      B32(0, 11111, 11100001, 00010000), // 0x1fe090, // 100000000000000000000
      B32(0, 00000, 00001111, 00100000)  // 0x000720  // 000000000000000000000
    };

    uint32_t frame[10][7] = {
      {
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100)
      },

      {
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010)
      },

      {
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001)
      },

      {
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100)
      },

      {
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010)
      },

      {
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001)
      },

      {
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100),
        B24(00100, 10010010, 01001001),
        B24(01001, 00100100, 10010010),
        B24(10010, 01001001, 00100100)
      },

      {
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001)
      },

      {
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110),
        B24(01110, 01111110, 01111110)
      },

      {
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001),
        B24(10001, 10000001, 10000001)
      }

    };

    /*
    printf("All on...\n");
    for (i=0; i<5; i++) {
      setAll(1);
      usleep(400000);
    }

    while (1) {
      setDisplay(pattern);
      usleep(400000);
    }
    */

    printf("Animate...\n");
    while (1) {
      for (i=0; i<10; i++) {
        setDisplay(frame[i]);
        usleep(400000);
        setDisplay(frame[i]);
        usleep(400000);
      }
    }

    usleep(500000);

    reset();

  } else {
    /*
    if        (strcmp(argv[1], "up")==0) {
      up();
    } else if (strcmp(argv[1], "down")==0) {
      down();
    } else if (strcmp(argv[1], "left")==0) {
      left();
    } else if (strcmp(argv[1], "right")==0) {
      right();
    } else if (strcmp(argv[1], "prime")==0) {
      prime();
    } else if (strcmp(argv[1], "fire")==0) {
      fire();
    } else {
      reset();
    }
    */
  }

  rprintf("Cleaning up...\n");

	cleanup();

  rprintf("Done.\n");

	return 0;
}
