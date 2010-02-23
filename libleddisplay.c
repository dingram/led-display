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

#include <usb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include "libleddisplay.h"
#include "libleddisplay_priv.h"

// USB Vendor and Product IDs (obtained via lsusb)
#define DEVICE_VID 0x1d34
#define DEVICE_PID 0x0013

// USB device info
static usb_dev_handle *udev;

// display brightness: 0-2, with 2 being brightest
static unsigned char _brightness = LDISPLAY_BRIGHT;

// current buffer
static ldisplay_buffer_t _buffer;

/******************************************************************************/

// internal USB functions

// send a control message to the device
static inline int _control_msg(char message[], int length) {
  // details from sniffing USB traffic:
  //   request type: 0x21
  //   request: 0x09
  //   index: 0x0200
  //   value: 0x0000
	return usb_control_msg(udev, 0x21, 0x09, 0x0200, 0x0000, message, length, 1000);
}

/*******************************************************************************/

static inline uint32_t _swapbits( uint32_t v )
{
  const uint32_t h_mask_1 = 0xaaaaaaaaUL;
  const uint32_t l_mask_1 = 0x55555555UL;

  const uint32_t h_mask_2 = 0xccccccccUL;
  const uint32_t l_mask_2 = 0x33333333UL;

  const uint32_t h_mask_4 = 0xf0f0f0f0UL;
  const uint32_t l_mask_4 = 0x0f0f0f0fUL;

  v =    ( ( v & h_mask_1 ) >> 1 ) | ( ( v & l_mask_1 ) << 1 );
  v =    ( ( v & h_mask_2 ) >> 2 ) | ( ( v & l_mask_2 ) << 2 );
  return ( ( v & h_mask_4 ) >> 4 ) | ( ( v & l_mask_4 ) << 4 );
}

/*******************************************************************************/

void _ldisplay_setBrightness(unsigned char brightness) {
  if (brightness != LDISPLAY_NOCHANGE) {
    if (brightness > LDISPLAY_BRIGHT) {
      _brightness = LDISPLAY_BRIGHT;
    } else {
      _brightness = brightness;
    }
  }
}

void _ldisplay_reset(void) {
  int i=0;

  for (i=0; i<7; ++i) {
    _buffer[i] = 0;
  }
}

void _ldisplay_invert(void) {
  int i=0;

  for (i=0; i<7; ++i) {
    _buffer[i] ^= 0xffffffff;
  }
}

void _ldisplay_set(ldisplay_buffer_t data) {
  int i=0;

  for (i=0; i<7; ++i) {
    _buffer[i] = data[i];
  }
}


// attempt to open the first device matching the VID/PID we have
// assignes a usb_dev_handle to the device to the udev global var
int ldisplay_init() {
#ifndef NODEV
  struct usb_bus *bus;
  struct usb_device *dev;

	usb_init();
	usb_set_debug(0);
	usb_find_busses();
	usb_find_devices();

	// scan the available busses
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		// scan the devices on this bus
		for (dev = bus->devices; dev; dev = dev->next) {
			// Try to open the device.
      usb_dev_handle *tdev = usb_open(dev);

			if (tdev) {
				if ((dev->descriptor.idVendor == DEVICE_VID) && (dev->descriptor.idProduct == DEVICE_PID)) {
					// save the device
					udev=tdev;

					// detach from the kernel if we need to
          char dname[32]={0};
					int retval = usb_get_driver_np(udev, 0, dname, 31);
					if (retval == 0 && strcmp(dname, "usbhid") == 0) {
						usb_detach_kernel_driver_np(udev, 0);
					}
					// Set configuration 1,interface 0,altinterface 0.
					usb_set_configuration(udev, 1);
					usleep(100);
					usb_claim_interface(udev, 0);

#else
  printf("\033[H\033[2J");
#endif
          anim_thread_init();

          // done
					return SUCCESS;
#ifndef NODEV
				} else {
					usb_close(tdev);
				}
			}
		}
	}

	return ERR_INIT_NODEV;
#endif
}

static int _ldisplay_update_hw(void) {
  unsigned char msg[] = { _brightness, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  for (msg[1]=0; msg[1]<7; msg[1]+=2) {
    int i=0;
    msg[4] = ~((_buffer[msg[1]  ] & 0x00ffffff) >> 0x0d);
    msg[3] = ~((_buffer[msg[1]  ] & 0x0000ffff) >> 0x05);
    msg[2] = ~((_buffer[msg[1]  ] & 0x000000ff) << 0x03);
    if (msg[1]<6) {
      msg[7] = ~((_buffer[msg[1]+1] & 0x00ffffff) >> 0x0d);
      msg[6] = ~((_buffer[msg[1]+1] & 0x0000ffff) >> 0x05);
      msg[5] = ~((_buffer[msg[1]+1] & 0x000000ff) << 0x03) | 0x07;
    } else {
      msg[5] = 0;
      msg[6] = 0;
      msg[7] = 0;
    }
    for (i=2; i<8; ++i) {
      msg[i] = _swapbits(msg[i]);
    }

    int ret;
    if ((ret = _control_msg((char*)msg, 8)) < 0) {
      return ret;
    }
  }

  return SUCCESS;
}

static int _ldisplay_update_sim(void) {
  int i, j;
  char o;

  switch (_brightness) {
    case LDISPLAY_DIM   : o='o'; break;
    case LDISPLAY_MEDIUM: o='*'; break;
    case LDISPLAY_BRIGHT: o='#'; break;
    default:              o='@'; break;
  }

  printf("\033[H");
  for (i=0; i<7; ++i) {
    for (j=21; j>=0; --j) {
      printf( "%c", ((_buffer[i] >> j) & 0x1) ? o : ' ' );
    }
    printf("|\n");
  }
  for (j=21; j>=0; --j) {
    printf("-");
  }
  printf("+\n");

  static uint16_t updcount=0;
  printf("%d\n", ++updcount);

  return SUCCESS;
}

int _ldisplay_update(void) {
  if (udev) {
    return _ldisplay_update_hw();
  } else {
    return _ldisplay_update_sim();
  }
}




// clean up after finishing with the device
void ldisplay_cleanup() {
	if (!udev)
    return; // nothing to do

  anim_thread_join();

	// release the interface
	usb_release_interface(udev,0);

	// close the device
  usb_close(udev);
}
