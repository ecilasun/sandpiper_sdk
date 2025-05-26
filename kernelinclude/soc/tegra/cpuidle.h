/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SOC_TEGRA_CPUIDLE_H__
#define __SOC_TEGRA_CPUIDLE_H__

#ifdef CONFIG_ARM_TEGRA_CPUIDLE
void tegra_cpuidle_pcie_irqs_in_use(void);
#else
static inline void tegra_cpuidle_pcie_irqs_in_use(void)
{
}
#endif

#endif 
