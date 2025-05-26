/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_IRQCHIP_ARM_VGIC_INFO_H
#define __LINUX_IRQCHIP_ARM_VGIC_INFO_H

#include <linux/types.h>
#include <linux/ioport.h>

enum gic_type {
	
	GIC_V2,
	
	GIC_V3,
};

struct gic_kvm_info {
	
	enum gic_type	type;
	
	struct resource vcpu;
	
	unsigned int	maint_irq;
	
	bool		no_maint_irq_mask;
	
	struct resource vctrl;
	
	bool		has_v4;
	
	bool		has_v4_1;
	
	bool		no_hw_deactivation;
};

#ifdef CONFIG_KVM
void vgic_set_kvm_info(const struct gic_kvm_info *info);
#else
static inline void vgic_set_kvm_info(const struct gic_kvm_info *info) {}
#endif

#endif
