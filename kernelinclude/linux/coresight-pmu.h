/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_CORESIGHT_PMU_H
#define _LINUX_CORESIGHT_PMU_H

#include <linux/bits.h>

#define CORESIGHT_ETM_PMU_NAME "cs_etm"


#define CORESIGHT_LEGACY_CPU_TRACE_ID(cpu)  (0x10 + (cpu * 2))


#define ETM_OPT_BRANCH_BROADCAST 8
#define ETM_OPT_CYCACC		12
#define ETM_OPT_CTXTID		14
#define ETM_OPT_CTXTID2		15
#define ETM_OPT_TS		28
#define ETM_OPT_RETSTK		29


#define ETM4_CFG_BIT_BB         3
#define ETM4_CFG_BIT_CYCACC	4
#define ETM4_CFG_BIT_CTXTID	6
#define ETM4_CFG_BIT_VMID	7
#define ETM4_CFG_BIT_TS		11
#define ETM4_CFG_BIT_RETSTK	12
#define ETM4_CFG_BIT_VMID_OPT	15


#define CS_AUX_HW_ID_TRACE_ID_MASK	GENMASK_ULL(7, 0)
#define CS_AUX_HW_ID_VERSION_MASK	GENMASK_ULL(63, 60)

#define CS_AUX_HW_ID_CURR_VERSION 0

#endif
