/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_NETDEVICE_H
#define _LINUX_NETDEVICE_H

#include <linux/timer.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/atomic.h>
#include <linux/prefetch.h>
#include <asm/cache.h>
#include <asm/byteorder.h>
#include <asm/local.h>

#include <linux/percpu.h>
#include <linux/rculist.h>
#include <linux/workqueue.h>
#include <linux/dynamic_queue_limits.h>

#include <net/net_namespace.h>
#ifdef CONFIG_DCB
#include <net/dcbnl.h>
#endif
#include <net/netprio_cgroup.h>

#include <linux/netdev_features.h>
#include <linux/neighbour.h>
#include <uapi/linux/netdevice.h>
#include <uapi/linux/if_bonding.h>
#include <uapi/linux/pkt_cls.h>
#include <uapi/linux/netdev.h>
#include <linux/hashtable.h>
#include <linux/rbtree.h>
#include <net/net_trackers.h>
#include <net/net_debug.h>
#include <net/dropreason-core.h>

struct netpoll_info;
struct device;
struct ethtool_ops;
struct kernel_hwtstamp_config;
struct phy_device;
struct dsa_port;
struct ip_tunnel_parm;
struct macsec_context;
struct macsec_ops;
struct netdev_name_node;
struct sd_flow_limit;
struct sfp_bus;

struct wireless_dev;

struct wpan_dev;
struct mpls_dev;

struct udp_tunnel_info;
struct udp_tunnel_nic_info;
struct udp_tunnel_nic;
struct bpf_prog;
struct xdp_buff;
struct xdp_frame;
struct xdp_metadata_ops;
struct xdp_md;

typedef u32 xdp_features_t;

void synchronize_net(void);
void netdev_set_default_ethtool_ops(struct net_device *dev,
				    const struct ethtool_ops *ops);
void netdev_sw_irq_coalesce_default_on(struct net_device *dev);


#define NET_RX_SUCCESS		0	
#define NET_RX_DROP		1	

#define MAX_NEST_DEV 8




#define NET_XMIT_SUCCESS	0x00
#define NET_XMIT_DROP		0x01	
#define NET_XMIT_CN		0x02	
#define NET_XMIT_MASK		0x0f	


#define net_xmit_eval(e)	((e) == NET_XMIT_CN ? 0 : (e))
#define net_xmit_errno(e)	((e) != NET_XMIT_CN ? -ENOBUFS : 0)


#define NETDEV_TX_MASK		0xf0

enum netdev_tx {
	__NETDEV_TX_MIN	 = INT_MIN,	
	NETDEV_TX_OK	 = 0x00,	
	NETDEV_TX_BUSY	 = 0x10,	
};
typedef enum netdev_tx netdev_tx_t;


static inline bool dev_xmit_complete(int rc)
{
	
	if (likely(rc < NET_XMIT_MASK))
		return true;

	return false;
}



#if defined(CONFIG_HYPERV_NET)
# define LL_MAX_HEADER 128
#elif defined(CONFIG_WLAN) || IS_ENABLED(CONFIG_AX25)
# if defined(CONFIG_MAC80211_MESH)
#  define LL_MAX_HEADER 128
# else
#  define LL_MAX_HEADER 96
# endif
#else
# define LL_MAX_HEADER 32
#endif

#if !IS_ENABLED(CONFIG_NET_IPIP) && !IS_ENABLED(CONFIG_NET_IPGRE) && \
    !IS_ENABLED(CONFIG_IPV6_SIT) && !IS_ENABLED(CONFIG_IPV6_TUNNEL)
#define MAX_HEADER LL_MAX_HEADER
#else
#define MAX_HEADER (LL_MAX_HEADER + 48)
#endif



#define NET_DEV_STAT(FIELD)			\
	union {					\
		unsigned long FIELD;		\
		atomic_long_t __##FIELD;	\
	}

struct net_device_stats {
	NET_DEV_STAT(rx_packets);
	NET_DEV_STAT(tx_packets);
	NET_DEV_STAT(rx_bytes);
	NET_DEV_STAT(tx_bytes);
	NET_DEV_STAT(rx_errors);
	NET_DEV_STAT(tx_errors);
	NET_DEV_STAT(rx_dropped);
	NET_DEV_STAT(tx_dropped);
	NET_DEV_STAT(multicast);
	NET_DEV_STAT(collisions);
	NET_DEV_STAT(rx_length_errors);
	NET_DEV_STAT(rx_over_errors);
	NET_DEV_STAT(rx_crc_errors);
	NET_DEV_STAT(rx_frame_errors);
	NET_DEV_STAT(rx_fifo_errors);
	NET_DEV_STAT(rx_missed_errors);
	NET_DEV_STAT(tx_aborted_errors);
	NET_DEV_STAT(tx_carrier_errors);
	NET_DEV_STAT(tx_fifo_errors);
	NET_DEV_STAT(tx_heartbeat_errors);
	NET_DEV_STAT(tx_window_errors);
	NET_DEV_STAT(rx_compressed);
	NET_DEV_STAT(tx_compressed);
};
#undef NET_DEV_STAT


struct net_device_core_stats {
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	rx_nohandler;
	unsigned long	rx_otherhost_dropped;
} __aligned(4 * sizeof(unsigned long));

#include <linux/cache.h>
#include <linux/skbuff.h>

#ifdef CONFIG_RPS
#include <linux/static_key.h>
extern struct static_key_false rps_needed;
extern struct static_key_false rfs_needed;
#endif

struct neighbour;
struct neigh_parms;
struct sk_buff;

struct netdev_hw_addr {
	struct list_head	list;
	struct rb_node		node;
	unsigned char		addr[MAX_ADDR_LEN];
	unsigned char		type;
#define NETDEV_HW_ADDR_T_LAN		1
#define NETDEV_HW_ADDR_T_SAN		2
#define NETDEV_HW_ADDR_T_UNICAST	3
#define NETDEV_HW_ADDR_T_MULTICAST	4
	bool			global_use;
	int			sync_cnt;
	int			refcount;
	int			synced;
	struct rcu_head		rcu_head;
};

struct netdev_hw_addr_list {
	struct list_head	list;
	int			count;

	
	struct rb_root		tree;
};

#define netdev_hw_addr_list_count(l) ((l)->count)
#define netdev_hw_addr_list_empty(l) (netdev_hw_addr_list_count(l) == 0)
#define netdev_hw_addr_list_for_each(ha, l) \
	list_for_each_entry(ha, &(l)->list, list)

#define netdev_uc_count(dev) netdev_hw_addr_list_count(&(dev)->uc)
#define netdev_uc_empty(dev) netdev_hw_addr_list_empty(&(dev)->uc)
#define netdev_for_each_uc_addr(ha, dev) \
	netdev_hw_addr_list_for_each(ha, &(dev)->uc)
#define netdev_for_each_synced_uc_addr(_ha, _dev) \
	netdev_for_each_uc_addr((_ha), (_dev)) \
		if ((_ha)->sync_cnt)

#define netdev_mc_count(dev) netdev_hw_addr_list_count(&(dev)->mc)
#define netdev_mc_empty(dev) netdev_hw_addr_list_empty(&(dev)->mc)
#define netdev_for_each_mc_addr(ha, dev) \
	netdev_hw_addr_list_for_each(ha, &(dev)->mc)
#define netdev_for_each_synced_mc_addr(_ha, _dev) \
	netdev_for_each_mc_addr((_ha), (_dev)) \
		if ((_ha)->sync_cnt)

struct hh_cache {
	unsigned int	hh_len;
	seqlock_t	hh_lock;

	
#define HH_DATA_MOD	16
#define HH_DATA_OFF(__len) \
	(HH_DATA_MOD - (((__len - 1) & (HH_DATA_MOD - 1)) + 1))
#define HH_DATA_ALIGN(__len) \
	(((__len)+(HH_DATA_MOD-1))&~(HH_DATA_MOD - 1))
	unsigned long	hh_data[HH_DATA_ALIGN(LL_MAX_HEADER) / sizeof(long)];
};


#define LL_RESERVED_SPACE(dev) \
	((((dev)->hard_header_len + READ_ONCE((dev)->needed_headroom)) \
	  & ~(HH_DATA_MOD - 1)) + HH_DATA_MOD)
#define LL_RESERVED_SPACE_EXTRA(dev,extra) \
	((((dev)->hard_header_len + READ_ONCE((dev)->needed_headroom) + (extra)) \
	  & ~(HH_DATA_MOD - 1)) + HH_DATA_MOD)

struct header_ops {
	int	(*create) (struct sk_buff *skb, struct net_device *dev,
			   unsigned short type, const void *daddr,
			   const void *saddr, unsigned int len);
	int	(*parse)(const struct sk_buff *skb, unsigned char *haddr);
	int	(*cache)(const struct neighbour *neigh, struct hh_cache *hh, __be16 type);
	void	(*cache_update)(struct hh_cache *hh,
				const struct net_device *dev,
				const unsigned char *haddr);
	bool	(*validate)(const char *ll_header, unsigned int len);
	__be16	(*parse_protocol)(const struct sk_buff *skb);
};



enum netdev_state_t {
	__LINK_STATE_START,
	__LINK_STATE_PRESENT,
	__LINK_STATE_NOCARRIER,
	__LINK_STATE_LINKWATCH_PENDING,
	__LINK_STATE_DORMANT,
	__LINK_STATE_TESTING,
};

struct gro_list {
	struct list_head	list;
	int			count;
};


#define GRO_HASH_BUCKETS	8


struct napi_struct {
	
	struct list_head	poll_list;

	unsigned long		state;
	int			weight;
	int			defer_hard_irqs_count;
	unsigned long		gro_bitmask;
	int			(*poll)(struct napi_struct *, int);
#ifdef CONFIG_NETPOLL
	
	int			poll_owner;
#endif
	
	int			list_owner;
	struct net_device	*dev;
	struct gro_list		gro_hash[GRO_HASH_BUCKETS];
	struct sk_buff		*skb;
	struct list_head	rx_list; 
	int			rx_count; 
	unsigned int		napi_id;
	struct hrtimer		timer;
	struct task_struct	*thread;
	
	struct list_head	dev_list;
	struct hlist_node	napi_hash_node;
};

enum {
	NAPI_STATE_SCHED,		
	NAPI_STATE_MISSED,		
	NAPI_STATE_DISABLE,		
	NAPI_STATE_NPSVC,		
	NAPI_STATE_LISTED,		
	NAPI_STATE_NO_BUSY_POLL,	
	NAPI_STATE_IN_BUSY_POLL,	
	NAPI_STATE_PREFER_BUSY_POLL,	
	NAPI_STATE_THREADED,		
	NAPI_STATE_SCHED_THREADED,	
};

enum {
	NAPIF_STATE_SCHED		= BIT(NAPI_STATE_SCHED),
	NAPIF_STATE_MISSED		= BIT(NAPI_STATE_MISSED),
	NAPIF_STATE_DISABLE		= BIT(NAPI_STATE_DISABLE),
	NAPIF_STATE_NPSVC		= BIT(NAPI_STATE_NPSVC),
	NAPIF_STATE_LISTED		= BIT(NAPI_STATE_LISTED),
	NAPIF_STATE_NO_BUSY_POLL	= BIT(NAPI_STATE_NO_BUSY_POLL),
	NAPIF_STATE_IN_BUSY_POLL	= BIT(NAPI_STATE_IN_BUSY_POLL),
	NAPIF_STATE_PREFER_BUSY_POLL	= BIT(NAPI_STATE_PREFER_BUSY_POLL),
	NAPIF_STATE_THREADED		= BIT(NAPI_STATE_THREADED),
	NAPIF_STATE_SCHED_THREADED	= BIT(NAPI_STATE_SCHED_THREADED),
};

enum gro_result {
	GRO_MERGED,
	GRO_MERGED_FREE,
	GRO_HELD,
	GRO_NORMAL,
	GRO_CONSUMED,
};
typedef enum gro_result gro_result_t;



enum rx_handler_result {
	RX_HANDLER_CONSUMED,
	RX_HANDLER_ANOTHER,
	RX_HANDLER_EXACT,
	RX_HANDLER_PASS,
};
typedef enum rx_handler_result rx_handler_result_t;
typedef rx_handler_result_t rx_handler_func_t(struct sk_buff **pskb);

void __napi_schedule(struct napi_struct *n);
void __napi_schedule_irqoff(struct napi_struct *n);

static inline bool napi_disable_pending(struct napi_struct *n)
{
	return test_bit(NAPI_STATE_DISABLE, &n->state);
}

static inline bool napi_prefer_busy_poll(struct napi_struct *n)
{
	return test_bit(NAPI_STATE_PREFER_BUSY_POLL, &n->state);
}

bool napi_schedule_prep(struct napi_struct *n);


static inline void napi_schedule(struct napi_struct *n)
{
	if (napi_schedule_prep(n))
		__napi_schedule(n);
}


static inline void napi_schedule_irqoff(struct napi_struct *n)
{
	if (napi_schedule_prep(n))
		__napi_schedule_irqoff(n);
}


static inline bool napi_reschedule(struct napi_struct *napi)
{
	if (napi_schedule_prep(napi)) {
		__napi_schedule(napi);
		return true;
	}
	return false;
}


bool napi_complete_done(struct napi_struct *n, int work_done);

static inline bool napi_complete(struct napi_struct *n)
{
	return napi_complete_done(n, 0);
}

int dev_set_threaded(struct net_device *dev, bool threaded);


void napi_disable(struct napi_struct *n);

void napi_enable(struct napi_struct *n);


static inline void napi_synchronize(const struct napi_struct *n)
{
	if (IS_ENABLED(CONFIG_SMP))
		while (test_bit(NAPI_STATE_SCHED, &n->state))
			msleep(1);
	else
		barrier();
}


static inline bool napi_if_scheduled_mark_missed(struct napi_struct *n)
{
	unsigned long val, new;

	val = READ_ONCE(n->state);
	do {
		if (val & NAPIF_STATE_DISABLE)
			return true;

		if (!(val & NAPIF_STATE_SCHED))
			return false;

		new = val | NAPIF_STATE_MISSED;
	} while (!try_cmpxchg(&n->state, &val, new));

	return true;
}

enum netdev_queue_state_t {
	__QUEUE_STATE_DRV_XOFF,
	__QUEUE_STATE_STACK_XOFF,
	__QUEUE_STATE_FROZEN,
};

#define QUEUE_STATE_DRV_XOFF	(1 << __QUEUE_STATE_DRV_XOFF)
#define QUEUE_STATE_STACK_XOFF	(1 << __QUEUE_STATE_STACK_XOFF)
#define QUEUE_STATE_FROZEN	(1 << __QUEUE_STATE_FROZEN)

#define QUEUE_STATE_ANY_XOFF	(QUEUE_STATE_DRV_XOFF | QUEUE_STATE_STACK_XOFF)
#define QUEUE_STATE_ANY_XOFF_OR_FROZEN (QUEUE_STATE_ANY_XOFF | \
					QUEUE_STATE_FROZEN)
#define QUEUE_STATE_DRV_XOFF_OR_FROZEN (QUEUE_STATE_DRV_XOFF | \
					QUEUE_STATE_FROZEN)



struct netdev_queue {

	struct net_device	*dev;
	netdevice_tracker	dev_tracker;

	struct Qdisc __rcu	*qdisc;
	struct Qdisc __rcu	*qdisc_sleeping;
#ifdef CONFIG_SYSFS
	struct kobject		kobj;
#endif
#if defined(CONFIG_XPS) && defined(CONFIG_NUMA)
	int			numa_node;
#endif
	unsigned long		tx_maxrate;
	
	atomic_long_t		trans_timeout;

	
	struct net_device	*sb_dev;
#ifdef CONFIG_XDP_SOCKETS
	struct xsk_buff_pool    *pool;
#endif

	spinlock_t		_xmit_lock ____cacheline_aligned_in_smp;
	int			xmit_lock_owner;
	
	unsigned long		trans_start;

	unsigned long		state;

#ifdef CONFIG_BQL
	struct dql		dql;
#endif
} ____cacheline_aligned_in_smp;

extern int sysctl_fb_tunnels_only_for_init_net;
extern int sysctl_devconf_inherit_init_net;


static inline bool net_has_fallback_tunnels(const struct net *net)
{
#if IS_ENABLED(CONFIG_SYSCTL)
	int fb_tunnels_only_for_init_net = READ_ONCE(sysctl_fb_tunnels_only_for_init_net);

	return !fb_tunnels_only_for_init_net ||
		(net_eq(net, &init_net) && fb_tunnels_only_for_init_net == 1);
#else
	return true;
#endif
}

static inline int net_inherit_devconf(void)
{
#if IS_ENABLED(CONFIG_SYSCTL)
	return READ_ONCE(sysctl_devconf_inherit_init_net);
#else
	return 0;
#endif
}

