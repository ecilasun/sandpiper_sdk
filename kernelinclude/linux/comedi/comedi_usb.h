/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _COMEDI_USB_H
#define _COMEDI_USB_H

#include <linux/usb.h>
#include <linux/comedi/comedidev.h>

struct usb_interface *comedi_to_usb_interface(struct comedi_device *dev);
struct usb_device *comedi_to_usb_dev(struct comedi_device *dev);

int comedi_usb_auto_config(struct usb_interface *intf,
			   struct comedi_driver *driver, unsigned long context);
void comedi_usb_auto_unconfig(struct usb_interface *intf);

int comedi_usb_driver_register(struct comedi_driver *comedi_driver,
			       struct usb_driver *usb_driver);
void comedi_usb_driver_unregister(struct comedi_driver *comedi_driver,
				  struct usb_driver *usb_driver);


#define module_comedi_usb_driver(__comedi_driver, __usb_driver) \
	module_driver(__comedi_driver, comedi_usb_driver_register, \
			comedi_usb_driver_unregister, &(__usb_driver))

#endif 
