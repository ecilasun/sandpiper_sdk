/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FPGA_MGR_H
#define _LINUX_FPGA_MGR_H

#include <linux/mutex.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>

#define ENCRYPTED_KEY_LEN	64 

struct fpga_manager;
struct sg_table;


enum fpga_mgr_states {
	
	FPGA_MGR_STATE_UNKNOWN,
	FPGA_MGR_STATE_POWER_OFF,
	FPGA_MGR_STATE_POWER_UP,
	FPGA_MGR_STATE_RESET,

	
	FPGA_MGR_STATE_FIRMWARE_REQ,
	FPGA_MGR_STATE_FIRMWARE_REQ_ERR,

	
	FPGA_MGR_STATE_PARSE_HEADER,
	FPGA_MGR_STATE_PARSE_HEADER_ERR,
	FPGA_MGR_STATE_WRITE_INIT,
	FPGA_MGR_STATE_WRITE_INIT_ERR,
	FPGA_MGR_STATE_WRITE,
	FPGA_MGR_STATE_WRITE_ERR,
	FPGA_MGR_STATE_WRITE_COMPLETE,
	FPGA_MGR_STATE_WRITE_COMPLETE_ERR,

	
	FPGA_MGR_STATE_OPERATING,
};


#define FPGA_MGR_PARTIAL_RECONFIG	BIT(0)
#define FPGA_MGR_EXTERNAL_CONFIG	BIT(1)
#define FPGA_MGR_ENCRYPTED_BITSTREAM	BIT(2)
#define FPGA_MGR_BITSTREAM_LSB_FIRST	BIT(3)
#define FPGA_MGR_COMPRESSED_BITSTREAM	BIT(4)
#define FPGA_MGR_USERKEY_ENCRYPTED_BITSTREAM	BIT(5)
#define FPGA_MGR_DDR_MEM_AUTH_BITSTREAM		BIT(6)
#define FPGA_MGR_SECURE_MEM_AUTH_BITSTREAM	BIT(7)
#define FPGA_MGR_CONFIG_DMA_BUF		BIT(5)


struct fpga_image_info {
	u32 flags;
	u32 enable_timeout_us;
	u32 disable_timeout_us;
	u32 config_complete_timeout_us;
	char *firmware_name;
	char key[ENCRYPTED_KEY_LEN];
	struct sg_table *sgt;
	const char *buf;
	size_t count;
	size_t header_size;
	size_t data_size;
	int region_id;
	struct device *dev;
#ifdef CONFIG_OF
	struct device_node *overlay;
#endif
};


struct fpga_compat_id {
	u64 id_h;
	u64 id_l;
};


struct fpga_manager_info {
	const char *name;
	struct fpga_compat_id *compat_id;
	const struct fpga_manager_ops *mops;
	void *priv;
};


struct fpga_manager_ops {
	size_t initial_header_size;
	bool skip_header;
	enum fpga_mgr_states (*state)(struct fpga_manager *mgr);
	u64 (*status)(struct fpga_manager *mgr);
	int (*parse_header)(struct fpga_manager *mgr,
			    struct fpga_image_info *info,
			    const char *buf, size_t count);
	int (*write_init)(struct fpga_manager *mgr,
			  struct fpga_image_info *info,
			  const char *buf, size_t count);
	int (*write)(struct fpga_manager *mgr, const char *buf, size_t count);
	int (*write_sg)(struct fpga_manager *mgr, struct sg_table *sgt);
	int (*write_complete)(struct fpga_manager *mgr,
			      struct fpga_image_info *info);
	int (*read)(struct fpga_manager *mgr, struct seq_file *s);
	void (*fpga_remove)(struct fpga_manager *mgr);
	const struct attribute_group **groups;
};


#define FPGA_MGR_STATUS_OPERATION_ERR		BIT(0)
#define FPGA_MGR_STATUS_CRC_ERR			BIT(1)
#define FPGA_MGR_STATUS_INCOMPATIBLE_IMAGE_ERR	BIT(2)
#define FPGA_MGR_STATUS_IP_PROTOCOL_ERR		BIT(3)
#define FPGA_MGR_STATUS_FIFO_OVERFLOW_ERR	BIT(4)


struct fpga_manager {
	const char *name;
	unsigned long flags;
	char key[ENCRYPTED_KEY_LEN + 1];
	struct device dev;
	struct dma_buf *dmabuf;
	struct mutex ref_mutex;
	struct miscdevice miscdev;
	enum fpga_mgr_states state;
	struct fpga_compat_id *compat_id;
	const struct fpga_manager_ops *mops;
	struct module *mops_owner;
	void *priv;
	int err;
#ifdef CONFIG_FPGA_MGR_DEBUG_FS
	struct dentry *dir;
#endif
};

#define to_fpga_manager(d) container_of(d, struct fpga_manager, dev)

struct fpga_image_info *fpga_image_info_alloc(struct device *dev);

void fpga_image_info_free(struct fpga_image_info *info);

int fpga_mgr_load(struct fpga_manager *mgr, struct fpga_image_info *info);

int fpga_mgr_lock(struct fpga_manager *mgr);
void fpga_mgr_unlock(struct fpga_manager *mgr);

struct fpga_manager *of_fpga_mgr_get(struct device_node *node);

struct fpga_manager *fpga_mgr_get(struct device *dev);

void fpga_mgr_put(struct fpga_manager *mgr);

#define fpga_mgr_register_full(parent, info) \
	__fpga_mgr_register_full(parent, info, THIS_MODULE)
struct fpga_manager *
__fpga_mgr_register_full(struct device *parent, const struct fpga_manager_info *info,
			 struct module *owner);

#define fpga_mgr_register(parent, name, mops, priv) \
	__fpga_mgr_register(parent, name, mops, priv, THIS_MODULE)
struct fpga_manager *
__fpga_mgr_register(struct device *parent, const char *name,
		    const struct fpga_manager_ops *mops, void *priv, struct module *owner);

void fpga_mgr_unregister(struct fpga_manager *mgr);

#define devm_fpga_mgr_register_full(parent, info) \
	__devm_fpga_mgr_register_full(parent, info, THIS_MODULE)
struct fpga_manager *
__devm_fpga_mgr_register_full(struct device *parent, const struct fpga_manager_info *info,
			      struct module *owner);
#define devm_fpga_mgr_register(parent, name, mops, priv) \
	__devm_fpga_mgr_register(parent, name, mops, priv, THIS_MODULE)
struct fpga_manager *
__devm_fpga_mgr_register(struct device *parent, const char *name,
			 const struct fpga_manager_ops *mops, void *priv,
			 struct module *owner);

#define FPGA_IOCTL_LOAD_DMA_BUFF	_IOWR('R', 1, __u32)

#endif 
