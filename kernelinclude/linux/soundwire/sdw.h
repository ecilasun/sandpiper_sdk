/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef __SOUNDWIRE_H
#define __SOUNDWIRE_H

#include <linux/bug.h>
#include <linux/lockdep_types.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/mod_devicetable.h>
#include <linux/bitfield.h>

struct sdw_bus;
struct sdw_slave;




#define SDW_BROADCAST_DEV_NUM		15


#define SDW_ENUM_DEV_NUM		0


#define SDW_GROUP12_DEV_NUM		12
#define SDW_GROUP13_DEV_NUM		13


#define SDW_MASTER_DEV_NUM		14

#define SDW_NUM_DEV_ID_REGISTERS	6



#define SDW_FRAME_ROWS		24
#define SDW_FRAME_COLS		8
#define SDW_FRAME_ROW_COLS		(SDW_FRAME_ROWS * SDW_FRAME_COLS)

#define SDW_FRAME_CTRL_BITS		48
#define SDW_MAX_DEVICES			11

#define SDW_MAX_PORTS			15
#define SDW_VALID_PORT_RANGE(n)		((n) < SDW_MAX_PORTS && (n) >= 1)

enum {
	SDW_PORT_DIRN_SINK = 0,
	SDW_PORT_DIRN_SOURCE,
	SDW_PORT_DIRN_MAX,
};




#define SDW_PORT_FLOW_MODE_ISOCH	0
#define SDW_PORT_FLOW_MODE_TX_CNTRL	BIT(0)
#define SDW_PORT_FLOW_MODE_RX_CNTRL	BIT(1)
#define SDW_PORT_FLOW_MODE_ASYNC	GENMASK(1, 0)


#define SDW_BLOCK_PACKG_PER_PORT	BIT(0)
#define SDW_BLOCK_PACKG_PER_CH		BIT(1)


enum sdw_slave_status {
	SDW_SLAVE_UNATTACHED = 0,
	SDW_SLAVE_ATTACHED = 1,
	SDW_SLAVE_ALERT = 2,
	SDW_SLAVE_RESERVED = 3,
};


enum sdw_clk_stop_type {
	SDW_CLK_PRE_PREPARE = 0,
	SDW_CLK_POST_PREPARE,
	SDW_CLK_PRE_DEPREPARE,
	SDW_CLK_POST_DEPREPARE,
};


enum sdw_command_response {
	SDW_CMD_OK = 0,
	SDW_CMD_IGNORED = 1,
	SDW_CMD_FAIL = 2,
	SDW_CMD_TIMEOUT = 3,
	SDW_CMD_FAIL_OTHER = 4,
};


enum sdw_dpn_grouping {
	SDW_BLK_GRP_CNT_1 = 0,
	SDW_BLK_GRP_CNT_2 = 1,
	SDW_BLK_GRP_CNT_3 = 2,
	SDW_BLK_GRP_CNT_4 = 3,
};


enum sdw_dpn_pkg_mode {
	SDW_BLK_PKG_PER_PORT = 0,
	SDW_BLK_PKG_PER_CHANNEL = 1
};


enum sdw_stream_type {
	SDW_STREAM_PCM = 0,
	SDW_STREAM_PDM = 1,
};


enum sdw_data_direction {
	SDW_DATA_DIR_RX = 0,
	SDW_DATA_DIR_TX = 1,
};


enum sdw_port_data_mode {
	SDW_PORT_DATA_MODE_NORMAL = 0,
	SDW_PORT_DATA_MODE_PRBS = 1,
	SDW_PORT_DATA_MODE_STATIC_0 = 2,
	SDW_PORT_DATA_MODE_STATIC_1 = 3,
};


enum sdw_clk_stop_reset_behave {
	SDW_CLK_STOP_KEEP_STATUS = 1,
};


enum sdw_p15_behave {
	SDW_P15_READ_IGNORED = 0,
	SDW_P15_CMD_OK = 1,
};


enum sdw_dpn_type {
	SDW_DPN_FULL = 0,
	SDW_DPN_SIMPLE = 1,
	SDW_DPN_REDUCED = 2,
};


enum sdw_clk_stop_mode {
	SDW_CLK_STOP_MODE0 = 0,
	SDW_CLK_STOP_MODE1 = 1,
};


