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

#include <stdint.h>

// Generic return codes
#define SUCCESS 0
#define ERR_INIT_NODEV 1

// Initialisation function to set up the connection to the device.
// MUST be called before any others.
// Returns 0 on success, 1 if device not found.
int init();

int reset();

int setAll(int val);
int setDisplay(uint32_t data[7]);

// Cleanup function to release the interface.
// MUST be called before exiting
void cleanup();

#endif /* defined LIBLEDDISPLAY_H */
