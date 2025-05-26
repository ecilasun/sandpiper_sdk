/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_PXA2XX_SSP_H
#define __LINUX_PXA2XX_SSP_H

#include <linux/bits.h>
#include <linux/compiler_types.h>
#include <linux/io.h>
#include <linux/kconfig.h>
#include <linux/list.h>
#include <linux/types.h>

struct clk;
struct device;
struct device_node;



#define SSCR0		(0x00)  
#define SSCR1		(0x04)  
#define SSSR		(0x08)  
#define SSITR		(0x0C)  
#define SSDR		(0x10)  

#define SSTO		(0x28)  
#define SSPSP		(0x2C)  
#define SSTSA		(0x30)  
#define SSRSA		(0x34)  
#define SSTSS		(0x38)  
#define SSACD		(0x3C)  
#define SSACDD		(0x40)	


#define SSCR0_DSS	GENMASK(3, 0)	
#define SSCR0_DataSize(x)  ((x) - 1)	
#define SSCR0_FRF	GENMASK(5, 4)	
#define SSCR0_Motorola	(0x0 << 4)	
#define SSCR0_TI	(0x1 << 4)	
#define SSCR0_National	(0x2 << 4)	
#define SSCR0_ECS	BIT(6)		
#define SSCR0_SSE	BIT(7)		
#define SSCR0_SCR(x)	((x) << 8)	


#define SSCR0_EDSS	BIT(20)		
#define SSCR0_NCS	BIT(21)		
#define SSCR0_RIM	BIT(22)		
#define SSCR0_TUM	BIT(23)		
#define SSCR0_FRDC	GENMASK(26, 24)	
#define SSCR0_SlotsPerFrm(x) (((x) - 1) << 24)	
#define SSCR0_FPCKE	BIT(29)		
#define SSCR0_ACS	BIT(30)		
#define SSCR0_MOD	BIT(31)		

#define SSCR1_RIE	BIT(0)		
#define SSCR1_TIE	BIT(1)		
#define SSCR1_LBM	BIT(2)		
#define SSCR1_SPO	BIT(3)		
#define SSCR1_SPH	BIT(4)		
#define SSCR1_MWDS	BIT(5)		

#define SSSR_ALT_FRM_MASK	GENMASK(1, 0)	
#define SSSR_TNF		BIT(2)		
#define SSSR_RNE		BIT(3)		
#define SSSR_BSY		BIT(4)		
#define SSSR_TFS		BIT(5)		
#define SSSR_RFS		BIT(6)		
#define SSSR_ROR		BIT(7)		

#define RX_THRESH_DFLT	8
#define TX_THRESH_DFLT	8

#define SSSR_TFL_MASK	GENMASK(11, 8)	
#define SSSR_RFL_MASK	GENMASK(15, 12)	

#define SSCR1_TFT	GENMASK(9, 6)	
#define SSCR1_TxTresh(x) (((x) - 1) << 6)	
#define SSCR1_RFT	GENMASK(13, 10)	
#define SSCR1_RxTresh(x) (((x) - 1) << 10)	

#define RX_THRESH_CE4100_DFLT	2
#define TX_THRESH_CE4100_DFLT	2

#define CE4100_SSSR_TFL_MASK	GENMASK(9, 8)	
#define CE4100_SSSR_RFL_MASK	GENMASK(13, 12)	

#define CE4100_SSCR1_TFT	GENMASK(7, 6)	
#define CE4100_SSCR1_TxTresh(x) (((x) - 1) << 6)	
#define CE4100_SSCR1_RFT	GENMASK(11, 10)	
#define CE4100_SSCR1_RxTresh(x) (((x) - 1) << 10)	


#define DDS_RATE		0x28		 


#define QUARK_X1000_SSCR0_DSS		GENMASK(4, 0)	
#define QUARK_X1000_SSCR0_DataSize(x)	((x) - 1)	
#define QUARK_X1000_SSCR0_FRF		GENMASK(6, 5)	
#define QUARK_X1000_SSCR0_Motorola	(0x0 << 5)	

#define RX_THRESH_QUARK_X1000_DFLT	1
#define TX_THRESH_QUARK_X1000_DFLT	16

#define QUARK_X1000_SSSR_TFL_MASK	GENMASK(12, 8)	
#define QUARK_X1000_SSSR_RFL_MASK	GENMASK(17, 13)	

#define QUARK_X1000_SSCR1_TFT	GENMASK(10, 6)	
#define QUARK_X1000_SSCR1_TxTresh(x) (((x) - 1) << 6)	
#define QUARK_X1000_SSCR1_RFT	GENMASK(15, 11)	
#define QUARK_X1000_SSCR1_RxTresh(x) (((x) - 1) << 11)	
#define QUARK_X1000_SSCR1_EFWR	BIT(16)		
#define QUARK_X1000_SSCR1_STRF	BIT(17)		


#define SSCR0_TISSP		(1 << 4)	
#define SSCR0_PSP		(3 << 4)	

#define SSCR1_EFWR		BIT(14)		
#define SSCR1_STRF		BIT(15)		
#define SSCR1_IFS		BIT(16)		
#define SSCR1_PINTE		BIT(18)		
#define SSCR1_TINTE		BIT(19)		
#define SSCR1_RSRE		BIT(20)		
#define SSCR1_TSRE		BIT(21)		
#define SSCR1_TRAIL		BIT(22)		
#define SSCR1_RWOT		BIT(23)		
#define SSCR1_SFRMDIR		BIT(24)		
#define SSCR1_SCLKDIR		BIT(25)		
#define SSCR1_ECRB		BIT(26)		
#define SSCR1_ECRA		BIT(27)		
#define SSCR1_SCFR		BIT(28)		
#define SSCR1_EBCEI		BIT(29)		
#define SSCR1_TTE		BIT(30)		
#define SSCR1_TTELP		BIT(31)		

