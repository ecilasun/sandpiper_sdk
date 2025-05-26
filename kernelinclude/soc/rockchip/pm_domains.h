/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __SOC_ROCKCHIP_PM_DOMAINS_H__
#define __SOC_ROCKCHIP_PM_DOMAINS_H__

#ifdef CONFIG_ROCKCHIP_PM_DOMAINS

int rockchip_pmu_block(void);
void rockchip_pmu_unblock(void);

#else 

static inline int rockchip_pmu_block(void)
{
	return 0;
}

static inline void rockchip_pmu_unblock(void) { }

#endif 

#endif 
