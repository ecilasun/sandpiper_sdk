/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _PXA_SDHCI_H_
#define _PXA_SDHCI_H_



#define PXA_FLAG_ENABLE_CLOCK_GATING (1<<0)

#define PXA_FLAG_CARD_PERMANENT	(1<<1)

#define PXA_FLAG_SD_8_BIT_CAPABLE_SLOT (1<<2)


struct sdhci_pxa_platdata {
	unsigned int	flags;
	unsigned int	clk_delay_cycles;
	unsigned int	clk_delay_sel;
	bool		clk_delay_enable;
	unsigned int	max_speed;
	u32		host_caps;
	u32		host_caps2;
	unsigned int	quirks;
	unsigned int	quirks2;
	unsigned int	pm_caps;
};
#endif 
