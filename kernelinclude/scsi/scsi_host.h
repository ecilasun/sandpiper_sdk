/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _SCSI_SCSI_HOST_H
#define _SCSI_SCSI_HOST_H

#include <linux/device.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/mutex.h>
#include <linux/seq_file.h>
#include <linux/blk-mq.h>
#include <scsi/scsi.h>

struct block_device;
struct completion;
struct module;
struct scsi_cmnd;
struct scsi_device;
struct scsi_target;
struct Scsi_Host;
struct scsi_transport_template;


#define SG_ALL	SG_CHUNK_SIZE

#define MODE_UNKNOWN 0x00
#define MODE_INITIATOR 0x01
#define MODE_TARGET 0x02


enum scsi_timeout_action {
	SCSI_EH_DONE,
	SCSI_EH_RESET_TIMER,
	SCSI_EH_NOT_HANDLED,
};

struct scsi_host_template {
	

	
	unsigned int cmd_size;

	
	int (* queuecommand)(struct Scsi_Host *, struct scsi_cmnd *);

	
	void (*commit_rqs)(struct Scsi_Host *, u16);

	struct module *module;
	const char *name;

	
	const char *(*info)(struct Scsi_Host *);

	
	int (*ioctl)(struct scsi_device *dev, unsigned int cmd,
		     void __user *arg);


#ifdef CONFIG_COMPAT
	
	int (*compat_ioctl)(struct scsi_device *dev, unsigned int cmd,
			    void __user *arg);
#endif

	int (*init_cmd_priv)(struct Scsi_Host *shost, struct scsi_cmnd *cmd);
	int (*exit_cmd_priv)(struct Scsi_Host *shost, struct scsi_cmnd *cmd);

	
	int (* eh_abort_handler)(struct scsi_cmnd *);
	int (* eh_device_reset_handler)(struct scsi_cmnd *);
	int (* eh_target_reset_handler)(struct scsi_cmnd *);
	int (* eh_bus_reset_handler)(struct scsi_cmnd *);
	int (* eh_host_reset_handler)(struct scsi_cmnd *);

	
	int (* slave_alloc)(struct scsi_device *);

	
	int (* slave_configure)(struct scsi_device *);

	
	void (* slave_destroy)(struct scsi_device *);

	
	int (* target_alloc)(struct scsi_target *);

	
	void (* target_destroy)(struct scsi_target *);

	
	int (* scan_finished)(struct Scsi_Host *, unsigned long);

	
	void (* scan_start)(struct Scsi_Host *);

	
	int (* change_queue_depth)(struct scsi_device *, int);

	
	void (* map_queues)(struct Scsi_Host *shost);

	
	int (* mq_poll)(struct Scsi_Host *shost, unsigned int queue_num);

	
	bool (* dma_need_drain)(struct request *rq);

	
	int (* bios_param)(struct scsi_device *, struct block_device *,
			sector_t, int []);

	
	void (*unlock_native_capacity)(struct scsi_device *);

	
	int (*show_info)(struct seq_file *, struct Scsi_Host *);
	int (*write_info)(struct Scsi_Host *, char *, int);

	
	enum scsi_timeout_action (*eh_timed_out)(struct scsi_cmnd *);
	
	bool (*eh_should_retry_cmd)(struct scsi_cmnd *scmd);

	

	int (*host_reset)(struct Scsi_Host *shost, int reset_type);
#define SCSI_ADAPTER_RESET	1
#define SCSI_FIRMWARE_RESET	2


	
	const char *proc_name;

	
	int can_queue;

	
	int this_id;

	
	unsigned short sg_tablesize;
	unsigned short sg_prot_tablesize;

	
	unsigned int max_sectors;

	
	unsigned int max_segment_size;

	
	unsigned long dma_boundary;

	unsigned long virt_boundary_mask;

	
#define SCSI_DEFAULT_MAX_SECTORS	1024

	
	short cmd_per_lun;

	
	int tag_alloc_policy;

	
	unsigned track_queue_depth:1;

	
	unsigned supported_mode:2;

	
	unsigned emulated:1;

	
	unsigned skip_settle_delay:1;

	
	unsigned no_write_same:1;

	
	unsigned host_tagset:1;

	
	unsigned queuecommand_may_block:1;

	
	unsigned int max_host_blocked;

	
#define SCSI_DEFAULT_HOST_BLOCKED	7

	
	const struct attribute_group **shost_groups;

	
	const struct attribute_group **sdev_groups;

	
	u64 vendor_id;

	
	int rpm_autosuspend_delay;
};


