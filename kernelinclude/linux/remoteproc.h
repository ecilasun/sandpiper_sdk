

#ifndef REMOTEPROC_H
#define REMOTEPROC_H

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/virtio.h>
#include <linux/cdev.h>
#include <linux/completion.h>
#include <linux/idr.h>
#include <linux/of.h>


struct resource_table {
	u32 ver;
	u32 num;
	u32 reserved[2];
	u32 offset[];
} __packed;


struct fw_rsc_hdr {
	u32 type;
	u8 data[];
} __packed;


enum fw_resource_type {
	RSC_CARVEOUT		= 0,
	RSC_DEVMEM		= 1,
	RSC_TRACE		= 2,
	RSC_VDEV		= 3,
	RSC_LAST		= 4,
	RSC_VENDOR_START	= 128,
	RSC_VENDOR_END		= 512,
};

#define FW_RSC_ADDR_ANY (-1)


struct fw_rsc_carveout {
	u32 da;
	u32 pa;
	u32 len;
	u32 flags;
	u32 reserved;
	u8 name[32];
} __packed;


struct fw_rsc_devmem {
	u32 da;
	u32 pa;
	u32 len;
	u32 flags;
	u32 reserved;
	u8 name[32];
} __packed;


struct fw_rsc_trace {
	u32 da;
	u32 len;
	u32 reserved;
	u8 name[32];
} __packed;


struct fw_rsc_vdev_vring {
	u32 da;
	u32 align;
	u32 num;
	u32 notifyid;
	u32 pa;
} __packed;


struct fw_rsc_vdev {
	u32 id;
	u32 notifyid;
	u32 dfeatures;
	u32 gfeatures;
	u32 config_len;
	u8 status;
	u8 num_of_vrings;
	u8 reserved[2];
	struct fw_rsc_vdev_vring vring[];
} __packed;

struct rproc;


struct rproc_mem_entry {
	void *va;
	bool is_iomem;
	dma_addr_t dma;
	size_t len;
	u32 da;
	void *priv;
	char name[32];
	struct list_head node;
	u32 rsc_offset;
	u32 flags;
	u32 of_resm_idx;
	int (*alloc)(struct rproc *rproc, struct rproc_mem_entry *mem);
	int (*release)(struct rproc *rproc, struct rproc_mem_entry *mem);
};

struct firmware;


enum rsc_handling_status {
	RSC_HANDLED	= 0,
	RSC_IGNORED	= 1,
};


struct rproc_ops {
	int (*prepare)(struct rproc *rproc);
	int (*unprepare)(struct rproc *rproc);
	int (*start)(struct rproc *rproc);
	int (*stop)(struct rproc *rproc);
	int (*attach)(struct rproc *rproc);
	int (*detach)(struct rproc *rproc);
	void (*kick)(struct rproc *rproc, int vqid);
	void * (*da_to_va)(struct rproc *rproc, u64 da, size_t len, bool *is_iomem);
	int (*parse_fw)(struct rproc *rproc, const struct firmware *fw);
	int (*handle_rsc)(struct rproc *rproc, u32 rsc_type, void *rsc,
			  int offset, int avail);
	struct resource_table *(*find_loaded_rsc_table)(
				struct rproc *rproc, const struct firmware *fw);
	struct resource_table *(*get_loaded_rsc_table)(
				struct rproc *rproc, size_t *size);
	int (*load)(struct rproc *rproc, const struct firmware *fw);
	int (*sanity_check)(struct rproc *rproc, const struct firmware *fw);
	u64 (*get_boot_addr)(struct rproc *rproc, const struct firmware *fw);
	unsigned long (*panic)(struct rproc *rproc);
	void (*coredump)(struct rproc *rproc);
};


enum rproc_state {
	RPROC_OFFLINE	= 0,
	RPROC_SUSPENDED	= 1,
	RPROC_RUNNING	= 2,
	RPROC_CRASHED	= 3,
	RPROC_DELETED	= 4,
	RPROC_ATTACHED	= 5,
	RPROC_DETACHED	= 6,
	RPROC_LAST	= 7,
};


enum rproc_crash_type {
	RPROC_MMUFAULT,
	RPROC_WATCHDOG,
	RPROC_FATAL_ERROR,
};


enum rproc_dump_mechanism {
	RPROC_COREDUMP_DISABLED,
	RPROC_COREDUMP_ENABLED,
	RPROC_COREDUMP_INLINE,
};


struct rproc_dump_segment {
	struct list_head node;

	dma_addr_t da;
	size_t size;

	void *priv;
	void (*dump)(struct rproc *rproc, struct rproc_dump_segment *segment,
		     void *dest, size_t offset, size_t size);
	loff_t offset;
};



