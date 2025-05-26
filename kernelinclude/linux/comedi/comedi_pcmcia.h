/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _COMEDI_PCMCIA_H
#define _COMEDI_PCMCIA_H

#include <pcmcia/cistpl.h>
#include <pcmcia/ds.h>
#include <linux/comedi/comedidev.h>

struct pcmcia_device *comedi_to_pcmcia_dev(struct comedi_device *dev);

int comedi_pcmcia_enable(struct comedi_device *dev,
			 int (*conf_check)(struct pcmcia_device *p_dev,
					   void *priv_data));
void comedi_pcmcia_disable(struct comedi_device *dev);

int comedi_pcmcia_auto_config(struct pcmcia_device *link,
			      struct comedi_driver *driver);
void comedi_pcmcia_auto_unconfig(struct pcmcia_device *link);

int comedi_pcmcia_driver_register(struct comedi_driver *comedi_driver,
				  struct pcmcia_driver *pcmcia_driver);
void comedi_pcmcia_driver_unregister(struct comedi_driver *comedi_driver,
				     struct pcmcia_driver *pcmcia_driver);


#define module_comedi_pcmcia_driver(__comedi_driver, __pcmcia_driver) \
	module_driver(__comedi_driver, comedi_pcmcia_driver_register, \
			comedi_pcmcia_driver_unregister, &(__pcmcia_driver))

#endif 
