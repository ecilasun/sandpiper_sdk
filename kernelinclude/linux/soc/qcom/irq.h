/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __QCOM_IRQ_H
#define __QCOM_IRQ_H

#include <linux/irqdomain.h>

#define GPIO_NO_WAKE_IRQ	~0U


#define IRQ_DOMAIN_FLAG_QCOM_PDC_WAKEUP		(IRQ_DOMAIN_FLAG_NONCORE << 0)
#define IRQ_DOMAIN_FLAG_QCOM_MPM_WAKEUP		(IRQ_DOMAIN_FLAG_NONCORE << 1)


static inline bool irq_domain_qcom_handle_wakeup(const struct irq_domain *d)
{
	return (d->flags & IRQ_DOMAIN_FLAG_QCOM_PDC_WAKEUP);
}

#endif
