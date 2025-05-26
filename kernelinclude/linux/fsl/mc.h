/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _FSL_MC_H_
#define _FSL_MC_H_

#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/interrupt.h>
#include <uapi/linux/fsl_mc.h>

#define FSL_MC_VENDOR_FREESCALE	0x1957

struct irq_domain;
struct msi_domain_info;

struct fsl_mc_device;
struct fsl_mc_io;


struct fsl_mc_driver {
	struct device_driver driver;
	const struct fsl_mc_device_id *match_id_table;
	int (*probe)(struct fsl_mc_device *dev);
	void (*remove)(struct fsl_mc_device *dev);
	void (*shutdown)(struct fsl_mc_device *dev);
	int (*suspend)(struct fsl_mc_device *dev, pm_message_t state);
	int (*resume)(struct fsl_mc_device *dev);
	bool driver_managed_dma;
};

#define to_fsl_mc_driver(_drv) \
	container_of(_drv, struct fsl_mc_driver, driver)


enum fsl_mc_pool_type {
	FSL_MC_POOL_DPMCP = 0x0,    
	FSL_MC_POOL_DPBP,	    
	FSL_MC_POOL_DPCON,	    
	FSL_MC_POOL_IRQ,

	
	FSL_MC_NUM_POOL_TYPES
};


struct fsl_mc_resource {
	enum fsl_mc_pool_type type;
	s32 id;
	void *data;
	struct fsl_mc_resource_pool *parent_pool;
	struct list_head node;
};


struct fsl_mc_device_irq {
	unsigned int virq;
	struct fsl_mc_device *mc_dev;
	u8 dev_irq_index;
	struct fsl_mc_resource resource;
};

#define to_fsl_mc_irq(_mc_resource) \
	container_of(_mc_resource, struct fsl_mc_device_irq, resource)


#define FSL_MC_OBJ_STATE_OPEN		0x00000001

#define FSL_MC_OBJ_STATE_PLUGGED	0x00000002


#define FSL_MC_OBJ_FLAG_NO_MEM_SHAREABILITY	0x0001


struct fsl_mc_obj_desc {
	char type[16];
	int id;
	u16 vendor;
	u16 ver_major;
	u16 ver_minor;
	u8 irq_count;
	u8 region_count;
	u32 state;
	char label[16];
	u16 flags;
};


#define FSL_MC_IS_DPRC	0x0001



#define FSL_MC_REGION_CACHEABLE	0x00000001


#define FSL_MC_REGION_SHAREABLE	0x00000002


struct fsl_mc_device {
	struct device dev;
	u64 dma_mask;
	u16 flags;
	u32 icid;
	u16 mc_handle;
	struct fsl_mc_io *mc_io;
	struct fsl_mc_obj_desc obj_desc;
	struct resource *regions;
	struct fsl_mc_device_irq **irqs;
	struct fsl_mc_resource *resource;
	struct device_link *consumer_link;
	const char *driver_override;
};

#define to_fsl_mc_device(_dev) \
	container_of(_dev, struct fsl_mc_device, dev)

struct mc_cmd_header {
	u8 src_id;
	u8 flags_hw;
	u8 status;
	u8 flags_sw;
	__le16 token;
	__le16 cmd_id;
};

enum mc_cmd_status {
	MC_CMD_STATUS_OK = 0x0, 
	MC_CMD_STATUS_READY = 0x1, 
	MC_CMD_STATUS_AUTH_ERR = 0x3, 
	MC_CMD_STATUS_NO_PRIVILEGE = 0x4, 
	MC_CMD_STATUS_DMA_ERR = 0x5, 
	MC_CMD_STATUS_CONFIG_ERR = 0x6, 
	MC_CMD_STATUS_TIMEOUT = 0x7, 
	MC_CMD_STATUS_NO_RESOURCE = 0x8, 
	MC_CMD_STATUS_NO_MEMORY = 0x9, 
	MC_CMD_STATUS_BUSY = 0xA, 
	MC_CMD_STATUS_UNSUPPORTED_OP = 0xB, 
	MC_CMD_STATUS_INVALID_STATE = 0xC 
};




#define MC_CMD_FLAG_PRI		0x80

#define MC_CMD_FLAG_INTR_DIS	0x01

static inline __le64 mc_encode_cmd_header(u16 cmd_id,
					  u32 cmd_flags,
					  u16 token)
{
	__le64 header = 0;
	struct mc_cmd_header *hdr = (struct mc_cmd_header *)&header;

	hdr->cmd_id = cpu_to_le16(cmd_id);
	hdr->token  = cpu_to_le16(token);
	hdr->status = MC_CMD_STATUS_READY;
	if (cmd_flags & MC_CMD_FLAG_PRI)
		hdr->flags_hw = MC_CMD_FLAG_PRI;
	if (cmd_flags & MC_CMD_FLAG_INTR_DIS)
		hdr->flags_sw = MC_CMD_FLAG_INTR_DIS;

	return header;
}

