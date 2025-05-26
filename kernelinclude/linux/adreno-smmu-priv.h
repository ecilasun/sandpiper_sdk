// SPDX-License-Identifier: GPL-2.0-only


#ifndef __ADRENO_SMMU_PRIV_H
#define __ADRENO_SMMU_PRIV_H

#include <linux/io-pgtable.h>


struct adreno_smmu_fault_info {
	u64 far;
	u64 ttbr0;
	u32 contextidr;
	u32 fsr;
	u32 fsynr0;
	u32 fsynr1;
	u32 cbfrsynra;
};


struct adreno_smmu_priv {
    const void *cookie;
    const struct io_pgtable_cfg *(*get_ttbr1_cfg)(const void *cookie);
    int (*set_ttbr0_cfg)(const void *cookie, const struct io_pgtable_cfg *cfg);
    void (*get_fault_info)(const void *cookie, struct adreno_smmu_fault_info *info);
    void (*set_stall)(const void *cookie, bool enabled);
    void (*resume_translation)(const void *cookie, bool terminate);
};

#endif 
