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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <getopt.h>
#include "libleddisplay.h"

static void sighandler(int sig) {
  // shut down cleanly
  printf("Cleaning up...\n");
  //ldisplay_reset(0);
	ldisplay_cleanup();
  exit(EXIT_SUCCESS);
}

static void usage(char *progname) {
  printf("Usage:\n");
  printf("    %s [--no-totally-unnecessary-brightness-flicker]\n\n", progname);
  printf("  -s, --no-totally-unnecessary-brightness-flicker  Disable brightness flicker\n");
  printf("                                                   when updating the time.\n");
  printf("  --help                                           Display this screen.\n");
  printf("\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  // parse arguments
  int opt_flicker = 1;
  int ret;

  {
    struct option long_options[] = {
      { "no-totally-unnecessary-brightness-flicker", 0, &opt_flicker, 0},
      { "disable-totally-unnecessary-brightness-flicker", 0, &opt_flicker, 0},
      { "help", 0, 0, 0},
      { 0, 0, 0, 0 }
    };

    while (1) {
      int opt_index=0;
      int c = getopt_long(argc, argv, "s", long_options, &opt_index);

      if (c == -1)
        break;

      switch (c) {
        case 0:
          if (opt_index==2) {
            usage(argv[0]);
          }
          break;
        case 's':
          opt_flicker=0;
          break;
        case '?':
          exit(EXIT_FAILURE);
        default:
          printf("??? getopt returned character code 0x%x ???\n", c);
      }
    }
  }

  // no output buffering please
	setbuf(stdout,0);

  printf("LED Display: clock program\n");
  printf("Written by Dave Ingram\n");
  printf("-----------------------------------------------\n");
  printf("  http://www.dmi.me.uk/code/linux/leddisplay/  \n\n");

  // initialise device
	if (ldisplay_init() != SUCCESS) {
    fprintf(stderr, "\033[1;31mDevice failed to initialise!\033[0m\n");
    return 1;
  }

  struct sigaction sigact = {
    .sa_handler = sighandler
  };
  // prepare for signals
  sigaction(SIGHUP, &sigact, NULL);
  sigaction(SIGINT, &sigact, NULL);

  // reset it to a known initial state
  ldisplay_reset(0);

  int oldtime_int=0;
  ldisplay_buffer_t buffer = {0};
  struct timespec ts = {1, 0};
  while (1) {
    time_t t = time(NULL);
    struct tm *curTime = localtime(&t);
    int curtime_int = (100*curTime->tm_hour) + curTime->tm_min;
    // check time again every few seconds
    int i=0;

    if (oldtime_int != curtime_int) {
      ldisplay_drawTime(buffer, curtime_int, 0);

      if (opt_flicker)
        ldisplay_set(100, buffer, LDISPLAY_BRIGHT);
      ldisplay_set(100, buffer, LDISPLAY_DIM);
      oldtime_int = curtime_int;
    }
    nanosleep(&ts, NULL);
  }

  //ldisplay_reset();

	ldisplay_cleanup();

	return 0;
}