static inline int netdev_queue_numa_node_read(const struct netdev_queue *q)
{
#if defined(CONFIG_XPS) && defined(CONFIG_NUMA)
	return q->numa_node;
#else
	return NUMA_NO_NODE;
#endif
}

static inline void netdev_queue_numa_node_write(struct netdev_queue *q, int node)
{
#if defined(CONFIG_XPS) && defined(CONFIG_NUMA)
	q->numa_node = node;
#endif
}

#ifdef CONFIG_RPS

struct rps_map {
	unsigned int len;
	struct rcu_head rcu;
	u16 cpus[];
};
#define RPS_MAP_SIZE(_num) (sizeof(struct rps_map) + ((_num) * sizeof(u16)))


struct rps_dev_flow {
	u16 cpu;
	u16 filter;
	unsigned int last_qtail;
};
#define RPS_NO_FILTER 0xffff


struct rps_dev_flow_table {
	unsigned int mask;
	struct rcu_head rcu;
	struct rps_dev_flow flows[];
};
#define RPS_DEV_FLOW_TABLE_SIZE(_num) (sizeof(struct rps_dev_flow_table) + \
    ((_num) * sizeof(struct rps_dev_flow)))


struct rps_sock_flow_table {
	u32	mask;

	u32	ents[] ____cacheline_aligned_in_smp;
};
#define	RPS_SOCK_FLOW_TABLE_SIZE(_num) (offsetof(struct rps_sock_flow_table, ents[_num]))

#define RPS_NO_CPU 0xffff

extern u32 rps_cpu_mask;
extern struct rps_sock_flow_table __rcu *rps_sock_flow_table;

static inline void rps_record_sock_flow(struct rps_sock_flow_table *table,
					u32 hash)
{
	if (table && hash) {
		unsigned int index = hash & table->mask;
		u32 val = hash & ~rps_cpu_mask;

		
		val |= raw_smp_processor_id();

		
		if (READ_ONCE(table->ents[index]) != val)
			WRITE_ONCE(table->ents[index], val);
	}
}

#ifdef CONFIG_RFS_ACCEL
bool rps_may_expire_flow(struct net_device *dev, u16 rxq_index, u32 flow_id,
			 u16 filter_id);
#endif
#endif 


enum xps_map_type {
	XPS_CPUS = 0,
	XPS_RXQS,
	XPS_MAPS_MAX,
};

#ifdef CONFIG_XPS

struct xps_map {
	unsigned int len;
	unsigned int alloc_len;
	struct rcu_head rcu;
	u16 queues[];
};
#define XPS_MAP_SIZE(_num) (sizeof(struct xps_map) + ((_num) * sizeof(u16)))
#define XPS_MIN_MAP_ALLOC ((L1_CACHE_ALIGN(offsetof(struct xps_map, queues[1])) \
       - sizeof(struct xps_map)) / sizeof(u16))


struct xps_dev_maps {
	struct rcu_head rcu;
	unsigned int nr_ids;
	s16 num_tc;
	struct xps_map __rcu *attr_map[]; 
};

#define XPS_CPU_DEV_MAPS_SIZE(_tcs) (sizeof(struct xps_dev_maps) +	\
	(nr_cpu_ids * (_tcs) * sizeof(struct xps_map *)))

#define XPS_RXQ_DEV_MAPS_SIZE(_tcs, _rxqs) (sizeof(struct xps_dev_maps) +\
	(_rxqs * (_tcs) * sizeof(struct xps_map *)))

#endif 

#define TC_MAX_QUEUE	16
#define TC_BITMASK	15

struct netdev_tc_txq {
	u16 count;
	u16 offset;
};

#if defined(CONFIG_FCOE) || defined(CONFIG_FCOE_MODULE)

struct netdev_fcoe_hbainfo {
	char	manufacturer[64];
	char	serial_number[64];
	char	hardware_version[64];
	char	driver_version[64];
	char	optionrom_version[64];
	char	firmware_version[64];
	char	model[256];
	char	model_description[256];
};
#endif

#define MAX_PHYS_ITEM_ID_LEN 32


struct netdev_phys_item_id {
	unsigned char id[MAX_PHYS_ITEM_ID_LEN];
	unsigned char id_len;
};

static inline bool netdev_phys_item_id_same(struct netdev_phys_item_id *a,
					    struct netdev_phys_item_id *b)
{
	return a->id_len == b->id_len &&
	       memcmp(a->id, b->id, a->id_len) == 0;
}

typedef u16 (*select_queue_fallback_t)(struct net_device *dev,
				       struct sk_buff *skb,
				       struct net_device *sb_dev);

enum net_device_path_type {
	DEV_PATH_ETHERNET = 0,
	DEV_PATH_VLAN,
	DEV_PATH_BRIDGE,
	DEV_PATH_PPPOE,
	DEV_PATH_DSA,
	DEV_PATH_MTK_WDMA,
};

struct net_device_path {
	enum net_device_path_type	type;
	const struct net_device		*dev;
	union {
		struct {
			u16		id;
			__be16		proto;
			u8		h_dest[ETH_ALEN];
		} encap;
		struct {
			enum {
				DEV_PATH_BR_VLAN_KEEP,
				DEV_PATH_BR_VLAN_TAG,
				DEV_PATH_BR_VLAN_UNTAG,
				DEV_PATH_BR_VLAN_UNTAG_HW,
			}		vlan_mode;
			u16		vlan_id;
			__be16		vlan_proto;
		} bridge;
		struct {
			int port;
			u16 proto;
		} dsa;
		struct {
			u8 wdma_idx;
			u8 queue;
			u16 wcid;
			u8 bss;
		} mtk_wdma;
	};
};

#define NET_DEVICE_PATH_STACK_MAX	5
#define NET_DEVICE_PATH_VLAN_MAX	2

struct net_device_path_stack {
	int			num_paths;
	struct net_device_path	path[NET_DEVICE_PATH_STACK_MAX];
};

struct net_device_path_ctx {
	const struct net_device *dev;
	u8			daddr[ETH_ALEN];

	int			num_vlans;
	struct {
		u16		id;
		__be16		proto;
	} vlan[NET_DEVICE_PATH_VLAN_MAX];
};

enum tc_setup_type {
	TC_QUERY_CAPS,
	TC_SETUP_QDISC_MQPRIO,
	TC_SETUP_CLSU32,
	TC_SETUP_CLSFLOWER,
	TC_SETUP_CLSMATCHALL,
	TC_SETUP_CLSBPF,
	TC_SETUP_BLOCK,
	TC_SETUP_QDISC_CBS,
	TC_SETUP_QDISC_RED,
	TC_SETUP_QDISC_PRIO,
	TC_SETUP_QDISC_MQ,
	TC_SETUP_QDISC_ETF,
	TC_SETUP_ROOT_QDISC,
	TC_SETUP_QDISC_GRED,
	TC_SETUP_QDISC_TAPRIO,
	TC_SETUP_FT,
	TC_SETUP_QDISC_ETS,
	TC_SETUP_QDISC_TBF,
	TC_SETUP_QDISC_FIFO,
	TC_SETUP_QDISC_HTB,
	TC_SETUP_ACT,
};


enum bpf_netdev_command {
	
	XDP_SETUP_PROG,
	XDP_SETUP_PROG_HW,
	
	BPF_OFFLOAD_MAP_ALLOC,
	BPF_OFFLOAD_MAP_FREE,
	XDP_SETUP_XSK_POOL,
};

struct bpf_prog_offload_ops;
struct netlink_ext_ack;
struct xdp_umem;
struct xdp_dev_bulk_queue;
struct bpf_xdp_link;

enum bpf_xdp_mode {
	XDP_MODE_SKB = 0,
	XDP_MODE_DRV = 1,
	XDP_MODE_HW = 2,
	__MAX_XDP_MODE
};

struct bpf_xdp_entity {
	struct bpf_prog *prog;
	struct bpf_xdp_link *link;
};

struct netdev_bpf {
	enum bpf_netdev_command command;
	union {
		
		struct {
			u32 flags;
			struct bpf_prog *prog;
			struct netlink_ext_ack *extack;
		};
		
		struct {
			struct bpf_offloaded_map *offmap;
		};
		
		struct {
			struct xsk_buff_pool *pool;
			u16 queue_id;
		} xsk;
	};
};


#define XDP_WAKEUP_RX (1 << 0)
#define XDP_WAKEUP_TX (1 << 1)

#ifdef CONFIG_XFRM_OFFLOAD
struct xfrmdev_ops {
	int	(*xdo_dev_state_add) (struct xfrm_state *x, struct netlink_ext_ack *extack);
	void	(*xdo_dev_state_delete) (struct xfrm_state *x);
	void	(*xdo_dev_state_free) (struct xfrm_state *x);
	bool	(*xdo_dev_offload_ok) (struct sk_buff *skb,
				       struct xfrm_state *x);
	void	(*xdo_dev_state_advance_esn) (struct xfrm_state *x);
	void	(*xdo_dev_state_update_curlft) (struct xfrm_state *x);
	int	(*xdo_dev_policy_add) (struct xfrm_policy *x, struct netlink_ext_ack *extack);
	void	(*xdo_dev_policy_delete) (struct xfrm_policy *x);
	void	(*xdo_dev_policy_free) (struct xfrm_policy *x);
};
#endif

struct dev_ifalias {
	struct rcu_head rcuhead;
	char ifalias[];
};

struct devlink;
struct tlsdev_ops;

struct netdev_net_notifier {
	struct list_head list;
	struct notifier_block *nb;
};


struct net_device_ops {
	int			(*ndo_init)(struct net_device *dev);
	void			(*ndo_uninit)(struct net_device *dev);
	int			(*ndo_open)(struct net_device *dev);
	int			(*ndo_stop)(struct net_device *dev);
	netdev_tx_t		(*ndo_start_xmit)(struct sk_buff *skb,
						  struct net_device *dev);
	netdev_features_t	(*ndo_features_check)(struct sk_buff *skb,
						      struct net_device *dev,
						      netdev_features_t features);
	u16			(*ndo_select_queue)(struct net_device *dev,
						    struct sk_buff *skb,
						    struct net_device *sb_dev);
	void			(*ndo_change_rx_flags)(struct net_device *dev,
						       int flags);
	void			(*ndo_set_rx_mode)(struct net_device *dev);
	int			(*ndo_set_mac_address)(struct net_device *dev,
						       void *addr);
	int			(*ndo_validate_addr)(struct net_device *dev);
	int			(*ndo_do_ioctl)(struct net_device *dev,
					        struct ifreq *ifr, int cmd);
	int			(*ndo_eth_ioctl)(struct net_device *dev,
						 struct ifreq *ifr, int cmd);
	int			(*ndo_siocbond)(struct net_device *dev,
						struct ifreq *ifr, int cmd);
	int			(*ndo_siocwandev)(struct net_device *dev,
						  struct if_settings *ifs);
	int			(*ndo_siocdevprivate)(struct net_device *dev,
						      struct ifreq *ifr,
						      void __user *data, int cmd);
	int			(*ndo_set_config)(struct net_device *dev,
					          struct ifmap *map);
	int			(*ndo_change_mtu)(struct net_device *dev,
						  int new_mtu);
	int			(*ndo_neigh_setup)(struct net_device *dev,
						   struct neigh_parms *);
	void			(*ndo_tx_timeout) (struct net_device *dev,
						   unsigned int txqueue);

	void			(*ndo_get_stats64)(struct net_device *dev,
						   struct rtnl_link_stats64 *storage);
	bool			(*ndo_has_offload_stats)(const struct net_device *dev, int attr_id);
	int			(*ndo_get_offload_stats)(int attr_id,
							 const struct net_device *dev,
							 void *attr_data);
	struct net_device_stats* (*ndo_get_stats)(struct net_device *dev);

	int			(*ndo_vlan_rx_add_vid)(struct net_device *dev,
						       __be16 proto, u16 vid);
	int			(*ndo_vlan_rx_kill_vid)(struct net_device *dev,
						        __be16 proto, u16 vid);
#ifdef CONFIG_NET_POLL_CONTROLLER
	void                    (*ndo_poll_controller)(struct net_device *dev);
	int			(*ndo_netpoll_setup)(struct net_device *dev,
						     struct netpoll_info *info);
	void			(*ndo_netpoll_cleanup)(struct net_device *dev);
#endif
	int			(*ndo_set_vf_mac)(struct net_device *dev,
						  int queue, u8 *mac);
	int			(*ndo_set_vf_vlan)(struct net_device *dev,
						   int queue, u16 vlan,
						   u8 qos, __be16 proto);
	int			(*ndo_set_vf_rate)(struct net_device *dev,
						   int vf, int min_tx_rate,
						   int max_tx_rate);
	int			(*ndo_set_vf_spoofchk)(struct net_device *dev,
						       int vf, bool setting);
	int			(*ndo_set_vf_trust)(struct net_device *dev,
						    int vf, bool setting);
	int			(*ndo_get_vf_config)(struct net_device *dev,
						     int vf,
						     struct ifla_vf_info *ivf);
	int			(*ndo_set_vf_link_state)(struct net_device *dev,
							 int vf, int link_state);
	int			(*ndo_get_vf_stats)(struct net_device *dev,
						    int vf,
						    struct ifla_vf_stats
						    *vf_stats);
	int			(*ndo_set_vf_port)(struct net_device *dev,
						   int vf,
						   struct nlattr *port[]);
	int			(*ndo_get_vf_port)(struct net_device *dev,
						   int vf, struct sk_buff *skb);
	int			(*ndo_get_vf_guid)(struct net_device *dev,
						   int vf,
						   struct ifla_vf_guid *node_guid,
						   struct ifla_vf_guid *port_guid);
	int			(*ndo_set_vf_guid)(struct net_device *dev,
						   int vf, u64 guid,
						   int guid_type);
	int			(*ndo_set_vf_rss_query_en)(
						   struct net_device *dev,
						   int vf, bool setting);
	int			(*ndo_setup_tc)(struct net_device *dev,
						enum tc_setup_type type,
						void *type_data);
#if IS_ENABLED(CONFIG_FCOE)
	int			(*ndo_fcoe_enable)(struct net_device *dev);
	int			(*ndo_fcoe_disable)(struct net_device *dev);
	int			(*ndo_fcoe_ddp_setup)(struct net_device *dev,
						      u16 xid,
						      struct scatterlist *sgl,
						      unsigned int sgc);
	int			(*ndo_fcoe_ddp_done)(struct net_device *dev,
						     u16 xid);
	int			(*ndo_fcoe_ddp_target)(struct net_device *dev,
						       u16 xid,
						       struct scatterlist *sgl,
						       unsigned int sgc);
	int			(*ndo_fcoe_get_hbainfo)(struct net_device *dev,
							struct netdev_fcoe_hbainfo *hbainfo);
#endif

#if IS_ENABLED(CONFIG_LIBFCOE)
#define NETDEV_FCOE_WWNN 0
#define NETDEV_FCOE_WWPN 1
	int			(*ndo_fcoe_get_wwn)(struct net_device *dev,
						    u64 *wwn, int type);
#endif

#ifdef CONFIG_RFS_ACCEL
	int			(*ndo_rx_flow_steer)(struct net_device *dev,
						     const struct sk_buff *skb,
						     u16 rxq_index,
						     u32 flow_id);
#endif
	int			(*ndo_add_slave)(struct net_device *dev,
						 struct net_device *slave_dev,
						 struct netlink_ext_ack *extack);
	int			(*ndo_del_slave)(struct net_device *dev,
						 struct net_device *slave_dev);
	struct net_device*	(*ndo_get_xmit_slave)(struct net_device *dev,
						      struct sk_buff *skb,
						      bool all_slaves);
	struct net_device*	(*ndo_sk_get_lower_dev)(struct net_device *dev,
							struct sock *sk);
	netdev_features_t	(*ndo_fix_features)(struct net_device *dev,
						    netdev_features_t features);
	int			(*ndo_set_features)(struct net_device *dev,
						    netdev_features_t features);
	int			(*ndo_neigh_construct)(struct net_device *dev,
						       struct neighbour *n);
	void			(*ndo_neigh_destroy)(struct net_device *dev,
						     struct neighbour *n);

