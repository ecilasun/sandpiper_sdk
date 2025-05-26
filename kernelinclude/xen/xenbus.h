

#ifndef _XEN_XENBUS_H
#define _XEN_XENBUS_H

#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/mutex.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/completion.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <xen/interface/xen.h>
#include <xen/interface/grant_table.h>
#include <xen/interface/io/xenbus.h>
#include <xen/interface/io/xs_wire.h>
#include <xen/interface/event_channel.h>

#define XENBUS_MAX_RING_GRANT_ORDER 4
#define XENBUS_MAX_RING_GRANTS      (1U << XENBUS_MAX_RING_GRANT_ORDER)


struct xenbus_watch
{
	struct list_head list;

	
	const char *node;

	unsigned int nr_pending;

	
	bool (*will_handle)(struct xenbus_watch *,
			      const char *path, const char *token);

	
	void (*callback)(struct xenbus_watch *,
			 const char *path, const char *token);
};



struct xenbus_device {
	const char *devicetype;
	const char *nodename;
	const char *otherend;
	int otherend_id;
	struct xenbus_watch otherend_watch;
	struct device dev;
	enum xenbus_state state;
	struct completion down;
	struct work_struct work;
	struct semaphore reclaim_sem;

	
	atomic_t event_channels;
	atomic_t events;
	atomic_t spurious_events;
	atomic_t jiffies_eoi_delayed;
	unsigned int spurious_threshold;
};

#define to_xenbus_device(__dev)	container_of_const(__dev, struct xenbus_device, dev)

struct xenbus_device_id
{
	
	char devicetype[32]; 	
};


struct xenbus_driver {
	const char *name;       
	const struct xenbus_device_id *ids;
	bool allow_rebind; 
	bool not_essential;     
	int (*probe)(struct xenbus_device *dev,
		     const struct xenbus_device_id *id);
	void (*otherend_changed)(struct xenbus_device *dev,
				 enum xenbus_state backend_state);
	void (*remove)(struct xenbus_device *dev);
	int (*suspend)(struct xenbus_device *dev);
	int (*resume)(struct xenbus_device *dev);
	int (*uevent)(const struct xenbus_device *, struct kobj_uevent_env *);
	struct device_driver driver;
	int (*read_otherend_details)(struct xenbus_device *dev);
	int (*is_ready)(struct xenbus_device *dev);
	void (*reclaim_memory)(struct xenbus_device *dev);
};

static inline struct xenbus_driver *to_xenbus_driver(struct device_driver *drv)
{
	return container_of(drv, struct xenbus_driver, driver);
}

int __must_check __xenbus_register_frontend(struct xenbus_driver *drv,
					    struct module *owner,
					    const char *mod_name);
int __must_check __xenbus_register_backend(struct xenbus_driver *drv,
					   struct module *owner,
					   const char *mod_name);

#define xenbus_register_frontend(drv) \
	__xenbus_register_frontend(drv, THIS_MODULE, KBUILD_MODNAME)
#define xenbus_register_backend(drv) \
	__xenbus_register_backend(drv, THIS_MODULE, KBUILD_MODNAME)

void xenbus_unregister_driver(struct xenbus_driver *drv);

struct xenbus_transaction
{
	u32 id;
};


#define XBT_NIL ((struct xenbus_transaction) { 0 })

char **xenbus_directory(struct xenbus_transaction t,
			const char *dir, const char *node, unsigned int *num);
void *xenbus_read(struct xenbus_transaction t,
		  const char *dir, const char *node, unsigned int *len);
int xenbus_write(struct xenbus_transaction t,
		 const char *dir, const char *node, const char *string);
int xenbus_mkdir(struct xenbus_transaction t,
		 const char *dir, const char *node);
int xenbus_exists(struct xenbus_transaction t,
		  const char *dir, const char *node);
int xenbus_rm(struct xenbus_transaction t, const char *dir, const char *node);
int xenbus_transaction_start(struct xenbus_transaction *t);
int xenbus_transaction_end(struct xenbus_transaction t, int abort);


__scanf(4, 5)
int xenbus_scanf(struct xenbus_transaction t,
		 const char *dir, const char *node, const char *fmt, ...);


unsigned int xenbus_read_unsigned(const char *dir, const char *node,
				  unsigned int default_val);


__printf(4, 5)
int xenbus_printf(struct xenbus_transaction t,
		  const char *dir, const char *node, const char *fmt, ...);


int xenbus_gather(struct xenbus_transaction t, const char *dir, ...);


extern int xenstored_ready;
int register_xenstore_notifier(struct notifier_block *nb);
void unregister_xenstore_notifier(struct notifier_block *nb);

int register_xenbus_watch(struct xenbus_watch *watch);
void unregister_xenbus_watch(struct xenbus_watch *watch);
void xs_suspend(void);
void xs_resume(void);
void xs_suspend_cancel(void);

struct work_struct;

#define XENBUS_IS_ERR_READ(str) ({			\
	if (!IS_ERR(str) && strlen(str) == 0) {		\
		kfree(str);				\
		str = ERR_PTR(-ERANGE);			\
	}						\
	IS_ERR(str);					\
})

#define XENBUS_EXIST_ERR(err) ((err) == -ENOENT || (err) == -ERANGE)

int xenbus_watch_path(struct xenbus_device *dev, const char *path,
		      struct xenbus_watch *watch,
		      bool (*will_handle)(struct xenbus_watch *,
					  const char *, const char *),
		      void (*callback)(struct xenbus_watch *,
				       const char *, const char *));
__printf(5, 6)
int xenbus_watch_pathfmt(struct xenbus_device *dev, struct xenbus_watch *watch,
			 bool (*will_handle)(struct xenbus_watch *,
					     const char *, const char *),
			 void (*callback)(struct xenbus_watch *,
					  const char *, const char *),
			 const char *pathfmt, ...);

int xenbus_switch_state(struct xenbus_device *dev, enum xenbus_state new_state);
int xenbus_setup_ring(struct xenbus_device *dev, gfp_t gfp, void **vaddr,
		      unsigned int nr_pages, grant_ref_t *grefs);
void xenbus_teardown_ring(void **vaddr, unsigned int nr_pages,
			  grant_ref_t *grefs);
int xenbus_map_ring_valloc(struct xenbus_device *dev, grant_ref_t *gnt_refs,
			   unsigned int nr_grefs, void **vaddr);

int xenbus_unmap_ring_vfree(struct xenbus_device *dev, void *vaddr);

int xenbus_alloc_evtchn(struct xenbus_device *dev, evtchn_port_t *port);
int xenbus_free_evtchn(struct xenbus_device *dev, evtchn_port_t port);

enum xenbus_state xenbus_read_driver_state(const char *path);

__printf(3, 4)
void xenbus_dev_error(struct xenbus_device *dev, int err, const char *fmt, ...);
__printf(3, 4)
void xenbus_dev_fatal(struct xenbus_device *dev, int err, const char *fmt, ...);

const char *xenbus_strstate(enum xenbus_state state);
int xenbus_dev_is_online(struct xenbus_device *dev);
int xenbus_frontend_closed(struct xenbus_device *dev);

extern const struct file_operations xen_xenbus_fops;
extern struct xenstore_domain_interface *xen_store_interface;
extern int xen_store_evtchn;

#endif 