struct sdw_dp0_prop {
	u32 max_word;
	u32 min_word;
	u32 num_words;
	u32 *words;
	bool BRA_flow_controlled;
	bool simple_ch_prep_sm;
	bool imp_def_interrupts;
};


struct sdw_dpn_audio_mode {
	u32 bus_min_freq;
	u32 bus_max_freq;
	u32 bus_num_freq;
	u32 *bus_freq;
	u32 max_freq;
	u32 min_freq;
	u32 num_freq;
	u32 *freq;
	u32 prep_ch_behave;
	u32 glitchless;
};


struct sdw_dpn_prop {
	u32 num;
	u32 max_word;
	u32 min_word;
	u32 num_words;
	u32 *words;
	enum sdw_dpn_type type;
	u32 max_grouping;
	bool simple_ch_prep_sm;
	u32 ch_prep_timeout;
	u32 imp_def_interrupts;
	u32 max_ch;
	u32 min_ch;
	u32 num_channels;
	u32 *channels;
	u32 num_ch_combinations;
	u32 *ch_combinations;
	u32 modes;
	u32 max_async_buffer;
	bool block_pack_mode;
	bool read_only_wordlength;
	u32 port_encoding;
	struct sdw_dpn_audio_mode *audio_modes;
};


struct sdw_slave_prop {
	u32 mipi_revision;
	bool wake_capable;
	bool test_mode_capable;
	bool clk_stop_mode1;
	bool simple_clk_stop_capable;
	u32 clk_stop_timeout;
	u32 ch_prep_timeout;
	enum sdw_clk_stop_reset_behave reset_behave;
	bool high_PHY_capable;
	bool paging_support;
	bool bank_delay_support;
	enum sdw_p15_behave p15_behave;
	bool lane_control_support;
	u32 master_count;
	u32 source_ports;
	u32 sink_ports;
	struct sdw_dp0_prop *dp0_prop;
	struct sdw_dpn_prop *src_dpn_prop;
	struct sdw_dpn_prop *sink_dpn_prop;
	u8 scp_int1_mask;
	u32 quirks;
	bool clock_reg_supported;
	bool use_domain_irq;
};

#define SDW_SLAVE_QUIRKS_INVALID_INITIAL_PARITY	BIT(0)


struct sdw_master_prop {
	u32 revision;
	u32 clk_stop_modes;
	u32 max_clk_freq;
	u32 num_clk_gears;
	u32 *clk_gears;
	u32 num_clk_freq;
	u32 *clk_freq;
	u32 default_frame_rate;
	u32 default_row;
	u32 default_col;
	bool dynamic_frame;
	u32 err_threshold;
	u32 mclk_freq;
	bool hw_disabled;
	u64 quirks;
};




#define SDW_MASTER_QUIRKS_CLEAR_INITIAL_CLASH	BIT(0)


#define SDW_MASTER_QUIRKS_CLEAR_INITIAL_PARITY	BIT(1)

int sdw_master_read_prop(struct sdw_bus *bus);
int sdw_slave_read_prop(struct sdw_slave *slave);



#define SDW_IGNORED_UNIQUE_ID 0xFF


struct sdw_slave_id {
	__u16 mfg_id;
	__u16 part_id;
	__u8 class_id;
	__u8 unique_id;
	__u8 sdw_version:4;
};

struct sdw_extended_slave_id {
	int link_id;
	struct sdw_slave_id id;
};


#define SDW_DISCO_LINK_ID_MASK	GENMASK_ULL(51, 48)
#define SDW_VERSION_MASK	GENMASK_ULL(47, 44)
#define SDW_UNIQUE_ID_MASK	GENMASK_ULL(43, 40)
#define SDW_MFG_ID_MASK		GENMASK_ULL(39, 24)
#define SDW_PART_ID_MASK	GENMASK_ULL(23, 8)
#define SDW_CLASS_ID_MASK	GENMASK_ULL(7, 0)

#define SDW_DISCO_LINK_ID(addr)	FIELD_GET(SDW_DISCO_LINK_ID_MASK, addr)
#define SDW_VERSION(addr)	FIELD_GET(SDW_VERSION_MASK, addr)
#define SDW_UNIQUE_ID(addr)	FIELD_GET(SDW_UNIQUE_ID_MASK, addr)
#define SDW_MFG_ID(addr)	FIELD_GET(SDW_MFG_ID_MASK, addr)
#define SDW_PART_ID(addr)	FIELD_GET(SDW_PART_ID_MASK, addr)
#define SDW_CLASS_ID(addr)	FIELD_GET(SDW_CLASS_ID_MASK, addr)