	int			(*ndo_fdb_add)(struct ndmsg *ndm,
					       struct nlattr *tb[],
					       struct net_device *dev,
					       const unsigned char *addr,
					       u16 vid,
					       u16 flags,
					       struct netlink_ext_ack *extack);
	int			(*ndo_fdb_del)(struct ndmsg *ndm,
					       struct nlattr *tb[],
					       struct net_device *dev,
					       const unsigned char *addr,
					       u16 vid, struct netlink_ext_ack *extack);
	int			(*ndo_fdb_del_bulk)(struct ndmsg *ndm,
						    struct nlattr *tb[],
						    struct net_device *dev,
						    u16 vid,
						    struct netlink_ext_ack *extack);
	int			(*ndo_fdb_dump)(struct sk_buff *skb,
						struct netlink_callback *cb,
						struct net_device *dev,
						struct net_device *filter_dev,
						int *idx);
	int			(*ndo_fdb_get)(struct sk_buff *skb,
					       struct nlattr *tb[],
					       struct net_device *dev,
					       const unsigned char *addr,
					       u16 vid, u32 portid, u32 seq,
					       struct netlink_ext_ack *extack);
	int			(*ndo_mdb_add)(struct net_device *dev,
					       struct nlattr *tb[],
					       u16 nlmsg_flags,
					       struct netlink_ext_ack *extack);
	int			(*ndo_mdb_del)(struct net_device *dev,
					       struct nlattr *tb[],
					       struct netlink_ext_ack *extack);
	int			(*ndo_mdb_dump)(struct net_device *dev,
						struct sk_buff *skb,
						struct netlink_callback *cb);
	int			(*ndo_bridge_setlink)(struct net_device *dev,
						      struct nlmsghdr *nlh,
						      u16 flags,
						      struct netlink_ext_ack *extack);
	int			(*ndo_bridge_getlink)(struct sk_buff *skb,
						      u32 pid, u32 seq,
						      struct net_device *dev,
						      u32 filter_mask,
						      int nlflags);
	int			(*ndo_bridge_dellink)(struct net_device *dev,
						      struct nlmsghdr *nlh,
						      u16 flags);
	int			(*ndo_change_carrier)(struct net_device *dev,
						      bool new_carrier);
	int			(*ndo_get_phys_port_id)(struct net_device *dev,
							struct netdev_phys_item_id *ppid);
	int			(*ndo_get_port_parent_id)(struct net_device *dev,
							  struct netdev_phys_item_id *ppid);
	int			(*ndo_get_phys_port_name)(struct net_device *dev,
							  char *name, size_t len);
	void*			(*ndo_dfwd_add_station)(struct net_device *pdev,
							struct net_device *dev);
	void			(*ndo_dfwd_del_station)(struct net_device *pdev,
							void *priv);

	int			(*ndo_set_tx_maxrate)(struct net_device *dev,
						      int queue_index,
						      u32 maxrate);
	int			(*ndo_get_iflink)(const struct net_device *dev);
	int			(*ndo_fill_metadata_dst)(struct net_device *dev,
						       struct sk_buff *skb);
	void			(*ndo_set_rx_headroom)(struct net_device *dev,
						       int needed_headroom);
	int			(*ndo_bpf)(struct net_device *dev,
					   struct netdev_bpf *bpf);
	int			(*ndo_xdp_xmit)(struct net_device *dev, int n,
						struct xdp_frame **xdp,
						u32 flags);
	struct net_device *	(*ndo_xdp_get_xmit_slave)(struct net_device *dev,
							  struct xdp_buff *xdp);
	int			(*ndo_xsk_wakeup)(struct net_device *dev,
						  u32 queue_id, u32 flags);
	int			(*ndo_tunnel_ctl)(struct net_device *dev,
						  struct ip_tunnel_parm *p, int cmd);
	struct net_device *	(*ndo_get_peer_dev)(struct net_device *dev);
	int                     (*ndo_fill_forward_path)(struct net_device_path_ctx *ctx,
                                                         struct net_device_path *path);
	ktime_t			(*ndo_get_tstamp)(struct net_device *dev,
						  const struct skb_shared_hwtstamps *hwtstamps,
						  bool cycles);
	int			(*ndo_hwtstamp_get)(struct net_device *dev,
						    struct kernel_hwtstamp_config *kernel_config);
	int			(*ndo_hwtstamp_set)(struct net_device *dev,
						    struct kernel_hwtstamp_config *kernel_config,
						    struct netlink_ext_ack *extack);
};


enum netdev_priv_flags {
	IFF_802_1Q_VLAN			= 1<<0,
	IFF_EBRIDGE			= 1<<1,
	IFF_BONDING			= 1<<2,
	IFF_ISATAP			= 1<<3,
	IFF_WAN_HDLC			= 1<<4,
	IFF_XMIT_DST_RELEASE		= 1<<5,
	IFF_DONT_BRIDGE			= 1<<6,
	IFF_DISABLE_NETPOLL		= 1<<7,
	IFF_MACVLAN_PORT		= 1<<8,
	IFF_BRIDGE_PORT			= 1<<9,
	IFF_OVS_DATAPATH		= 1<<10,
	IFF_TX_SKB_SHARING		= 1<<11,
	IFF_UNICAST_FLT			= 1<<12,
	IFF_TEAM_PORT			= 1<<13,
	IFF_SUPP_NOFCS			= 1<<14,
	IFF_LIVE_ADDR_CHANGE		= 1<<15,
	IFF_MACVLAN			= 1<<16,
	IFF_XMIT_DST_RELEASE_PERM	= 1<<17,
	IFF_L3MDEV_MASTER		= 1<<18,
	IFF_NO_QUEUE			= 1<<19,
	IFF_OPENVSWITCH			= 1<<20,
	IFF_L3MDEV_SLAVE		= 1<<21,
	IFF_TEAM			= 1<<22,
	IFF_RXFH_CONFIGURED		= 1<<23,
	IFF_PHONY_HEADROOM		= 1<<24,
	IFF_MACSEC			= 1<<25,
	IFF_NO_RX_HANDLER		= 1<<26,
	IFF_FAILOVER			= 1<<27,
	IFF_FAILOVER_SLAVE		= 1<<28,
	IFF_L3MDEV_RX_HANDLER		= 1<<29,
	IFF_NO_ADDRCONF			= BIT_ULL(30),
	IFF_TX_SKB_NO_LINEAR		= BIT_ULL(31),
	IFF_CHANGE_PROTO_DOWN		= BIT_ULL(32),
	IFF_SEE_ALL_HWTSTAMP_REQUESTS	= BIT_ULL(33),
};

#define IFF_802_1Q_VLAN			IFF_802_1Q_VLAN
#define IFF_EBRIDGE			IFF_EBRIDGE
#define IFF_BONDING			IFF_BONDING
#define IFF_ISATAP			IFF_ISATAP
#define IFF_WAN_HDLC			IFF_WAN_HDLC
#define IFF_XMIT_DST_RELEASE		IFF_XMIT_DST_RELEASE
#define IFF_DONT_BRIDGE			IFF_DONT_BRIDGE
#define IFF_DISABLE_NETPOLL		IFF_DISABLE_NETPOLL
#define IFF_MACVLAN_PORT		IFF_MACVLAN_PORT
#define IFF_BRIDGE_PORT			IFF_BRIDGE_PORT
#define IFF_OVS_DATAPATH		IFF_OVS_DATAPATH
#define IFF_TX_SKB_SHARING		IFF_TX_SKB_SHARING
#define IFF_UNICAST_FLT			IFF_UNICAST_FLT
#define IFF_TEAM_PORT			IFF_TEAM_PORT
#define IFF_SUPP_NOFCS			IFF_SUPP_NOFCS
#define IFF_LIVE_ADDR_CHANGE		IFF_LIVE_ADDR_CHANGE
#define IFF_MACVLAN			IFF_MACVLAN
#define IFF_XMIT_DST_RELEASE_PERM	IFF_XMIT_DST_RELEASE_PERM
#define IFF_L3MDEV_MASTER		IFF_L3MDEV_MASTER
#define IFF_NO_QUEUE			IFF_NO_QUEUE
#define IFF_OPENVSWITCH			IFF_OPENVSWITCH
#define IFF_L3MDEV_SLAVE		IFF_L3MDEV_SLAVE
#define IFF_TEAM			IFF_TEAM
#define IFF_RXFH_CONFIGURED		IFF_RXFH_CONFIGURED
#define IFF_PHONY_HEADROOM		IFF_PHONY_HEADROOM
#define IFF_MACSEC			IFF_MACSEC
#define IFF_NO_RX_HANDLER		IFF_NO_RX_HANDLER
#define IFF_FAILOVER			IFF_FAILOVER
#define IFF_FAILOVER_SLAVE		IFF_FAILOVER_SLAVE
#define IFF_L3MDEV_RX_HANDLER		IFF_L3MDEV_RX_HANDLER
#define IFF_TX_SKB_NO_LINEAR		IFF_TX_SKB_NO_LINEAR


enum netdev_ml_priv_type {
	ML_PRIV_NONE,
	ML_PRIV_CAN,
};

enum netdev_stat_type {
	NETDEV_PCPU_STAT_NONE,
	NETDEV_PCPU_STAT_LSTATS, 
	NETDEV_PCPU_STAT_TSTATS, 
	NETDEV_PCPU_STAT_DSTATS, 
};



struct net_device {
	char			name[IFNAMSIZ];
	struct netdev_name_node	*name_node;
	struct dev_ifalias	__rcu *ifalias;
	
	unsigned long		mem_end;
	unsigned long		mem_start;
	unsigned long		base_addr;

	

	unsigned long		state;

	struct list_head	dev_list;
	struct list_head	napi_list;
	struct list_head	unreg_list;
	struct list_head	close_list;
	struct list_head	ptype_all;
	struct list_head	ptype_specific;

	struct {
		struct list_head upper;
		struct list_head lower;
	} adj_list;

	
	unsigned int		flags;
	xdp_features_t		xdp_features;
	unsigned long long	priv_flags;
	const struct net_device_ops *netdev_ops;
	const struct xdp_metadata_ops *xdp_metadata_ops;
	int			ifindex;
	unsigned short		gflags;
	unsigned short		hard_header_len;

	
	unsigned int		mtu;
	unsigned short		needed_headroom;
	unsigned short		needed_tailroom;

	netdev_features_t	features;
	netdev_features_t	hw_features;
	netdev_features_t	wanted_features;
	netdev_features_t	vlan_features;
	netdev_features_t	hw_enc_features;
	netdev_features_t	mpls_features;
	netdev_features_t	gso_partial_features;

	unsigned int		min_mtu;
	unsigned int		max_mtu;
	unsigned short		type;
	unsigned char		min_header_len;
	unsigned char		name_assign_type;

	int			group;

	struct net_device_stats	stats; 

	struct net_device_core_stats __percpu *core_stats;

	
	atomic_t		carrier_up_count;
	atomic_t		carrier_down_count;

#ifdef CONFIG_WIRELESS_EXT
	const struct iw_handler_def *wireless_handlers;
	struct iw_public_data	*wireless_data;
#endif
	const struct ethtool_ops *ethtool_ops;
#ifdef CONFIG_NET_L3_MASTER_DEV
	const struct l3mdev_ops	*l3mdev_ops;
#endif
#if IS_ENABLED(CONFIG_IPV6)
	const struct ndisc_ops *ndisc_ops;
#endif

#ifdef CONFIG_XFRM_OFFLOAD
	const struct xfrmdev_ops *xfrmdev_ops;
#endif

#if IS_ENABLED(CONFIG_TLS_DEVICE)
	const struct tlsdev_ops *tlsdev_ops;
#endif

	const struct header_ops *header_ops;

	unsigned char		operstate;
	unsigned char		link_mode;

	unsigned char		if_port;
	unsigned char		dma;

	
	unsigned char		perm_addr[MAX_ADDR_LEN];
	unsigned char		addr_assign_type;
	unsigned char		addr_len;
	unsigned char		upper_level;
	unsigned char		lower_level;

	unsigned short		neigh_priv_len;
	unsigned short          dev_id;
	unsigned short          dev_port;
	unsigned short		padded;

	spinlock_t		addr_list_lock;
	int			irq;

	struct netdev_hw_addr_list	uc;
	struct netdev_hw_addr_list	mc;
	struct netdev_hw_addr_list	dev_addrs;

#ifdef CONFIG_SYSFS
	struct kset		*queues_kset;
#endif
#ifdef CONFIG_LOCKDEP
	struct list_head	unlink_list;
#endif
	unsigned int		promiscuity;
	unsigned int		allmulti;
	bool			uc_promisc;
#ifdef CONFIG_LOCKDEP
	unsigned char		nested_level;
#endif


	

	struct in_device __rcu	*ip_ptr;
	struct inet6_dev __rcu	*ip6_ptr;
#if IS_ENABLED(CONFIG_VLAN_8021Q)
	struct vlan_info __rcu	*vlan_info;
#endif
#if IS_ENABLED(CONFIG_NET_DSA)
	struct dsa_port		*dsa_ptr;
#endif
#if IS_ENABLED(CONFIG_TIPC)
	struct tipc_bearer __rcu *tipc_ptr;
#endif
#if IS_ENABLED(CONFIG_ATALK)
	void 			*atalk_ptr;
#endif
#if IS_ENABLED(CONFIG_AX25)
	void			*ax25_ptr;
#endif
#if IS_ENABLED(CONFIG_CFG80211)
	struct wireless_dev	*ieee80211_ptr;
#endif
#if IS_ENABLED(CONFIG_IEEE802154) || IS_ENABLED(CONFIG_6LOWPAN)
	struct wpan_dev		*ieee802154_ptr;
#endif
#if IS_ENABLED(CONFIG_MPLS_ROUTING)
	struct mpls_dev __rcu	*mpls_ptr;
#endif
#if IS_ENABLED(CONFIG_MCTP)
	struct mctp_dev __rcu	*mctp_ptr;
#endif


	
	const unsigned char	*dev_addr;

	struct netdev_rx_queue	*_rx;
	unsigned int		num_rx_queues;
	unsigned int		real_num_rx_queues;

	struct bpf_prog __rcu	*xdp_prog;
	unsigned long		gro_flush_timeout;
	int			napi_defer_hard_irqs;
#define GRO_LEGACY_MAX_SIZE	65536u

#define GRO_MAX_SIZE		(8 * 65535u)
	unsigned int		gro_max_size;
	unsigned int		gro_ipv4_max_size;
	unsigned int		xdp_zc_max_segs;
	rx_handler_func_t __rcu	*rx_handler;
	void __rcu		*rx_handler_data;
#ifdef CONFIG_NET_XGRESS
	struct bpf_mprog_entry __rcu *tcx_ingress;
#endif
	struct netdev_queue __rcu *ingress_queue;
#ifdef CONFIG_NETFILTER_INGRESS
	struct nf_hook_entries __rcu *nf_hooks_ingress;
#endif

	unsigned char		broadcast[MAX_ADDR_LEN];
#ifdef CONFIG_RFS_ACCEL
	struct cpu_rmap		*rx_cpu_rmap;
#endif
	struct hlist_node	index_hlist;


	struct netdev_queue	*_tx ____cacheline_aligned_in_smp;
	unsigned int		num_tx_queues;
	unsigned int		real_num_tx_queues;
	struct Qdisc __rcu	*qdisc;
	unsigned int		tx_queue_len;
	spinlock_t		tx_global_lock;

	struct xdp_dev_bulk_queue __percpu *xdp_bulkq;

#ifdef CONFIG_XPS
	struct xps_dev_maps __rcu *xps_maps[XPS_MAPS_MAX];
#endif
#ifdef CONFIG_NET_XGRESS
	struct bpf_mprog_entry __rcu *tcx_egress;
#endif
#ifdef CONFIG_NETFILTER_EGRESS
	struct nf_hook_entries __rcu *nf_hooks_egress;
#endif

#ifdef CONFIG_NET_SCHED
	DECLARE_HASHTABLE	(qdisc_hash, 4);
#endif
	
	struct timer_list	watchdog_timer;
	int			watchdog_timeo;

	u32                     proto_down_reason;

	struct list_head	todo_list;

#ifdef CONFIG_PCPU_DEV_REFCNT
	int __percpu		*pcpu_refcnt;
#else
	refcount_t		dev_refcnt;
#endif
	struct ref_tracker_dir	refcnt_tracker;

	struct list_head	link_watch_list;

	enum { NETREG_UNINITIALIZED=0,
	       NETREG_REGISTERED,	
	       NETREG_UNREGISTERING,	
	       NETREG_UNREGISTERED,	
	       NETREG_RELEASED,		
	       NETREG_DUMMY,		
	} reg_state:8;

	bool dismantle;

	enum {
		RTNL_LINK_INITIALIZED,
		RTNL_LINK_INITIALIZING,
	} rtnl_link_state:16;

	bool needs_free_netdev;
	void (*priv_destructor)(struct net_device *dev);

#ifdef CONFIG_NETPOLL
	struct netpoll_info __rcu	*npinfo;
#endif

	possible_net_t			nd_net;

	
	void				*ml_priv;
	enum netdev_ml_priv_type	ml_priv_type;

