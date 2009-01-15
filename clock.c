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
#include "libleddisplay.h"

static void sighandler(int sig) {
  // shut down cleanly
  printf("Cleaning up...\n");
  ldisplay_reset();
	ldisplay_cleanup();
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
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
  ldisplay_reset();

  ldisplay_setBrightness(LDISPLAY_DIM);
  int oldtime_int=0;
  while (1) {
    time_t t = time(NULL);
    struct tm *curTime = localtime(&t);
    int curtime_int = (100*curTime->tm_hour) + curTime->tm_min;
    // check time again every few seconds
    int i=0;

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

  ldisplay_reset();

	ldisplay_cleanup();

	return 0;
}