struct sdw_slave_intr_status {
	bool sdca_cascade;
	u8 control_port;
	u8 port[15];
};


enum sdw_reg_bank {
	SDW_BANK0,
	SDW_BANK1,
};


struct sdw_bus_conf {
	unsigned int clk_freq;
	unsigned int num_rows;
	unsigned int num_cols;
	unsigned int bank;
};


struct sdw_prepare_ch {
	unsigned int num;
	unsigned int ch_mask;
	bool prepare;
	unsigned int bank;
};


enum sdw_port_prep_ops {
	SDW_OPS_PORT_PRE_PREP = 0,
	SDW_OPS_PORT_PRE_DEPREP,
	SDW_OPS_PORT_POST_PREP,
	SDW_OPS_PORT_POST_DEPREP,
};


struct sdw_bus_params {
	enum sdw_reg_bank curr_bank;
	enum sdw_reg_bank next_bank;
	unsigned int max_dr_freq;
	unsigned int curr_dr_freq;
	unsigned int bandwidth;
	unsigned int col;
	unsigned int row;
	int s_data_mode;
	int m_data_mode;
};


struct sdw_slave_ops {
	int (*read_prop)(struct sdw_slave *sdw);
	int (*interrupt_callback)(struct sdw_slave *slave,
				  struct sdw_slave_intr_status *status);
	int (*update_status)(struct sdw_slave *slave,
			     enum sdw_slave_status status);
	int (*bus_config)(struct sdw_slave *slave,
			  struct sdw_bus_params *params);
	int (*port_prep)(struct sdw_slave *slave,
			 struct sdw_prepare_ch *prepare_ch,
			 enum sdw_port_prep_ops pre_ops);
	int (*clk_stop)(struct sdw_slave *slave,
			enum sdw_clk_stop_mode mode,
			enum sdw_clk_stop_type type);

};


struct sdw_slave {
	struct sdw_slave_id id;
	struct device dev;
	int irq;
	enum sdw_slave_status status;
	struct sdw_bus *bus;
	struct sdw_slave_prop prop;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs;
#endif
	struct list_head node;
	struct completion port_ready[SDW_MAX_PORTS];
	unsigned int m_port_map[SDW_MAX_PORTS];
	u16 dev_num;
	u16 dev_num_sticky;
	bool probed;
	struct completion enumeration_complete;
	struct completion initialization_complete;
	u32 unattach_request;
	bool first_interrupt_done;
	bool is_mockup_device;
	struct mutex sdw_dev_lock; 
};

#define dev_to_sdw_dev(_dev) container_of(_dev, struct sdw_slave, dev)


struct sdw_master_device {
	struct device dev;
	struct sdw_bus *bus;
};

#define dev_to_sdw_master_device(d)	\
	container_of(d, struct sdw_master_device, dev)

struct sdw_driver {
	const char *name;

	int (*probe)(struct sdw_slave *sdw,
			const struct sdw_device_id *id);
	int (*remove)(struct sdw_slave *sdw);
	void (*shutdown)(struct sdw_slave *sdw);

	const struct sdw_device_id *id_table;
	const struct sdw_slave_ops *ops;

	struct device_driver driver;
};

#define SDW_SLAVE_ENTRY_EXT(_mfg_id, _part_id, _version, _c_id, _drv_data) \
	{ .mfg_id = (_mfg_id), .part_id = (_part_id),		\
	  .sdw_version = (_version), .class_id = (_c_id),	\
	  .driver_data = (unsigned long)(_drv_data) }

#define SDW_SLAVE_ENTRY(_mfg_id, _part_id, _drv_data)	\
	SDW_SLAVE_ENTRY_EXT((_mfg_id), (_part_id), 0, 0, (_drv_data))

int sdw_handle_slave_status(struct sdw_bus *bus,
			enum sdw_slave_status status[]);




struct sdw_port_params {
	unsigned int num;
	unsigned int bps;
	unsigned int flow_mode;
	unsigned int data_mode;
};


