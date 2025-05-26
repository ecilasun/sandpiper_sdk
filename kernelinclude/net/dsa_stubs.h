/* SPDX-License-Identifier: GPL-2.0-or-later */


#include <linux/mutex.h>
#include <linux/netdevice.h>
#include <linux/net_tstamp.h>
#include <net/dsa.h>

#if IS_ENABLED(CONFIG_NET_DSA)

extern const struct dsa_stubs *dsa_stubs;

struct dsa_stubs {
	int (*master_hwtstamp_validate)(struct net_device *dev,
					const struct kernel_hwtstamp_config *config,
					struct netlink_ext_ack *extack);
};

static inline int dsa_master_hwtstamp_validate(struct net_device *dev,
					       const struct kernel_hwtstamp_config *config,
					       struct netlink_ext_ack *extack)
{
	if (!netdev_uses_dsa(dev))
		return 0;

	
	ASSERT_RTNL();

	return dsa_stubs->master_hwtstamp_validate(dev, config, extack);
}

#else

static inline int dsa_master_hwtstamp_validate(struct net_device *dev,
					       const struct kernel_hwtstamp_config *config,
					       struct netlink_ext_ack *extack)
{
	return 0;
}

#endif
