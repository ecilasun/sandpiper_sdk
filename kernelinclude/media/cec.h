/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _MEDIA_CEC_H
#define _MEDIA_CEC_H

#include <linux/poll.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/cec-funcs.h>
#include <media/rc-core.h>

#define CEC_CAP_DEFAULTS (CEC_CAP_LOG_ADDRS | CEC_CAP_TRANSMIT | \
			  CEC_CAP_PASSTHROUGH | CEC_CAP_RC)


struct cec_devnode {
	
	struct device dev;
	struct cdev cdev;

	
	int minor;
	
	struct mutex lock;
	bool registered;
	bool unregistered;
	
	struct mutex lock_fhs;
	struct list_head fhs;
};

struct cec_adapter;
struct cec_data;
struct cec_pin;
struct cec_notifier;

struct cec_data {
	struct list_head list;
	struct list_head xfer_list;
	struct cec_adapter *adap;
	struct cec_msg msg;
	struct cec_fh *fh;
	struct delayed_work work;
	struct completion c;
	u8 attempts;
	bool blocking;
	bool completed;
};

struct cec_msg_entry {
	struct list_head	list;
	struct cec_msg		msg;
};

struct cec_event_entry {
	struct list_head	list;
	struct cec_event	ev;
};

#define CEC_NUM_CORE_EVENTS 2
#define CEC_NUM_EVENTS CEC_EVENT_PIN_5V_HIGH

struct cec_fh {
	struct list_head	list;
	struct list_head	xfer_list;
	struct cec_adapter	*adap;
	u8			mode_initiator;
	u8			mode_follower;

	
	wait_queue_head_t	wait;
	struct mutex		lock;
	struct list_head	events[CEC_NUM_EVENTS]; 
	u16			queued_events[CEC_NUM_EVENTS];
	unsigned int		total_queued_events;
	struct cec_event_entry	core_events[CEC_NUM_CORE_EVENTS];
	struct list_head	msgs; 
	unsigned int		queued_msgs;
};

#define CEC_SIGNAL_FREE_TIME_RETRY		3
#define CEC_SIGNAL_FREE_TIME_NEW_INITIATOR	5
#define CEC_SIGNAL_FREE_TIME_NEXT_XFER		7


#define CEC_FREE_TIME_TO_USEC(ft)		((ft) * 2400)

struct cec_adap_ops {
	
	int (*adap_enable)(struct cec_adapter *adap, bool enable);
	int (*adap_monitor_all_enable)(struct cec_adapter *adap, bool enable);
	int (*adap_monitor_pin_enable)(struct cec_adapter *adap, bool enable);
	int (*adap_log_addr)(struct cec_adapter *adap, u8 logical_addr);
	void (*adap_unconfigured)(struct cec_adapter *adap);
	int (*adap_transmit)(struct cec_adapter *adap, u8 attempts,
			     u32 signal_free_time, struct cec_msg *msg);
	void (*adap_nb_transmit_canceled)(struct cec_adapter *adap,
					  const struct cec_msg *msg);
	void (*adap_status)(struct cec_adapter *adap, struct seq_file *file);
	void (*adap_free)(struct cec_adapter *adap);

	
	int (*error_inj_show)(struct cec_adapter *adap, struct seq_file *sf);
	bool (*error_inj_parse_line)(struct cec_adapter *adap, char *line);

	
	void (*configured)(struct cec_adapter *adap);
	int (*received)(struct cec_adapter *adap, struct cec_msg *msg);
};


#define CEC_MAX_MSG_RX_QUEUE_SZ		(18 * 3)


#define CEC_MAX_MSG_TX_QUEUE_SZ		(18 * 1)


struct cec_adapter {
	struct module *owner;
	char name[32];
	struct cec_devnode devnode;
	struct mutex lock;
	struct rc_dev *rc;

	struct list_head transmit_queue;
	unsigned int transmit_queue_sz;
	struct list_head wait_queue;
	struct cec_data *transmitting;
	bool transmit_in_progress;
	bool transmit_in_progress_aborted;
	unsigned int xfer_timeout_ms;

	struct task_struct *kthread_config;
	struct completion config_completion;

	struct task_struct *kthread;
	wait_queue_head_t kthread_waitq;

	const struct cec_adap_ops *ops;
	void *priv;
	u32 capabilities;
	u8 available_log_addrs;

	u16 phys_addr;
	bool needs_hpd;
	bool is_enabled;
	bool is_claiming_log_addrs;
	bool is_configuring;
	bool must_reconfigure;
	bool is_configured;
	bool cec_pin_is_high;
	bool adap_controls_phys_addr;
	u8 last_initiator;
	u32 monitor_all_cnt;
	u32 monitor_pin_cnt;
	u32 follower_cnt;
	struct cec_fh *cec_follower;
	struct cec_fh *cec_initiator;
	bool passthrough;
	struct cec_log_addrs log_addrs;
	struct cec_connector_info conn_info;

	u32 tx_timeouts;

#ifdef CONFIG_CEC_NOTIFIER
	struct cec_notifier *notifier;
#endif
#ifdef CONFIG_CEC_PIN
	struct cec_pin *pin;
#endif

	struct dentry *cec_dir;

	u32 sequence;

	char input_phys[32];
};

static inline void *cec_get_drvdata(const struct cec_adapter *adap)
{
	return adap->priv;
}

static inline bool cec_has_log_addr(const struct cec_adapter *adap, u8 log_addr)
{
	return adap->log_addrs.log_addr_mask & (1 << log_addr);
}

