/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_IRQ_SIM_H
#define _LINUX_IRQ_SIM_H

#include <linux/device.h>
#include <linux/fwnode.h>
#include <linux/irqdomain.h>



struct irq_domain *irq_domain_create_sim(struct fwnode_handle *fwnode,
					 unsigned int num_irqs);
struct irq_domain *devm_irq_domain_create_sim(struct device *dev,
					      struct fwnode_handle *fwnode,
					      unsigned int num_irqs);
void irq_domain_remove_sim(struct irq_domain *domain);

#endif 
