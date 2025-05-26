/* SPDX-License-Identifier: GPL-2.0 */


#ifndef THUNDERBOLT_H_
#define THUNDERBOLT_H_

#include <linux/device.h>
#include <linux/idr.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/pci.h>
#include <linux/uuid.h>
#include <linux/workqueue.h>

enum tb_cfg_pkg_type {
	TB_CFG_PKG_READ = 1,
	TB_CFG_PKG_WRITE = 2,
	TB_CFG_PKG_ERROR = 3,
	TB_CFG_PKG_NOTIFY_ACK = 4,
	TB_CFG_PKG_EVENT = 5,
	TB_CFG_PKG_XDOMAIN_REQ = 6,
	TB_CFG_PKG_XDOMAIN_RESP = 7,
	TB_CFG_PKG_OVERRIDE = 8,
	TB_CFG_PKG_RESET = 9,
	TB_CFG_PKG_ICM_EVENT = 10,
	TB_CFG_PKG_ICM_CMD = 11,
	TB_CFG_PKG_ICM_RESP = 12,
	TB_CFG_PKG_PREPARE_TO_SLEEP = 13,
};


enum tb_security_level {
	TB_SECURITY_NONE,
	TB_SECURITY_USER,
	TB_SECURITY_SECURE,
	TB_SECURITY_DPONLY,
	TB_SECURITY_USBONLY,
	TB_SECURITY_NOPCIE,
};


struct tb {
	struct device dev;
	struct mutex lock;
	struct tb_nhi *nhi;
	struct tb_ctl *ctl;
	struct workqueue_struct *wq;
	struct tb_switch *root_switch;
	const struct tb_cm_ops *cm_ops;
	int index;
	enum tb_security_level security_level;
	size_t nboot_acl;
	unsigned long privdata[];
};

extern struct bus_type tb_bus_type;
extern struct device_type tb_service_type;
extern struct device_type tb_xdomain_type;

#define TB_LINKS_PER_PHY_PORT	2

static inline unsigned int tb_phy_port_from_link(unsigned int link)
{
	return (link - 1) / TB_LINKS_PER_PHY_PORT;
}


struct tb_property_dir {
	const uuid_t *uuid;
	struct list_head properties;
};

enum tb_property_type {
	TB_PROPERTY_TYPE_UNKNOWN = 0x00,
	TB_PROPERTY_TYPE_DIRECTORY = 0x44,
	TB_PROPERTY_TYPE_DATA = 0x64,
	TB_PROPERTY_TYPE_TEXT = 0x74,
	TB_PROPERTY_TYPE_VALUE = 0x76,
};

#define TB_PROPERTY_KEY_SIZE	8


struct tb_property {
	struct list_head list;
	char key[TB_PROPERTY_KEY_SIZE + 1];
	enum tb_property_type type;
	size_t length;
	union {
		struct tb_property_dir *dir;
		u8 *data;
		char *text;
		u32 immediate;
	} value;
};

struct tb_property_dir *tb_property_parse_dir(const u32 *block,
					      size_t block_len);
ssize_t tb_property_format_dir(const struct tb_property_dir *dir, u32 *block,
			       size_t block_len);
struct tb_property_dir *tb_property_copy_dir(const struct tb_property_dir *dir);
struct tb_property_dir *tb_property_create_dir(const uuid_t *uuid);
void tb_property_free_dir(struct tb_property_dir *dir);
int tb_property_add_immediate(struct tb_property_dir *parent, const char *key,
			      u32 value);
int tb_property_add_data(struct tb_property_dir *parent, const char *key,
			 const void *buf, size_t buflen);
int tb_property_add_text(struct tb_property_dir *parent, const char *key,
			 const char *text);
int tb_property_add_dir(struct tb_property_dir *parent, const char *key,
			struct tb_property_dir *dir);
void tb_property_remove(struct tb_property *tb_property);
struct tb_property *tb_property_find(struct tb_property_dir *dir,
			const char *key, enum tb_property_type type);
struct tb_property *tb_property_get_next(struct tb_property_dir *dir,
					 struct tb_property *prev);

#define tb_property_for_each(dir, property)			\
	for (property = tb_property_get_next(dir, NULL);	\
	     property;						\
	     property = tb_property_get_next(dir, property))

int tb_register_property_dir(const char *key, struct tb_property_dir *dir);
void tb_unregister_property_dir(const char *key, struct tb_property_dir *dir);


enum tb_link_width {
	TB_LINK_WIDTH_SINGLE = BIT(0),
	TB_LINK_WIDTH_DUAL = BIT(1),
	TB_LINK_WIDTH_ASYM_TX = BIT(2),
	TB_LINK_WIDTH_ASYM_RX = BIT(3),
};


