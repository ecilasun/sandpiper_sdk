/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef CAN_ML_H
#define CAN_ML_H

#include <linux/can.h>
#include <linux/list.h>
#include <linux/netdevice.h>

#define CAN_SFF_RCV_ARRAY_SZ (1 << CAN_SFF_ID_BITS)
#define CAN_EFF_RCV_HASH_BITS 10
#define CAN_EFF_RCV_ARRAY_SZ (1 << CAN_EFF_RCV_HASH_BITS)

enum { RX_ERR, RX_ALL, RX_FIL, RX_INV, RX_MAX };

struct can_dev_rcv_lists {
	struct hlist_head rx[RX_MAX];
	struct hlist_head rx_sff[CAN_SFF_RCV_ARRAY_SZ];
	struct hlist_head rx_eff[CAN_EFF_RCV_ARRAY_SZ];
	int entries;
};

struct can_ml_priv {
	struct can_dev_rcv_lists dev_rcv_lists;
#ifdef CAN_J1939
	struct j1939_priv *j1939_priv;
#endif
};

static inline struct can_ml_priv *can_get_ml_priv(struct net_device *dev)
{
	return netdev_get_ml_priv(dev, ML_PRIV_CAN);
}

static inline void can_set_ml_priv(struct net_device *dev,
				   struct can_ml_priv *ml_priv)
{
	netdev_set_ml_priv(dev, ml_priv, ML_PRIV_CAN);
}

#endif 