	enum netdev_stat_type		pcpu_stat_type:8;
	union {
		struct pcpu_lstats __percpu		*lstats;
		struct pcpu_sw_netstats __percpu	*tstats;
		struct pcpu_dstats __percpu		*dstats;
	};

#if IS_ENABLED(CONFIG_GARP)
	struct garp_port __rcu	*garp_port;
#endif
#if IS_ENABLED(CONFIG_MRP)
	struct mrp_port __rcu	*mrp_port;
#endif
#if IS_ENABLED(CONFIG_NET_DROP_MONITOR)
	struct dm_hw_stat_delta __rcu *dm_private;
#endif
	struct device		dev;
	const struct attribute_group *sysfs_groups[4];
	const struct attribute_group *sysfs_rx_queue_group;

	const struct rtnl_link_ops *rtnl_link_ops;

	
#define GSO_MAX_SEGS		65535u
#define GSO_LEGACY_MAX_SIZE	65536u

#define GSO_MAX_SIZE		(8 * GSO_MAX_SEGS)

	unsigned int		gso_max_size;
#define TSO_LEGACY_MAX_SIZE	65536
#define TSO_MAX_SIZE		UINT_MAX
	unsigned int		tso_max_size;
	u16			gso_max_segs;
#define TSO_MAX_SEGS		U16_MAX
	u16			tso_max_segs;
	unsigned int		gso_ipv4_max_size;

#ifdef CONFIG_DCB
	const struct dcbnl_rtnl_ops *dcbnl_ops;
#endif
	s16			num_tc;
	struct netdev_tc_txq	tc_to_txq[TC_MAX_QUEUE];
	u8			prio_tc_map[TC_BITMASK + 1];

#if IS_ENABLED(CONFIG_FCOE)
	unsigned int		fcoe_ddp_xid;
#endif
#if IS_ENABLED(CONFIG_CGROUP_NET_PRIO)
	struct netprio_map __rcu *priomap;
#endif
	struct phy_device	*phydev;
	struct sfp_bus		*sfp_bus;
	struct lock_class_key	*qdisc_tx_busylock;
	bool			proto_down;
	unsigned		wol_enabled:1;
	unsigned		threaded:1;

	struct list_head	net_notifier_list;

#if IS_ENABLED(CONFIG_MACSEC)
	
	const struct macsec_ops *macsec_ops;
#endif
	const struct udp_tunnel_nic_info	*udp_tunnel_nic_info;
	struct udp_tunnel_nic	*udp_tunnel_nic;

	
	struct bpf_xdp_entity	xdp_state[__MAX_XDP_MODE];

	u8 dev_addr_shadow[MAX_ADDR_LEN];
	netdevice_tracker	linkwatch_dev_tracker;
	netdevice_tracker	watchdog_dev_tracker;
	netdevice_tracker	dev_registered_tracker;
	struct rtnl_hw_stats64	*offload_xstats_l3;

	struct devlink_port	*devlink_port;
};
#define to_net_dev(d) container_of(d, struct net_device, dev)


#define SET_NETDEV_DEVLINK_PORT(dev, port)			\
({								\
	WARN_ON((dev)->reg_state != NETREG_UNINITIALIZED);	\
	((dev)->devlink_port = (port));				\
})

static inline bool netif_elide_gro(const struct net_device *dev)
{
	if (!(dev->features & NETIF_F_GRO) || dev->xdp_prog)
		return true;
	return false;
}

#define	NETDEV_ALIGN		32

static inline
int netdev_get_prio_tc_map(const struct net_device *dev, u32 prio)
{
	return dev->prio_tc_map[prio & TC_BITMASK];
}

static inline
int netdev_set_prio_tc_map(struct net_device *dev, u8 prio, u8 tc)
{
	if (tc >= dev->num_tc)
		return -EINVAL;

	dev->prio_tc_map[prio & TC_BITMASK] = tc & TC_BITMASK;
	return 0;
}

int netdev_txq_to_tc(struct net_device *dev, unsigned int txq);
void netdev_reset_tc(struct net_device *dev);
int netdev_set_tc_queue(struct net_device *dev, u8 tc, u16 count, u16 offset);
int netdev_set_num_tc(struct net_device *dev, u8 num_tc);

static inline
int netdev_get_num_tc(struct net_device *dev)
{
	return dev->num_tc;
}

static inline void net_prefetch(void *p)
{
	prefetch(p);
#if L1_CACHE_BYTES < 128
	prefetch((u8 *)p + L1_CACHE_BYTES);
#endif
}

static inline void net_prefetchw(void *p)
{
	prefetchw(p);
#if L1_CACHE_BYTES < 128
	prefetchw((u8 *)p + L1_CACHE_BYTES);
#endif
}

void netdev_unbind_sb_channel(struct net_device *dev,
			      struct net_device *sb_dev);
int netdev_bind_sb_channel_queue(struct net_device *dev,
				 struct net_device *sb_dev,
				 u8 tc, u16 count, u16 offset);
int netdev_set_sb_channel(struct net_device *dev, u16 channel);
static inline int netdev_get_sb_channel(struct net_device *dev)
{
	return max_t(int, -dev->num_tc, 0);
}

static inline
struct netdev_queue *netdev_get_tx_queue(const struct net_device *dev,
					 unsigned int index)
{
	DEBUG_NET_WARN_ON_ONCE(index >= dev->num_tx_queues);
	return &dev->_tx[index];
}

static inline struct netdev_queue *skb_get_tx_queue(const struct net_device *dev,
						    const struct sk_buff *skb)
{
	return netdev_get_tx_queue(dev, skb_get_queue_mapping(skb));
}

static inline void netdev_for_each_tx_queue(struct net_device *dev,
					    void (*f)(struct net_device *,
						      struct netdev_queue *,
						      void *),
					    void *arg)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++)
		f(dev, &dev->_tx[i], arg);
}

#define netdev_lockdep_set_classes(dev)				\
{								\
	static struct lock_class_key qdisc_tx_busylock_key;	\
	static struct lock_class_key qdisc_xmit_lock_key;	\
	static struct lock_class_key dev_addr_list_lock_key;	\
	unsigned int i;						\
								\
	(dev)->qdisc_tx_busylock = &qdisc_tx_busylock_key;	\
	lockdep_set_class(&(dev)->addr_list_lock,		\
			  &dev_addr_list_lock_key);		\
	for (i = 0; i < (dev)->num_tx_queues; i++)		\
		lockdep_set_class(&(dev)->_tx[i]._xmit_lock,	\
				  &qdisc_xmit_lock_key);	\
}

u16 netdev_pick_tx(struct net_device *dev, struct sk_buff *skb,
		     struct net_device *sb_dev);
struct netdev_queue *netdev_core_pick_tx(struct net_device *dev,
					 struct sk_buff *skb,
					 struct net_device *sb_dev);


static inline unsigned netdev_get_fwd_headroom(struct net_device *dev)
{
	return dev->priv_flags & IFF_PHONY_HEADROOM ? 0 : dev->needed_headroom;
}

static inline void netdev_set_rx_headroom(struct net_device *dev, int new_hr)
{
	if (dev->netdev_ops->ndo_set_rx_headroom)
		dev->netdev_ops->ndo_set_rx_headroom(dev, new_hr);
}


static inline void netdev_reset_rx_headroom(struct net_device *dev)
{
	netdev_set_rx_headroom(dev, -1);
}

static inline void *netdev_get_ml_priv(struct net_device *dev,
				       enum netdev_ml_priv_type type)
{
	if (dev->ml_priv_type != type)
		return NULL;

	return dev->ml_priv;
}

static inline void netdev_set_ml_priv(struct net_device *dev,
				      void *ml_priv,
				      enum netdev_ml_priv_type type)
{
	WARN(dev->ml_priv_type && dev->ml_priv_type != type,
	     "Overwriting already set ml_priv_type (%u) with different ml_priv_type (%u)!\n",
	     dev->ml_priv_type, type);
	WARN(!dev->ml_priv_type && dev->ml_priv,
	     "Overwriting already set ml_priv and ml_priv_type is ML_PRIV_NONE!\n");

	dev->ml_priv = ml_priv;
	dev->ml_priv_type = type;
}


static inline
struct net *dev_net(const struct net_device *dev)
{
	return read_pnet(&dev->nd_net);
}

static inline
void dev_net_set(struct net_device *dev, struct net *net)
{
	write_pnet(&dev->nd_net, net);
}


static inline void *netdev_priv(const struct net_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct net_device), NETDEV_ALIGN);
}


#define SET_NETDEV_DEV(net, pdev)	((net)->dev.parent = (pdev))


#define SET_NETDEV_DEVTYPE(net, devtype)	((net)->dev.type = (devtype))


#define NAPI_POLL_WEIGHT 64

void netif_napi_add_weight(struct net_device *dev, struct napi_struct *napi,
			   int (*poll)(struct napi_struct *, int), int weight);


static inline void
netif_napi_add(struct net_device *dev, struct napi_struct *napi,
	       int (*poll)(struct napi_struct *, int))
{
	netif_napi_add_weight(dev, napi, poll, NAPI_POLL_WEIGHT);
}

static inline void
netif_napi_add_tx_weight(struct net_device *dev,
			 struct napi_struct *napi,
			 int (*poll)(struct napi_struct *, int),
			 int weight)
{
	set_bit(NAPI_STATE_NO_BUSY_POLL, &napi->state);
	netif_napi_add_weight(dev, napi, poll, weight);
}


static inline void netif_napi_add_tx(struct net_device *dev,
				     struct napi_struct *napi,
				     int (*poll)(struct napi_struct *, int))
{
	netif_napi_add_tx_weight(dev, napi, poll, NAPI_POLL_WEIGHT);
}


void __netif_napi_del(struct napi_struct *napi);


static inline void netif_napi_del(struct napi_struct *napi)
{
	__netif_napi_del(napi);
	synchronize_net();
}

struct packet_type {
	__be16			type;	
	bool			ignore_outgoing;
	struct net_device	*dev;	
	netdevice_tracker	dev_tracker;
	int			(*func) (struct sk_buff *,
					 struct net_device *,
					 struct packet_type *,
					 struct net_device *);
	void			(*list_func) (struct list_head *,
					      struct packet_type *,
					      struct net_device *);
	bool			(*id_match)(struct packet_type *ptype,
					    struct sock *sk);
	struct net		*af_packet_net;
	void			*af_packet_priv;
	struct list_head	list;
};

struct offload_callbacks {
	struct sk_buff		*(*gso_segment)(struct sk_buff *skb,
						netdev_features_t features);
	struct sk_buff		*(*gro_receive)(struct list_head *head,
						struct sk_buff *skb);
	int			(*gro_complete)(struct sk_buff *skb, int nhoff);
};

struct packet_offload {
	__be16			 type;	
	u16			 priority;
	struct offload_callbacks callbacks;
	struct list_head	 list;
};


struct pcpu_sw_netstats {
	u64_stats_t		rx_packets;
	u64_stats_t		rx_bytes;
	u64_stats_t		tx_packets;
	u64_stats_t		tx_bytes;
	struct u64_stats_sync   syncp;
} __aligned(4 * sizeof(u64));

struct pcpu_dstats {
	u64			rx_packets;
	u64			rx_bytes;
	u64			rx_drops;
	u64			tx_packets;
	u64			tx_bytes;
	u64			tx_drops;
	struct u64_stats_sync	syncp;
} __aligned(8 * sizeof(u64));

struct pcpu_lstats {
	u64_stats_t packets;
	u64_stats_t bytes;
	struct u64_stats_sync syncp;
} __aligned(2 * sizeof(u64));

void dev_lstats_read(struct net_device *dev, u64 *packets, u64 *bytes);

static inline void dev_sw_netstats_rx_add(struct net_device *dev, unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	u64_stats_add(&tstats->rx_bytes, len);
	u64_stats_inc(&tstats->rx_packets);
	u64_stats_update_end(&tstats->syncp);
}

static inline void dev_sw_netstats_tx_add(struct net_device *dev,
					  unsigned int packets,
					  unsigned int len)
{
	struct pcpu_sw_netstats *tstats = this_cpu_ptr(dev->tstats);

	u64_stats_update_begin(&tstats->syncp);
	u64_stats_add(&tstats->tx_bytes, len);
	u64_stats_add(&tstats->tx_packets, packets);
	u64_stats_update_end(&tstats->syncp);
}

static inline void dev_lstats_add(struct net_device *dev, unsigned int len)
{
	struct pcpu_lstats *lstats = this_cpu_ptr(dev->lstats);

	u64_stats_update_begin(&lstats->syncp);
	u64_stats_add(&lstats->bytes, len);
	u64_stats_inc(&lstats->packets);
	u64_stats_update_end(&lstats->syncp);
}

#define __netdev_alloc_pcpu_stats(type, gfp)				\
({									\
	typeof(type) __percpu *pcpu_stats = alloc_percpu_gfp(type, gfp);\
	if (pcpu_stats)	{						\
		int __cpu;						\
		for_each_possible_cpu(__cpu) {				\
			typeof(type) *stat;				\
			stat = per_cpu_ptr(pcpu_stats, __cpu);		\
			u64_stats_init(&stat->syncp);			\
		}							\
	}								\
	pcpu_stats;							\
})

#define netdev_alloc_pcpu_stats(type)					\
	__netdev_alloc_pcpu_stats(type, GFP_KERNEL)

#define devm_netdev_alloc_pcpu_stats(dev, type)				\
({									\
	typeof(type) __percpu *pcpu_stats = devm_alloc_percpu(dev, type);\
	if (pcpu_stats) {						\
		int __cpu;						\
		for_each_possible_cpu(__cpu) {				\
			typeof(type) *stat;				\
			stat = per_cpu_ptr(pcpu_stats, __cpu);		\
			u64_stats_init(&stat->syncp);			\
		}							\
	}								\
	pcpu_stats;							\
})

enum netdev_lag_tx_type {
	NETDEV_LAG_TX_TYPE_UNKNOWN,
	NETDEV_LAG_TX_TYPE_RANDOM,
	NETDEV_LAG_TX_TYPE_BROADCAST,
	NETDEV_LAG_TX_TYPE_ROUNDROBIN,
	NETDEV_LAG_TX_TYPE_ACTIVEBACKUP,
	NETDEV_LAG_TX_TYPE_HASH,
};

enum netdev_lag_hash {
	NETDEV_LAG_HASH_NONE,
	NETDEV_LAG_HASH_L2,
	NETDEV_LAG_HASH_L34,
	NETDEV_LAG_HASH_L23,
	NETDEV_LAG_HASH_E23,
	NETDEV_LAG_HASH_E34,
	NETDEV_LAG_HASH_VLAN_SRCMAC,
	NETDEV_LAG_HASH_UNKNOWN,
};

struct netdev_lag_upper_info {
	enum netdev_lag_tx_type tx_type;
	enum netdev_lag_hash hash_type;
};

struct netdev_lag_lower_state_info {
	u8 link_up : 1,
	   tx_enabled : 1;
};

#include <linux/notifier.h>


enum netdev_cmd {
	NETDEV_UP	= 1,	
	NETDEV_DOWN,
	NETDEV_REBOOT,		
	NETDEV_CHANGE,		
	NETDEV_REGISTER,
	NETDEV_UNREGISTER,
	NETDEV_CHANGEMTU,	
	NETDEV_CHANGEADDR,	
	NETDEV_PRE_CHANGEADDR,	
	NETDEV_GOING_DOWN,
	NETDEV_CHANGENAME,
	NETDEV_FEAT_CHANGE,
	NETDEV_BONDING_FAILOVER,
	NETDEV_PRE_UP,
	NETDEV_PRE_TYPE_CHANGE,
	NETDEV_POST_TYPE_CHANGE,
	NETDEV_POST_INIT,
	NETDEV_PRE_UNINIT,
	NETDEV_RELEASE,
	NETDEV_NOTIFY_PEERS,
	NETDEV_JOIN,
	NETDEV_CHANGEUPPER,
	NETDEV_RESEND_IGMP,
	NETDEV_PRECHANGEMTU,	
	NETDEV_CHANGEINFODATA,
	NETDEV_BONDING_INFO,
	NETDEV_PRECHANGEUPPER,
	NETDEV_CHANGELOWERSTATE,
	NETDEV_UDP_TUNNEL_PUSH_INFO,
	NETDEV_UDP_TUNNEL_DROP_INFO,
	NETDEV_CHANGE_TX_QUEUE_LEN,
	NETDEV_CVLAN_FILTER_PUSH_INFO,
	NETDEV_CVLAN_FILTER_DROP_INFO,
	NETDEV_SVLAN_FILTER_PUSH_INFO,
	NETDEV_SVLAN_FILTER_DROP_INFO,
	NETDEV_OFFLOAD_XSTATS_ENABLE,
	NETDEV_OFFLOAD_XSTATS_DISABLE,
	NETDEV_OFFLOAD_XSTATS_REPORT_USED,
	NETDEV_OFFLOAD_XSTATS_REPORT_DELTA,
	NETDEV_XDP_FEAT_CHANGE,
};
const char *netdev_cmd_to_name(enum netdev_cmd cmd);

