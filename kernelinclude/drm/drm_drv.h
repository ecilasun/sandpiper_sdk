

#ifndef _DRM_DRV_H_
#define _DRM_DRV_H_

#include <linux/list.h>
#include <linux/irqreturn.h>

#include <video/nomodeset.h>

#include <drm/drm_device.h>

struct drm_file;
struct drm_gem_object;
struct drm_master;
struct drm_minor;
struct dma_buf;
struct dma_buf_attachment;
struct drm_display_mode;
struct drm_mode_create_dumb;
struct drm_printer;
struct sg_table;


enum drm_driver_feature {
	
	DRIVER_GEM			= BIT(0),
	
	DRIVER_MODESET			= BIT(1),
	
	DRIVER_RENDER			= BIT(3),
	
	DRIVER_ATOMIC			= BIT(4),
	
	DRIVER_SYNCOBJ                  = BIT(5),
	
	DRIVER_SYNCOBJ_TIMELINE         = BIT(6),
	
	DRIVER_COMPUTE_ACCEL            = BIT(7),
	
	DRIVER_GEM_GPUVA		= BIT(8),
	
	DRIVER_CURSOR_HOTSPOT           = BIT(9),

	

	
	DRIVER_USE_AGP			= BIT(25),
	
	DRIVER_LEGACY			= BIT(26),
	
	DRIVER_PCI_DMA			= BIT(27),
	
	DRIVER_SG			= BIT(28),

	
	DRIVER_HAVE_DMA			= BIT(29),
	
	DRIVER_HAVE_IRQ			= BIT(30),
};


struct drm_driver {
	
	int (*load) (struct drm_device *, unsigned long flags);

	
	int (*open) (struct drm_device *, struct drm_file *);

	
	void (*postclose) (struct drm_device *, struct drm_file *);

	
	void (*lastclose) (struct drm_device *);

	
	void (*unload) (struct drm_device *);

	
	void (*release) (struct drm_device *);

	
	void (*master_set)(struct drm_device *dev, struct drm_file *file_priv,
			   bool from_open);
	
	void (*master_drop)(struct drm_device *dev, struct drm_file *file_priv);

	
	void (*debugfs_init)(struct drm_minor *minor);

	
	struct drm_gem_object *(*gem_create_object)(struct drm_device *dev,
						    size_t size);

	
	int (*prime_handle_to_fd)(struct drm_device *dev, struct drm_file *file_priv,
				uint32_t handle, uint32_t flags, int *prime_fd);
	
	int (*prime_fd_to_handle)(struct drm_device *dev, struct drm_file *file_priv,
				int prime_fd, uint32_t *handle);

	
	struct drm_gem_object * (*gem_prime_import)(struct drm_device *dev,
				struct dma_buf *dma_buf);
	
	struct drm_gem_object *(*gem_prime_import_sg_table)(
				struct drm_device *dev,
				struct dma_buf_attachment *attach,
				struct sg_table *sgt);

	
	int (*dumb_create)(struct drm_file *file_priv,
			   struct drm_device *dev,
			   struct drm_mode_create_dumb *args);
	
	int (*dumb_map_offset)(struct drm_file *file_priv,
			       struct drm_device *dev, uint32_t handle,
			       uint64_t *offset);

	
	void (*show_fdinfo)(struct drm_printer *p, struct drm_file *f);

	
	int major;
	
	int minor;
	
	int patchlevel;
	
	char *name;
	
	char *desc;
	
	char *date;

	
	u32 driver_features;

	

	const struct drm_ioctl_desc *ioctls;
	
	int num_ioctls;

	
	const struct file_operations *fops;

#ifdef CONFIG_DRM_LEGACY
	
	

	int (*firstopen) (struct drm_device *);
	void (*preclose) (struct drm_device *, struct drm_file *file_priv);
	int (*dma_ioctl) (struct drm_device *dev, void *data, struct drm_file *file_priv);
	int (*dma_quiescent) (struct drm_device *);
	int (*context_dtor) (struct drm_device *dev, int context);
	irqreturn_t (*irq_handler)(int irq, void *arg);
	void (*irq_preinstall)(struct drm_device *dev);
	int (*irq_postinstall)(struct drm_device *dev);
	void (*irq_uninstall)(struct drm_device *dev);
	u32 (*get_vblank_counter)(struct drm_device *dev, unsigned int pipe);
	int (*enable_vblank)(struct drm_device *dev, unsigned int pipe);
	void (*disable_vblank)(struct drm_device *dev, unsigned int pipe);
	int dev_priv_size;
#endif
};

void *__devm_drm_dev_alloc(struct device *parent,
			   const struct drm_driver *driver,
			   size_t size, size_t offset);


#define devm_drm_dev_alloc(parent, driver, type, member) \
	((type *) __devm_drm_dev_alloc(parent, driver, sizeof(type), \
				       offsetof(type, member)))

struct drm_device *drm_dev_alloc(const struct drm_driver *driver,
				 struct device *parent);
int drm_dev_register(struct drm_device *dev, unsigned long flags);
void drm_dev_unregister(struct drm_device *dev);

void drm_dev_get(struct drm_device *dev);
void drm_dev_put(struct drm_device *dev);
void drm_put_dev(struct drm_device *dev);
bool drm_dev_enter(struct drm_device *dev, int *idx);
void drm_dev_exit(int idx);
void drm_dev_unplug(struct drm_device *dev);


static inline bool drm_dev_is_unplugged(struct drm_device *dev)
{
	int idx;

	if (drm_dev_enter(dev, &idx)) {
		drm_dev_exit(idx);
		return false;
	}

	return true;
}


static inline bool drm_core_check_all_features(const struct drm_device *dev,
					       u32 features)
{
	u32 supported = dev->driver->driver_features & dev->driver_features;

	return features && (supported & features) == features;
}


static inline bool drm_core_check_feature(const struct drm_device *dev,
					  enum drm_driver_feature feature)
{
	return drm_core_check_all_features(dev, feature);
}


static inline bool drm_drv_uses_atomic_modeset(struct drm_device *dev)
{
	return drm_core_check_feature(dev, DRIVER_ATOMIC) ||
		(dev->mode_config.funcs && dev->mode_config.funcs->atomic_commit != NULL);
}



static inline bool drm_firmware_drivers_only(void)
{
	return video_firmware_drivers_only();
}

#endif
