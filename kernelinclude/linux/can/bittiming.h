/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _CAN_BITTIMING_H
#define _CAN_BITTIMING_H

#include <linux/netdevice.h>
#include <linux/can/netlink.h>

#define CAN_SYNC_SEG 1

#define CAN_BITRATE_UNSET 0
#define CAN_BITRATE_UNKNOWN (-1U)

#define CAN_CTRLMODE_TDC_MASK					\
	(CAN_CTRLMODE_TDC_AUTO | CAN_CTRLMODE_TDC_MANUAL)


struct can_tdc {
	u32 tdcv;
	u32 tdco;
	u32 tdcf;
};


struct can_tdc_const {
	u32 tdcv_min;
	u32 tdcv_max;
	u32 tdco_min;
	u32 tdco_max;
	u32 tdcf_min;
	u32 tdcf_max;
};

#ifdef CONFIG_CAN_CALC_BITTIMING
int can_calc_bittiming(const struct net_device *dev, struct can_bittiming *bt,
		       const struct can_bittiming_const *btc, struct netlink_ext_ack *extack);

void can_calc_tdco(struct can_tdc *tdc, const struct can_tdc_const *tdc_const,
		   const struct can_bittiming *dbt,
		   u32 *ctrlmode, u32 ctrlmode_supported);
#else 
static inline int
can_calc_bittiming(const struct net_device *dev, struct can_bittiming *bt,
		   const struct can_bittiming_const *btc, struct netlink_ext_ack *extack)
{
	netdev_err(dev, "bit-timing calculation not available\n");
	return -EINVAL;
}

static inline void
can_calc_tdco(struct can_tdc *tdc, const struct can_tdc_const *tdc_const,
	      const struct can_bittiming *dbt,
	      u32 *ctrlmode, u32 ctrlmode_supported)
{
}
#endif 

void can_sjw_set_default(struct can_bittiming *bt);

int can_sjw_check(const struct net_device *dev, const struct can_bittiming *bt,
		  const struct can_bittiming_const *btc, struct netlink_ext_ack *extack);

int can_get_bittiming(const struct net_device *dev, struct can_bittiming *bt,
		      const struct can_bittiming_const *btc,
		      const u32 *bitrate_const,
		      const unsigned int bitrate_const_cnt,
		      struct netlink_ext_ack *extack);


static inline unsigned int can_bit_time(const struct can_bittiming *bt)
{
	return CAN_SYNC_SEG + bt->prop_seg + bt->phase_seg1 + bt->phase_seg2;
}

#endif 
