

#ifndef _DRM_DEBUGFS_H_
#define _DRM_DEBUGFS_H_

#include <linux/types.h>
#include <linux/seq_file.h>

#include <drm/drm_gpuva_mgr.h>


#define DRM_DEBUGFS_GPUVA_INFO(show, data) {"gpuvas", show, DRIVER_GEM_GPUVA, data}


struct drm_info_list {
	
	const char *name;
	
	int (*show)(struct seq_file*, void*);
	
	u32 driver_features;
	
	void *data;
};


struct drm_info_node {
	
	struct drm_minor *minor;
	
	const struct drm_info_list *info_ent;
	
	struct list_head list;
	struct dentry *dent;
};


struct drm_debugfs_info {
	
	const char *name;

	
	int (*show)(struct seq_file*, void*);

	
	u32 driver_features;

	
	void *data;
};


struct drm_debugfs_entry {
	
	struct drm_device *dev;

	
	struct drm_debugfs_info file;

	
	struct list_head list;
};

#if defined(CONFIG_DEBUG_FS)
void drm_debugfs_create_files(const struct drm_info_list *files,
			      int count, struct dentry *root,
			      struct drm_minor *minor);
int drm_debugfs_remove_files(const struct drm_info_list *files,
			     int count, struct drm_minor *minor);

void drm_debugfs_add_file(struct drm_device *dev, const char *name,
			  int (*show)(struct seq_file*, void*), void *data);

void drm_debugfs_add_files(struct drm_device *dev,
			   const struct drm_debugfs_info *files, int count);

int drm_debugfs_gpuva_info(struct seq_file *m,
			   struct drm_gpuva_manager *mgr);
#else
static inline void drm_debugfs_create_files(const struct drm_info_list *files,
					    int count, struct dentry *root,
					    struct drm_minor *minor)
{}

static inline int drm_debugfs_remove_files(const struct drm_info_list *files,
					   int count, struct drm_minor *minor)
{
	return 0;
}

static inline void drm_debugfs_add_file(struct drm_device *dev, const char *name,
					int (*show)(struct seq_file*, void*),
					void *data)
{}

static inline void drm_debugfs_add_files(struct drm_device *dev,
					 const struct drm_debugfs_info *files,
					 int count)
{}

static inline int drm_debugfs_gpuva_info(struct seq_file *m,
					 struct drm_gpuva_manager *mgr)
{
	return 0;
}
#endif

#endif 
