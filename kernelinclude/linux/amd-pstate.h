/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LINUX_AMD_PSTATE_H
#define _LINUX_AMD_PSTATE_H

#include <linux/pm_qos.h>

#define AMD_CPPC_EPP_PERFORMANCE		0x00
#define AMD_CPPC_EPP_BALANCE_PERFORMANCE	0x80
#define AMD_CPPC_EPP_BALANCE_POWERSAVE		0xBF
#define AMD_CPPC_EPP_POWERSAVE			0xFF



struct amd_aperf_mperf {
	u64 aperf;
	u64 mperf;
	u64 tsc;
};


struct amd_cpudata {
	int	cpu;

	struct	freq_qos_request req[2];
	u64	cppc_req_cached;

	u32	highest_perf;
	u32	nominal_perf;
	u32	lowest_nonlinear_perf;
	u32	lowest_perf;
	u32     min_limit_perf;
	u32     max_limit_perf;
	u32     min_limit_freq;
	u32     max_limit_freq;

	u32	max_freq;
	u32	min_freq;
	u32	nominal_freq;
	u32	lowest_nonlinear_freq;

	struct amd_aperf_mperf cur;
	struct amd_aperf_mperf prev;

	u64	freq;
	bool	boost_supported;

	
	s16	epp_policy;
	s16	epp_cached;
	u32	policy;
	u64	cppc_cap1_cached;
	bool	suspended;
};


enum amd_pstate_mode {
	AMD_PSTATE_UNDEFINED = 0,
	AMD_PSTATE_DISABLE,
	AMD_PSTATE_PASSIVE,
	AMD_PSTATE_ACTIVE,
	AMD_PSTATE_GUIDED,
	AMD_PSTATE_MAX,
};

static const char * const amd_pstate_mode_string[] = {
	[AMD_PSTATE_UNDEFINED]   = "undefined",
	[AMD_PSTATE_DISABLE]     = "disable",
	[AMD_PSTATE_PASSIVE]     = "passive",
	[AMD_PSTATE_ACTIVE]      = "active",
	[AMD_PSTATE_GUIDED]      = "guided",
	NULL,
};
#endif 
