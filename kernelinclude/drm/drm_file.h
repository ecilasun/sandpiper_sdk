

#ifndef _DRM_FILE_H_
#define _DRM_FILE_H_

#include <linux/types.h>
#include <linux/completion.h>
#include <linux/idr.h>

#include <uapi/drm/drm.h>

#include <drm/drm_prime.h>

struct dma_fence;
struct drm_file;
struct drm_device;
struct drm_printer;
struct device;
struct file;




enum drm_minor_type {
	DRM_MINOR_PRIMARY = 0,
	DRM_MINOR_CONTROL = 1,
	DRM_MINOR_RENDER = 2,
	DRM_MINOR_ACCEL = 32,
};


struct drm_minor {
	
	int index;			
	int type;                       
	struct device *kdev;		
	struct drm_device *dev;

	struct dentry *debugfs_root;

	struct list_head debugfs_list;
	struct mutex debugfs_lock; 
};


struct drm_pending_event {
	
	struct completion *completion;

	
	void (*completion_release)(struct completion *completion);

	
	struct drm_event *event;

	
	struct dma_fence *fence;

	
	struct drm_file *file_priv;

	
	struct list_head link;

	
	struct list_head pending_link;
};


struct drm_file {
	
	bool authenticated;

	
	bool stereo_allowed;

	
	bool universal_planes;

	
	bool atomic;

	
	bool aspect_ratio_allowed;

	
	bool writeback_connectors;

	
	bool was_master;

	
	bool is_master;

	
	bool supports_virtualized_cursor_plane;

	
	struct drm_master *master;

	
	spinlock_t master_lookup_lock;

	
	struct pid __rcu *pid;

	
	u64 client_id;

	
	drm_magic_t magic;

	
	struct list_head lhead;

	
	struct drm_minor *minor;

	
	struct idr object_idr;

	
	spinlock_t table_lock;

	
	struct idr syncobj_idr;
	
	spinlock_t syncobj_table_lock;

	
	struct file *filp;

	
	void *driver_priv;

	
	struct list_head fbs;

	
	struct mutex fbs_lock;

	
	struct list_head blobs;

	
	wait_queue_head_t event_wait;

	
	struct list_head pending_event_list;

	
	struct list_head event_list;

	
	int event_space;

	
	struct mutex event_read_lock;

	
	struct drm_prime_file_private prime;

	
#if IS_ENABLED(CONFIG_DRM_LEGACY)
	unsigned long lock_count; 
#endif
};


static inline bool drm_is_primary_client(const struct drm_file *file_priv)
{
	return file_priv->minor->type == DRM_MINOR_PRIMARY;
}


static inline bool drm_is_render_client(const struct drm_file *file_priv)
{
	return file_priv->minor->type == DRM_MINOR_RENDER;
}


static inline bool drm_is_accel_client(const struct drm_file *file_priv)
{
	return file_priv->minor->type == DRM_MINOR_ACCEL;
}

void drm_file_update_pid(struct drm_file *);

int drm_open(struct inode *inode, struct file *filp);
int drm_open_helper(struct file *filp, struct drm_minor *minor);
ssize_t drm_read(struct file *filp, char __user *buffer,
		 size_t count, loff_t *offset);
int drm_release(struct inode *inode, struct file *filp);
int drm_release_noglobal(struct inode *inode, struct file *filp);
__poll_t drm_poll(struct file *filp, struct poll_table_struct *wait);
int drm_event_reserve_init_locked(struct drm_device *dev,
				  struct drm_file *file_priv,
				  struct drm_pending_event *p,
				  struct drm_event *e);
int drm_event_reserve_init(struct drm_device *dev,
			   struct drm_file *file_priv,
			   struct drm_pending_event *p,
			   struct drm_event *e);
void drm_event_cancel_free(struct drm_device *dev,
			   struct drm_pending_event *p);
void drm_send_event_locked(struct drm_device *dev, struct drm_pending_event *e);
void drm_send_event(struct drm_device *dev, struct drm_pending_event *e);
void drm_send_event_timestamp_locked(struct drm_device *dev,
				     struct drm_pending_event *e,
				     ktime_t timestamp);


struct drm_memory_stats {
	u64 shared;
	u64 private;
	u64 resident;
	u64 purgeable;
	u64 active;
};

enum drm_gem_object_status;

void drm_print_memory_stats(struct drm_printer *p,
			    const struct drm_memory_stats *stats,
			    enum drm_gem_object_status supported_status,
			    const char *region);

void drm_show_memory_stats(struct drm_printer *p, struct drm_file *file);
void drm_show_fdinfo(struct seq_file *m, struct file *f);

struct file *mock_drm_getfile(struct drm_minor *minor, unsigned int flags);

#endif 