static inline u16 mc_cmd_hdr_read_token(struct fsl_mc_command *cmd)
{
	struct mc_cmd_header *hdr = (struct mc_cmd_header *)&cmd->header;
	u16 token = le16_to_cpu(hdr->token);

	return token;
}

struct mc_rsp_create {
	__le32 object_id;
};

struct mc_rsp_api_ver {
	__le16 major_ver;
	__le16 minor_ver;
};

static inline u32 mc_cmd_read_object_id(struct fsl_mc_command *cmd)
{
	struct mc_rsp_create *rsp_params;

	rsp_params = (struct mc_rsp_create *)cmd->params;
	return le32_to_cpu(rsp_params->object_id);
}

static inline void mc_cmd_read_api_version(struct fsl_mc_command *cmd,
					   u16 *major_ver,
					   u16 *minor_ver)
{
	struct mc_rsp_api_ver *rsp_params;

	rsp_params = (struct mc_rsp_api_ver *)cmd->params;
	*major_ver = le16_to_cpu(rsp_params->major_ver);
	*minor_ver = le16_to_cpu(rsp_params->minor_ver);
}


#define FSL_MC_IO_ATOMIC_CONTEXT_PORTAL	0x0001


struct fsl_mc_io {
	struct device *dev;
	u16 flags;
	u32 portal_size;
	phys_addr_t portal_phys_addr;
	void __iomem *portal_virt_addr;
	struct fsl_mc_device *dpmcp_dev;
	union {
		
		struct mutex mutex; 

		
		raw_spinlock_t spinlock; 
	};
};

int mc_send_command(struct fsl_mc_io *mc_io, struct fsl_mc_command *cmd);

#ifdef CONFIG_FSL_MC_BUS
#define dev_is_fsl_mc(_dev) ((_dev)->bus == &fsl_mc_bus_type)
#else

#define dev_is_fsl_mc(_dev) (0)
#endif


#define fsl_mc_is_cont_dev(_dev) (to_fsl_mc_device(_dev)->flags & \
	FSL_MC_IS_DPRC)


#define fsl_mc_cont_dev(_dev) (fsl_mc_is_cont_dev(_dev) ? \
	(_dev) : (_dev)->parent)


#define module_fsl_mc_driver(__fsl_mc_driver) \
	module_driver(__fsl_mc_driver, fsl_mc_driver_register, \
		      fsl_mc_driver_unregister)


#define fsl_mc_driver_register(drv) \
	__fsl_mc_driver_register(drv, THIS_MODULE)

int __must_check __fsl_mc_driver_register(struct fsl_mc_driver *fsl_mc_driver,
					  struct module *owner);

void fsl_mc_driver_unregister(struct fsl_mc_driver *driver);


struct fsl_mc_version {
	u32 major;
	u32 minor;
	u32 revision;
};

struct fsl_mc_version *fsl_mc_get_version(void);

int __must_check fsl_mc_portal_allocate(struct fsl_mc_device *mc_dev,
					u16 mc_io_flags,
					struct fsl_mc_io **new_mc_io);

void fsl_mc_portal_free(struct fsl_mc_io *mc_io);

int fsl_mc_portal_reset(struct fsl_mc_io *mc_io);

int __must_check fsl_mc_object_allocate(struct fsl_mc_device *mc_dev,
					enum fsl_mc_pool_type pool_type,
					struct fsl_mc_device **new_mc_adev);

void fsl_mc_object_free(struct fsl_mc_device *mc_adev);

struct irq_domain *fsl_mc_msi_create_irq_domain(struct fwnode_handle *fwnode,
						struct msi_domain_info *info,
						struct irq_domain *parent);

int __must_check fsl_mc_allocate_irqs(struct fsl_mc_device *mc_dev);

void fsl_mc_free_irqs(struct fsl_mc_device *mc_dev);

struct fsl_mc_device *fsl_mc_get_endpoint(struct fsl_mc_device *mc_dev,
					  u16 if_id);

extern struct bus_type fsl_mc_bus_type;

extern struct device_type fsl_mc_bus_dprc_type;
extern struct device_type fsl_mc_bus_dpni_type;
extern struct device_type fsl_mc_bus_dpio_type;
extern struct device_type fsl_mc_bus_dpsw_type;
extern struct device_type fsl_mc_bus_dpbp_type;
extern struct device_type fsl_mc_bus_dpcon_type;
extern struct device_type fsl_mc_bus_dpmcp_type;
extern struct device_type fsl_mc_bus_dpmac_type;
extern struct device_type fsl_mc_bus_dprtc_type;
extern struct device_type fsl_mc_bus_dpseci_type;
extern struct device_type fsl_mc_bus_dpdmux_type;
extern struct device_type fsl_mc_bus_dpdcei_type;
extern struct device_type fsl_mc_bus_dpaiop_type;
extern struct device_type fsl_mc_bus_dpci_type;
extern struct device_type fsl_mc_bus_dpdmai_type;