struct tb_xdomain {
	struct device dev;
	struct tb *tb;
	uuid_t *remote_uuid;
	const uuid_t *local_uuid;
	u64 route;
	u16 vendor;
	u16 device;
	unsigned int local_max_hopid;
	unsigned int remote_max_hopid;
	struct mutex lock;
	const char *vendor_name;
	const char *device_name;
	unsigned int link_speed;
	enum tb_link_width link_width;
	bool link_usb4;
	bool is_unplugged;
	bool needs_uuid;
	struct ida service_ids;
	struct ida in_hopids;
	struct ida out_hopids;
	u32 *local_property_block;
	u32 local_property_block_gen;
	u32 local_property_block_len;
	struct tb_property_dir *remote_properties;
	u32 remote_property_block_gen;
	int state;
	struct delayed_work state_work;
	int state_retries;
	struct delayed_work properties_changed_work;
	int properties_changed_retries;
	bool bonding_possible;
	u8 target_link_width;
	u8 link;
	u8 depth;
};

int tb_xdomain_lane_bonding_enable(struct tb_xdomain *xd);
void tb_xdomain_lane_bonding_disable(struct tb_xdomain *xd);
int tb_xdomain_alloc_in_hopid(struct tb_xdomain *xd, int hopid);
void tb_xdomain_release_in_hopid(struct tb_xdomain *xd, int hopid);
int tb_xdomain_alloc_out_hopid(struct tb_xdomain *xd, int hopid);
void tb_xdomain_release_out_hopid(struct tb_xdomain *xd, int hopid);
int tb_xdomain_enable_paths(struct tb_xdomain *xd, int transmit_path,
			    int transmit_ring, int receive_path,
			    int receive_ring);
int tb_xdomain_disable_paths(struct tb_xdomain *xd, int transmit_path,
			     int transmit_ring, int receive_path,
			     int receive_ring);

static inline int tb_xdomain_disable_all_paths(struct tb_xdomain *xd)
{
	return tb_xdomain_disable_paths(xd, -1, -1, -1, -1);
}

struct tb_xdomain *tb_xdomain_find_by_uuid(struct tb *tb, const uuid_t *uuid);
struct tb_xdomain *tb_xdomain_find_by_route(struct tb *tb, u64 route);

static inline struct tb_xdomain *
tb_xdomain_find_by_uuid_locked(struct tb *tb, const uuid_t *uuid)
{
	struct tb_xdomain *xd;

	mutex_lock(&tb->lock);
	xd = tb_xdomain_find_by_uuid(tb, uuid);
	mutex_unlock(&tb->lock);

	return xd;
}

static inline struct tb_xdomain *
tb_xdomain_find_by_route_locked(struct tb *tb, u64 route)
{
	struct tb_xdomain *xd;

	mutex_lock(&tb->lock);
	xd = tb_xdomain_find_by_route(tb, route);
	mutex_unlock(&tb->lock);

	return xd;
}

static inline struct tb_xdomain *tb_xdomain_get(struct tb_xdomain *xd)
{
	if (xd)
		get_device(&xd->dev);
	return xd;
}

static inline void tb_xdomain_put(struct tb_xdomain *xd)
{
	if (xd)
		put_device(&xd->dev);
}

static inline bool tb_is_xdomain(const struct device *dev)
{
	return dev->type == &tb_xdomain_type;
}

static inline struct tb_xdomain *tb_to_xdomain(struct device *dev)
{
	if (tb_is_xdomain(dev))
		return container_of(dev, struct tb_xdomain, dev);
	return NULL;
}

int tb_xdomain_response(struct tb_xdomain *xd, const void *response,
			size_t size, enum tb_cfg_pkg_type type);
int tb_xdomain_request(struct tb_xdomain *xd, const void *request,
		       size_t request_size, enum tb_cfg_pkg_type request_type,
		       void *response, size_t response_size,
		       enum tb_cfg_pkg_type response_type,
		       unsigned int timeout_msec);


struct tb_protocol_handler {
	const uuid_t *uuid;
	int (*callback)(const void *buf, size_t size, void *data);
	void *data;
	struct list_head list;
};

int tb_register_protocol_handler(struct tb_protocol_handler *handler);
void tb_unregister_protocol_handler(struct tb_protocol_handler *handler);


struct tb_service {
	struct device dev;
	int id;
	const char *key;
	u32 prtcid;
	u32 prtcvers;
	u32 prtcrevs;
	u32 prtcstns;
	struct dentry *debugfs_dir;
};

static inline struct tb_service *tb_service_get(struct tb_service *svc)
{
	if (svc)
		get_device(&svc->dev);
	return svc;
}