int register_netdevice_notifier(struct notifier_block *nb);
int unregister_netdevice_notifier(struct notifier_block *nb);
int register_netdevice_notifier_net(struct net *net, struct notifier_block *nb);
int unregister_netdevice_notifier_net(struct net *net,
				      struct notifier_block *nb);
int register_netdevice_notifier_dev_net(struct net_device *dev,
					struct notifier_block *nb,
					struct netdev_net_notifier *nn);
int unregister_netdevice_notifier_dev_net(struct net_device *dev,
					  struct notifier_block *nb,
					  struct netdev_net_notifier *nn);

struct netdev_notifier_info {
	struct net_device	*dev;
	struct netlink_ext_ack	*extack;
};

struct netdev_notifier_info_ext {
	struct netdev_notifier_info info; 
	union {
		u32 mtu;
	} ext;
};

struct netdev_notifier_change_info {
	struct netdev_notifier_info info; 
	unsigned int flags_changed;
};

struct netdev_notifier_changeupper_info {
	struct netdev_notifier_info info; 
	struct net_device *upper_dev; 
	bool master; 
	bool linking; 
	void *upper_info; 
};

struct netdev_notifier_changelowerstate_info {
	struct netdev_notifier_info info; 
	void *lower_state_info; 
};

struct netdev_notifier_pre_changeaddr_info {
	struct netdev_notifier_info info; 
	const unsigned char *dev_addr;
};

enum netdev_offload_xstats_type {
	NETDEV_OFFLOAD_XSTATS_TYPE_L3 = 1,
};

struct netdev_notifier_offload_xstats_info {
	struct netdev_notifier_info info; 
	enum netdev_offload_xstats_type type;

	union {
		
		struct netdev_notifier_offload_xstats_rd *report_delta;
		
		struct netdev_notifier_offload_xstats_ru *report_used;
	};
};

int netdev_offload_xstats_enable(struct net_device *dev,
				 enum netdev_offload_xstats_type type,
				 struct netlink_ext_ack *extack);
int netdev_offload_xstats_disable(struct net_device *dev,
				  enum netdev_offload_xstats_type type);
bool netdev_offload_xstats_enabled(const struct net_device *dev,
				   enum netdev_offload_xstats_type type);
int netdev_offload_xstats_get(struct net_device *dev,
			      enum netdev_offload_xstats_type type,
			      struct rtnl_hw_stats64 *stats, bool *used,
			      struct netlink_ext_ack *extack);
void
netdev_offload_xstats_report_delta(struct netdev_notifier_offload_xstats_rd *rd,
				   const struct rtnl_hw_stats64 *stats);
void
netdev_offload_xstats_report_used(struct netdev_notifier_offload_xstats_ru *ru);
void netdev_offload_xstats_push_delta(struct net_device *dev,
				      enum netdev_offload_xstats_type type,
				      const struct rtnl_hw_stats64 *stats);

static inline void netdev_notifier_info_init(struct netdev_notifier_info *info,
					     struct net_device *dev)
{
	info->dev = dev;
	info->extack = NULL;
}

static inline struct net_device *
netdev_notifier_info_to_dev(const struct netdev_notifier_info *info)
{
	return info->dev;
}

static inline struct netlink_ext_ack *
netdev_notifier_info_to_extack(const struct netdev_notifier_info *info)
{
	return info->extack;
}

int call_netdevice_notifiers(unsigned long val, struct net_device *dev);
int call_netdevice_notifiers_info(unsigned long val,
				  struct netdev_notifier_info *info);

extern rwlock_t				dev_base_lock;		

#define for_each_netdev(net, d)		\
		list_for_each_entry(d, &(net)->dev_base_head, dev_list)
#define for_each_netdev_reverse(net, d)	\
		list_for_each_entry_reverse(d, &(net)->dev_base_head, dev_list)
#define for_each_netdev_rcu(net, d)		\
		list_for_each_entry_rcu(d, &(net)->dev_base_head, dev_list)
#define for_each_netdev_safe(net, d, n)	\
		list_for_each_entry_safe(d, n, &(net)->dev_base_head, dev_list)
#define for_each_netdev_continue(net, d)		\
		list_for_each_entry_continue(d, &(net)->dev_base_head, dev_list)
#define for_each_netdev_continue_reverse(net, d)		\
		list_for_each_entry_continue_reverse(d, &(net)->dev_base_head, \
						     dev_list)
#define for_each_netdev_continue_rcu(net, d)		\
	list_for_each_entry_continue_rcu(d, &(net)->dev_base_head, dev_list)
#define for_each_netdev_in_bond_rcu(bond, slave)	\
		for_each_netdev_rcu(&init_net, slave)	\
			if (netdev_master_upper_dev_get_rcu(slave) == (bond))
#define net_device_entry(lh)	list_entry(lh, struct net_device, dev_list)

#define for_each_netdev_dump(net, d, ifindex)				\
	xa_for_each_start(&(net)->dev_by_index, (ifindex), (d), (ifindex))

static inline struct net_device *next_net_device(struct net_device *dev)
{
	struct list_head *lh;
	struct net *net;

	net = dev_net(dev);
	lh = dev->dev_list.next;
	return lh == &net->dev_base_head ? NULL : net_device_entry(lh);
}

static inline struct net_device *next_net_device_rcu(struct net_device *dev)
{
	struct list_head *lh;
	struct net *net;

	net = dev_net(dev);
	lh = rcu_dereference(list_next_rcu(&dev->dev_list));
	return lh == &net->dev_base_head ? NULL : net_device_entry(lh);
}

static inline struct net_device *first_net_device(struct net *net)
{
	return list_empty(&net->dev_base_head) ? NULL :
		net_device_entry(net->dev_base_head.next);
}

static inline struct net_device *first_net_device_rcu(struct net *net)
{
	struct list_head *lh = rcu_dereference(list_next_rcu(&net->dev_base_head));

	return lh == &net->dev_base_head ? NULL : net_device_entry(lh);
}

int netdev_boot_setup_check(struct net_device *dev);
struct net_device *dev_getbyhwaddr_rcu(struct net *net, unsigned short type,
				       const char *hwaddr);
struct net_device *dev_getfirstbyhwtype(struct net *net, unsigned short type);
void dev_add_pack(struct packet_type *pt);
void dev_remove_pack(struct packet_type *pt);
void __dev_remove_pack(struct packet_type *pt);
void dev_add_offload(struct packet_offload *po);
void dev_remove_offload(struct packet_offload *po);

int dev_get_iflink(const struct net_device *dev);
int dev_fill_metadata_dst(struct net_device *dev, struct sk_buff *skb);
int dev_fill_forward_path(const struct net_device *dev, const u8 *daddr,
			  struct net_device_path_stack *stack);
struct net_device *__dev_get_by_flags(struct net *net, unsigned short flags,
				      unsigned short mask);
struct net_device *dev_get_by_name(struct net *net, const char *name);
struct net_device *dev_get_by_name_rcu(struct net *net, const char *name);
struct net_device *__dev_get_by_name(struct net *net, const char *name);
bool netdev_name_in_use(struct net *net, const char *name);
int dev_alloc_name(struct net_device *dev, const char *name);
int dev_open(struct net_device *dev, struct netlink_ext_ack *extack);
void dev_close(struct net_device *dev);
void dev_close_many(struct list_head *head, bool unlink);
void dev_disable_lro(struct net_device *dev);
int dev_loopback_xmit(struct net *net, struct sock *sk, struct sk_buff *newskb);
u16 dev_pick_tx_zero(struct net_device *dev, struct sk_buff *skb,
		     struct net_device *sb_dev);
u16 dev_pick_tx_cpu_id(struct net_device *dev, struct sk_buff *skb,
		       struct net_device *sb_dev);

int __dev_queue_xmit(struct sk_buff *skb, struct net_device *sb_dev);
int __dev_direct_xmit(struct sk_buff *skb, u16 queue_id);

static inline int dev_queue_xmit(struct sk_buff *skb)
{
	return __dev_queue_xmit(skb, NULL);
}

static inline int dev_queue_xmit_accel(struct sk_buff *skb,
				       struct net_device *sb_dev)
{
	return __dev_queue_xmit(skb, sb_dev);
}

static inline int dev_direct_xmit(struct sk_buff *skb, u16 queue_id)
{
	int ret;

	ret = __dev_direct_xmit(skb, queue_id);
	if (!dev_xmit_complete(ret))
		kfree_skb(skb);
	return ret;
}

int register_netdevice(struct net_device *dev);
void unregister_netdevice_queue(struct net_device *dev, struct list_head *head);
void unregister_netdevice_many(struct list_head *head);
static inline void unregister_netdevice(struct net_device *dev)
{
	unregister_netdevice_queue(dev, NULL);
}

int netdev_refcnt_read(const struct net_device *dev);
void free_netdev(struct net_device *dev);
void netdev_freemem(struct net_device *dev);
int init_dummy_netdev(struct net_device *dev);

struct net_device *netdev_get_xmit_slave(struct net_device *dev,
					 struct sk_buff *skb,
					 bool all_slaves);
struct net_device *netdev_sk_get_lowest_dev(struct net_device *dev,
					    struct sock *sk);
struct net_device *dev_get_by_index(struct net *net, int ifindex);
struct net_device *__dev_get_by_index(struct net *net, int ifindex);
struct net_device *netdev_get_by_index(struct net *net, int ifindex,
				       netdevice_tracker *tracker, gfp_t gfp);
struct net_device *netdev_get_by_name(struct net *net, const char *name,
				      netdevice_tracker *tracker, gfp_t gfp);
struct net_device *dev_get_by_index_rcu(struct net *net, int ifindex);
struct net_device *dev_get_by_napi_id(unsigned int napi_id);

static inline int dev_hard_header(struct sk_buff *skb, struct net_device *dev,
				  unsigned short type,
				  const void *daddr, const void *saddr,
				  unsigned int len)
{
	if (!dev->header_ops || !dev->header_ops->create)
		return 0;

	return dev->header_ops->create(skb, dev, type, daddr, saddr, len);
}

static inline int dev_parse_header(const struct sk_buff *skb,
				   unsigned char *haddr)
{
	const struct net_device *dev = skb->dev;

	if (!dev->header_ops || !dev->header_ops->parse)
		return 0;
	return dev->header_ops->parse(skb, haddr);
}

static inline __be16 dev_parse_header_protocol(const struct sk_buff *skb)
{
	const struct net_device *dev = skb->dev;

	if (!dev->header_ops || !dev->header_ops->parse_protocol)
		return 0;
	return dev->header_ops->parse_protocol(skb);
}


static inline bool dev_validate_header(const struct net_device *dev,
				       char *ll_header, int len)
{
	if (likely(len >= dev->hard_header_len))
		return true;
	if (len < dev->min_header_len)
		return false;

	if (capable(CAP_SYS_RAWIO)) {
		memset(ll_header + len, 0, dev->hard_header_len - len);
		return true;
	}

	if (dev->header_ops && dev->header_ops->validate)
		return dev->header_ops->validate(ll_header, len);

	return false;
}

static inline bool dev_has_header(const struct net_device *dev)
{
	return dev->header_ops && dev->header_ops->create;
}


struct softnet_data {
	struct list_head	poll_list;
	struct sk_buff_head	process_queue;

	
	unsigned int		processed;
	unsigned int		time_squeeze;
#ifdef CONFIG_RPS
	struct softnet_data	*rps_ipi_list;
#endif

	bool			in_net_rx_action;
	bool			in_napi_threaded_poll;

#ifdef CONFIG_NET_FLOW_LIMIT
	struct sd_flow_limit __rcu *flow_limit;
#endif
	struct Qdisc		*output_queue;
	struct Qdisc		**output_queue_tailp;
	struct sk_buff		*completion_queue;
#ifdef CONFIG_XFRM_OFFLOAD
	struct sk_buff_head	xfrm_backlog;
#endif
	
	struct {
		u16 recursion;
		u8  more;
#ifdef CONFIG_NET_EGRESS
		u8  skip_txqueue;
#endif
	} xmit;
#ifdef CONFIG_RPS
	
	unsigned int		input_queue_head ____cacheline_aligned_in_smp;

	
	call_single_data_t	csd ____cacheline_aligned_in_smp;
	struct softnet_data	*rps_ipi_next;
	unsigned int		cpu;
	unsigned int		input_queue_tail;
#endif
	unsigned int		received_rps;
	unsigned int		dropped;
	struct sk_buff_head	input_pkt_queue;
	struct napi_struct	backlog;

	
	spinlock_t		defer_lock ____cacheline_aligned_in_smp;
	int			defer_count;
	int			defer_ipi_scheduled;
	struct sk_buff		*defer_list;
	call_single_data_t	defer_csd;
};

static inline void input_queue_head_incr(struct softnet_data *sd)
{
#ifdef CONFIG_RPS
	sd->input_queue_head++;
#endif
}

static inline void input_queue_tail_incr_save(struct softnet_data *sd,
					      unsigned int *qtail)
{
#ifdef CONFIG_RPS
	*qtail = ++sd->input_queue_tail;
#endif
}

DECLARE_PER_CPU_ALIGNED(struct softnet_data, softnet_data);

static inline int dev_recursion_level(void)
{
	return this_cpu_read(softnet_data.xmit.recursion);
}

#define XMIT_RECURSION_LIMIT	8
static inline bool dev_xmit_recursion(void)
{
	return unlikely(__this_cpu_read(softnet_data.xmit.recursion) >
			XMIT_RECURSION_LIMIT);
}

static inline void dev_xmit_recursion_inc(void)
{
	__this_cpu_inc(softnet_data.xmit.recursion);
}

static inline void dev_xmit_recursion_dec(void)
{
	__this_cpu_dec(softnet_data.xmit.recursion);
}

void __netif_schedule(struct Qdisc *q);
void netif_schedule_queue(struct netdev_queue *txq);

static inline void netif_tx_schedule_all(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++)
		netif_schedule_queue(netdev_get_tx_queue(dev, i));
}

static __always_inline void netif_tx_start_queue(struct netdev_queue *dev_queue)
{
	clear_bit(__QUEUE_STATE_DRV_XOFF, &dev_queue->state);
}


static inline void netif_start_queue(struct net_device *dev)
{
	netif_tx_start_queue(netdev_get_tx_queue(dev, 0));
}

static inline void netif_tx_start_all_queues(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		netif_tx_start_queue(txq);
	}
}

void netif_tx_wake_queue(struct netdev_queue *dev_queue);


static inline void netif_wake_queue(struct net_device *dev)
{
	netif_tx_wake_queue(netdev_get_tx_queue(dev, 0));
}

static inline void netif_tx_wake_all_queues(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		netif_tx_wake_queue(txq);
	}
}

static __always_inline void netif_tx_stop_queue(struct netdev_queue *dev_queue)
{
	
	set_bit(__QUEUE_STATE_DRV_XOFF, &dev_queue->state);
}


static inline void netif_stop_queue(struct net_device *dev)
{
	netif_tx_stop_queue(netdev_get_tx_queue(dev, 0));
}

void netif_tx_stop_all_queues(struct net_device *dev);

static inline bool netif_tx_queue_stopped(const struct netdev_queue *dev_queue)
{
	return test_bit(__QUEUE_STATE_DRV_XOFF, &dev_queue->state);
}


static inline bool netif_queue_stopped(const struct net_device *dev)
{
	return netif_tx_queue_stopped(netdev_get_tx_queue(dev, 0));
}

static inline bool netif_xmit_stopped(const struct netdev_queue *dev_queue)
{
	return dev_queue->state & QUEUE_STATE_ANY_XOFF;
}

static inline bool
netif_xmit_frozen_or_stopped(const struct netdev_queue *dev_queue)
{
	return dev_queue->state & QUEUE_STATE_ANY_XOFF_OR_FROZEN;
}

static inline bool
netif_xmit_frozen_or_drv_stopped(const struct netdev_queue *dev_queue)
{
	return dev_queue->state & QUEUE_STATE_DRV_XOFF_OR_FROZEN;
}


static inline void netdev_queue_set_dql_min_limit(struct netdev_queue *dev_queue,
						  unsigned int min_limit)
{
#ifdef CONFIG_BQL
	dev_queue->dql.min_limit = min_limit;
#endif
}


static inline void netdev_txq_bql_enqueue_prefetchw(struct netdev_queue *dev_queue)
{
#ifdef CONFIG_BQL
	prefetchw(&dev_queue->dql.num_queued);
#endif
}


static inline void netdev_txq_bql_complete_prefetchw(struct netdev_queue *dev_queue)
{
#ifdef CONFIG_BQL
	prefetchw(&dev_queue->dql.limit);
#endif
}