enum rproc_features {
	RPROC_FEAT_ATTACH_ON_RECOVERY,
	RPROC_MAX_FEATURES,
};


struct rproc {
	struct list_head node;
	struct iommu_domain *domain;
	const char *name;
	const char *firmware;
	void *priv;
	struct rproc_ops *ops;
	struct device dev;
	atomic_t power;
	unsigned int state;
	enum rproc_dump_mechanism dump_conf;
	struct mutex lock;
	struct dentry *dbg_dir;
	struct list_head traces;
	int num_traces;
	struct list_head carveouts;
	struct list_head mappings;
	u64 bootaddr;
	struct list_head rvdevs;
	struct list_head subdevs;
	struct idr notifyids;
	int index;
	struct work_struct crash_handler;
	unsigned int crash_cnt;
	bool recovery_disabled;
	int max_notifyid;
	struct resource_table *table_ptr;
	struct resource_table *clean_table;
	struct resource_table *cached_table;
	size_t table_sz;
	bool has_iommu;
	bool auto_boot;
	bool sysfs_read_only;
	struct list_head dump_segments;
	int nb_vdev;
	u8 elf_class;
	u16 elf_machine;
	struct cdev cdev;
	bool cdev_put_on_release;
	DECLARE_BITMAP(features, RPROC_MAX_FEATURES);
};


struct rproc_subdev {
	struct list_head node;

	int (*prepare)(struct rproc_subdev *subdev);
	int (*start)(struct rproc_subdev *subdev);
	void (*stop)(struct rproc_subdev *subdev, bool crashed);
	void (*unprepare)(struct rproc_subdev *subdev);
};



#define RVDEV_NUM_VRINGS 2


struct rproc_vring {
	void *va;
	int num;
	u32 da;
	u32 align;
	int notifyid;
	struct rproc_vdev *rvdev;
	struct virtqueue *vq;
};


struct rproc_vdev {

	struct rproc_subdev subdev;
	struct platform_device *pdev;

	unsigned int id;
	struct list_head node;
	struct rproc *rproc;
	struct rproc_vring vring[RVDEV_NUM_VRINGS];
	u32 rsc_offset;
	u32 index;
};

struct rproc *rproc_get_by_phandle(phandle phandle);
struct rproc *rproc_get_by_child(struct device *dev);

struct rproc *rproc_alloc(struct device *dev, const char *name,
			  const struct rproc_ops *ops,
			  const char *firmware, int len);
void rproc_put(struct rproc *rproc);
int rproc_add(struct rproc *rproc);
int rproc_del(struct rproc *rproc);
void rproc_free(struct rproc *rproc);
void rproc_resource_cleanup(struct rproc *rproc);

struct rproc *devm_rproc_alloc(struct device *dev, const char *name,
			       const struct rproc_ops *ops,
			       const char *firmware, int len);
int devm_rproc_add(struct device *dev, struct rproc *rproc);

void rproc_add_carveout(struct rproc *rproc, struct rproc_mem_entry *mem);

struct rproc_mem_entry *
rproc_mem_entry_init(struct device *dev,
		     void *va, dma_addr_t dma, size_t len, u32 da,
		     int (*alloc)(struct rproc *, struct rproc_mem_entry *),
		     int (*release)(struct rproc *, struct rproc_mem_entry *),
		     const char *name, ...);

struct rproc_mem_entry *
rproc_of_resm_mem_entry_init(struct device *dev, u32 of_resm_idx, size_t len,
			     u32 da, const char *name, ...);

int rproc_boot(struct rproc *rproc);
int rproc_shutdown(struct rproc *rproc);
int rproc_detach(struct rproc *rproc);
int rproc_set_firmware(struct rproc *rproc, const char *fw_name);
void rproc_report_crash(struct rproc *rproc, enum rproc_crash_type type);
void *rproc_da_to_va(struct rproc *rproc, u64 da, size_t len, bool *is_iomem);


void rproc_coredump_cleanup(struct rproc *rproc);
void rproc_coredump(struct rproc *rproc);
void rproc_coredump_using_sections(struct rproc *rproc);
int rproc_coredump_add_segment(struct rproc *rproc, dma_addr_t da, size_t size);
int rproc_coredump_add_custom_segment(struct rproc *rproc,
				      dma_addr_t da, size_t size,
				      void (*dumpfn)(struct rproc *rproc,
						     struct rproc_dump_segment *segment,
						     void *dest, size_t offset,
						     size_t size),
				      void *priv);
int rproc_coredump_set_elf_info(struct rproc *rproc, u8 class, u16 machine);

void rproc_add_subdev(struct rproc *rproc, struct rproc_subdev *subdev);

void rproc_remove_subdev(struct rproc *rproc, struct rproc_subdev *subdev);

#endif 