static inline bool cec_is_sink(const struct cec_adapter *adap)
{
	return adap->phys_addr == 0;
}


static inline bool cec_is_registered(const struct cec_adapter *adap)
{
	return adap && adap->devnode.registered;
}

#define cec_phys_addr_exp(pa) \
	((pa) >> 12), ((pa) >> 8) & 0xf, ((pa) >> 4) & 0xf, (pa) & 0xf

struct edid;
struct drm_connector;

#if IS_REACHABLE(CONFIG_CEC_CORE)
struct cec_adapter *cec_allocate_adapter(const struct cec_adap_ops *ops,
		void *priv, const char *name, u32 caps, u8 available_las);
int cec_register_adapter(struct cec_adapter *adap, struct device *parent);
void cec_unregister_adapter(struct cec_adapter *adap);
void cec_delete_adapter(struct cec_adapter *adap);

int cec_s_log_addrs(struct cec_adapter *adap, struct cec_log_addrs *log_addrs,
		    bool block);
void cec_s_phys_addr(struct cec_adapter *adap, u16 phys_addr,
		     bool block);
void cec_s_phys_addr_from_edid(struct cec_adapter *adap,
			       const struct edid *edid);
void cec_s_conn_info(struct cec_adapter *adap,
		     const struct cec_connector_info *conn_info);
int cec_transmit_msg(struct cec_adapter *adap, struct cec_msg *msg,
		     bool block);


void cec_transmit_done_ts(struct cec_adapter *adap, u8 status,
			  u8 arb_lost_cnt, u8 nack_cnt, u8 low_drive_cnt,
			  u8 error_cnt, ktime_t ts);

static inline void cec_transmit_done(struct cec_adapter *adap, u8 status,
				     u8 arb_lost_cnt, u8 nack_cnt,
				     u8 low_drive_cnt, u8 error_cnt)
{
	cec_transmit_done_ts(adap, status, arb_lost_cnt, nack_cnt,
			     low_drive_cnt, error_cnt, ktime_get());
}

void cec_transmit_attempt_done_ts(struct cec_adapter *adap,
				  u8 status, ktime_t ts);

static inline void cec_transmit_attempt_done(struct cec_adapter *adap,
					     u8 status)
{
	cec_transmit_attempt_done_ts(adap, status, ktime_get());
}

void cec_received_msg_ts(struct cec_adapter *adap,
			 struct cec_msg *msg, ktime_t ts);

static inline void cec_received_msg(struct cec_adapter *adap,
				    struct cec_msg *msg)
{
	cec_received_msg_ts(adap, msg, ktime_get());
}


void cec_queue_pin_cec_event(struct cec_adapter *adap, bool is_high,
			     bool dropped_events, ktime_t ts);


void cec_queue_pin_hpd_event(struct cec_adapter *adap, bool is_high, ktime_t ts);


void cec_queue_pin_5v_event(struct cec_adapter *adap, bool is_high, ktime_t ts);


u16 cec_get_edid_phys_addr(const u8 *edid, unsigned int size,
			   unsigned int *offset);

void cec_fill_conn_info_from_drm(struct cec_connector_info *conn_info,
				 const struct drm_connector *connector);

#else

static inline int cec_register_adapter(struct cec_adapter *adap,
				       struct device *parent)
{
	return 0;
}

static inline void cec_unregister_adapter(struct cec_adapter *adap)
{
}

static inline void cec_delete_adapter(struct cec_adapter *adap)
{
}

static inline void cec_s_phys_addr(struct cec_adapter *adap, u16 phys_addr,
				   bool block)
{
}

static inline void cec_s_phys_addr_from_edid(struct cec_adapter *adap,
					     const struct edid *edid)
{
}

static inline u16 cec_get_edid_phys_addr(const u8 *edid, unsigned int size,
					 unsigned int *offset)
{
	if (offset)
		*offset = 0;
	return CEC_PHYS_ADDR_INVALID;
}

static inline void cec_s_conn_info(struct cec_adapter *adap,
				   const struct cec_connector_info *conn_info)
{
}

static inline void
cec_fill_conn_info_from_drm(struct cec_connector_info *conn_info,
			    const struct drm_connector *connector)
{
	memset(conn_info, 0, sizeof(*conn_info));
}

#endif


static inline void cec_phys_addr_invalidate(struct cec_adapter *adap)
{
	cec_s_phys_addr(adap, CEC_PHYS_ADDR_INVALID, false);
}


static inline unsigned int cec_get_edid_spa_location(const u8 *edid,
						     unsigned int size)
{
	unsigned int blocks = size / 128;
	unsigned int block;
	u8 d;

	
	if (blocks < 2 || size % 128)
		return 0;

	
	if (edid[0x7e] + 1 < blocks)
		blocks = edid[0x7e] + 1;

	for (block = 1; block < blocks; block++) {
		unsigned int offset = block * 128;

		
		if (edid[offset] != 0x02 || edid[offset + 1] != 0x03)
			continue;

		
		d = edid[offset + 2] & 0x7f;
		
		if (d <= 4)
			continue;
		if (d > 4) {
			unsigned int i = offset + 4;
			unsigned int end = offset + d;

			
			do {
				u8 tag = edid[i] >> 5;
				u8 len = edid[i] & 0x1f;

				if (tag == 3 && len >= 5 && i + len <= end &&
				    edid[i + 1] == 0x03 &&
				    edid[i + 2] == 0x0c &&
				    edid[i + 3] == 0x00)
					return i + 4;
				i += len + 1;
			} while (i < end);
		}
	}
	return 0;
}

#endif 
