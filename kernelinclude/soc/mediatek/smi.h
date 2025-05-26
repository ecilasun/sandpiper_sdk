/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MTK_IOMMU_SMI_H
#define MTK_IOMMU_SMI_H

#include <linux/bitops.h>
#include <linux/device.h>

#if IS_ENABLED(CONFIG_MTK_SMI)

enum iommu_atf_cmd {
	IOMMU_ATF_CMD_CONFIG_SMI_LARB,		
	IOMMU_ATF_CMD_CONFIG_INFRA_IOMMU,	
	IOMMU_ATF_CMD_MAX,
};

#define MTK_SMI_MMU_EN(port)	BIT(port)

struct mtk_smi_larb_iommu {
	struct device *dev;
	unsigned int   mmu;
	unsigned char  bank[32];
};

#endif

#endif
