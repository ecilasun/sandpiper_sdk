/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_MII_TIMESTAMPER_H
#define _LINUX_MII_TIMESTAMPER_H

#include <linux/device.h>
#include <linux/ethtool.h>
#include <linux/skbuff.h>

struct phy_device;


struct mii_timestamper {
	bool (*rxtstamp)(struct mii_timestamper *mii_ts,
			 struct sk_buff *skb, int type);

	void (*txtstamp)(struct mii_timestamper *mii_ts,
			 struct sk_buff *skb, int type);

	int  (*hwtstamp)(struct mii_timestamper *mii_ts,
			 struct ifreq *ifreq);

	void (*link_state)(struct mii_timestamper *mii_ts,
			   struct phy_device *phydev);

	int  (*ts_info)(struct mii_timestamper *mii_ts,
			struct ethtool_ts_info *ts_info);

	struct device *device;
};


struct mii_timestamping_ctrl {
	struct mii_timestamper *(*probe_channel)(struct device *device,
						 unsigned int port);
	void (*release_channel)(struct device *device,
				struct mii_timestamper *mii_ts);
};

#ifdef CONFIG_NETWORK_PHY_TIMESTAMPING

int register_mii_tstamp_controller(struct device *device,
				   struct mii_timestamping_ctrl *ctrl);

void unregister_mii_tstamp_controller(struct device *device);

struct mii_timestamper *register_mii_timestamper(struct device_node *node,
						 unsigned int port);

void unregister_mii_timestamper(struct mii_timestamper *mii_ts);

#else

static inline
int register_mii_tstamp_controller(struct device *device,
				   struct mii_timestamping_ctrl *ctrl)
{
	return -EOPNOTSUPP;
}

static inline void unregister_mii_tstamp_controller(struct device *device)
{
}

static inline
struct mii_timestamper *register_mii_timestamper(struct device_node *node,
						 unsigned int port)
{
	return NULL;
}

static inline void unregister_mii_timestamper(struct mii_timestamper *mii_ts)
{
}

#endif

#endif
