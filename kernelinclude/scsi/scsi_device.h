/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SCSI_SCSI_DEVICE_H
#define _SCSI_SCSI_DEVICE_H

#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/blk-mq.h>
#include <scsi/scsi.h>
#include <linux/atomic.h>
#include <linux/sbitmap.h>

struct bsg_device;
struct device;
struct request_queue;
struct scsi_cmnd;
struct scsi_lun;
struct scsi_sense_hdr;

typedef __u64 __bitwise blist_flags_t;

#define SCSI_SENSE_BUFFERSIZE	96

struct scsi_mode_data {
	__u32	length;
	__u16	block_descriptor_length;
	__u8	medium_type;
	__u8	device_specific;
	__u8	header_length;
	__u8	longlba:1;
};


enum scsi_device_state {
	SDEV_CREATED = 1,	
	SDEV_RUNNING,		
	SDEV_CANCEL,		
	SDEV_DEL,		
	SDEV_QUIESCE,		
	SDEV_OFFLINE,		
	SDEV_TRANSPORT_OFFLINE,	
	SDEV_BLOCK,		
	SDEV_CREATED_BLOCK,	
};

enum scsi_scan_mode {
	SCSI_SCAN_INITIAL = 0,
	SCSI_SCAN_RESCAN,
	SCSI_SCAN_MANUAL,
};

enum scsi_device_event {
	SDEV_EVT_MEDIA_CHANGE	= 1,	
	SDEV_EVT_INQUIRY_CHANGE_REPORTED,		
	SDEV_EVT_CAPACITY_CHANGE_REPORTED,		
	SDEV_EVT_SOFT_THRESHOLD_REACHED_REPORTED,	
	SDEV_EVT_MODE_PARAMETER_CHANGE_REPORTED,	
	SDEV_EVT_LUN_CHANGE_REPORTED,			
	SDEV_EVT_ALUA_STATE_CHANGE_REPORTED,		
	SDEV_EVT_POWER_ON_RESET_OCCURRED,		

	SDEV_EVT_FIRST		= SDEV_EVT_MEDIA_CHANGE,
	SDEV_EVT_LAST		= SDEV_EVT_POWER_ON_RESET_OCCURRED,

	SDEV_EVT_MAXBITS	= SDEV_EVT_LAST + 1
};

struct scsi_event {
	enum scsi_device_event	evt_type;
	struct list_head	node;

	
};


struct scsi_vpd {
	struct rcu_head	rcu;
	int		len;
	unsigned char	data[];
};

struct scsi_device {
	struct Scsi_Host *host;
	struct request_queue *request_queue;

	
	struct list_head    siblings;   
	struct list_head    same_target_siblings; 

	struct sbitmap budget_map;
	atomic_t device_blocked;	

	atomic_t restarts;
	spinlock_t list_lock;
	struct list_head starved_entry;
	unsigned short queue_depth;	
	unsigned short max_queue_depth;	
	unsigned short last_queue_full_depth; 
	unsigned short last_queue_full_count; 
	unsigned long last_queue_full_time;	
	unsigned long queue_ramp_up_period;	
#define SCSI_DEFAULT_RAMP_UP_PERIOD	(120 * HZ)

	unsigned long last_queue_ramp_up;	

	unsigned int id, channel;
	u64 lun;
	unsigned int manufacturer;	
	unsigned sector_size;	

	void *hostdata;		
	unsigned char type;
	char scsi_level;
	char inq_periph_qual;		
	struct mutex inquiry_mutex;
	unsigned char inquiry_len;	
	unsigned char * inquiry;	
	const char * vendor;		
	const char * model;		
	const char * rev;		

#define SCSI_DEFAULT_VPD_LEN	255	
	struct scsi_vpd __rcu *vpd_pg0;
	struct scsi_vpd __rcu *vpd_pg83;
	struct scsi_vpd __rcu *vpd_pg80;
	struct scsi_vpd __rcu *vpd_pg89;
	struct scsi_vpd __rcu *vpd_pgb0;
	struct scsi_vpd __rcu *vpd_pgb1;
	struct scsi_vpd __rcu *vpd_pgb2;

