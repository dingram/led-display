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
#include "libleddisplay.h"

// fonts
#include "led_font_time.h"
#include "led_font_std.h"

// USB Vendor and Product IDs (obtained via lsusb)
#define DEVICE_VID 0x1d34
#define DEVICE_PID 0x0013

// USB device info
static usb_dev_handle *udev;

// display brightness: 0-2, with 2 being brightest
static unsigned char _brightness = LDISPLAY_BRIGHT;

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

static inline void _overlay(const uint32_t *foreground, uint32_t background[7], char xOff, char yOff) {
  int i;
  // index bounds checking
  if (yOff<-6 || yOff>6 || xOff<-20 || xOff>20) return;

  if (xOff<0) {
    xOff = -xOff;
    for (i= (yOff>0) ? yOff : 0; i<7; ++i) {
      if (i-yOff < 0 || i-yOff>6) continue;
      background[i] |= (uint32_t)(foreground[i-yOff] << xOff);
    }
  } else {
    for (i= (yOff>0) ? yOff : 0; i<7; ++i) {
      if (i-yOff < 0 || i-yOff>6) continue;
      background[i] |= (uint32_t)(foreground[i-yOff] >> xOff);
    }
  }
}

void ldisplay__test__overlay() {
  uint32_t buffer[7] = {};

  CLEAR_BUFFER(buffer);
  ldisplay_dumpBuffer(buffer);

  uint32_t pat1[7] = {
    B8(10),
    B8(10),
    B8(11),
    0,
    0,
    0,
    0
  };

  uint32_t pat2[7] = {
    B8(111),
    B8(100),
    0,
    0,
    0,
    0,
    0
  };

  _overlay(pat1, buffer, 0, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat1, buffer, -5, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat1, buffer, -10, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat2, buffer, -15, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat2, buffer, -20, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat2, buffer, -40, 0);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat2, buffer, 0, 4);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat1, buffer, -10, 5);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat1, buffer, -3, -1);
  ldisplay_dumpBuffer(buffer);

  _overlay(pat1, buffer, -8, -2);
  ldisplay_dumpBuffer(buffer);

  return;
}

/*******************************************************************************/


// attempt to open the first device matching the VID/PID we have
// assignes a usb_dev_handle to the device to the udev global var
int ldisplay_init() {
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

          // done
					return SUCCESS;
				} else {
					usb_close(tdev);
				}
			}
		}
	}

	return ERR_INIT_NODEV;
}

int ldisplay_reset() {
  return ldisplay_setAll(0);
}

int ldisplay_setAll(int val) {
  char msg[] = { _brightness, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  if (!val) {
    int i;
    for (i=2; i<8; ++i)
      msg[i] |= 0xff;
  }
  for (msg[1]=0; msg[1]<7; msg[1]+=2) {
    int ret;
    if (ret = _control_msg(msg, 8) < 0) {
      return ret;
    }
  }

  return SUCCESS;
}

int ldisplay_setDisplay(uint32_t data[7]) {
  unsigned char msg[] = { _brightness, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  for (msg[1]=0; msg[1]<7; msg[1]+=2) {
    int i=0;
    msg[4] = ~((data[msg[1]  ] & 0x00ffffff) >> 0x0d);
    msg[3] = ~((data[msg[1]  ] & 0x0000ffff) >> 0x05);
    msg[2] = ~((data[msg[1]  ] & 0x000000ff) << 0x03);
    if (msg[1]<6) {
      msg[7] = ~((data[msg[1]+1] & 0x00ffffff) >> 0x0d);
      msg[6] = ~((data[msg[1]+1] & 0x0000ffff) >> 0x05);
      msg[5] = ~((data[msg[1]+1] & 0x000000ff) << 0x03) | 0x07;
    } else {
      msg[5] = 0;
      msg[6] = 0;
      msg[7] = 0;
    }
    for (i=2; i<8; ++i) {
      msg[i] = _swapbits(msg[i]);
    }

    int ret;
    if (ret = _control_msg((char*)msg, 8) < 0) {
      return ret;
    }
  }

  return SUCCESS;
}

int ldisplay_showTime(unsigned int time, int style) {
  if (time>9999 || style<0 || style>1) {
    return ERR_BAD_ARGS;
  }

  uint32_t buffer[7] = {0};

  _overlay(time_font_colon, buffer, 0, 0);

  if (style) {
    _overlay(time_segment_font_digits[(time     )%10], buffer,   0, 0);
    _overlay(time_segment_font_digits[(time/10  )%10], buffer, - 5, 0);
    _overlay(time_segment_font_digits[(time/100 )%10], buffer, -12, 0);
    _overlay(time_segment_font_digits[(time/1000)%10], buffer, -17, 0);
  } else {
    _overlay(time_font_digits[(time     )%10], buffer,   0, 0);
    _overlay(time_font_digits[(time/10  )%10], buffer, - 5, 0);
    _overlay(time_font_digits[(time/100 )%10], buffer, -12, 0);
    _overlay(time_font_digits[(time/1000)%10], buffer, -17, 0);
  }

  return ldisplay_setDisplay(buffer);
}

void ldisplay_setBrightness(unsigned char brightness) {
  if (brightness>2)
    brightness=2;
  _brightness = brightness;
}

int ldisplay_showChars(const char chars[5], char offset) {
  uint32_t buffer[7] = {0};

  _overlay(font_std_fixed_ascii[chars[0]], buffer, offset - 21, 0);
  _overlay(font_std_fixed_ascii[chars[1]], buffer, offset - 16, 0);
  _overlay(font_std_fixed_ascii[chars[2]], buffer, offset - 11, 0);
  _overlay(font_std_fixed_ascii[chars[3]], buffer, offset -  6, 0);
  //_overlay(font_std_fixed_ascii[chars[4]], buffer, offset);

  return ldisplay_setDisplay(buffer);
}

// clean up after finishing with the device
void ldisplay_cleanup() {
	if (!udev)
    return; // nothing to do

	// release the interface
	usb_release_interface(udev,0);

	// close the device
  usb_close(udev);
}

void ldisplay_dumpBuffer(uint32_t data[7]) {
  int i, j;

  printf("\n");
  for (i=0; i<7; ++i) {
    for (j=21; j>=0; --j) {
      printf( ((data[i] >> j) & 0x1) ? "#" : "-" );
    }
    printf("\n");
  }
  printf("\n");
}