static inline bool is_fsl_mc_bus_dprc(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dprc_type;
}

static inline bool is_fsl_mc_bus_dpni(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpni_type;
}

static inline bool is_fsl_mc_bus_dpio(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpio_type;
}

static inline bool is_fsl_mc_bus_dpsw(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpsw_type;
}

static inline bool is_fsl_mc_bus_dpdmux(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpdmux_type;
}

static inline bool is_fsl_mc_bus_dpbp(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpbp_type;
}

static inline bool is_fsl_mc_bus_dpcon(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpcon_type;
}

static inline bool is_fsl_mc_bus_dpmcp(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpmcp_type;
}

static inline bool is_fsl_mc_bus_dpmac(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpmac_type;
}

static inline bool is_fsl_mc_bus_dprtc(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dprtc_type;
}

static inline bool is_fsl_mc_bus_dpseci(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpseci_type;
}

static inline bool is_fsl_mc_bus_dpdcei(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpdcei_type;
}

static inline bool is_fsl_mc_bus_dpaiop(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpaiop_type;
}

static inline bool is_fsl_mc_bus_dpci(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpci_type;
}

static inline bool is_fsl_mc_bus_dpdmai(const struct fsl_mc_device *mc_dev)
{
	return mc_dev->dev.type == &fsl_mc_bus_dpdmai_type;
}

#define DPRC_RESET_OPTION_NON_RECURSIVE                0x00000001
int dprc_reset_container(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 token,
			 int child_container_id,
			 u32 options);

int dprc_scan_container(struct fsl_mc_device *mc_bus_dev,
			bool alloc_interrupts);

void dprc_remove_devices(struct fsl_mc_device *mc_bus_dev,
			 struct fsl_mc_obj_desc *obj_desc_array,
			 int num_child_objects_in_mc);

int dprc_cleanup(struct fsl_mc_device *mc_dev);

int dprc_setup(struct fsl_mc_device *mc_dev);


#define FSL_MC_IRQ_POOL_MAX_TOTAL_IRQS	256

int fsl_mc_populate_irq_pool(struct fsl_mc_device *mc_bus_dev,
			     unsigned int irq_count);

void fsl_mc_cleanup_irq_pool(struct fsl_mc_device *mc_bus_dev);



int dpbp_open(struct fsl_mc_io *mc_io,
	      u32 cmd_flags,
	      int dpbp_id,
	      u16 *token);

int dpbp_close(struct fsl_mc_io *mc_io,
	       u32 cmd_flags,
	       u16 token);

int dpbp_enable(struct fsl_mc_io *mc_io,
		u32 cmd_flags,
		u16 token);

int dpbp_disable(struct fsl_mc_io *mc_io,
		 u32 cmd_flags,
		 u16 token);

int dpbp_reset(struct fsl_mc_io *mc_io,
	       u32 cmd_flags,
	       u16 token);


struct dpbp_attr {
	int id;
	u16 bpid;
};

int dpbp_get_attributes(struct fsl_mc_io *mc_io,
			u32 cmd_flags,
			u16 token,
			struct dpbp_attr *attr);




#define DPCON_INVALID_DPIO_ID		(int)(-1)

int dpcon_open(struct fsl_mc_io *mc_io,
	       u32 cmd_flags,
	       int dpcon_id,
	       u16 *token);

int dpcon_close(struct fsl_mc_io *mc_io,
		u32 cmd_flags,
		u16 token);

int dpcon_enable(struct fsl_mc_io *mc_io,
		 u32 cmd_flags,
		 u16 token);

int dpcon_disable(struct fsl_mc_io *mc_io,
		  u32 cmd_flags,
		  u16 token);

int dpcon_reset(struct fsl_mc_io *mc_io,
		u32 cmd_flags,
		u16 token);

int fsl_mc_obj_open(struct fsl_mc_io *mc_io,
		    u32 cmd_flags,
		    int obj_id,
		    char *obj_type,
		    u16 *token);

int fsl_mc_obj_close(struct fsl_mc_io *mc_io,
		     u32 cmd_flags,
		     u16 token);

int fsl_mc_obj_reset(struct fsl_mc_io *mc_io,
		     u32 cmd_flags,
		     u16 token);


struct dpcon_attr {
	int id;
	u16 qbman_ch_id;
	u8 num_priorities;
};

int dpcon_get_attributes(struct fsl_mc_io *mc_io,
			 u32 cmd_flags,
			 u16 token,
			 struct dpcon_attr *attr);


struct dpcon_notification_cfg {
	int dpio_id;
	u8 priority;
	u64 user_ctx;
};

int dpcon_set_notification(struct fsl_mc_io *mc_io,
			   u32 cmd_flags,
			   u16 token,
			   struct dpcon_notification_cfg *cfg);

#endif 