	struct scsi_target      *sdev_target;

	blist_flags_t		sdev_bflags; 
	unsigned int eh_timeout; 

	
	unsigned manage_system_start_stop:1;

	
	unsigned manage_runtime_start_stop:1;

	
	unsigned manage_shutdown:1;

	
	unsigned force_runtime_start_on_system_start:1;

	unsigned removable:1;
	unsigned changed:1;	
	unsigned busy:1;	
	unsigned lockable:1;	
	unsigned locked:1;      
	unsigned borken:1;	
	unsigned disconnect:1;	
	unsigned soft_reset:1;	
	unsigned sdtr:1;	
	unsigned wdtr:1;	
	unsigned ppr:1;		
	unsigned tagged_supported:1;	
	unsigned simple_tags:1;	
	unsigned was_reset:1;	
	unsigned expecting_cc_ua:1; 
	unsigned use_10_for_rw:1; 
	unsigned use_10_for_ms:1; 
	unsigned set_dbd_for_ms:1; 
	unsigned read_before_ms:1;	
	unsigned no_report_opcodes:1;	
	unsigned no_write_same:1;	
	unsigned use_16_for_rw:1; 
	unsigned use_16_for_sync:1;	
	unsigned skip_ms_page_8:1;	
	unsigned skip_ms_page_3f:1;	
	unsigned skip_vpd_pages:1;	
	unsigned try_vpd_pages:1;	
	unsigned use_192_bytes_for_3f:1; 
	unsigned no_start_on_add:1;	
	unsigned allow_restart:1; 
	unsigned no_start_on_resume:1; 
	unsigned start_stop_pwr_cond:1;	
	unsigned no_uld_attach:1; 
	unsigned select_no_atn:1;
	unsigned fix_capacity:1;	
	unsigned guess_capacity:1;	
	unsigned retry_hwerror:1;	
	unsigned last_sector_bug:1;	
	unsigned no_read_disc_info:1;	
	unsigned no_read_capacity_16:1; 
	unsigned try_rc_10_first:1;	
	unsigned security_supported:1;	
	unsigned is_visible:1;	
	unsigned wce_default_on:1;	
	unsigned no_dif:1;	
	unsigned broken_fua:1;		
	unsigned lun_in_cdb:1;		
	unsigned unmap_limit_for_ws:1;	
	unsigned rpm_autosuspend:1;	
	unsigned ignore_media_change:1; 
	unsigned silence_suspend:1;	
	unsigned no_vpd_size:1;		

	unsigned cdl_supported:1;	
	unsigned cdl_enable:1;		

	unsigned int queue_stopped;	
	bool offline_already;		

	atomic_t disk_events_disable_depth; 

	DECLARE_BITMAP(supported_events, SDEV_EVT_MAXBITS); 
	DECLARE_BITMAP(pending_events, SDEV_EVT_MAXBITS); 
	struct list_head event_list;	
	struct work_struct event_work;

	unsigned int max_device_blocked; 
#define SCSI_DEFAULT_DEVICE_BLOCKED	3

	atomic_t iorequest_cnt;
	atomic_t iodone_cnt;
	atomic_t ioerr_cnt;
	atomic_t iotmo_cnt;

	struct device		sdev_gendev,
				sdev_dev;

	struct work_struct	requeue_work;

	struct scsi_device_handler *handler;
	void			*handler_data;

	size_t			dma_drain_len;
	void			*dma_drain_buf;

	unsigned int		sg_timeout;
	unsigned int		sg_reserved_size;

	struct bsg_device	*bsg_dev;
	unsigned char		access_state;
	struct mutex		state_mutex;
	enum scsi_device_state sdev_state;
	struct task_struct	*quiesced_by;
	unsigned long		sdev_data[];
} __attribute__((aligned(sizeof(unsigned long))));

