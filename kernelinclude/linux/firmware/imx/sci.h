/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _SC_SCI_H
#define _SC_SCI_H

#include <linux/firmware/imx/ipc.h>

#include <linux/firmware/imx/svc/misc.h>
#include <linux/firmware/imx/svc/pm.h>
#include <linux/firmware/imx/svc/rm.h>

#if IS_ENABLED(CONFIG_IMX_SCU)
int imx_scu_enable_general_irq_channel(struct device *dev);
int imx_scu_irq_register_notifier(struct notifier_block *nb);
int imx_scu_irq_unregister_notifier(struct notifier_block *nb);
int imx_scu_irq_group_enable(u8 group, u32 mask, u8 enable);
int imx_scu_irq_get_status(u8 group, u32 *irq_status);
int imx_scu_soc_init(struct device *dev);
#else
static inline int imx_scu_soc_init(struct device *dev)
{
	return -EOPNOTSUPP;
}

static inline int imx_scu_enable_general_irq_channel(struct device *dev)
{
	return -EOPNOTSUPP;
}

static inline int imx_scu_irq_register_notifier(struct notifier_block *nb)
{
	return -EOPNOTSUPP;
}

static inline int imx_scu_irq_unregister_notifier(struct notifier_block *nb)
{
	return -EOPNOTSUPP;
}

static inline int imx_scu_irq_group_enable(u8 group, u32 mask, u8 enable)
{
	return -EOPNOTSUPP;
}

static inline int imx_scu_irq_get_status(u8 group, u32 *irq_status)
{
	return -EOPNOTSUPP;
}
#endif
#endif 