#define DEF_SCSI_QCMD(func_name) \
	int func_name(struct Scsi_Host *shost, struct scsi_cmnd *cmd)	\
	{								\
		unsigned long irq_flags;				\
		int rc;							\
		spin_lock_irqsave(shost->host_lock, irq_flags);		\
		rc = func_name##_lck(cmd);				\
		spin_unlock_irqrestore(shost->host_lock, irq_flags);	\
		return rc;						\
	}



enum scsi_host_state {
	SHOST_CREATED = 1,
	SHOST_RUNNING,
	SHOST_CANCEL,
	SHOST_DEL,
	SHOST_RECOVERY,
	SHOST_CANCEL_RECOVERY,
	SHOST_DEL_RECOVERY,
};

struct Scsi_Host {
	
	struct list_head	__devices;
	struct list_head	__targets;
	
	struct list_head	starved_list;

	spinlock_t		default_lock;
	spinlock_t		*host_lock;

	struct mutex		scan_mutex;

	struct list_head	eh_abort_list;
	struct list_head	eh_cmd_q;
	struct task_struct    * ehandler;  
	struct completion     * eh_action; 
	wait_queue_head_t       host_wait;
	const struct scsi_host_template *hostt;
	struct scsi_transport_template *transportt;

	struct kref		tagset_refcnt;
	struct completion	tagset_freed;
	
	struct blk_mq_tag_set	tag_set;

	atomic_t host_blocked;

	unsigned int host_failed;	   
	unsigned int host_eh_scheduled;    
    
	unsigned int host_no;  

	
	int eh_deadline;
	unsigned long last_reset;


	
	unsigned int max_channel;
	unsigned int max_id;
	u64 max_lun;

	
	unsigned int unique_id;

	
	unsigned short max_cmd_len;

	int this_id;
	int can_queue;
	short cmd_per_lun;
	short unsigned int sg_tablesize;
	short unsigned int sg_prot_tablesize;
	unsigned int max_sectors;
	unsigned int opt_sectors;
	unsigned int max_segment_size;
	unsigned long dma_boundary;
	unsigned long virt_boundary_mask;
	
	unsigned nr_hw_queues;
	unsigned nr_maps;
	unsigned active_mode:2;

	
	unsigned host_self_blocked:1;
    
	
	unsigned reverse_ordering:1;

	
	unsigned tmf_in_progress:1;

	
	unsigned async_scan:1;

	
	unsigned eh_noresume:1;

	
	unsigned no_write_same:1;

	
	unsigned host_tagset:1;

	
	unsigned queuecommand_may_block:1;

	
	unsigned short_inquiry:1;

	
	unsigned no_scsi2_lun_in_cdb:1;

	
	char work_q_name[20];
	struct workqueue_struct *work_q;

	
	struct workqueue_struct *tmf_work_q;

	
	unsigned int max_host_blocked;

	
	unsigned int prot_capabilities;
	unsigned char prot_guard_type;

	
	unsigned long base;
	unsigned long io_port;
	unsigned char n_io_port;
	unsigned char dma_channel;
	unsigned int  irq;
	

	enum scsi_host_state shost_state;

	
	struct device		shost_gendev, shost_dev;

	
	void *shost_data;

	
	struct device *dma_dev;

	
	unsigned long hostdata[]  
		__attribute__ ((aligned (sizeof(unsigned long))));
};

#define		class_to_shost(d)	\
	container_of(d, struct Scsi_Host, shost_dev)

