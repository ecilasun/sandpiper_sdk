/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_ARM_PSCI_H
#define __ASM_ARM_PSCI_H

extern const struct smp_operations psci_smp_ops;

#if defined(CONFIG_SMP) && defined(CONFIG_ARM_PSCI)
bool psci_smp_available(void);
#else
static inline bool psci_smp_available(void) { return false; }
#endif

#endif 
