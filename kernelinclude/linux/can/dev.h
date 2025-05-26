/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _CAN_DEV_H
#define _CAN_DEV_H

#include <linux/can.h>
#include <linux/can/bittiming.h>
#include <linux/can/error.h>
#include <linux/can/length.h>
#include <linux/can/netlink.h>
#include <linux/can/skb.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>


enum can_mode {
	CAN_MODE_STOP = 0,
	CAN_MODE_START,
	CAN_MODE_SLEEP
};

enum can_termination_gpio {
	CAN_TERMINATION_GPIO_DISABLED = 0,
	CAN_TERMINATION_GPIO_ENABLED,
	CAN_TERMINATION_GPIO_MAX,
};


struct can_priv {
	struct net_device *dev;
	struct can_device_stats can_stats;

	const struct can_bittiming_const *bittiming_const,
		*data_bittiming_const;
	struct can_bittiming bittiming, data_bittiming;
	const struct can_tdc_const *tdc_const;
	struct can_tdc tdc;

	unsigned int bitrate_const_cnt;
	const u32 *bitrate_const;
	const u32 *data_bitrate_const;
	unsigned int data_bitrate_const_cnt;
	u32 bitrate_max;
	struct can_clock clock;

	unsigned int termination_const_cnt;
	const u16 *termination_const;
	u16 termination;
	struct gpio_desc *termination_gpio;
	u16 termination_gpio_ohms[CAN_TERMINATION_GPIO_MAX];

	unsigned int echo_skb_max;
	struct sk_buff **echo_skb;

	enum can_state state;

	
	u32 ctrlmode;		
	u32 ctrlmode_supported;	

	int restart_ms;
	struct delayed_work restart_work;

	int (*do_set_bittiming)(struct net_device *dev);
	int (*do_set_data_bittiming)(struct net_device *dev);
	int (*do_set_mode)(struct net_device *dev, enum can_mode mode);
	int (*do_set_termination)(struct net_device *dev, u16 term);
	int (*do_get_state)(const struct net_device *dev,
			    enum can_state *state);
	int (*do_get_berr_counter)(const struct net_device *dev,
				   struct can_berr_counter *bec);
	int (*do_get_auto_tdcv)(const struct net_device *dev, u32 *tdcv);
};

static inline bool can_tdc_is_enabled(const struct can_priv *priv)
{
	return !!(priv->ctrlmode & CAN_CTRLMODE_TDC_MASK);
}


static inline s32 can_get_relative_tdco(const struct can_priv *priv)
{
	const struct can_bittiming *dbt = &priv->data_bittiming;
	s32 sample_point_in_tc = (CAN_SYNC_SEG + dbt->prop_seg +
				  dbt->phase_seg1) * dbt->brp;

	return (s32)priv->tdc.tdco - sample_point_in_tc;
}


static inline int __must_check can_set_static_ctrlmode(struct net_device *dev,
						       u32 static_mode)
{
	struct can_priv *priv = netdev_priv(dev);

	
	if (priv->ctrlmode_supported & static_mode) {
		netdev_warn(dev,
			    "Controller features can not be supported and static at the same time\n");
		return -EINVAL;
	}
	priv->ctrlmode = static_mode;

	
	if (static_mode & CAN_CTRLMODE_FD)
		dev->mtu = CANFD_MTU;

	return 0;
}

static inline u32 can_get_static_ctrlmode(struct can_priv *priv)
{
	return priv->ctrlmode & ~priv->ctrlmode_supported;
}

static inline bool can_is_canxl_dev_mtu(unsigned int mtu)
{
	return (mtu >= CANXL_MIN_MTU && mtu <= CANXL_MAX_MTU);
}


static inline bool can_dev_dropped_skb(struct net_device *dev, struct sk_buff *skb)
{
	struct can_priv *priv = netdev_priv(dev);

	if (priv->ctrlmode & CAN_CTRLMODE_LISTENONLY) {
		netdev_info_once(dev,
				 "interface in listen only mode, dropping skb\n");
		kfree_skb(skb);
		dev->stats.tx_dropped++;
		return true;
	}

	return can_dropped_invalid_skb(dev, skb);
}

void can_setup(struct net_device *dev);

struct net_device *alloc_candev_mqs(int sizeof_priv, unsigned int echo_skb_max,
				    unsigned int txqs, unsigned int rxqs);
#define alloc_candev(sizeof_priv, echo_skb_max) \
	alloc_candev_mqs(sizeof_priv, echo_skb_max, 1, 1)
#define alloc_candev_mq(sizeof_priv, echo_skb_max, count) \
	alloc_candev_mqs(sizeof_priv, echo_skb_max, count, count)
void free_candev(struct net_device *dev);


struct can_priv *safe_candev_priv(struct net_device *dev);

int open_candev(struct net_device *dev);
void close_candev(struct net_device *dev);
int can_change_mtu(struct net_device *dev, int new_mtu);
int can_eth_ioctl_hwts(struct net_device *netdev, struct ifreq *ifr, int cmd);
int can_ethtool_op_get_ts_info_hwts(struct net_device *dev,
				    struct ethtool_ts_info *info);

int register_candev(struct net_device *dev);
void unregister_candev(struct net_device *dev);

int can_restart_now(struct net_device *dev);
void can_bus_off(struct net_device *dev);

const char *can_get_state_str(const enum can_state state);
void can_change_state(struct net_device *dev, struct can_frame *cf,
		      enum can_state tx_state, enum can_state rx_state);

#ifdef CONFIG_OF
void of_can_transceiver(struct net_device *dev);
#else
static inline void of_can_transceiver(struct net_device *dev) { }
#endif

extern struct rtnl_link_ops can_link_ops;
int can_netlink_register(void);
void can_netlink_unregister(void);

#endif 
