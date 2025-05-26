/* SPDX-License-Identifier: GPL-2.0 */
#ifndef MFD_INTEL_PMC_BXT_H
#define MFD_INTEL_PMC_BXT_H


#define PMC_GCR_PMC_CFG_REG		0x08
#define PMC_GCR_TELEM_DEEP_S0IX_REG	0x78
#define PMC_GCR_TELEM_SHLW_S0IX_REG	0x80


#define PMC_CFG_NO_REBOOT_EN		BIT(4)


struct intel_pmc_dev {
	struct device *dev;
	struct intel_scu_ipc_dev *scu;
	void __iomem *gcr_mem_base;
	spinlock_t gcr_lock;
	struct resource *telem_base;
};

#if IS_ENABLED(CONFIG_MFD_INTEL_PMC_BXT)
int intel_pmc_gcr_read64(struct intel_pmc_dev *pmc, u32 offset, u64 *data);
int intel_pmc_gcr_update(struct intel_pmc_dev *pmc, u32 offset, u32 mask, u32 val);
int intel_pmc_s0ix_counter_read(struct intel_pmc_dev *pmc, u64 *data);
#else
static inline int intel_pmc_gcr_read64(struct intel_pmc_dev *pmc, u32 offset,
				       u64 *data)
{
	return -ENOTSUPP;
}

static inline int intel_pmc_gcr_update(struct intel_pmc_dev *pmc, u32 offset,
				       u32 mask, u32 val)
{
	return -ENOTSUPP;
}

static inline int intel_pmc_s0ix_counter_read(struct intel_pmc_dev *pmc, u64 *data)
{
	return -ENOTSUPP;
}
#endif

#endif 