struct sdw_transport_params {
	bool blk_grp_ctrl_valid;
	unsigned int port_num;
	unsigned int blk_grp_ctrl;
	unsigned int sample_interval;
	unsigned int offset1;
	unsigned int offset2;
	unsigned int hstart;
	unsigned int hstop;
	unsigned int blk_pkg_mode;
	unsigned int lane_ctrl;
};


struct sdw_enable_ch {
	unsigned int port_num;
	unsigned int ch_mask;
	bool enable;
};


struct sdw_master_port_ops {
	int (*dpn_set_port_params)(struct sdw_bus *bus,
			struct sdw_port_params *port_params,
			unsigned int bank);
	int (*dpn_set_port_transport_params)(struct sdw_bus *bus,
			struct sdw_transport_params *transport_params,
			enum sdw_reg_bank bank);
	int (*dpn_port_prep)(struct sdw_bus *bus,
			struct sdw_prepare_ch *prepare_ch);
	int (*dpn_port_enable_ch)(struct sdw_bus *bus,
			struct sdw_enable_ch *enable_ch, unsigned int bank);
};

struct sdw_msg;


struct sdw_defer {
	int length;
	struct completion complete;
	struct sdw_msg *msg;
};


struct sdw_master_ops {
	int (*read_prop)(struct sdw_bus *bus);
	u64 (*override_adr)
			(struct sdw_bus *bus, u64 addr);
	enum sdw_command_response (*xfer_msg)
			(struct sdw_bus *bus, struct sdw_msg *msg);
	enum sdw_command_response (*xfer_msg_defer)
			(struct sdw_bus *bus);
	int (*set_bus_conf)(struct sdw_bus *bus,
			struct sdw_bus_params *params);
	int (*pre_bank_switch)(struct sdw_bus *bus);
	int (*post_bank_switch)(struct sdw_bus *bus);
	u32 (*read_ping_status)(struct sdw_bus *bus);
	int (*get_device_num)(struct sdw_bus *bus, struct sdw_slave *slave);
	void (*put_device_num)(struct sdw_bus *bus, struct sdw_slave *slave);
	void (*new_peripheral_assigned)(struct sdw_bus *bus,
					struct sdw_slave *slave,
					int dev_num);
};


struct sdw_bus {
	struct device *dev;
	struct sdw_master_device *md;
	int controller_id;
	unsigned int link_id;
	int id;
	struct list_head slaves;
	DECLARE_BITMAP(assigned, SDW_MAX_DEVICES);
	struct mutex bus_lock;
	struct lock_class_key bus_lock_key;
	struct mutex msg_lock;
	struct lock_class_key msg_lock_key;
	int (*compute_params)(struct sdw_bus *bus);
	const struct sdw_master_ops *ops;
	const struct sdw_master_port_ops *port_ops;
	struct sdw_bus_params params;
	struct sdw_master_prop prop;
	struct list_head m_rt_list;
#ifdef CONFIG_DEBUG_FS
	struct dentry *debugfs;
#endif
	struct irq_chip irq_chip;
	struct irq_domain *domain;
	struct sdw_defer defer_msg;
	unsigned int clk_stop_timeout;
	u32 bank_switch_timeout;
	bool multi_link;
	int hw_sync_min_links;
};

int sdw_bus_master_add(struct sdw_bus *bus, struct device *parent,
		       struct fwnode_handle *fwnode);
void sdw_bus_master_delete(struct sdw_bus *bus);

void sdw_show_ping_status(struct sdw_bus *bus, bool sync_delay);


struct sdw_port_config {
	unsigned int num;
	unsigned int ch_mask;
};


struct sdw_stream_config {
	unsigned int frame_rate;
	unsigned int ch_count;
	unsigned int bps;
	enum sdw_data_direction direction;
	enum sdw_stream_type type;
};


enum sdw_stream_state {
	SDW_STREAM_ALLOCATED = 0,
	SDW_STREAM_CONFIGURED = 1,
	SDW_STREAM_PREPARED = 2,
	SDW_STREAM_ENABLED = 3,
	SDW_STREAM_DISABLED = 4,
	SDW_STREAM_DEPREPARED = 5,
	SDW_STREAM_RELEASED = 6,
};


struct sdw_stream_params {
	unsigned int rate;
	unsigned int ch_count;
	unsigned int bps;
};