static inline void netdev_tx_sent_queue(struct netdev_queue *dev_queue,
					unsigned int bytes)
{
#ifdef CONFIG_BQL
	dql_queued(&dev_queue->dql, bytes);

	if (likely(dql_avail(&dev_queue->dql) >= 0))
		return;

	set_bit(__QUEUE_STATE_STACK_XOFF, &dev_queue->state);

	
	smp_mb();

	
	if (unlikely(dql_avail(&dev_queue->dql) >= 0))
		clear_bit(__QUEUE_STATE_STACK_XOFF, &dev_queue->state);
#endif
}


static inline bool __netdev_tx_sent_queue(struct netdev_queue *dev_queue,
					  unsigned int bytes,
					  bool xmit_more)
{
	if (xmit_more) {
#ifdef CONFIG_BQL
		dql_queued(&dev_queue->dql, bytes);
#endif
		return netif_tx_queue_stopped(dev_queue);
	}
	netdev_tx_sent_queue(dev_queue, bytes);
	return true;
}


static inline void netdev_sent_queue(struct net_device *dev, unsigned int bytes)
{
	netdev_tx_sent_queue(netdev_get_tx_queue(dev, 0), bytes);
}

static inline bool __netdev_sent_queue(struct net_device *dev,
				       unsigned int bytes,
				       bool xmit_more)
{
	return __netdev_tx_sent_queue(netdev_get_tx_queue(dev, 0), bytes,
				      xmit_more);
}


static inline void netdev_tx_completed_queue(struct netdev_queue *dev_queue,
					     unsigned int pkts, unsigned int bytes)
{
#ifdef CONFIG_BQL
	if (unlikely(!bytes))
		return;

	dql_completed(&dev_queue->dql, bytes);

	
	smp_mb(); 

	if (unlikely(dql_avail(&dev_queue->dql) < 0))
		return;

	if (test_and_clear_bit(__QUEUE_STATE_STACK_XOFF, &dev_queue->state))
		netif_schedule_queue(dev_queue);
#endif
}


static inline void netdev_completed_queue(struct net_device *dev,
					  unsigned int pkts, unsigned int bytes)
{
	netdev_tx_completed_queue(netdev_get_tx_queue(dev, 0), pkts, bytes);
}

static inline void netdev_tx_reset_queue(struct netdev_queue *q)
{
#ifdef CONFIG_BQL
	clear_bit(__QUEUE_STATE_STACK_XOFF, &q->state);
	dql_reset(&q->dql);
#endif
}


static inline void netdev_reset_queue(struct net_device *dev_queue)
{
	netdev_tx_reset_queue(netdev_get_tx_queue(dev_queue, 0));
}


static inline u16 netdev_cap_txqueue(struct net_device *dev, u16 queue_index)
{
	if (unlikely(queue_index >= dev->real_num_tx_queues)) {
		net_warn_ratelimited("%s selects TX queue %d, but real number of TX queues is %d\n",
				     dev->name, queue_index,
				     dev->real_num_tx_queues);
		return 0;
	}

	return queue_index;
}


static inline bool netif_running(const struct net_device *dev)
{
	return test_bit(__LINK_STATE_START, &dev->state);
}




static inline void netif_start_subqueue(struct net_device *dev, u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	netif_tx_start_queue(txq);
}


static inline void netif_stop_subqueue(struct net_device *dev, u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);
	netif_tx_stop_queue(txq);
}


static inline bool __netif_subqueue_stopped(const struct net_device *dev,
					    u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	return netif_tx_queue_stopped(txq);
}


static inline bool netif_subqueue_stopped(const struct net_device *dev,
					  struct sk_buff *skb)
{
	return __netif_subqueue_stopped(dev, skb_get_queue_mapping(skb));
}


static inline void netif_wake_subqueue(struct net_device *dev, u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	netif_tx_wake_queue(txq);
}

#ifdef CONFIG_XPS
int netif_set_xps_queue(struct net_device *dev, const struct cpumask *mask,
			u16 index);
int __netif_set_xps_queue(struct net_device *dev, const unsigned long *mask,
			  u16 index, enum xps_map_type type);


static inline bool netif_attr_test_mask(unsigned long j,
					const unsigned long *mask,
					unsigned int nr_bits)
{
	cpu_max_bits_warn(j, nr_bits);
	return test_bit(j, mask);
}


static inline bool netif_attr_test_online(unsigned long j,
					  const unsigned long *online_mask,
					  unsigned int nr_bits)
{
	cpu_max_bits_warn(j, nr_bits);

	if (online_mask)
		return test_bit(j, online_mask);

	return (j < nr_bits);
}


static inline unsigned int netif_attrmask_next(int n, const unsigned long *srcp,
					       unsigned int nr_bits)
{
	
	if (n != -1)
		cpu_max_bits_warn(n, nr_bits);

	if (srcp)
		return find_next_bit(srcp, nr_bits, n + 1);

	return n + 1;
}


static inline int netif_attrmask_next_and(int n, const unsigned long *src1p,
					  const unsigned long *src2p,
					  unsigned int nr_bits)
{
	
	if (n != -1)
		cpu_max_bits_warn(n, nr_bits);

	if (src1p && src2p)
		return find_next_and_bit(src1p, src2p, nr_bits, n + 1);
	else if (src1p)
		return find_next_bit(src1p, nr_bits, n + 1);
	else if (src2p)
		return find_next_bit(src2p, nr_bits, n + 1);

	return n + 1;
}
#else
static inline int netif_set_xps_queue(struct net_device *dev,
				      const struct cpumask *mask,
				      u16 index)
{
	return 0;
}

static inline int __netif_set_xps_queue(struct net_device *dev,
					const unsigned long *mask,
					u16 index, enum xps_map_type type)
{
	return 0;
}
#endif


static inline bool netif_is_multiqueue(const struct net_device *dev)
{
	return dev->num_tx_queues > 1;
}

int netif_set_real_num_tx_queues(struct net_device *dev, unsigned int txq);

#ifdef CONFIG_SYSFS
int netif_set_real_num_rx_queues(struct net_device *dev, unsigned int rxq);
#else
static inline int netif_set_real_num_rx_queues(struct net_device *dev,
						unsigned int rxqs)
{
	dev->real_num_rx_queues = rxqs;
	return 0;
}
#endif
int netif_set_real_num_queues(struct net_device *dev,
			      unsigned int txq, unsigned int rxq);

int netif_get_num_default_rss_queues(void);

void dev_kfree_skb_irq_reason(struct sk_buff *skb, enum skb_drop_reason reason);
void dev_kfree_skb_any_reason(struct sk_buff *skb, enum skb_drop_reason reason);


static inline void dev_kfree_skb_irq(struct sk_buff *skb)
{
	dev_kfree_skb_irq_reason(skb, SKB_DROP_REASON_NOT_SPECIFIED);
}

static inline void dev_consume_skb_irq(struct sk_buff *skb)
{
	dev_kfree_skb_irq_reason(skb, SKB_CONSUMED);
}

static inline void dev_kfree_skb_any(struct sk_buff *skb)
{
	dev_kfree_skb_any_reason(skb, SKB_DROP_REASON_NOT_SPECIFIED);
}

static inline void dev_consume_skb_any(struct sk_buff *skb)
{
	dev_kfree_skb_any_reason(skb, SKB_CONSUMED);
}

u32 bpf_prog_run_generic_xdp(struct sk_buff *skb, struct xdp_buff *xdp,
			     struct bpf_prog *xdp_prog);
void generic_xdp_tx(struct sk_buff *skb, struct bpf_prog *xdp_prog);
int do_xdp_generic(struct bpf_prog *xdp_prog, struct sk_buff *skb);
int netif_rx(struct sk_buff *skb);
int __netif_rx(struct sk_buff *skb);

int netif_receive_skb(struct sk_buff *skb);
int netif_receive_skb_core(struct sk_buff *skb);
void netif_receive_skb_list_internal(struct list_head *head);
void netif_receive_skb_list(struct list_head *head);
gro_result_t napi_gro_receive(struct napi_struct *napi, struct sk_buff *skb);
void napi_gro_flush(struct napi_struct *napi, bool flush_old);
struct sk_buff *napi_get_frags(struct napi_struct *napi);
void napi_get_frags_check(struct napi_struct *napi);
gro_result_t napi_gro_frags(struct napi_struct *napi);
struct packet_offload *gro_find_receive_by_type(__be16 type);
struct packet_offload *gro_find_complete_by_type(__be16 type);

static inline void napi_free_frags(struct napi_struct *napi)
{
	kfree_skb(napi->skb);
	napi->skb = NULL;
}

bool netdev_is_rx_handler_busy(struct net_device *dev);
int netdev_rx_handler_register(struct net_device *dev,
			       rx_handler_func_t *rx_handler,
			       void *rx_handler_data);
void netdev_rx_handler_unregister(struct net_device *dev);

bool dev_valid_name(const char *name);
static inline bool is_socket_ioctl_cmd(unsigned int cmd)
{
	return _IOC_TYPE(cmd) == SOCK_IOC_TYPE;
}
int get_user_ifreq(struct ifreq *ifr, void __user **ifrdata, void __user *arg);
int put_user_ifreq(struct ifreq *ifr, void __user *arg);
int dev_ioctl(struct net *net, unsigned int cmd, struct ifreq *ifr,
		void __user *data, bool *need_copyout);
int dev_ifconf(struct net *net, struct ifconf __user *ifc);
int generic_hwtstamp_get_lower(struct net_device *dev,
			       struct kernel_hwtstamp_config *kernel_cfg);
int generic_hwtstamp_set_lower(struct net_device *dev,
			       struct kernel_hwtstamp_config *kernel_cfg,
			       struct netlink_ext_ack *extack);
int dev_ethtool(struct net *net, struct ifreq *ifr, void __user *userdata);
unsigned int dev_get_flags(const struct net_device *);
int __dev_change_flags(struct net_device *dev, unsigned int flags,
		       struct netlink_ext_ack *extack);
int dev_change_flags(struct net_device *dev, unsigned int flags,
		     struct netlink_ext_ack *extack);
int dev_set_alias(struct net_device *, const char *, size_t);
int dev_get_alias(const struct net_device *, char *, size_t);
int __dev_change_net_namespace(struct net_device *dev, struct net *net,
			       const char *pat, int new_ifindex);
static inline
int dev_change_net_namespace(struct net_device *dev, struct net *net,
			     const char *pat)
{
	return __dev_change_net_namespace(dev, net, pat, 0);
}
int __dev_set_mtu(struct net_device *, int);
int dev_set_mtu(struct net_device *, int);
int dev_pre_changeaddr_notify(struct net_device *dev, const char *addr,
			      struct netlink_ext_ack *extack);
int dev_set_mac_address(struct net_device *dev, struct sockaddr *sa,
			struct netlink_ext_ack *extack);
int dev_set_mac_address_user(struct net_device *dev, struct sockaddr *sa,
			     struct netlink_ext_ack *extack);
int dev_get_mac_address(struct sockaddr *sa, struct net *net, char *dev_name);
int dev_get_port_parent_id(struct net_device *dev,
			   struct netdev_phys_item_id *ppid, bool recurse);
bool netdev_port_same_parent_id(struct net_device *a, struct net_device *b);
struct sk_buff *validate_xmit_skb_list(struct sk_buff *skb, struct net_device *dev, bool *again);
struct sk_buff *dev_hard_start_xmit(struct sk_buff *skb, struct net_device *dev,
				    struct netdev_queue *txq, int *ret);

int bpf_xdp_link_attach(const union bpf_attr *attr, struct bpf_prog *prog);
u8 dev_xdp_prog_count(struct net_device *dev);
u32 dev_xdp_prog_id(struct net_device *dev, enum bpf_xdp_mode mode);

int __dev_forward_skb(struct net_device *dev, struct sk_buff *skb);
int dev_forward_skb(struct net_device *dev, struct sk_buff *skb);
int dev_forward_skb_nomtu(struct net_device *dev, struct sk_buff *skb);
bool is_skb_forwardable(const struct net_device *dev,
			const struct sk_buff *skb);

static __always_inline bool __is_skb_forwardable(const struct net_device *dev,
						 const struct sk_buff *skb,
						 const bool check_mtu)
{
	const u32 vlan_hdr_len = 4; 
	unsigned int len;

	if (!(dev->flags & IFF_UP))
		return false;

	if (!check_mtu)
		return true;

	len = dev->mtu + dev->hard_header_len + vlan_hdr_len;
	if (skb->len <= len)
		return true;

	
	if (skb_is_gso(skb))
		return true;

	return false;
}

struct net_device_core_stats __percpu *netdev_core_stats_alloc(struct net_device *dev);

static inline struct net_device_core_stats __percpu *dev_core_stats(struct net_device *dev)
{
	
	struct net_device_core_stats __percpu *p = READ_ONCE(dev->core_stats);

	if (likely(p))
		return p;

	return netdev_core_stats_alloc(dev);
}

#define DEV_CORE_STATS_INC(FIELD)						\
static inline void dev_core_stats_##FIELD##_inc(struct net_device *dev)		\
{										\
	struct net_device_core_stats __percpu *p;				\
										\
	p = dev_core_stats(dev);						\
	if (p)									\
		this_cpu_inc(p->FIELD);						\
}
DEV_CORE_STATS_INC(rx_dropped)
DEV_CORE_STATS_INC(tx_dropped)
DEV_CORE_STATS_INC(rx_nohandler)
DEV_CORE_STATS_INC(rx_otherhost_dropped)

static __always_inline int ____dev_forward_skb(struct net_device *dev,
					       struct sk_buff *skb,
					       const bool check_mtu)
{
	if (skb_orphan_frags(skb, GFP_ATOMIC) ||
	    unlikely(!__is_skb_forwardable(dev, skb, check_mtu))) {
		dev_core_stats_rx_dropped_inc(dev);
		kfree_skb(skb);
		return NET_RX_DROP;
	}

	skb_scrub_packet(skb, !net_eq(dev_net(dev), dev_net(skb->dev)));
	skb->priority = 0;
	return 0;
}

bool dev_nit_active(struct net_device *dev);
void dev_queue_xmit_nit(struct sk_buff *skb, struct net_device *dev);

static inline void __dev_put(struct net_device *dev)
{
	if (dev) {
#ifdef CONFIG_PCPU_DEV_REFCNT
		this_cpu_dec(*dev->pcpu_refcnt);
#else
		refcount_dec(&dev->dev_refcnt);
#endif
	}
}

static inline void __dev_hold(struct net_device *dev)
{
	if (dev) {
#ifdef CONFIG_PCPU_DEV_REFCNT
		this_cpu_inc(*dev->pcpu_refcnt);
#else
		refcount_inc(&dev->dev_refcnt);
#endif
	}
}

static inline void __netdev_tracker_alloc(struct net_device *dev,
					  netdevice_tracker *tracker,
					  gfp_t gfp)
{
#ifdef CONFIG_NET_DEV_REFCNT_TRACKER
	ref_tracker_alloc(&dev->refcnt_tracker, tracker, gfp);
#endif
}


static inline void netdev_tracker_alloc(struct net_device *dev,
					netdevice_tracker *tracker, gfp_t gfp)
{
#ifdef CONFIG_NET_DEV_REFCNT_TRACKER
	refcount_dec(&dev->refcnt_tracker.no_tracker);
	__netdev_tracker_alloc(dev, tracker, gfp);
#endif
}

static inline void netdev_tracker_free(struct net_device *dev,
				       netdevice_tracker *tracker)
{
#ifdef CONFIG_NET_DEV_REFCNT_TRACKER
	ref_tracker_free(&dev->refcnt_tracker, tracker);
#endif
}

static inline void netdev_hold(struct net_device *dev,
			       netdevice_tracker *tracker, gfp_t gfp)
{
	if (dev) {
		__dev_hold(dev);
		__netdev_tracker_alloc(dev, tracker, gfp);
	}
}

static inline void netdev_put(struct net_device *dev,
			      netdevice_tracker *tracker)
{
	if (dev) {
		netdev_tracker_free(dev, tracker);
		__dev_put(dev);
	}
}


static inline void dev_hold(struct net_device *dev)
{
	netdev_hold(dev, NULL, GFP_ATOMIC);
}


static inline void dev_put(struct net_device *dev)
{
	netdev_put(dev, NULL);
}

static inline void netdev_ref_replace(struct net_device *odev,
				      struct net_device *ndev,
				      netdevice_tracker *tracker,
				      gfp_t gfp)
{
	if (odev)
		netdev_tracker_free(odev, tracker);

	__dev_hold(ndev);
	__dev_put(odev);

	if (ndev)
		__netdev_tracker_alloc(ndev, tracker, gfp);
}


void linkwatch_fire_event(struct net_device *dev);


static inline bool netif_carrier_ok(const struct net_device *dev)
{
	return !test_bit(__LINK_STATE_NOCARRIER, &dev->state);
}

unsigned long dev_trans_start(struct net_device *dev);

void __netdev_watchdog_up(struct net_device *dev);

void netif_carrier_on(struct net_device *dev);
void netif_carrier_off(struct net_device *dev);
void netif_carrier_event(struct net_device *dev);