#define	to_scsi_device(d)	\
	container_of(d, struct scsi_device, sdev_gendev)
#define	class_to_sdev(d)	\
	container_of(d, struct scsi_device, sdev_dev)
#define transport_class_to_sdev(class_dev) \
	to_scsi_device(class_dev->parent)

#define sdev_dbg(sdev, fmt, a...) \
	dev_dbg(&(sdev)->sdev_gendev, fmt, ##a)


__printf(4, 5) void
sdev_prefix_printk(const char *, const struct scsi_device *, const char *,
		const char *, ...);

#define sdev_printk(l, sdev, fmt, a...)				\
	sdev_prefix_printk(l, sdev, NULL, fmt, ##a)

__printf(3, 4) void
scmd_printk(const char *, const struct scsi_cmnd *, const char *, ...);

#define scmd_dbg(scmd, fmt, a...)					\
	do {								\
		struct request *__rq = scsi_cmd_to_rq((scmd));		\
									\
		if (__rq->q->disk)					\
			sdev_dbg((scmd)->device, "[%s] " fmt,		\
				 __rq->q->disk->disk_name, ##a);	\
		else							\
			sdev_dbg((scmd)->device, fmt, ##a);		\
	} while (0)

enum scsi_target_state {
	STARGET_CREATED = 1,
	STARGET_RUNNING,
	STARGET_REMOVE,
	STARGET_CREATED_REMOVE,
	STARGET_DEL,
};


struct scsi_target {
	struct scsi_device	*starget_sdev_user;
	struct list_head	siblings;
	struct list_head	devices;
	struct device		dev;
	struct kref		reap_ref; 
	unsigned int		channel;
	unsigned int		id; 
	unsigned int		create:1; 
	unsigned int		single_lun:1;	
	unsigned int		pdt_1f_for_no_lun:1;	
	unsigned int		no_report_luns:1;	
	unsigned int		expecting_lun_change:1;	
	
	atomic_t		target_busy;
	atomic_t		target_blocked;

	
	unsigned int		can_queue;
	unsigned int		max_target_blocked;
#define SCSI_DEFAULT_TARGET_BLOCKED	3

	char			scsi_level;
	enum scsi_target_state	state;
	void 			*hostdata; 
	unsigned long		starget_data[]; 
	
} __attribute__((aligned(sizeof(unsigned long))));

#define to_scsi_target(d)	container_of(d, struct scsi_target, dev)
static inline struct scsi_target *scsi_target(struct scsi_device *sdev)
{
	return to_scsi_target(sdev->sdev_gendev.parent);
}
#define transport_class_to_starget(class_dev) \
	to_scsi_target(class_dev->parent)

#define starget_printk(prefix, starget, fmt, a...)	\
	dev_printk(prefix, &(starget)->dev, fmt, ##a)

extern struct scsi_device *__scsi_add_device(struct Scsi_Host *,
		uint, uint, u64, void *hostdata);
extern int scsi_add_device(struct Scsi_Host *host, uint channel,
			   uint target, u64 lun);
extern int scsi_register_device_handler(struct scsi_device_handler *scsi_dh);
extern void scsi_remove_device(struct scsi_device *);
extern int scsi_unregister_device_handler(struct scsi_device_handler *scsi_dh);
void scsi_attach_vpd(struct scsi_device *sdev);
void scsi_cdl_check(struct scsi_device *sdev);
int scsi_cdl_enable(struct scsi_device *sdev, bool enable);

extern struct scsi_device *scsi_device_from_queue(struct request_queue *q);
extern int __must_check scsi_device_get(struct scsi_device *);
extern void scsi_device_put(struct scsi_device *);
extern struct scsi_device *scsi_device_lookup(struct Scsi_Host *,
					      uint, uint, u64);
extern struct scsi_device *__scsi_device_lookup(struct Scsi_Host *,
						uint, uint, u64);
extern struct scsi_device *scsi_device_lookup_by_target(struct scsi_target *,
							u64);
extern struct scsi_device *__scsi_device_lookup_by_target(struct scsi_target *,
							  u64);
extern void starget_for_each_device(struct scsi_target *, void *,
		     void (*fn)(struct scsi_device *, void *));
extern void __starget_for_each_device(struct scsi_target *, void *,
				      void (*fn)(struct scsi_device *,
						 void *));


extern struct scsi_device *__scsi_iterate_devices(struct Scsi_Host *,
						  struct scsi_device *);


#define shost_for_each_device(sdev, shost) \
	for ((sdev) = __scsi_iterate_devices((shost), NULL); \
	     (sdev); \
	     (sdev) = __scsi_iterate_devices((shost), (sdev)))


#define __shost_for_each_device(sdev, shost) \
	list_for_each_entry((sdev), &((shost)->__devices), siblings)

extern int scsi_change_queue_depth(struct scsi_device *, int);
extern int scsi_track_queue_full(struct scsi_device *, int);

extern int scsi_set_medium_removal(struct scsi_device *, char);

int scsi_mode_sense(struct scsi_device *sdev, int dbd, int modepage,
		    int subpage, unsigned char *buffer, int len, int timeout,
		    int retries, struct scsi_mode_data *data,
		    struct scsi_sense_hdr *);
extern int scsi_mode_select(struct scsi_device *sdev, int pf, int sp,
			    unsigned char *buffer, int len, int timeout,
			    int retries, struct scsi_mode_data *data,
			    struct scsi_sense_hdr *);
extern int scsi_test_unit_ready(struct scsi_device *sdev, int timeout,
				int retries, struct scsi_sense_hdr *sshdr);
extern int scsi_get_vpd_page(struct scsi_device *, u8 page, unsigned char *buf,
			     int buf_len);
int scsi_report_opcode(struct scsi_device *sdev, unsigned char *buffer,
		       unsigned int len, unsigned char opcode,
		       unsigned short sa);
extern int scsi_device_set_state(struct scsi_device *sdev,
				 enum scsi_device_state state);
extern struct scsi_event *sdev_evt_alloc(enum scsi_device_event evt_type,
					  gfp_t gfpflags);
extern void sdev_evt_send(struct scsi_device *sdev, struct scsi_event *evt);
extern void sdev_evt_send_simple(struct scsi_device *sdev,
			  enum scsi_device_event evt_type, gfp_t gfpflags);
extern int scsi_device_quiesce(struct scsi_device *sdev);
extern void scsi_device_resume(struct scsi_device *sdev);
extern void scsi_target_quiesce(struct scsi_target *);
extern void scsi_target_resume(struct scsi_target *);
extern void scsi_scan_target(struct device *parent, unsigned int channel,
			     unsigned int id, u64 lun,
			     enum scsi_scan_mode rescan);
extern void scsi_target_reap(struct scsi_target *);
void scsi_block_targets(struct Scsi_Host *shost, struct device *dev);
extern void scsi_target_unblock(struct device *, enum scsi_device_state);
extern void scsi_remove_target(struct device *);
extern const char *scsi_device_state_name(enum scsi_device_state);
extern int scsi_is_sdev_device(const struct device *);
extern int scsi_is_target_device(const struct device *);
extern void scsi_sanitize_inquiry_string(unsigned char *s, int len);


struct scsi_exec_args {
	unsigned char *sense;		
	unsigned int sense_len;		
	struct scsi_sense_hdr *sshdr;	
	blk_mq_req_flags_t req_flags;	
	int scmd_flags;			
	int *resid;			
};

int scsi_execute_cmd(struct scsi_device *sdev, const unsigned char *cmd,
		     blk_opf_t opf, void *buffer, unsigned int bufflen,
		     int timeout, int retries,
		     const struct scsi_exec_args *args);

extern void sdev_disable_disk_events(struct scsi_device *sdev);
extern void sdev_enable_disk_events(struct scsi_device *sdev);
extern int scsi_vpd_lun_id(struct scsi_device *, char *, size_t);
extern int scsi_vpd_tpg_id(struct scsi_device *, int *);

#ifdef CONFIG_PM
extern int scsi_autopm_get_device(struct scsi_device *);
extern void scsi_autopm_put_device(struct scsi_device *);
#else
static inline int scsi_autopm_get_device(struct scsi_device *d) { return 0; }
static inline void scsi_autopm_put_device(struct scsi_device *d) {}
#endif 

static inline int __must_check scsi_device_reprobe(struct scsi_device *sdev)
{
	return device_reprobe(&sdev->sdev_gendev);
}

static inline unsigned int sdev_channel(struct scsi_device *sdev)
{
	return sdev->channel;
}

static inline unsigned int sdev_id(struct scsi_device *sdev)
{
	return sdev->id;
}

#define scmd_id(scmd) sdev_id((scmd)->device)
#define scmd_channel(scmd) sdev_channel((scmd)->device)


static inline int scsi_device_online(struct scsi_device *sdev)
{
	return (sdev->sdev_state != SDEV_OFFLINE &&
		sdev->sdev_state != SDEV_TRANSPORT_OFFLINE &&
		sdev->sdev_state != SDEV_DEL);
}
static inline int scsi_device_blocked(struct scsi_device *sdev)
{
	return sdev->sdev_state == SDEV_BLOCK ||
		sdev->sdev_state == SDEV_CREATED_BLOCK;
}
static inline int scsi_device_created(struct scsi_device *sdev)
{
	return sdev->sdev_state == SDEV_CREATED ||
		sdev->sdev_state == SDEV_CREATED_BLOCK;
}

int scsi_internal_device_block_nowait(struct scsi_device *sdev);
int scsi_internal_device_unblock_nowait(struct scsi_device *sdev,
					enum scsi_device_state new_state);


static inline int scsi_device_sync(struct scsi_device *sdev)
{
	return sdev->sdtr;
}
static inline int scsi_device_wide(struct scsi_device *sdev)
{
	return sdev->wdtr;
}
static inline int scsi_device_dt(struct scsi_device *sdev)
{
	return sdev->ppr;
}
static inline int scsi_device_dt_only(struct scsi_device *sdev)
{
	if (sdev->inquiry_len < 57)
		return 0;
	return (sdev->inquiry[56] & 0x0c) == 0x04;
}
static inline int scsi_device_ius(struct scsi_device *sdev)
{
	if (sdev->inquiry_len < 57)
		return 0;
	return sdev->inquiry[56] & 0x01;
}
static inline int scsi_device_qas(struct scsi_device *sdev)
{
	if (sdev->inquiry_len < 57)
		return 0;
	return sdev->inquiry[56] & 0x02;
}
static inline int scsi_device_enclosure(struct scsi_device *sdev)
{
	return sdev->inquiry ? (sdev->inquiry[6] & (1<<6)) : 1;
}

static inline int scsi_device_protection(struct scsi_device *sdev)
{
	if (sdev->no_dif)
		return 0;

	return sdev->scsi_level > SCSI_2 && sdev->inquiry[5] & (1<<0);
}

static inline int scsi_device_tpgs(struct scsi_device *sdev)
{
	return sdev->inquiry ? (sdev->inquiry[5] >> 4) & 0x3 : 0;
}


static inline int scsi_device_supports_vpd(struct scsi_device *sdev)
{
	
	if (sdev->try_vpd_pages)
		return 1;
	
	if (sdev->scsi_level >= SCSI_SPC_2 && !sdev->skip_vpd_pages)
		return 1;
	return 0;
}

static inline int scsi_device_busy(struct scsi_device *sdev)
{
	return sbitmap_weight(&sdev->budget_map);
}

#define MODULE_ALIAS_SCSI_DEVICE(type) \
	MODULE_ALIAS("scsi:t-" __stringify(type) "*")
#define SCSI_DEVICE_MODALIAS_FMT "scsi:t-0x%02x"

#endif 