#define SSSR_PINT		BIT(18)		
#define SSSR_TINT		BIT(19)		
#define SSSR_EOC		BIT(20)		
#define SSSR_TUR		BIT(21)		
#define SSSR_CSS		BIT(22)		
#define SSSR_BCE		BIT(23)		

#define SSPSP_SCMODE(x)		((x) << 0)	
#define SSPSP_SFRMP		BIT(2)		
#define SSPSP_ETDS		BIT(3)		
#define SSPSP_STRTDLY(x)	((x) << 4)	
#define SSPSP_DMYSTRT(x)	((x) << 7)	
#define SSPSP_SFRMDLY(x)	((x) << 9)	
#define SSPSP_SFRMWDTH(x)	((x) << 16)	
#define SSPSP_DMYSTOP(x)	((x) << 23)	
#define SSPSP_FSRT		BIT(25)		


#define SSPSP_EDMYSTRT(x)	((x) << 26)     
#define SSPSP_EDMYSTOP(x)	((x) << 28)     
#define SSPSP_TIMING_MASK	(0x7f8001f0)

#define SSACD_ACDS(x)		((x) << 0)	
#define SSACD_ACDS_1		(0)
#define SSACD_ACDS_2		(1)
#define SSACD_ACDS_4		(2)
#define SSACD_ACDS_8		(3)
#define SSACD_ACDS_16		(4)
#define SSACD_ACDS_32		(5)
#define SSACD_SCDB		BIT(3)		
#define SSACD_SCDB_4X		(0)
#define SSACD_SCDB_1X		(1)
#define SSACD_ACPS(x)		((x) << 4)	
#define SSACD_SCDX8		BIT(7)		


#define SFIFOL			0x68		
#define SFIFOTT			0x6c		

#define RX_THRESH_MRFLD_DFLT	16
#define TX_THRESH_MRFLD_DFLT	16

#define SFIFOL_TFL_MASK		GENMASK(15, 0)	
#define SFIFOL_RFL_MASK		GENMASK(31, 16)	

#define SFIFOTT_TFT		GENMASK(15, 0)	
#define SFIFOTT_TxThresh(x)	(((x) - 1) << 0)	
#define SFIFOTT_RFT		GENMASK(31, 16)	
#define SFIFOTT_RxThresh(x)	(((x) - 1) << 16)	


#define SSITF			0x44		
#define SSITF_TxHiThresh(x)	(((x) - 1) << 0)
#define SSITF_TxLoThresh(x)	(((x) - 1) << 8)

#define SSIRF			0x48		
#define SSIRF_RxThresh(x)	((x) - 1)


#define SSCR2		(0x40)	
#define SSPSP2		(0x44)	

enum pxa_ssp_type {
	SSP_UNDEFINED = 0,
	PXA25x_SSP,  
	PXA25x_NSSP, 
	PXA27x_SSP,
	PXA3xx_SSP,
	PXA168_SSP,
	MMP2_SSP,
	PXA910_SSP,
	CE4100_SSP,
	MRFLD_SSP,
	QUARK_X1000_SSP,
	
	LPSS_LPT_SSP,
	LPSS_BYT_SSP,
	LPSS_BSW_SSP,
	LPSS_SPT_SSP,
	LPSS_BXT_SSP,
	LPSS_CNL_SSP,
	SSP_MAX
};

struct ssp_device {
	struct device	*dev;
	struct list_head	node;

	struct clk	*clk;
	void __iomem	*mmio_base;
	unsigned long	phys_base;

	const char	*label;
	int		port_id;
	enum pxa_ssp_type type;
	int		use_count;
	int		irq;

	struct device_node	*of_node;
};


static inline void pxa_ssp_write_reg(struct ssp_device *dev, u32 reg, u32 val)
{
	__raw_writel(val, dev->mmio_base + reg);
}


static inline u32 pxa_ssp_read_reg(struct ssp_device *dev, u32 reg)
{
	return __raw_readl(dev->mmio_base + reg);
}

static inline void pxa_ssp_enable(struct ssp_device *ssp)
{
	u32 sscr0;

	sscr0 = pxa_ssp_read_reg(ssp, SSCR0) | SSCR0_SSE;
	pxa_ssp_write_reg(ssp, SSCR0, sscr0);
}

static inline void pxa_ssp_disable(struct ssp_device *ssp)
{
	u32 sscr0;

	sscr0 = pxa_ssp_read_reg(ssp, SSCR0) & ~SSCR0_SSE;
	pxa_ssp_write_reg(ssp, SSCR0, sscr0);
}

#if IS_ENABLED(CONFIG_PXA_SSP)
struct ssp_device *pxa_ssp_request(int port, const char *label);
void pxa_ssp_free(struct ssp_device *);
struct ssp_device *pxa_ssp_request_of(const struct device_node *of_node,
				      const char *label);
#else
static inline struct ssp_device *pxa_ssp_request(int port, const char *label)
{
	return NULL;
}
static inline struct ssp_device *pxa_ssp_request_of(const struct device_node *n,
						    const char *name)
{
	return NULL;
}
static inline void pxa_ssp_free(struct ssp_device *ssp) {}
#endif

#endif	