static inline void netif_dormant_on(struct net_device *dev)
{
	if (!test_and_set_bit(__LINK_STATE_DORMANT, &dev->state))
		linkwatch_fire_event(dev);
}


static inline void netif_dormant_off(struct net_device *dev)
{
	if (test_and_clear_bit(__LINK_STATE_DORMANT, &dev->state))
		linkwatch_fire_event(dev);
}


static inline bool netif_dormant(const struct net_device *dev)
{
	return test_bit(__LINK_STATE_DORMANT, &dev->state);
}



static inline void netif_testing_on(struct net_device *dev)
{
	if (!test_and_set_bit(__LINK_STATE_TESTING, &dev->state))
		linkwatch_fire_event(dev);
}


static inline void netif_testing_off(struct net_device *dev)
{
	if (test_and_clear_bit(__LINK_STATE_TESTING, &dev->state))
		linkwatch_fire_event(dev);
}


static inline bool netif_testing(const struct net_device *dev)
{
	return test_bit(__LINK_STATE_TESTING, &dev->state);
}



static inline bool netif_oper_up(const struct net_device *dev)
{
	return (dev->operstate == IF_OPER_UP ||
		dev->operstate == IF_OPER_UNKNOWN );
}


static inline bool netif_device_present(const struct net_device *dev)
{
	return test_bit(__LINK_STATE_PRESENT, &dev->state);
}

void netif_device_detach(struct net_device *dev);

void netif_device_attach(struct net_device *dev);



enum {
	NETIF_MSG_DRV_BIT,
	NETIF_MSG_PROBE_BIT,
	NETIF_MSG_LINK_BIT,
	NETIF_MSG_TIMER_BIT,
	NETIF_MSG_IFDOWN_BIT,
	NETIF_MSG_IFUP_BIT,
	NETIF_MSG_RX_ERR_BIT,
	NETIF_MSG_TX_ERR_BIT,
	NETIF_MSG_TX_QUEUED_BIT,
	NETIF_MSG_INTR_BIT,
	NETIF_MSG_TX_DONE_BIT,
	NETIF_MSG_RX_STATUS_BIT,
	NETIF_MSG_PKTDATA_BIT,
	NETIF_MSG_HW_BIT,
	NETIF_MSG_WOL_BIT,

	
	NETIF_MSG_CLASS_COUNT,
};

static_assert(NETIF_MSG_CLASS_COUNT <= 32);

#define __NETIF_MSG_BIT(bit)	((u32)1 << (bit))
#define __NETIF_MSG(name)	__NETIF_MSG_BIT(NETIF_MSG_ ## name ## _BIT)

#define NETIF_MSG_DRV		__NETIF_MSG(DRV)
#define NETIF_MSG_PROBE		__NETIF_MSG(PROBE)
#define NETIF_MSG_LINK		__NETIF_MSG(LINK)
#define NETIF_MSG_TIMER		__NETIF_MSG(TIMER)
#define NETIF_MSG_IFDOWN	__NETIF_MSG(IFDOWN)
#define NETIF_MSG_IFUP		__NETIF_MSG(IFUP)
#define NETIF_MSG_RX_ERR	__NETIF_MSG(RX_ERR)
#define NETIF_MSG_TX_ERR	__NETIF_MSG(TX_ERR)
#define NETIF_MSG_TX_QUEUED	__NETIF_MSG(TX_QUEUED)
#define NETIF_MSG_INTR		__NETIF_MSG(INTR)
#define NETIF_MSG_TX_DONE	__NETIF_MSG(TX_DONE)
#define NETIF_MSG_RX_STATUS	__NETIF_MSG(RX_STATUS)
#define NETIF_MSG_PKTDATA	__NETIF_MSG(PKTDATA)
#define NETIF_MSG_HW		__NETIF_MSG(HW)
#define NETIF_MSG_WOL		__NETIF_MSG(WOL)

#define netif_msg_drv(p)	((p)->msg_enable & NETIF_MSG_DRV)
#define netif_msg_probe(p)	((p)->msg_enable & NETIF_MSG_PROBE)
#define netif_msg_link(p)	((p)->msg_enable & NETIF_MSG_LINK)
#define netif_msg_timer(p)	((p)->msg_enable & NETIF_MSG_TIMER)
#define netif_msg_ifdown(p)	((p)->msg_enable & NETIF_MSG_IFDOWN)
#define netif_msg_ifup(p)	((p)->msg_enable & NETIF_MSG_IFUP)
#define netif_msg_rx_err(p)	((p)->msg_enable & NETIF_MSG_RX_ERR)
#define netif_msg_tx_err(p)	((p)->msg_enable & NETIF_MSG_TX_ERR)
#define netif_msg_tx_queued(p)	((p)->msg_enable & NETIF_MSG_TX_QUEUED)
#define netif_msg_intr(p)	((p)->msg_enable & NETIF_MSG_INTR)
#define netif_msg_tx_done(p)	((p)->msg_enable & NETIF_MSG_TX_DONE)
#define netif_msg_rx_status(p)	((p)->msg_enable & NETIF_MSG_RX_STATUS)
#define netif_msg_pktdata(p)	((p)->msg_enable & NETIF_MSG_PKTDATA)
#define netif_msg_hw(p)		((p)->msg_enable & NETIF_MSG_HW)
#define netif_msg_wol(p)	((p)->msg_enable & NETIF_MSG_WOL)

static inline u32 netif_msg_init(int debug_value, int default_msg_enable_bits)
{
	
	if (debug_value < 0 || debug_value >= (sizeof(u32) * 8))
		return default_msg_enable_bits;
	if (debug_value == 0)	
		return 0;
	
	return (1U << debug_value) - 1;
}

static inline void __netif_tx_lock(struct netdev_queue *txq, int cpu)
{
	spin_lock(&txq->_xmit_lock);
	
	WRITE_ONCE(txq->xmit_lock_owner, cpu);
}

static inline bool __netif_tx_acquire(struct netdev_queue *txq)
{
	__acquire(&txq->_xmit_lock);
	return true;
}

static inline void __netif_tx_release(struct netdev_queue *txq)
{
	__release(&txq->_xmit_lock);
}

static inline void __netif_tx_lock_bh(struct netdev_queue *txq)
{
	spin_lock_bh(&txq->_xmit_lock);
	
	WRITE_ONCE(txq->xmit_lock_owner, smp_processor_id());
}

static inline bool __netif_tx_trylock(struct netdev_queue *txq)
{
	bool ok = spin_trylock(&txq->_xmit_lock);

	if (likely(ok)) {
		
		WRITE_ONCE(txq->xmit_lock_owner, smp_processor_id());
	}
	return ok;
}

static inline void __netif_tx_unlock(struct netdev_queue *txq)
{
	
	WRITE_ONCE(txq->xmit_lock_owner, -1);
	spin_unlock(&txq->_xmit_lock);
}

static inline void __netif_tx_unlock_bh(struct netdev_queue *txq)
{
	
	WRITE_ONCE(txq->xmit_lock_owner, -1);
	spin_unlock_bh(&txq->_xmit_lock);
}


static inline void txq_trans_update(struct netdev_queue *txq)
{
	if (txq->xmit_lock_owner != -1)
		WRITE_ONCE(txq->trans_start, jiffies);
}

static inline void txq_trans_cond_update(struct netdev_queue *txq)
{
	unsigned long now = jiffies;

	if (READ_ONCE(txq->trans_start) != now)
		WRITE_ONCE(txq->trans_start, now);
}


static inline void netif_trans_update(struct net_device *dev)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, 0);

	txq_trans_cond_update(txq);
}


void netif_tx_lock(struct net_device *dev);

static inline void netif_tx_lock_bh(struct net_device *dev)
{
	local_bh_disable();
	netif_tx_lock(dev);
}

void netif_tx_unlock(struct net_device *dev);

static inline void netif_tx_unlock_bh(struct net_device *dev)
{
	netif_tx_unlock(dev);
	local_bh_enable();
}

#define HARD_TX_LOCK(dev, txq, cpu) {			\
	if ((dev->features & NETIF_F_LLTX) == 0) {	\
		__netif_tx_lock(txq, cpu);		\
	} else {					\
		__netif_tx_acquire(txq);		\
	}						\
}

#define HARD_TX_TRYLOCK(dev, txq)			\
	(((dev->features & NETIF_F_LLTX) == 0) ?	\
		__netif_tx_trylock(txq) :		\
		__netif_tx_acquire(txq))

#define HARD_TX_UNLOCK(dev, txq) {			\
	if ((dev->features & NETIF_F_LLTX) == 0) {	\
		__netif_tx_unlock(txq);			\
	} else {					\
		__netif_tx_release(txq);		\
	}						\
}

static inline void netif_tx_disable(struct net_device *dev)
{
	unsigned int i;
	int cpu;

	local_bh_disable();
	cpu = smp_processor_id();
	spin_lock(&dev->tx_global_lock);
	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);

		__netif_tx_lock(txq, cpu);
		netif_tx_stop_queue(txq);
		__netif_tx_unlock(txq);
	}
	spin_unlock(&dev->tx_global_lock);
	local_bh_enable();
}

static inline void netif_addr_lock(struct net_device *dev)
{
	unsigned char nest_level = 0;

#ifdef CONFIG_LOCKDEP
	nest_level = dev->nested_level;
#endif
	spin_lock_nested(&dev->addr_list_lock, nest_level);
}

static inline void netif_addr_lock_bh(struct net_device *dev)
{
	unsigned char nest_level = 0;

#ifdef CONFIG_LOCKDEP
	nest_level = dev->nested_level;
#endif
	local_bh_disable();
	spin_lock_nested(&dev->addr_list_lock, nest_level);
}

static inline void netif_addr_unlock(struct net_device *dev)
{
	spin_unlock(&dev->addr_list_lock);
}

static inline void netif_addr_unlock_bh(struct net_device *dev)
{
	spin_unlock_bh(&dev->addr_list_lock);
}


#define for_each_dev_addr(dev, ha) \
		list_for_each_entry_rcu(ha, &dev->dev_addrs.list, list)



void ether_setup(struct net_device *dev);


struct net_device *alloc_netdev_mqs(int sizeof_priv, const char *name,
				    unsigned char name_assign_type,
				    void (*setup)(struct net_device *),
				    unsigned int txqs, unsigned int rxqs);
#define alloc_netdev(sizeof_priv, name, name_assign_type, setup) \
	alloc_netdev_mqs(sizeof_priv, name, name_assign_type, setup, 1, 1)

#define alloc_netdev_mq(sizeof_priv, name, name_assign_type, setup, count) \
	alloc_netdev_mqs(sizeof_priv, name, name_assign_type, setup, count, \
			 count)

int register_netdev(struct net_device *dev);
void unregister_netdev(struct net_device *dev);

int devm_register_netdev(struct device *dev, struct net_device *ndev);


int __hw_addr_sync(struct netdev_hw_addr_list *to_list,
		   struct netdev_hw_addr_list *from_list, int addr_len);
void __hw_addr_unsync(struct netdev_hw_addr_list *to_list,
		      struct netdev_hw_addr_list *from_list, int addr_len);
int __hw_addr_sync_dev(struct netdev_hw_addr_list *list,
		       struct net_device *dev,
		       int (*sync)(struct net_device *, const unsigned char *),
		       int (*unsync)(struct net_device *,
				     const unsigned char *));
int __hw_addr_ref_sync_dev(struct netdev_hw_addr_list *list,
			   struct net_device *dev,
			   int (*sync)(struct net_device *,
				       const unsigned char *, int),
			   int (*unsync)(struct net_device *,
					 const unsigned char *, int));
void __hw_addr_ref_unsync_dev(struct netdev_hw_addr_list *list,
			      struct net_device *dev,
			      int (*unsync)(struct net_device *,
					    const unsigned char *, int));
void __hw_addr_unsync_dev(struct netdev_hw_addr_list *list,
			  struct net_device *dev,
			  int (*unsync)(struct net_device *,
					const unsigned char *));
void __hw_addr_init(struct netdev_hw_addr_list *list);


void dev_addr_mod(struct net_device *dev, unsigned int offset,
		  const void *addr, size_t len);

static inline void
__dev_addr_set(struct net_device *dev, const void *addr, size_t len)
{
	dev_addr_mod(dev, 0, addr, len);
}

static inline void dev_addr_set(struct net_device *dev, const u8 *addr)
{
	__dev_addr_set(dev, addr, dev->addr_len);
}

int dev_addr_add(struct net_device *dev, const unsigned char *addr,
		 unsigned char addr_type);
int dev_addr_del(struct net_device *dev, const unsigned char *addr,
		 unsigned char addr_type);


int dev_uc_add(struct net_device *dev, const unsigned char *addr);
int dev_uc_add_excl(struct net_device *dev, const unsigned char *addr);
int dev_uc_del(struct net_device *dev, const unsigned char *addr);
int dev_uc_sync(struct net_device *to, struct net_device *from);
int dev_uc_sync_multiple(struct net_device *to, struct net_device *from);
void dev_uc_unsync(struct net_device *to, struct net_device *from);
void dev_uc_flush(struct net_device *dev);
void dev_uc_init(struct net_device *dev);


static inline int __dev_uc_sync(struct net_device *dev,
				int (*sync)(struct net_device *,
					    const unsigned char *),
				int (*unsync)(struct net_device *,
					      const unsigned char *))
{
	return __hw_addr_sync_dev(&dev->uc, dev, sync, unsync);
}


static inline void __dev_uc_unsync(struct net_device *dev,
				   int (*unsync)(struct net_device *,
						 const unsigned char *))
{
	__hw_addr_unsync_dev(&dev->uc, dev, unsync);
}


int dev_mc_add(struct net_device *dev, const unsigned char *addr);
int dev_mc_add_global(struct net_device *dev, const unsigned char *addr);
int dev_mc_add_excl(struct net_device *dev, const unsigned char *addr);
int dev_mc_del(struct net_device *dev, const unsigned char *addr);
int dev_mc_del_global(struct net_device *dev, const unsigned char *addr);
int dev_mc_sync(struct net_device *to, struct net_device *from);
int dev_mc_sync_multiple(struct net_device *to, struct net_device *from);
void dev_mc_unsync(struct net_device *to, struct net_device *from);
void dev_mc_flush(struct net_device *dev);
void dev_mc_init(struct net_device *dev);


static inline int __dev_mc_sync(struct net_device *dev,
				int (*sync)(struct net_device *,
					    const unsigned char *),
				int (*unsync)(struct net_device *,
					      const unsigned char *))
{
	return __hw_addr_sync_dev(&dev->mc, dev, sync, unsync);
}


static inline void __dev_mc_unsync(struct net_device *dev,
				   int (*unsync)(struct net_device *,
						 const unsigned char *))
{
	__hw_addr_unsync_dev(&dev->mc, dev, unsync);
}


void dev_set_rx_mode(struct net_device *dev);
int dev_set_promiscuity(struct net_device *dev, int inc);
int dev_set_allmulti(struct net_device *dev, int inc);
void netdev_state_change(struct net_device *dev);
void __netdev_notify_peers(struct net_device *dev);
void netdev_notify_peers(struct net_device *dev);
void netdev_features_change(struct net_device *dev);

void dev_load(struct net *net, const char *name);
struct rtnl_link_stats64 *dev_get_stats(struct net_device *dev,
					struct rtnl_link_stats64 *storage);
void netdev_stats_to_stats64(struct rtnl_link_stats64 *stats64,
			     const struct net_device_stats *netdev_stats);
void dev_fetch_sw_netstats(struct rtnl_link_stats64 *s,
			   const struct pcpu_sw_netstats __percpu *netstats);
void dev_get_tstats64(struct net_device *dev, struct rtnl_link_stats64 *s);

extern int		netdev_max_backlog;
extern int		dev_rx_weight;
extern int		dev_tx_weight;
extern int		gro_normal_batch;

enum {
	NESTED_SYNC_IMM_BIT,
	NESTED_SYNC_TODO_BIT,
};

#define __NESTED_SYNC_BIT(bit)	((u32)1 << (bit))
#define __NESTED_SYNC(name)	__NESTED_SYNC_BIT(NESTED_SYNC_ ## name ## _BIT)

#define NESTED_SYNC_IMM		__NESTED_SYNC(IMM)
#define NESTED_SYNC_TODO	__NESTED_SYNC(TODO)

struct netdev_nested_priv {
	unsigned char flags;
	void *data;
};

bool netdev_has_upper_dev(struct net_device *dev, struct net_device *upper_dev);
struct net_device *netdev_upper_get_next_dev_rcu(struct net_device *dev,
						     struct list_head **iter);


#define netdev_for_each_upper_dev_rcu(dev, updev, iter) \
	for (iter = &(dev)->adj_list.upper, \
	     updev = netdev_upper_get_next_dev_rcu(dev, &(iter)); \
	     updev; \
	     updev = netdev_upper_get_next_dev_rcu(dev, &(iter)))