#define shost_printk(prefix, shost, fmt, a...)	\
	dev_printk(prefix, &(shost)->shost_gendev, fmt, ##a)

static inline void *shost_priv(struct Scsi_Host *shost)
{
	return (void *)shost->hostdata;
}

int scsi_is_host_device(const struct device *);

static inline struct Scsi_Host *dev_to_shost(struct device *dev)
{
	while (!scsi_is_host_device(dev)) {
		if (!dev->parent)
			return NULL;
		dev = dev->parent;
	}
	return container_of(dev, struct Scsi_Host, shost_gendev);
}

static inline int scsi_host_in_recovery(struct Scsi_Host *shost)
{
	return shost->shost_state == SHOST_RECOVERY ||
		shost->shost_state == SHOST_CANCEL_RECOVERY ||
		shost->shost_state == SHOST_DEL_RECOVERY ||
		shost->tmf_in_progress;
}

extern int scsi_queue_work(struct Scsi_Host *, struct work_struct *);
extern void scsi_flush_work(struct Scsi_Host *);

extern struct Scsi_Host *scsi_host_alloc(const struct scsi_host_template *, int);
extern int __must_check scsi_add_host_with_dma(struct Scsi_Host *,
					       struct device *,
					       struct device *);
#if defined(CONFIG_SCSI_PROC_FS)
struct proc_dir_entry *
scsi_template_proc_dir(const struct scsi_host_template *sht);
#else
#define scsi_template_proc_dir(sht) NULL
#endif
extern void scsi_scan_host(struct Scsi_Host *);
extern int scsi_resume_device(struct scsi_device *sdev);
extern int scsi_rescan_device(struct scsi_device *sdev);
extern void scsi_remove_host(struct Scsi_Host *);
extern struct Scsi_Host *scsi_host_get(struct Scsi_Host *);
extern int scsi_host_busy(struct Scsi_Host *shost);
extern void scsi_host_put(struct Scsi_Host *t);
extern struct Scsi_Host *scsi_host_lookup(unsigned int hostnum);
extern const char *scsi_host_state_name(enum scsi_host_state);
extern void scsi_host_complete_all_commands(struct Scsi_Host *shost,
					    enum scsi_host_status status);

static inline int __must_check scsi_add_host(struct Scsi_Host *host,
					     struct device *dev)
{
	return scsi_add_host_with_dma(host, dev, dev);
}

static inline struct device *scsi_get_device(struct Scsi_Host *shost)
{
        return shost->shost_gendev.parent;
}


static inline int scsi_host_scan_allowed(struct Scsi_Host *shost)
{
	return shost->shost_state == SHOST_RUNNING ||
	       shost->shost_state == SHOST_RECOVERY;
}

extern void scsi_unblock_requests(struct Scsi_Host *);
extern void scsi_block_requests(struct Scsi_Host *);
extern int scsi_host_block(struct Scsi_Host *shost);
extern int scsi_host_unblock(struct Scsi_Host *shost, int new_state);

void scsi_host_busy_iter(struct Scsi_Host *,
			 bool (*fn)(struct scsi_cmnd *, void *), void *priv);

struct class_container;


enum scsi_host_prot_capabilities {
	SHOST_DIF_TYPE1_PROTECTION = 1 << 0, 
	SHOST_DIF_TYPE2_PROTECTION = 1 << 1, 
	SHOST_DIF_TYPE3_PROTECTION = 1 << 2, 

	SHOST_DIX_TYPE0_PROTECTION = 1 << 3, 
	SHOST_DIX_TYPE1_PROTECTION = 1 << 4, 
	SHOST_DIX_TYPE2_PROTECTION = 1 << 5, 
	SHOST_DIX_TYPE3_PROTECTION = 1 << 6, 
};


static inline void scsi_host_set_prot(struct Scsi_Host *shost, unsigned int mask)
{
	shost->prot_capabilities = mask;
}

static inline unsigned int scsi_host_get_prot(struct Scsi_Host *shost)
{
	return shost->prot_capabilities;
}

static inline int scsi_host_prot_dma(struct Scsi_Host *shost)
{
	return shost->prot_capabilities >= SHOST_DIX_TYPE0_PROTECTION;
}

static inline unsigned int scsi_host_dif_capable(struct Scsi_Host *shost, unsigned int target_type)
{
	static unsigned char cap[] = { 0,
				       SHOST_DIF_TYPE1_PROTECTION,
				       SHOST_DIF_TYPE2_PROTECTION,
				       SHOST_DIF_TYPE3_PROTECTION };

	if (target_type >= ARRAY_SIZE(cap))
		return 0;

	return shost->prot_capabilities & cap[target_type] ? target_type : 0;
}

static inline unsigned int scsi_host_dix_capable(struct Scsi_Host *shost, unsigned int target_type)
{
#if defined(CONFIG_BLK_DEV_INTEGRITY)
	static unsigned char cap[] = { SHOST_DIX_TYPE0_PROTECTION,
				       SHOST_DIX_TYPE1_PROTECTION,
				       SHOST_DIX_TYPE2_PROTECTION,
				       SHOST_DIX_TYPE3_PROTECTION };

	if (target_type >= ARRAY_SIZE(cap))
		return 0;

	return shost->prot_capabilities & cap[target_type];
#endif
	return 0;
}



enum scsi_host_guard_type {
	SHOST_DIX_GUARD_CRC = 1 << 0,
	SHOST_DIX_GUARD_IP  = 1 << 1,
};

static inline void scsi_host_set_guard(struct Scsi_Host *shost, unsigned char type)
{
	shost->prot_guard_type = type;
}

static inline unsigned char scsi_host_get_guard(struct Scsi_Host *shost)
{
	return shost->prot_guard_type;
}

extern int scsi_host_set_state(struct Scsi_Host *, enum scsi_host_state);

#endif 