static inline void tb_service_put(struct tb_service *svc)
{
	if (svc)
		put_device(&svc->dev);
}

static inline bool tb_is_service(const struct device *dev)
{
	return dev->type == &tb_service_type;
}

static inline struct tb_service *tb_to_service(struct device *dev)
{
	if (tb_is_service(dev))
		return container_of(dev, struct tb_service, dev);
	return NULL;
}


struct tb_service_driver {
	struct device_driver driver;
	int (*probe)(struct tb_service *svc, const struct tb_service_id *id);
	void (*remove)(struct tb_service *svc);
	void (*shutdown)(struct tb_service *svc);
	const struct tb_service_id *id_table;
};

#define TB_SERVICE(key, id)				\
	.match_flags = TBSVC_MATCH_PROTOCOL_KEY |	\
		       TBSVC_MATCH_PROTOCOL_ID,		\
	.protocol_key = (key),				\
	.protocol_id = (id)

int tb_register_service_driver(struct tb_service_driver *drv);
void tb_unregister_service_driver(struct tb_service_driver *drv);

static inline void *tb_service_get_drvdata(const struct tb_service *svc)
{
	return dev_get_drvdata(&svc->dev);
}

static inline void tb_service_set_drvdata(struct tb_service *svc, void *data)
{
	dev_set_drvdata(&svc->dev, data);
}

static inline struct tb_xdomain *tb_service_parent(struct tb_service *svc)
{
	return tb_to_xdomain(svc->dev.parent);
}


struct tb_nhi {
	spinlock_t lock;
	struct pci_dev *pdev;
	const struct tb_nhi_ops *ops;
	void __iomem *iobase;
	struct tb_ring **tx_rings;
	struct tb_ring **rx_rings;
	struct ida msix_ida;
	bool going_away;
	bool iommu_dma_protection;
	struct work_struct interrupt_work;
	u32 hop_count;
	unsigned long quirks;
};


struct tb_ring {
	spinlock_t lock;
	struct tb_nhi *nhi;
	int size;
	int hop;
	int head;
	int tail;
	struct ring_desc *descriptors;
	dma_addr_t descriptors_dma;
	struct list_head queue;
	struct list_head in_flight;
	struct work_struct work;
	bool is_tx:1;
	bool running:1;
	int irq;
	u8 vector;
	unsigned int flags;
	int e2e_tx_hop;
	u16 sof_mask;
	u16 eof_mask;
	void (*start_poll)(void *data);
	void *poll_data;
};


#define RING_FLAG_NO_SUSPEND	BIT(0)

#define RING_FLAG_FRAME		BIT(1)

#define RING_FLAG_E2E		BIT(2)

struct ring_frame;
typedef void (*ring_cb)(struct tb_ring *, struct ring_frame *, bool canceled);


enum ring_desc_flags {
	RING_DESC_ISOCH = 0x1,
	RING_DESC_CRC_ERROR = 0x1,
	RING_DESC_COMPLETED = 0x2,
	RING_DESC_POSTED = 0x4,
	RING_DESC_BUFFER_OVERRUN = 0x04,
	RING_DESC_INTERRUPT = 0x8,
};


struct ring_frame {
	dma_addr_t buffer_phy;
	ring_cb callback;
	struct list_head list;
	u32 size:12;
	u32 flags:12;
	u32 eof:4;
	u32 sof:4;
};


#define TB_FRAME_SIZE		0x100

struct tb_ring *tb_ring_alloc_tx(struct tb_nhi *nhi, int hop, int size,
				 unsigned int flags);
struct tb_ring *tb_ring_alloc_rx(struct tb_nhi *nhi, int hop, int size,
				 unsigned int flags, int e2e_tx_hop,
				 u16 sof_mask, u16 eof_mask,
				 void (*start_poll)(void *), void *poll_data);
void tb_ring_start(struct tb_ring *ring);
void tb_ring_stop(struct tb_ring *ring);
void tb_ring_free(struct tb_ring *ring);

int __tb_ring_enqueue(struct tb_ring *ring, struct ring_frame *frame);


static inline int tb_ring_rx(struct tb_ring *ring, struct ring_frame *frame)
{
	WARN_ON(ring->is_tx);
	return __tb_ring_enqueue(ring, frame);
}


static inline int tb_ring_tx(struct tb_ring *ring, struct ring_frame *frame)
{
	WARN_ON(!ring->is_tx);
	return __tb_ring_enqueue(ring, frame);
}


struct ring_frame *tb_ring_poll(struct tb_ring *ring);
void tb_ring_poll_complete(struct tb_ring *ring);


static inline struct device *tb_ring_dma_device(struct tb_ring *ring)
{
	return &ring->nhi->pdev->dev;
}

#endif 
