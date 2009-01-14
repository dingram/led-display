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

// USB Vendor and Product IDs (obtained via lsusb)
#define DEVICE_VID 0x1d34
#define DEVICE_PID 0x0013

// USB device info
usb_dev_handle *udev;

/******************************************************************************/

// internal USB functions

// send a control message to the device
int _control_msg(char message[], int length) {
  // details from sniffing USB traffic:
  //   request type: 0x21
  //   request: 0x09
  //   index: 0x0200
  //   value: 0x0000
	return usb_control_msg(udev, 0x21, 0x09, 0x0200, 0x0000, message, length, 1000);
}

/*******************************************************************************/

static inline uint32_t swapbits( uint32_t v )
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


// attempt to open the device
// assignes a usb_dev_handle to the device to the udev global var
int init()
{
	int buscount;
	int devcount;
  struct usb_bus *bus;
	char dname[32]={0};
  struct usb_device *dev;

	usb_dev_handle *tdev;

	tdev = NULL;

	usb_init();
	usb_set_debug(0);
	usb_find_busses();
	usb_find_devices();

	buscount = 0;
	// scan the available busses
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		devcount = 0;
		// scan the devices on this bus
		for (dev = bus->devices; dev; dev = dev->next) {

			/*
			 * Try to open the device.
			 */
			tdev = usb_open(dev);

			if (tdev) {
				if ((dev->descriptor.idVendor == DEVICE_VID) &&
					(dev->descriptor.idProduct == DEVICE_PID)) {

					// save the device
					udev=tdev;

					// detach from the kernel if we need to
					int retval = usb_get_driver_np(udev, 0, dname, 31);
					if (retval == 0 && strcmp(dname, "usbhid") == 0) {
						usb_detach_kernel_driver_np(udev, 0);
					}
					// Set configuration 1,interface 0,altinterface 0.
					usb_set_configuration(udev, 1);
					usleep(100);
					usb_claim_interface(udev, 0);

					return SUCCESS;
				} else {
					usb_close(tdev);
				}
			}
		}
		buscount++;
	}

	return ERR_INIT_NODEV;
}

int reset() {
  return setAll(0);
}

int setAll(int val) {
  char msg[] = { 0x00, 0x02, 0x00, 0x00, 0x07, 0x00, 0x00, 0x07 };

  if (!val) {
    int i;
    for (i=2; i<8; i++)
      msg[i] |= 0xff;
  }
  for (msg[1]=0; msg[1]<7; msg[1]+=2) {
    _control_msg(msg, 8);
  }

  return SUCCESS;
}

int setDisplay(uint32_t data[7]) {
  unsigned char msg[] = { 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x07 };

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
    for (i=2; i<8; i++) {
      msg[i] = swapbits(msg[i]);
    }

    _control_msg((char*)msg, 8);
  }

  return SUCCESS;
}

/*
// move cannon in given direction
int move(char direction) {
  char msg[] = { (direction & 0x0f), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

  // naughty, naughty: can't move up and down at the same time!
  if ((direction & 0x03) == 0x03) msg[0] &= 0xfc;

  // naughty, naughty: can't move left and right at the same time!
  if ((direction & 0x0c) == 0x0c) msg[0] &= 0xf3;

  // move cannon
  _control_msg(msg, 8);

  return SUCCESS;
}

// fire the cannon
int fire() {
  char msg[]  = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  _control_msg(msg, 8);
  return SUCCESS;
}
*/

// clean up after finishing with the device
void cleanup() {
	// release the interface
	usb_release_interface(udev,0);

	// close the device
	if (udev)
		usb_close(udev);
}
