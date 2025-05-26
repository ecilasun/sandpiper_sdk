/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_NET_QUEUES_H
#define _LINUX_NET_QUEUES_H

#include <linux/netdevice.h>



#define netif_txq_try_stop(txq, get_desc, start_thrs)			\
	({								\
		int _res;						\
									\
		netif_tx_stop_queue(txq);				\
									\
		smp_mb__after_atomic();					\
									\
									\
		_res = 0;						\
		if (unlikely(get_desc >= start_thrs)) {			\
			netif_tx_start_queue(txq);			\
			_res = -1;					\
		}							\
		_res;							\
	})								\


#define netif_txq_maybe_stop(txq, get_desc, stop_thrs, start_thrs)	\
	({								\
		int _res;						\
									\
		_res = 1;						\
		if (unlikely(get_desc < stop_thrs))			\
			_res = netif_txq_try_stop(txq, get_desc, start_thrs); \
		_res;							\
	})								\


static inline void
netdev_txq_completed_mb(struct netdev_queue *dev_queue,
			unsigned int pkts, unsigned int bytes)
{
	if (IS_ENABLED(CONFIG_BQL))
		netdev_tx_completed_queue(dev_queue, pkts, bytes);
	else if (bytes)
		smp_mb();
}


#define __netif_txq_completed_wake(txq, pkts, bytes,			\
				   get_desc, start_thrs, down_cond)	\
	({								\
		int _res;						\
									\
									\
		netdev_txq_completed_mb(txq, pkts, bytes);		\
									\
		_res = -1;						\
		if (pkts && likely(get_desc >= start_thrs)) {		\
			_res = 1;					\
			if (unlikely(netif_tx_queue_stopped(txq)) &&	\
			    !(down_cond)) {				\
				netif_tx_wake_queue(txq);		\
				_res = 0;				\
			}						\
		}							\
		_res;							\
	})

#define netif_txq_completed_wake(txq, pkts, bytes, get_desc, start_thrs) \
	__netif_txq_completed_wake(txq, pkts, bytes, get_desc, start_thrs, false)



#define netif_subqueue_try_stop(dev, idx, get_desc, start_thrs)		\
	({								\
		struct netdev_queue *txq;				\
									\
		txq = netdev_get_tx_queue(dev, idx);			\
		netif_txq_try_stop(txq, get_desc, start_thrs);		\
	})

#define netif_subqueue_maybe_stop(dev, idx, get_desc, stop_thrs, start_thrs) \
	({								\
		struct netdev_queue *txq;				\
									\
		txq = netdev_get_tx_queue(dev, idx);			\
		netif_txq_maybe_stop(txq, get_desc, stop_thrs, start_thrs); \
	})

#define netif_subqueue_completed_wake(dev, idx, pkts, bytes,		\
				      get_desc, start_thrs)		\
	({								\
		struct netdev_queue *txq;				\
									\
		txq = netdev_get_tx_queue(dev, idx);			\
		netif_txq_completed_wake(txq, pkts, bytes,		\
					 get_desc, start_thrs);		\
	})

#endif
