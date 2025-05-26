/* SPDX-License-Identifier: (GPL-2.0 OR Linux-OpenIB) OR BSD-2-Clause */


#ifndef _PDS_INTR_H_
#define _PDS_INTR_H_


struct pds_core_intr {
	u32 coal_init;
	u32 mask;
	u16 credits;
	u16 flags;
#define PDS_CORE_INTR_F_UNMASK		0x0001
#define PDS_CORE_INTR_F_TIMER_RESET	0x0002
	u32 mask_on_assert;
	u32 coalescing_curr;
	u32 rsvd6[3];
};

#ifndef __CHECKER__
static_assert(sizeof(struct pds_core_intr) == 32);
#endif 

#define PDS_CORE_INTR_CTRL_REGS_MAX		2048
#define PDS_CORE_INTR_CTRL_COAL_MAX		0x3F
#define PDS_CORE_INTR_INDEX_NOT_ASSIGNED	-1

struct pds_core_intr_status {
	u32 status[2];
};


enum pds_core_intr_mask_vals {
	PDS_CORE_INTR_MASK_CLEAR	= 0,
	PDS_CORE_INTR_MASK_SET		= 1,
};


enum pds_core_intr_credits_bits {
	PDS_CORE_INTR_CRED_COUNT		= 0x7fffu,
	PDS_CORE_INTR_CRED_COUNT_SIGNED		= 0xffffu,
	PDS_CORE_INTR_CRED_UNMASK		= 0x10000u,
	PDS_CORE_INTR_CRED_RESET_COALESCE	= 0x20000u,
	PDS_CORE_INTR_CRED_REARM		= (PDS_CORE_INTR_CRED_UNMASK |
					   PDS_CORE_INTR_CRED_RESET_COALESCE),
};

static inline void
pds_core_intr_coal_init(struct pds_core_intr __iomem *intr_ctrl, u32 coal)
{
	iowrite32(coal, &intr_ctrl->coal_init);
}

static inline void
pds_core_intr_mask(struct pds_core_intr __iomem *intr_ctrl, u32 mask)
{
	iowrite32(mask, &intr_ctrl->mask);
}

static inline void
pds_core_intr_credits(struct pds_core_intr __iomem *intr_ctrl,
		      u32 cred, u32 flags)
{
	if (WARN_ON_ONCE(cred > PDS_CORE_INTR_CRED_COUNT)) {
		cred = ioread32(&intr_ctrl->credits);
		cred &= PDS_CORE_INTR_CRED_COUNT_SIGNED;
	}

	iowrite32(cred | flags, &intr_ctrl->credits);
}

static inline void
pds_core_intr_clean_flags(struct pds_core_intr __iomem *intr_ctrl, u32 flags)
{
	u32 cred;

	cred = ioread32(&intr_ctrl->credits);
	cred &= PDS_CORE_INTR_CRED_COUNT_SIGNED;
	cred |= flags;
	iowrite32(cred, &intr_ctrl->credits);
}

static inline void
pds_core_intr_clean(struct pds_core_intr __iomem *intr_ctrl)
{
	pds_core_intr_clean_flags(intr_ctrl, PDS_CORE_INTR_CRED_RESET_COALESCE);
}

static inline void
pds_core_intr_mask_assert(struct pds_core_intr __iomem *intr_ctrl, u32 mask)
{
	iowrite32(mask, &intr_ctrl->mask_on_assert);
}

#endif 
