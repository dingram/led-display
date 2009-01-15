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
	if (ldisplay_init() != SUCCESS) {
    fprintf(stderr, "\033[1;31mDevice failed to initialise!\033[0m\n");
    return 1;
  }

  rprintf("Connected to the display\n");
  rprintf("Resetting...\n");

  // only delay if required
  if (canprint)
    usleep(10000);

  // reset it to a known initial state
  ldisplay_reset();

  if (argc<2) {
    int32_t i;

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
      }

    };

    /*
    printf("All on...\n");
    for (i=0; i<5; i++) {
      setAll(1);
      usleep(400000);
    }
    */

    if (1) {
      ldisplay__test__overlay();
      exit(0);
    }

    if (1) {
      char *message[] = {
        "     ",
        "    I",
        "   In",
        "  Ini",
        " Init",
        "Initi",
        "nitia",
        "itial",
        "tiali",
        "ialis",
        "alisi",
        "lisin",
        "ising",
        "sing.",
        "ing..",
        "ng...",
        "g... ",
        "...  ",
        "..   ",
        ".    ",
        "    #",
        "   #@",
        "  #@$",
        " #@$%",
        "#@$%&",
        "@$%&=",
        "$%&= ",
        "%&=  ",
        "&=   ",
        "=    ",
        "     "
      };

      int m=0;
      for (m=0; m<31; m++) {
        ldisplay_showChars(message[m],  6); usleep(100000);
        ldisplay_showChars(message[m],  5); usleep(100000);
        ldisplay_showChars(message[m],  4); usleep(100000);
        ldisplay_showChars(message[m],  3); usleep(100000);
        ldisplay_showChars(message[m],  2); usleep(100000);
      }
    }

#if 0
    ldisplay_setBrightness(LDISPLAY_BRIGHT);
    for (i=10; i>=0; --i) {
      ldisplay_showTime(i, (i/10)%2);
      usleep(400000);
      ldisplay_showTime(i, (i/10)%2);
      usleep(400000);
    }

    int j=2;

    ldisplay_setBrightness(LDISPLAY_MEDIUM);
    printf("Animate...\n");
    while (j) {
      for (i=0; i<6; i++) {
        ldisplay_setDisplay(frame[i]);
        usleep(150000);
        /*
        setDisplay(frame[i]);
        usleep(400000);
        */
      }
      --j;
    }
#endif

    if (0) {
      ldisplay_setBrightness(LDISPLAY_DIM);
      printf("Current time (dim)\n");
      int oldtime_int=0;
      while (1) {
        time_t t = time(NULL);
        struct tm *curTime = localtime(&t);
        int curtime_int = (100*curTime->tm_hour) + curTime->tm_min;
        // check time again every few seconds
        for (i=0; i<5; ++i) {
          if (oldtime_int!=curtime_int)
            ldisplay_setBrightness(LDISPLAY_BRIGHT);
          ldisplay_showTime(curtime_int, 0);
          usleep(100000);
          if (oldtime_int!=curtime_int) {
            oldtime_int = curtime_int;
            ldisplay_setBrightness(LDISPLAY_DIM);
          }
          ldisplay_showTime(curtime_int, 0);
          usleep(300000);
        }
      }
    }

    usleep(500000);

    ldisplay_reset();

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

	ldisplay_cleanup();

  rprintf("Done.\n");

	return 0;
}
