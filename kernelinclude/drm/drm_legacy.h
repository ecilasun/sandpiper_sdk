#ifndef __DRM_DRM_LEGACY_H__
#define __DRM_DRM_LEGACY_H__


#include <linux/agp_backend.h>

#include <drm/drm.h>
#include <drm/drm_auth.h>

struct drm_device;
struct drm_driver;
struct file;
struct pci_driver;





struct drm_hash_item {
	struct hlist_node head;
	unsigned long key;
};

struct drm_open_hash {
	struct hlist_head *table;
	u8 order;
};


struct drm_buf {
	int idx;		       
	int total;		       
	int order;		       
	int used;		       
	unsigned long offset;	       
	void *address;		       
	unsigned long bus_address;     
	struct drm_buf *next;	       
	__volatile__ int waiting;      
	__volatile__ int pending;      
	struct drm_file *file_priv;    
	int context;		       
	int while_locked;	       
	enum {
		DRM_LIST_NONE = 0,
		DRM_LIST_FREE = 1,
		DRM_LIST_WAIT = 2,
		DRM_LIST_PEND = 3,
		DRM_LIST_PRIO = 4,
		DRM_LIST_RECLAIM = 5
	} list;			       

	int dev_priv_size;		 
	void *dev_private;		 
};

typedef struct drm_dma_handle {
	dma_addr_t busaddr;
	void *vaddr;
	size_t size;
} drm_dma_handle_t;


struct drm_buf_entry {
	int buf_size;			
	int buf_count;			
	struct drm_buf *buflist;		
	int seg_count;
	int page_order;
	struct drm_dma_handle **seglist;

	int low_mark;			
	int high_mark;			
};


struct drm_device_dma {

	struct drm_buf_entry bufs[DRM_MAX_ORDER + 1];	
	int buf_count;			
	struct drm_buf **buflist;		
	int seg_count;
	int page_count;			
	unsigned long *pagelist;	
	unsigned long byte_count;
	enum {
		_DRM_DMA_USE_AGP = 0x01,
		_DRM_DMA_USE_SG = 0x02,
		_DRM_DMA_USE_FB = 0x04,
		_DRM_DMA_USE_PCI_RO = 0x08
	} flags;

};


struct drm_sg_mem {
	unsigned long handle;
	void *virtual;
	int pages;
	struct page **pagelist;
	dma_addr_t *busaddr;
};


struct drm_local_map {
	dma_addr_t offset;	 
	unsigned long size;	 
	enum drm_map_type type;	 
	enum drm_map_flags flags;	 
	void *handle;		 
				 
	int mtrr;		 
};

typedef struct drm_local_map drm_local_map_t;


struct drm_map_list {
	struct list_head head;		
	struct drm_hash_item hash;
	struct drm_local_map *map;	
	uint64_t user_token;
	struct drm_master *master;
};

int drm_legacy_addmap(struct drm_device *d, resource_size_t offset,
		      unsigned int size, enum drm_map_type type,
		      enum drm_map_flags flags, struct drm_local_map **map_p);
struct drm_local_map *drm_legacy_findmap(struct drm_device *dev, unsigned int token);
void drm_legacy_rmmap(struct drm_device *d, struct drm_local_map *map);
int drm_legacy_rmmap_locked(struct drm_device *d, struct drm_local_map *map);
struct drm_local_map *drm_legacy_getsarea(struct drm_device *dev);
int drm_legacy_mmap(struct file *filp, struct vm_area_struct *vma);

int drm_legacy_addbufs_agp(struct drm_device *d, struct drm_buf_desc *req);
int drm_legacy_addbufs_pci(struct drm_device *d, struct drm_buf_desc *req);


#define LOCK_TEST_WITH_RETURN( dev, _file_priv )				\
do {										\
	if (!_DRM_LOCK_IS_HELD(_file_priv->master->lock.hw_lock->lock) ||	\
	    _file_priv->master->lock.file_priv != _file_priv)	{		\
		DRM_ERROR( "%s called without lock held, held  %d owner %p %p\n",\
			   __func__, _DRM_LOCK_IS_HELD(_file_priv->master->lock.hw_lock->lock),\
			   _file_priv->master->lock.file_priv, _file_priv);	\
		return -EINVAL;							\
	}									\
} while (0)

void drm_legacy_idlelock_take(struct drm_lock_data *lock);
void drm_legacy_idlelock_release(struct drm_lock_data *lock);


int drm_legacy_irq_uninstall(struct drm_device *dev);



#ifdef CONFIG_PCI

int drm_legacy_pci_init(const struct drm_driver *driver,
			struct pci_driver *pdriver);
void drm_legacy_pci_exit(const struct drm_driver *driver,
			 struct pci_driver *pdriver);

#else

static inline struct drm_dma_handle *drm_pci_alloc(struct drm_device *dev,
						   size_t size, size_t align)
{
	return NULL;
}

static inline void drm_pci_free(struct drm_device *dev,
				struct drm_dma_handle *dmah)
{
}

static inline int drm_legacy_pci_init(const struct drm_driver *driver,
				      struct pci_driver *pdriver)
{
	return -EINVAL;
}

static inline void drm_legacy_pci_exit(const struct drm_driver *driver,
				       struct pci_driver *pdriver)
{
}

#endif



struct drm_agp_head {
	struct agp_kern_info agp_info;
	struct list_head memory;
	unsigned long mode;
	struct agp_bridge_data *bridge;
	int enabled;
	int acquired;
	unsigned long base;
	int agp_mtrr;
	int cant_use_aperture;
	unsigned long page_mask;
};

#if IS_ENABLED(CONFIG_DRM_LEGACY) && IS_ENABLED(CONFIG_AGP)
struct drm_agp_head *drm_legacy_agp_init(struct drm_device *dev);
int drm_legacy_agp_acquire(struct drm_device *dev);
int drm_legacy_agp_release(struct drm_device *dev);
int drm_legacy_agp_enable(struct drm_device *dev, struct drm_agp_mode mode);
int drm_legacy_agp_info(struct drm_device *dev, struct drm_agp_info *info);
int drm_legacy_agp_alloc(struct drm_device *dev, struct drm_agp_buffer *request);
int drm_legacy_agp_free(struct drm_device *dev, struct drm_agp_buffer *request);
int drm_legacy_agp_unbind(struct drm_device *dev, struct drm_agp_binding *request);
int drm_legacy_agp_bind(struct drm_device *dev, struct drm_agp_binding *request);
#else
static inline struct drm_agp_head *drm_legacy_agp_init(struct drm_device *dev)
{
	return NULL;
}

static inline int drm_legacy_agp_acquire(struct drm_device *dev)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_release(struct drm_device *dev)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_enable(struct drm_device *dev,
					struct drm_agp_mode mode)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_info(struct drm_device *dev,
				      struct drm_agp_info *info)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_alloc(struct drm_device *dev,
				       struct drm_agp_buffer *request)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_free(struct drm_device *dev,
				      struct drm_agp_buffer *request)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_unbind(struct drm_device *dev,
					struct drm_agp_binding *request)
{
	return -ENODEV;
}

static inline int drm_legacy_agp_bind(struct drm_device *dev,
				      struct drm_agp_binding *request)
{
	return -ENODEV;
}
#endif


void drm_legacy_ioremap(struct drm_local_map *map, struct drm_device *dev);
void drm_legacy_ioremap_wc(struct drm_local_map *map, struct drm_device *dev);
void drm_legacy_ioremapfree(struct drm_local_map *map, struct drm_device *dev);

#endif 
