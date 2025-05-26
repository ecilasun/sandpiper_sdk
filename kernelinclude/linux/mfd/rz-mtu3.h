/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __MFD_RZ_MTU3_H__
#define __MFD_RZ_MTU3_H__

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/mutex.h>


#define RZ_MTU3_TSTRA	0x080 
#define RZ_MTU3_TSTRB	0x880 


#define RZ_MTU3_TDDRA	0x016 
#define RZ_MTU3_TDDRB	0x816 
#define RZ_MTU3_TCDRA	0x014 
#define RZ_MTU3_TCDRB	0x814 
#define RZ_MTU3_TCBRA	0x022 
#define RZ_MTU3_TCBRB	0x822 
#define RZ_MTU3_TCNTSA	0x020 
#define RZ_MTU3_TCNTSB	0x820 




#define RZ_MTU3_TIER	0 
#define RZ_MTU3_NFCR	1 
#define RZ_MTU3_TSR	2 
#define RZ_MTU3_TCR	3 
#define RZ_MTU3_TCR2	4 


#define RZ_MTU3_TMDR1	5
#define RZ_MTU3_TMDR1_MD		GENMASK(3, 0)
#define RZ_MTU3_TMDR1_MD_NORMAL		FIELD_PREP(RZ_MTU3_TMDR1_MD, 0)
#define RZ_MTU3_TMDR1_MD_PWMMODE1	FIELD_PREP(RZ_MTU3_TMDR1_MD, 2)

#define RZ_MTU3_TIOR	6 
#define RZ_MTU3_TIORH	6 
#define RZ_MTU3_TIORL	7 

#define RZ_MTU3_TBTM	8 


#define RZ_MTU3_TSTR		2 
#define RZ_MTU3_TCNTCMPCLR	3 
#define RZ_MTU3_TCRU		4 
#define RZ_MTU3_TCR2U		5 
#define RZ_MTU3_TIORU		6 
#define RZ_MTU3_TCRV		7 
#define RZ_MTU3_TCR2V		8 
#define RZ_MTU3_TIORV		9 
#define RZ_MTU3_TCRW		10 
#define RZ_MTU3_TCR2W		11 
#define RZ_MTU3_TIORW		12 


#define RZ_MTU3_TCNT		0 
#define RZ_MTU3_TGRA		1 
#define RZ_MTU3_TGRB		2 
#define RZ_MTU3_TGRC		3 
#define RZ_MTU3_TGRD		4 
#define RZ_MTU3_TGRE		5 
#define RZ_MTU3_TGRF		6 

#define RZ_MTU3_TADCR		7 
#define RZ_MTU3_TADCORA		8 
#define RZ_MTU3_TADCORB		9 
#define RZ_MTU3_TADCOBRA	10 
#define RZ_MTU3_TADCOBRB	11 


#define RZ_MTU3_TCNTU		0 
#define RZ_MTU3_TGRU		1 
#define RZ_MTU3_TCNTV		2 
#define RZ_MTU3_TGRV		3 
#define RZ_MTU3_TCNTW		4 
#define RZ_MTU3_TGRW		5 


#define RZ_MTU3_TCNTLW		0 
#define RZ_MTU3_TGRALW		1 
#define RZ_MTU3_TGRBLW		2 

#define RZ_MTU3_TMDR3		0x191 


#define RZ_MTU3_TCR_CCLR	GENMASK(7, 5)
#define RZ_MTU3_TCR_CKEG	GENMASK(4, 3)
#define RZ_MTU3_TCR_TPCS	GENMASK(2, 0)
#define RZ_MTU3_TCR_CCLR_TGRA	BIT(5)
#define RZ_MTU3_TCR_CCLR_TGRC	FIELD_PREP(RZ_MTU3_TCR_CCLR, 5)
#define RZ_MTU3_TCR_CKEG_RISING	FIELD_PREP(RZ_MTU3_TCR_CKEG, 0)

#define RZ_MTU3_TIOR_IOB			GENMASK(7, 4)
#define RZ_MTU3_TIOR_IOA			GENMASK(3, 0)
#define RZ_MTU3_TIOR_OC_RETAIN			0
#define RZ_MTU3_TIOR_OC_INIT_OUT_LO_HI_OUT	2
#define RZ_MTU3_TIOR_OC_INIT_OUT_HI_TOGGLE_OUT	7

#define RZ_MTU3_TIOR_OC_IOA_H_COMP_MATCH \
	FIELD_PREP(RZ_MTU3_TIOR_IOA, RZ_MTU3_TIOR_OC_INIT_OUT_LO_HI_OUT)
#define RZ_MTU3_TIOR_OC_IOB_TOGGLE \
	FIELD_PREP(RZ_MTU3_TIOR_IOB, RZ_MTU3_TIOR_OC_INIT_OUT_HI_TOGGLE_OUT)

enum rz_mtu3_channels {
	RZ_MTU3_CHAN_0,
	RZ_MTU3_CHAN_1,
	RZ_MTU3_CHAN_2,
	RZ_MTU3_CHAN_3,
	RZ_MTU3_CHAN_4,
	RZ_MTU3_CHAN_5,
	RZ_MTU3_CHAN_6,
	RZ_MTU3_CHAN_7,
	RZ_MTU3_CHAN_8,
	RZ_MTU_NUM_CHANNELS
};


struct rz_mtu3_channel {
	struct device *dev;
	unsigned int channel_number;
	struct mutex lock;
	bool is_busy;
};


struct rz_mtu3 {
	struct clk *clk;
	struct rz_mtu3_channel channels[RZ_MTU_NUM_CHANNELS];

	void *priv_data;
};

static inline bool rz_mtu3_request_channel(struct rz_mtu3_channel *ch)
{
	mutex_lock(&ch->lock);
	if (ch->is_busy) {
		mutex_unlock(&ch->lock);
		return false;
	}

	ch->is_busy = true;
	mutex_unlock(&ch->lock);

	return true;
}

static inline void rz_mtu3_release_channel(struct rz_mtu3_channel *ch)
{
	mutex_lock(&ch->lock);
	ch->is_busy = false;
	mutex_unlock(&ch->lock);
}

bool rz_mtu3_is_enabled(struct rz_mtu3_channel *ch);
void rz_mtu3_disable(struct rz_mtu3_channel *ch);
int rz_mtu3_enable(struct rz_mtu3_channel *ch);

u8 rz_mtu3_8bit_ch_read(struct rz_mtu3_channel *ch, u16 off);
u16 rz_mtu3_16bit_ch_read(struct rz_mtu3_channel *ch, u16 off);
u32 rz_mtu3_32bit_ch_read(struct rz_mtu3_channel *ch, u16 off);
u16 rz_mtu3_shared_reg_read(struct rz_mtu3_channel *ch, u16 off);

void rz_mtu3_8bit_ch_write(struct rz_mtu3_channel *ch, u16 off, u8 val);
void rz_mtu3_16bit_ch_write(struct rz_mtu3_channel *ch, u16 off, u16 val);
void rz_mtu3_32bit_ch_write(struct rz_mtu3_channel *ch, u16 off, u32 val);
void rz_mtu3_shared_reg_write(struct rz_mtu3_channel *ch, u16 off, u16 val);
void rz_mtu3_shared_reg_update_bit(struct rz_mtu3_channel *ch, u16 off,
				   u16 pos, u8 val);

#endif 
