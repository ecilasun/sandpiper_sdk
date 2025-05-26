/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_STM32_GPTIMER_H_
#define _LINUX_STM32_GPTIMER_H_

#include <linux/clk.h>
#include <linux/dmaengine.h>
#include <linux/dma-mapping.h>
#include <linux/regmap.h>

#define TIM_CR1		0x00	
#define TIM_CR2		0x04	
#define TIM_SMCR	0x08	
#define TIM_DIER	0x0C	
#define TIM_SR		0x10	
#define TIM_EGR		0x14	
#define TIM_CCMR1	0x18	
#define TIM_CCMR2	0x1C	
#define TIM_CCER	0x20	
#define TIM_CNT		0x24	
#define TIM_PSC		0x28	
#define TIM_ARR		0x2c	
#define TIM_CCR1	0x34	
#define TIM_CCR2	0x38	
#define TIM_CCR3	0x3C	
#define TIM_CCR4	0x40	
#define TIM_BDTR	0x44	
#define TIM_DCR		0x48	
#define TIM_DMAR	0x4C	
#define TIM_TISEL	0x68	

#define TIM_CR1_CEN	BIT(0)	
#define TIM_CR1_DIR	BIT(4)  
#define TIM_CR1_ARPE	BIT(7)	
#define TIM_CR2_MMS	(BIT(4) | BIT(5) | BIT(6)) 
#define TIM_CR2_MMS2	GENMASK(23, 20) 
#define TIM_SMCR_SMS	(BIT(0) | BIT(1) | BIT(2)) 
#define TIM_SMCR_TS	(BIT(4) | BIT(5) | BIT(6)) 
#define TIM_DIER_UIE	BIT(0)	
#define TIM_DIER_UDE	BIT(8)  
#define TIM_DIER_CC1DE	BIT(9)  
#define TIM_DIER_CC2DE	BIT(10) 
#define TIM_DIER_CC3DE	BIT(11) 
#define TIM_DIER_CC4DE	BIT(12) 
#define TIM_DIER_COMDE	BIT(13) 
#define TIM_DIER_TDE	BIT(14) 
#define TIM_SR_UIF	BIT(0)	
#define TIM_EGR_UG	BIT(0)	
#define TIM_CCMR_PE	BIT(3)	
#define TIM_CCMR_M1	(BIT(6) | BIT(5))  
#define TIM_CCMR_CC1S		(BIT(0) | BIT(1)) 
#define TIM_CCMR_IC1PSC		GENMASK(3, 2)	
#define TIM_CCMR_CC2S		(BIT(8) | BIT(9)) 
#define TIM_CCMR_IC2PSC		GENMASK(11, 10)	
#define TIM_CCMR_CC1S_TI1	BIT(0)	
#define TIM_CCMR_CC1S_TI2	BIT(1)	
#define TIM_CCMR_CC2S_TI2	BIT(8)	
#define TIM_CCMR_CC2S_TI1	BIT(9)	
#define TIM_CCER_CC1E	BIT(0)	
#define TIM_CCER_CC1P	BIT(1)	
#define TIM_CCER_CC1NE	BIT(2)	
#define TIM_CCER_CC1NP	BIT(3)	
#define TIM_CCER_CC2E	BIT(4)	
#define TIM_CCER_CC2P	BIT(5)	
#define TIM_CCER_CC3E	BIT(8)	
#define TIM_CCER_CC3P	BIT(9)	
#define TIM_CCER_CC4E	BIT(12)	
#define TIM_CCER_CC4P	BIT(13)	
#define TIM_CCER_CCXE	(BIT(0) | BIT(4) | BIT(8) | BIT(12))
#define TIM_BDTR_BKE(x)	BIT(12 + (x) * 12) 
#define TIM_BDTR_BKP(x)	BIT(13 + (x) * 12) 
#define TIM_BDTR_AOE	BIT(14)	
#define TIM_BDTR_MOE	BIT(15)	
#define TIM_BDTR_BKF(x)	(0xf << (16 + (x) * 4))
#define TIM_DCR_DBA	GENMASK(4, 0)	
#define TIM_DCR_DBL	GENMASK(12, 8)	

#define MAX_TIM_PSC		0xFFFF
#define MAX_TIM_ICPSC		0x3
#define TIM_CR2_MMS_SHIFT	4
#define TIM_CR2_MMS2_SHIFT	20
#define TIM_SMCR_SMS_SLAVE_MODE_DISABLED	0 
#define TIM_SMCR_SMS_ENCODER_MODE_1		1 
#define TIM_SMCR_SMS_ENCODER_MODE_2		2 
#define TIM_SMCR_SMS_ENCODER_MODE_3		3 
#define TIM_SMCR_TS_SHIFT	4
#define TIM_BDTR_BKF_MASK	0xF
#define TIM_BDTR_BKF_SHIFT(x)	(16 + (x) * 4)

enum stm32_timers_dmas {
	STM32_TIMERS_DMA_CH1,
	STM32_TIMERS_DMA_CH2,
	STM32_TIMERS_DMA_CH3,
	STM32_TIMERS_DMA_CH4,
	STM32_TIMERS_DMA_UP,
	STM32_TIMERS_DMA_TRIG,
	STM32_TIMERS_DMA_COM,
	STM32_TIMERS_MAX_DMAS,
};


struct stm32_timers_dma {
	struct completion completion;
	phys_addr_t phys_base;
	struct mutex lock;
	struct dma_chan *chan;
	struct dma_chan *chans[STM32_TIMERS_MAX_DMAS];
};

struct stm32_timers {
	struct clk *clk;
	struct regmap *regmap;
	u32 max_arr;
	struct stm32_timers_dma dma; 
};

#if IS_REACHABLE(CONFIG_MFD_STM32_TIMERS)
int stm32_timers_dma_burst_read(struct device *dev, u32 *buf,
				enum stm32_timers_dmas id, u32 reg,
				unsigned int num_reg, unsigned int bursts,
				unsigned long tmo_ms);
#else
static inline int stm32_timers_dma_burst_read(struct device *dev, u32 *buf,
					      enum stm32_timers_dmas id,
					      u32 reg,
					      unsigned int num_reg,
					      unsigned int bursts,
					      unsigned long tmo_ms)
{
	return -ENODEV;
}
#endif
#endif