struct sdw_stream_runtime {
	const char *name;
	struct sdw_stream_params params;
	enum sdw_stream_state state;
	enum sdw_stream_type type;
	struct list_head master_list;
	int m_rt_count;
};

struct sdw_stream_runtime *sdw_alloc_stream(const char *stream_name);
void sdw_release_stream(struct sdw_stream_runtime *stream);

int sdw_compute_params(struct sdw_bus *bus);

int sdw_stream_add_master(struct sdw_bus *bus,
		struct sdw_stream_config *stream_config,
		struct sdw_port_config *port_config,
		unsigned int num_ports,
		struct sdw_stream_runtime *stream);
int sdw_stream_remove_master(struct sdw_bus *bus,
		struct sdw_stream_runtime *stream);
int sdw_startup_stream(void *sdw_substream);
int sdw_prepare_stream(struct sdw_stream_runtime *stream);
int sdw_enable_stream(struct sdw_stream_runtime *stream);
int sdw_disable_stream(struct sdw_stream_runtime *stream);
int sdw_deprepare_stream(struct sdw_stream_runtime *stream);
void sdw_shutdown_stream(void *sdw_substream);
int sdw_bus_prep_clk_stop(struct sdw_bus *bus);
int sdw_bus_clk_stop(struct sdw_bus *bus);
int sdw_bus_exit_clk_stop(struct sdw_bus *bus);

int sdw_compare_devid(struct sdw_slave *slave, struct sdw_slave_id id);
void sdw_extract_slave_id(struct sdw_bus *bus, u64 addr, struct sdw_slave_id *id);

#if IS_ENABLED(CONFIG_SOUNDWIRE)

int sdw_stream_add_slave(struct sdw_slave *slave,
			 struct sdw_stream_config *stream_config,
			 struct sdw_port_config *port_config,
			 unsigned int num_ports,
			 struct sdw_stream_runtime *stream);
int sdw_stream_remove_slave(struct sdw_slave *slave,
			    struct sdw_stream_runtime *stream);


int sdw_read(struct sdw_slave *slave, u32 addr);
int sdw_write(struct sdw_slave *slave, u32 addr, u8 value);
int sdw_write_no_pm(struct sdw_slave *slave, u32 addr, u8 value);
int sdw_read_no_pm(struct sdw_slave *slave, u32 addr);
int sdw_nread(struct sdw_slave *slave, u32 addr, size_t count, u8 *val);
int sdw_nread_no_pm(struct sdw_slave *slave, u32 addr, size_t count, u8 *val);
int sdw_nwrite(struct sdw_slave *slave, u32 addr, size_t count, const u8 *val);
int sdw_nwrite_no_pm(struct sdw_slave *slave, u32 addr, size_t count, const u8 *val);
int sdw_update(struct sdw_slave *slave, u32 addr, u8 mask, u8 val);
int sdw_update_no_pm(struct sdw_slave *slave, u32 addr, u8 mask, u8 val);

#else

static inline int sdw_stream_add_slave(struct sdw_slave *slave,
				       struct sdw_stream_config *stream_config,
				       struct sdw_port_config *port_config,
				       unsigned int num_ports,
				       struct sdw_stream_runtime *stream)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_stream_remove_slave(struct sdw_slave *slave,
					  struct sdw_stream_runtime *stream)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}


static inline int sdw_read(struct sdw_slave *slave, u32 addr)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_write(struct sdw_slave *slave, u32 addr, u8 value)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_write_no_pm(struct sdw_slave *slave, u32 addr, u8 value)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_read_no_pm(struct sdw_slave *slave, u32 addr)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_nread(struct sdw_slave *slave, u32 addr, size_t count, u8 *val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_nread_no_pm(struct sdw_slave *slave, u32 addr, size_t count, u8 *val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_nwrite(struct sdw_slave *slave, u32 addr, size_t count, const u8 *val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_nwrite_no_pm(struct sdw_slave *slave, u32 addr, size_t count, const u8 *val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_update(struct sdw_slave *slave, u32 addr, u8 mask, u8 val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

static inline int sdw_update_no_pm(struct sdw_slave *slave, u32 addr, u8 mask, u8 val)
{
	WARN_ONCE(1, "SoundWire API is disabled");
	return -EINVAL;
}

#endif 

#endif 
