/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __MOST_CORE_H__
#define __MOST_CORE_H__

#include <linux/types.h>
#include <linux/device.h>

struct module;
struct interface_private;


enum most_interface_type {
	ITYPE_LOOPBACK = 1,
	ITYPE_I2C,
	ITYPE_I2S,
	ITYPE_TSI,
	ITYPE_HBI,
	ITYPE_MEDIALB_DIM,
	ITYPE_MEDIALB_DIM2,
	ITYPE_USB,
	ITYPE_PCIE
};


enum most_channel_direction {
	MOST_CH_RX = 1 << 0,
	MOST_CH_TX = 1 << 1,
};


enum most_channel_data_type {
	MOST_CH_CONTROL = 1 << 0,
	MOST_CH_ASYNC = 1 << 1,
	MOST_CH_ISOC = 1 << 2,
	MOST_CH_SYNC = 1 << 5,
};

enum most_status_flags {
	
	MBO_SUCCESS = 0,
	
	MBO_E_INVAL,
	
	MBO_E_CLOSE,
};


struct most_channel_capability {
	u16 direction;
	u16 data_type;
	u16 num_buffers_packet;
	u16 buffer_size_packet;
	u16 num_buffers_streaming;
	u16 buffer_size_streaming;
	const char *name_suffix;
};


struct most_channel_config {
	enum most_channel_direction direction;
	enum most_channel_data_type data_type;
	u16 num_buffers;
	u16 buffer_size;
	u16 extra_len;
	u16 subbuffer_size;
	u16 packets_per_xact;
	u16 dbr_size;
};


struct mbo {
	void *context;
	void *priv;
	struct list_head list;
	struct most_interface *ifp;
	int *num_buffers_ptr;
	u16 hdm_channel_id;
	void *virt_address;
	dma_addr_t bus_address;
	u16 buffer_length;
	u16 processed_length;
	enum most_status_flags status;
	void (*complete)(struct mbo *mbo);
};


struct most_interface {
	struct device *dev;
	struct device *driver_dev;
	struct module *mod;
	enum most_interface_type interface;
	const char *description;
	unsigned int num_channels;
	struct most_channel_capability *channel_vector;
	void *(*dma_alloc)(struct mbo *mbo, u32 size);
	void (*dma_free)(struct mbo *mbo, u32 size);
	int (*configure)(struct most_interface *iface, int channel_idx,
			 struct most_channel_config *channel_config);
	int (*enqueue)(struct most_interface *iface, int channel_idx,
		       struct mbo *mbo);
	int (*poison_channel)(struct most_interface *iface, int channel_idx);
	void (*request_netinfo)(struct most_interface *iface, int channel_idx,
				void (*on_netinfo)(struct most_interface *iface,
						   unsigned char link_stat,
						   unsigned char *mac_addr));
	void *priv;
	struct interface_private *p;
};


struct most_component {
	struct list_head list;
	const char *name;
	struct module *mod;
	int (*probe_channel)(struct most_interface *iface, int channel_idx,
			     struct most_channel_config *cfg, char *name,
			     char *param);
	int (*disconnect_channel)(struct most_interface *iface,
				  int channel_idx);
	int (*rx_completion)(struct mbo *mbo);
	int (*tx_completion)(struct most_interface *iface, int channel_idx);
	int (*cfg_complete)(void);
};


int most_register_interface(struct most_interface *iface);


void most_deregister_interface(struct most_interface *iface);
void most_submit_mbo(struct mbo *mbo);


void most_stop_enqueue(struct most_interface *iface, int channel_idx);


void most_resume_enqueue(struct most_interface *iface, int channel_idx);
int most_register_component(struct most_component *comp);
int most_deregister_component(struct most_component *comp);
struct mbo *most_get_mbo(struct most_interface *iface, int channel_idx,
			 struct most_component *comp);
void most_put_mbo(struct mbo *mbo);
int channel_has_mbo(struct most_interface *iface, int channel_idx,
		    struct most_component *comp);
int most_start_channel(struct most_interface *iface, int channel_idx,
		       struct most_component *comp);
int most_stop_channel(struct most_interface *iface, int channel_idx,
		      struct most_component *comp);
int __init configfs_init(void);
int most_register_configfs_subsys(struct most_component *comp);
void most_deregister_configfs_subsys(struct most_component *comp);
int most_add_link(char *mdev, char *mdev_ch, char *comp_name, char *link_name,
		  char *comp_param);
int most_remove_link(char *mdev, char *mdev_ch, char *comp_name);
int most_set_cfg_buffer_size(char *mdev, char *mdev_ch, u16 val);
int most_set_cfg_subbuffer_size(char *mdev, char *mdev_ch, u16 val);
int most_set_cfg_dbr_size(char *mdev, char *mdev_ch, u16 val);
int most_set_cfg_num_buffers(char *mdev, char *mdev_ch, u16 val);
int most_set_cfg_datatype(char *mdev, char *mdev_ch, char *buf);
int most_set_cfg_direction(char *mdev, char *mdev_ch, char *buf);
int most_set_cfg_packets_xact(char *mdev, char *mdev_ch, u16 val);
int most_cfg_complete(char *comp_name);
void most_interface_register_notify(const char *mdev_name);
#endif 