int netdev_walk_all_upper_dev_rcu(struct net_device *dev,
				  int (*fn)(struct net_device *upper_dev,
					    struct netdev_nested_priv *priv),
				  struct netdev_nested_priv *priv);

bool netdev_has_upper_dev_all_rcu(struct net_device *dev,
				  struct net_device *upper_dev);

bool netdev_has_any_upper_dev(struct net_device *dev);

void *netdev_lower_get_next_private(struct net_device *dev,
				    struct list_head **iter);
void *netdev_lower_get_next_private_rcu(struct net_device *dev,
					struct list_head **iter);

#define netdev_for_each_lower_private(dev, priv, iter) \
	for (iter = (dev)->adj_list.lower.next, \
	     priv = netdev_lower_get_next_private(dev, &(iter)); \
	     priv; \
	     priv = netdev_lower_get_next_private(dev, &(iter)))

#define netdev_for_each_lower_private_rcu(dev, priv, iter) \
	for (iter = &(dev)->adj_list.lower, \
	     priv = netdev_lower_get_next_private_rcu(dev, &(iter)); \
	     priv; \
	     priv = netdev_lower_get_next_private_rcu(dev, &(iter)))

void *netdev_lower_get_next(struct net_device *dev,
				struct list_head **iter);

#define netdev_for_each_lower_dev(dev, ldev, iter) \
	for (iter = (dev)->adj_list.lower.next, \
	     ldev = netdev_lower_get_next(dev, &(iter)); \
	     ldev; \
	     ldev = netdev_lower_get_next(dev, &(iter)))

struct net_device *netdev_next_lower_dev_rcu(struct net_device *dev,
					     struct list_head **iter);
int netdev_walk_all_lower_dev(struct net_device *dev,
			      int (*fn)(struct net_device *lower_dev,
					struct netdev_nested_priv *priv),
			      struct netdev_nested_priv *priv);
int netdev_walk_all_lower_dev_rcu(struct net_device *dev,
				  int (*fn)(struct net_device *lower_dev,
					    struct netdev_nested_priv *priv),
				  struct netdev_nested_priv *priv);

void *netdev_adjacent_get_private(struct list_head *adj_list);
void *netdev_lower_get_first_private_rcu(struct net_device *dev);
struct net_device *netdev_master_upper_dev_get(struct net_device *dev);
struct net_device *netdev_master_upper_dev_get_rcu(struct net_device *dev);
int netdev_upper_dev_link(struct net_device *dev, struct net_device *upper_dev,
			  struct netlink_ext_ack *extack);
int netdev_master_upper_dev_link(struct net_device *dev,
				 struct net_device *upper_dev,
				 void *upper_priv, void *upper_info,
				 struct netlink_ext_ack *extack);
void netdev_upper_dev_unlink(struct net_device *dev,
			     struct net_device *upper_dev);
int netdev_adjacent_change_prepare(struct net_device *old_dev,
				   struct net_device *new_dev,
				   struct net_device *dev,
				   struct netlink_ext_ack *extack);
void netdev_adjacent_change_commit(struct net_device *old_dev,
				   struct net_device *new_dev,
				   struct net_device *dev);
void netdev_adjacent_change_abort(struct net_device *old_dev,
				  struct net_device *new_dev,
				  struct net_device *dev);
void netdev_adjacent_rename_links(struct net_device *dev, char *oldname);
void *netdev_lower_dev_get_private(struct net_device *dev,
				   struct net_device *lower_dev);
void netdev_lower_state_changed(struct net_device *lower_dev,
				void *lower_state_info);


#define NETDEV_RSS_KEY_LEN 52
extern u8 netdev_rss_key[NETDEV_RSS_KEY_LEN] __read_mostly;
void netdev_rss_key_fill(void *buffer, size_t len);

int skb_checksum_help(struct sk_buff *skb);
int skb_crc32c_csum_help(struct sk_buff *skb);
int skb_csum_hwoffload_help(struct sk_buff *skb,
			    const netdev_features_t features);

struct netdev_bonding_info {
	ifslave	slave;
	ifbond	master;
};

struct netdev_notifier_bonding_info {
	struct netdev_notifier_info info; 
	struct netdev_bonding_info  bonding_info;
};

void netdev_bonding_info_change(struct net_device *dev,
				struct netdev_bonding_info *bonding_info);

#if IS_ENABLED(CONFIG_ETHTOOL_NETLINK)
void ethtool_notify(struct net_device *dev, unsigned int cmd, const void *data);
#else
static inline void ethtool_notify(struct net_device *dev, unsigned int cmd,
				  const void *data)
{
}
#endif

__be16 skb_network_protocol(struct sk_buff *skb, int *depth);

static inline bool can_checksum_protocol(netdev_features_t features,
					 __be16 protocol)
{
	if (protocol == htons(ETH_P_FCOE))
		return !!(features & NETIF_F_FCOE_CRC);

	

	if (features & NETIF_F_HW_CSUM) {
		
		return true;
	}

	switch (protocol) {
	case htons(ETH_P_IP):
		return !!(features & NETIF_F_IP_CSUM);
	case htons(ETH_P_IPV6):
		return !!(features & NETIF_F_IPV6_CSUM);
	default:
		return false;
	}
}

#ifdef CONFIG_BUG
void netdev_rx_csum_fault(struct net_device *dev, struct sk_buff *skb);
#else
static inline void netdev_rx_csum_fault(struct net_device *dev,
					struct sk_buff *skb)
{
}
#endif

void net_enable_timestamp(void);
void net_disable_timestamp(void);

static inline ktime_t netdev_get_tstamp(struct net_device *dev,
					const struct skb_shared_hwtstamps *hwtstamps,
					bool cycles)
{
	const struct net_device_ops *ops = dev->netdev_ops;

	if (ops->ndo_get_tstamp)
		return ops->ndo_get_tstamp(dev, hwtstamps, cycles);

	return hwtstamps->hwtstamp;
}

static inline netdev_tx_t __netdev_start_xmit(const struct net_device_ops *ops,
					      struct sk_buff *skb, struct net_device *dev,
					      bool more)
{
	__this_cpu_write(softnet_data.xmit.more, more);
	return ops->ndo_start_xmit(skb, dev);
}

static inline bool netdev_xmit_more(void)
{
	return __this_cpu_read(softnet_data.xmit.more);
}

static inline netdev_tx_t netdev_start_xmit(struct sk_buff *skb, struct net_device *dev,
					    struct netdev_queue *txq, bool more)
{
	const struct net_device_ops *ops = dev->netdev_ops;
	netdev_tx_t rc;

	rc = __netdev_start_xmit(ops, skb, dev, more);
	if (rc == NETDEV_TX_OK)
		txq_trans_update(txq);

	return rc;
}

int netdev_class_create_file_ns(const struct class_attribute *class_attr,
				const void *ns);
void netdev_class_remove_file_ns(const struct class_attribute *class_attr,
				 const void *ns);

extern const struct kobj_ns_type_operations net_ns_type_operations;

const char *netdev_drivername(const struct net_device *dev);

static inline netdev_features_t netdev_intersect_features(netdev_features_t f1,
							  netdev_features_t f2)
{
	if ((f1 ^ f2) & NETIF_F_HW_CSUM) {
		if (f1 & NETIF_F_HW_CSUM)
			f1 |= (NETIF_F_IP_CSUM|NETIF_F_IPV6_CSUM);
		else
			f2 |= (NETIF_F_IP_CSUM|NETIF_F_IPV6_CSUM);
	}

	return f1 & f2;
}

static inline netdev_features_t netdev_get_wanted_features(
	struct net_device *dev)
{
	return (dev->features & ~dev->hw_features) | dev->wanted_features;
}
netdev_features_t netdev_increment_features(netdev_features_t all,
	netdev_features_t one, netdev_features_t mask);


static inline netdev_features_t netdev_add_tso_features(netdev_features_t features,
							netdev_features_t mask)
{
	return netdev_increment_features(features, NETIF_F_ALL_TSO, mask);
}

int __netdev_update_features(struct net_device *dev);
void netdev_update_features(struct net_device *dev);
void netdev_change_features(struct net_device *dev);

void netif_stacked_transfer_operstate(const struct net_device *rootdev,
					struct net_device *dev);

netdev_features_t passthru_features_check(struct sk_buff *skb,
					  struct net_device *dev,
					  netdev_features_t features);
netdev_features_t netif_skb_features(struct sk_buff *skb);
void skb_warn_bad_offload(const struct sk_buff *skb);

static inline bool net_gso_ok(netdev_features_t features, int gso_type)
{
	netdev_features_t feature = (netdev_features_t)gso_type << NETIF_F_GSO_SHIFT;

	
	BUILD_BUG_ON(SKB_GSO_TCPV4   != (NETIF_F_TSO >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_DODGY   != (NETIF_F_GSO_ROBUST >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_TCP_ECN != (NETIF_F_TSO_ECN >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_TCP_FIXEDID != (NETIF_F_TSO_MANGLEID >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_TCPV6   != (NETIF_F_TSO6 >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_FCOE    != (NETIF_F_FSO >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_GRE     != (NETIF_F_GSO_GRE >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_GRE_CSUM != (NETIF_F_GSO_GRE_CSUM >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_IPXIP4  != (NETIF_F_GSO_IPXIP4 >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_IPXIP6  != (NETIF_F_GSO_IPXIP6 >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_UDP_TUNNEL != (NETIF_F_GSO_UDP_TUNNEL >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_UDP_TUNNEL_CSUM != (NETIF_F_GSO_UDP_TUNNEL_CSUM >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_PARTIAL != (NETIF_F_GSO_PARTIAL >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_TUNNEL_REMCSUM != (NETIF_F_GSO_TUNNEL_REMCSUM >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_SCTP    != (NETIF_F_GSO_SCTP >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_ESP != (NETIF_F_GSO_ESP >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_UDP != (NETIF_F_GSO_UDP >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_UDP_L4 != (NETIF_F_GSO_UDP_L4 >> NETIF_F_GSO_SHIFT));
	BUILD_BUG_ON(SKB_GSO_FRAGLIST != (NETIF_F_GSO_FRAGLIST >> NETIF_F_GSO_SHIFT));

	return (features & feature) == feature;
}

static inline bool skb_gso_ok(struct sk_buff *skb, netdev_features_t features)
{
	return net_gso_ok(features, skb_shinfo(skb)->gso_type) &&
	       (!skb_has_frag_list(skb) || (features & NETIF_F_FRAGLIST));
}

static inline bool netif_needs_gso(struct sk_buff *skb,
				   netdev_features_t features)
{
	return skb_is_gso(skb) && (!skb_gso_ok(skb, features) ||
		unlikely((skb->ip_summed != CHECKSUM_PARTIAL) &&
			 (skb->ip_summed != CHECKSUM_UNNECESSARY)));
}

void netif_set_tso_max_size(struct net_device *dev, unsigned int size);
void netif_set_tso_max_segs(struct net_device *dev, unsigned int segs);
void netif_inherit_tso_max(struct net_device *to,
			   const struct net_device *from);

static inline bool netif_is_macsec(const struct net_device *dev)
{
	return dev->priv_flags & IFF_MACSEC;
}

static inline bool netif_is_macvlan(const struct net_device *dev)
{
	return dev->priv_flags & IFF_MACVLAN;
}

static inline bool netif_is_macvlan_port(const struct net_device *dev)
{
	return dev->priv_flags & IFF_MACVLAN_PORT;
}

static inline bool netif_is_bond_master(const struct net_device *dev)
{
	return dev->flags & IFF_MASTER && dev->priv_flags & IFF_BONDING;
}

static inline bool netif_is_bond_slave(const struct net_device *dev)
{
	return dev->flags & IFF_SLAVE && dev->priv_flags & IFF_BONDING;
}

static inline bool netif_supports_nofcs(struct net_device *dev)
{
	return dev->priv_flags & IFF_SUPP_NOFCS;
}

static inline bool netif_has_l3_rx_handler(const struct net_device *dev)
{
	return dev->priv_flags & IFF_L3MDEV_RX_HANDLER;
}

static inline bool netif_is_l3_master(const struct net_device *dev)
{
	return dev->priv_flags & IFF_L3MDEV_MASTER;
}

static inline bool netif_is_l3_slave(const struct net_device *dev)
{
	return dev->priv_flags & IFF_L3MDEV_SLAVE;
}

static inline int dev_sdif(const struct net_device *dev)
{
#ifdef CONFIG_NET_L3_MASTER_DEV
	if (netif_is_l3_slave(dev))
		return dev->ifindex;
#endif
	return 0;
}

static inline bool netif_is_bridge_master(const struct net_device *dev)
{
	return dev->priv_flags & IFF_EBRIDGE;
}

static inline bool netif_is_bridge_port(const struct net_device *dev)
{
	return dev->priv_flags & IFF_BRIDGE_PORT;
}

static inline bool netif_is_ovs_master(const struct net_device *dev)
{
	return dev->priv_flags & IFF_OPENVSWITCH;
}

static inline bool netif_is_ovs_port(const struct net_device *dev)
{
	return dev->priv_flags & IFF_OVS_DATAPATH;
}

static inline bool netif_is_any_bridge_master(const struct net_device *dev)
{
	return netif_is_bridge_master(dev) || netif_is_ovs_master(dev);
}

static inline bool netif_is_any_bridge_port(const struct net_device *dev)
{
	return netif_is_bridge_port(dev) || netif_is_ovs_port(dev);
}

static inline bool netif_is_team_master(const struct net_device *dev)
{
	return dev->priv_flags & IFF_TEAM;
}

static inline bool netif_is_team_port(const struct net_device *dev)
{
	return dev->priv_flags & IFF_TEAM_PORT;
}

static inline bool netif_is_lag_master(const struct net_device *dev)
{
	return netif_is_bond_master(dev) || netif_is_team_master(dev);
}

static inline bool netif_is_lag_port(const struct net_device *dev)
{
	return netif_is_bond_slave(dev) || netif_is_team_port(dev);
}

static inline bool netif_is_rxfh_configured(const struct net_device *dev)
{
	return dev->priv_flags & IFF_RXFH_CONFIGURED;
}

static inline bool netif_is_failover(const struct net_device *dev)
{
	return dev->priv_flags & IFF_FAILOVER;
}

static inline bool netif_is_failover_slave(const struct net_device *dev)
{
	return dev->priv_flags & IFF_FAILOVER_SLAVE;
}


static inline void netif_keep_dst(struct net_device *dev)
{
	dev->priv_flags &= ~(IFF_XMIT_DST_RELEASE | IFF_XMIT_DST_RELEASE_PERM);
}


static inline bool netif_reduces_vlan_mtu(struct net_device *dev)
{
	
	return netif_is_macsec(dev);
}

extern struct pernet_operations __net_initdata loopback_net_ops;





static inline const char *netdev_name(const struct net_device *dev)
{
	if (!dev->name[0] || strchr(dev->name, '%'))
		return "(unnamed net_device)";
	return dev->name;
}

static inline const char *netdev_reg_state(const struct net_device *dev)
{
	switch (dev->reg_state) {
	case NETREG_UNINITIALIZED: return " (uninitialized)";
	case NETREG_REGISTERED: return "";
	case NETREG_UNREGISTERING: return " (unregistering)";
	case NETREG_UNREGISTERED: return " (unregistered)";
	case NETREG_RELEASED: return " (released)";
	case NETREG_DUMMY: return " (dummy)";
	}

	WARN_ONCE(1, "%s: unknown reg_state %d\n", dev->name, dev->reg_state);
	return " (unknown)";
}

#define MODULE_ALIAS_NETDEV(device) \
	MODULE_ALIAS("netdev-" device)


#define netdev_WARN(dev, format, args...)			\
	WARN(1, "netdevice: %s%s: " format, netdev_name(dev),	\
	     netdev_reg_state(dev), ##args)

#define netdev_WARN_ONCE(dev, format, args...)				\
	WARN_ONCE(1, "netdevice: %s%s: " format, netdev_name(dev),	\
		  netdev_reg_state(dev), ##args)


#define PTYPE_HASH_SIZE	(16)
#define PTYPE_HASH_MASK	(PTYPE_HASH_SIZE - 1)

extern struct list_head ptype_all __read_mostly;
extern struct list_head ptype_base[PTYPE_HASH_SIZE] __read_mostly;

extern struct net_device *blackhole_netdev;


#define DEV_STATS_INC(DEV, FIELD) atomic_long_inc(&(DEV)->stats.__##FIELD)
#define DEV_STATS_ADD(DEV, FIELD, VAL) 	\
		atomic_long_add((VAL), &(DEV)->stats.__##FIELD)
#define DEV_STATS_READ(DEV, FIELD) atomic_long_read(&(DEV)->stats.__##FIELD)

#endif	
