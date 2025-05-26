
#ifndef _DRM_DP_MST_HELPER_H_
#define _DRM_DP_MST_HELPER_H_

#include <linux/types.h>
#include <drm/display/drm_dp_helper.h>
#include <drm/drm_atomic.h>

#if IS_ENABLED(CONFIG_DRM_DEBUG_DP_MST_TOPOLOGY_REFS)
#include <linux/stackdepot.h>
#include <linux/timekeeping.h>

enum drm_dp_mst_topology_ref_type {
	DRM_DP_MST_TOPOLOGY_REF_GET,
	DRM_DP_MST_TOPOLOGY_REF_PUT,
};

struct drm_dp_mst_topology_ref_history {
	struct drm_dp_mst_topology_ref_entry {
		enum drm_dp_mst_topology_ref_type type;
		int count;
		ktime_t ts_nsec;
		depot_stack_handle_t backtrace;
	} *entries;
	int len;
};
#endif 

struct drm_dp_mst_branch;


struct drm_dp_mst_port {
	
	struct kref topology_kref;

	
	struct kref malloc_kref;

#if IS_ENABLED(CONFIG_DRM_DEBUG_DP_MST_TOPOLOGY_REFS)
	
	struct drm_dp_mst_topology_ref_history topology_ref_history;
#endif

	u8 port_num;
	bool input;
	bool mcs;
	bool ddps;
	u8 pdt;
	bool ldps;
	u8 dpcd_rev;
	u8 num_sdp_streams;
	u8 num_sdp_stream_sinks;
	uint16_t full_pbn;
	struct list_head next;
	
	struct drm_dp_mst_branch *mstb;
	struct drm_dp_aux aux; 
	struct drm_dp_aux *passthrough_aux;
	struct drm_dp_mst_branch *parent;

	struct drm_connector *connector;
	struct drm_dp_mst_topology_mgr *mgr;

	
	const struct drm_edid *cached_edid;

	
	bool fec_capable;
};


struct drm_dp_sideband_msg_hdr {
	u8 lct;
	u8 lcr;
	u8 rad[8];
	bool broadcast;
	bool path_msg;
	u8 msg_len;
	bool somt;
	bool eomt;
	bool seqno;
};

struct drm_dp_sideband_msg_rx {
	u8 chunk[48];
	u8 msg[256];
	u8 curchunk_len;
	u8 curchunk_idx; 
	u8 curchunk_hdrlen;
	u8 curlen; 
	bool have_somt;
	bool have_eomt;
	struct drm_dp_sideband_msg_hdr initial_hdr;
};


struct drm_dp_mst_branch {
	
	struct kref topology_kref;

	
	struct kref malloc_kref;

#if IS_ENABLED(CONFIG_DRM_DEBUG_DP_MST_TOPOLOGY_REFS)
	
	struct drm_dp_mst_topology_ref_history topology_ref_history;
#endif

	
	struct list_head destroy_next;

	u8 rad[8];
	u8 lct;
	int num_ports;

	
	struct list_head ports;

	struct drm_dp_mst_port *port_parent;
	struct drm_dp_mst_topology_mgr *mgr;

	bool link_address_sent;

	
	u8 guid[16];
};


struct drm_dp_nak_reply {
	u8 guid[16];
	u8 reason;
	u8 nak_data;
};

struct drm_dp_link_address_ack_reply {
	u8 guid[16];
	u8 nports;
	struct drm_dp_link_addr_reply_port {
		bool input_port;
		u8 peer_device_type;
		u8 port_number;
		bool mcs;
		bool ddps;
		bool legacy_device_plug_status;
		u8 dpcd_revision;
		u8 peer_guid[16];
		u8 num_sdp_streams;
		u8 num_sdp_stream_sinks;
	} ports[16];
};

struct drm_dp_remote_dpcd_read_ack_reply {
	u8 port_number;
	u8 num_bytes;
	u8 bytes[255];
};

struct drm_dp_remote_dpcd_write_ack_reply {
	u8 port_number;
};

struct drm_dp_remote_dpcd_write_nak_reply {
	u8 port_number;
	u8 reason;
	u8 bytes_written_before_failure;
};

struct drm_dp_remote_i2c_read_ack_reply {
	u8 port_number;
	u8 num_bytes;
	u8 bytes[255];
};

struct drm_dp_remote_i2c_read_nak_reply {
	u8 port_number;
	u8 nak_reason;
	u8 i2c_nak_transaction;
};

struct drm_dp_remote_i2c_write_ack_reply {
	u8 port_number;
};

struct drm_dp_query_stream_enc_status_ack_reply {
	
	u8 stream_id;

	
	bool reply_signed;

	
	bool unauthorizable_device_present;
	bool legacy_device_present;
	bool query_capable_device_present;

	
	bool hdcp_1x_device_present;
	bool hdcp_2x_device_present;

	
	bool auth_completed;

	
	bool encryption_enabled;

	
	bool repeater_present;

	
	u8 state;
};

#define DRM_DP_MAX_SDP_STREAMS 16
struct drm_dp_allocate_payload {
	u8 port_number;
	u8 number_sdp_streams;
	u8 vcpi;
	u16 pbn;
	u8 sdp_stream_sink[DRM_DP_MAX_SDP_STREAMS];
};

struct drm_dp_allocate_payload_ack_reply {
	u8 port_number;
	u8 vcpi;
	u16 allocated_pbn;
};

struct drm_dp_connection_status_notify {
	u8 guid[16];
	u8 port_number;
	bool legacy_device_plug_status;
	bool displayport_device_plug_status;
	bool message_capability_status;
	bool input_port;
	u8 peer_device_type;
};

struct drm_dp_remote_dpcd_read {
	u8 port_number;
	u32 dpcd_address;
	u8 num_bytes;
};

struct drm_dp_remote_dpcd_write {
	u8 port_number;
	u32 dpcd_address;
	u8 num_bytes;
	u8 *bytes;
};

#define DP_REMOTE_I2C_READ_MAX_TRANSACTIONS 4
struct drm_dp_remote_i2c_read {
	u8 num_transactions;
	u8 port_number;
	struct drm_dp_remote_i2c_read_tx {
		u8 i2c_dev_id;
		u8 num_bytes;
		u8 *bytes;
		u8 no_stop_bit;
		u8 i2c_transaction_delay;
	} transactions[DP_REMOTE_I2C_READ_MAX_TRANSACTIONS];
	u8 read_i2c_device_id;
	u8 num_bytes_read;
};

struct drm_dp_remote_i2c_write {
	u8 port_number;
	u8 write_i2c_device_id;
	u8 num_bytes;
	u8 *bytes;
};

struct drm_dp_query_stream_enc_status {
	u8 stream_id;
	u8 client_id[7];	
	u8 stream_event;
	bool valid_stream_event;
	u8 stream_behavior;
	u8 valid_stream_behavior;
};


struct drm_dp_port_number_req {
	u8 port_number;
};

struct drm_dp_enum_path_resources_ack_reply {
	u8 port_number;
	bool fec_capable;
	u16 full_payload_bw_number;
	u16 avail_payload_bw_number;
};


struct drm_dp_port_number_rep {
	u8 port_number;
};

struct drm_dp_query_payload {
	u8 port_number;
	u8 vcpi;
};

struct drm_dp_resource_status_notify {
	u8 port_number;
	u8 guid[16];
	u16 available_pbn;
};

struct drm_dp_query_payload_ack_reply {
	u8 port_number;
	u16 allocated_pbn;
};

struct drm_dp_sideband_msg_req_body {
	u8 req_type;
	union ack_req {
		struct drm_dp_connection_status_notify conn_stat;
		struct drm_dp_port_number_req port_num;
		struct drm_dp_resource_status_notify resource_stat;

		struct drm_dp_query_payload query_payload;
		struct drm_dp_allocate_payload allocate_payload;

		struct drm_dp_remote_dpcd_read dpcd_read;
		struct drm_dp_remote_dpcd_write dpcd_write;

		struct drm_dp_remote_i2c_read i2c_read;
		struct drm_dp_remote_i2c_write i2c_write;

		struct drm_dp_query_stream_enc_status enc_status;
	} u;
};

struct drm_dp_sideband_msg_reply_body {
	u8 reply_type;
	u8 req_type;
	union ack_replies {
		struct drm_dp_nak_reply nak;
		struct drm_dp_link_address_ack_reply link_addr;
		struct drm_dp_port_number_rep port_number;

		struct drm_dp_enum_path_resources_ack_reply path_resources;
		struct drm_dp_allocate_payload_ack_reply allocate_payload;
		struct drm_dp_query_payload_ack_reply query_payload;

		struct drm_dp_remote_dpcd_read_ack_reply remote_dpcd_read_ack;
		struct drm_dp_remote_dpcd_write_ack_reply remote_dpcd_write_ack;
		struct drm_dp_remote_dpcd_write_nak_reply remote_dpcd_write_nack;

		struct drm_dp_remote_i2c_read_ack_reply remote_i2c_read_ack;
		struct drm_dp_remote_i2c_read_nak_reply remote_i2c_read_nack;
		struct drm_dp_remote_i2c_write_ack_reply remote_i2c_write_ack;

		struct drm_dp_query_stream_enc_status_ack_reply enc_status;
	} u;
};


#define DRM_DP_SIDEBAND_TX_QUEUED 0

#define DRM_DP_SIDEBAND_TX_START_SEND 1

#define DRM_DP_SIDEBAND_TX_SENT 2

#define DRM_DP_SIDEBAND_TX_RX 3
#define DRM_DP_SIDEBAND_TX_TIMEOUT 4

struct drm_dp_sideband_msg_tx {
	u8 msg[256];
	u8 chunk[48];
	u8 cur_offset;
	u8 cur_len;
	struct drm_dp_mst_branch *dst;
	struct list_head next;
	int seqno;
	int state;
	bool path_msg;
	struct drm_dp_sideband_msg_reply_body reply;
};


struct drm_dp_mst_topology_mgr;
struct drm_dp_mst_topology_cbs {
	
	struct drm_connector *(*add_connector)(struct drm_dp_mst_topology_mgr *mgr, struct drm_dp_mst_port *port, const char *path);
	
	void (*poll_hpd_irq)(struct drm_dp_mst_topology_mgr *mgr);
};

#define to_dp_mst_topology_state(x) container_of(x, struct drm_dp_mst_topology_state, base)


struct drm_dp_mst_atomic_payload {
	
	struct drm_dp_mst_port *port;

	
	s8 vc_start_slot;

	
	u8 vcpi;
	
	int time_slots;
	
	int pbn;

	
	bool delete : 1;
	
	bool dsc_enabled : 1;

	
	struct list_head next;
};


struct drm_dp_mst_topology_state {
	
	struct drm_private_state base;

	
	struct drm_dp_mst_topology_mgr *mgr;

	
	u32 pending_crtc_mask;
	
	struct drm_crtc_commit **commit_deps;
	
	size_t num_commit_deps;

	
	u32 payload_mask;
	
	struct list_head payloads;

	
	u8 total_avail_slots;
	
	u8 start_slot;

	
	int pbn_div;
};

#define to_dp_mst_topology_mgr(x) container_of(x, struct drm_dp_mst_topology_mgr, base)


struct drm_dp_mst_topology_mgr {
	
	struct drm_private_obj base;

	
	struct drm_device *dev;
	
	const struct drm_dp_mst_topology_cbs *cbs;
	
	int max_dpcd_transaction_bytes;
	
	struct drm_dp_aux *aux;
	
	int max_payloads;
	
	int conn_base_id;

	
	struct drm_dp_sideband_msg_rx up_req_recv;

	
	struct drm_dp_sideband_msg_rx down_rep_recv;

	
	struct mutex lock;

	
	struct mutex probe_lock;

	
	bool mst_state : 1;

	
	bool payload_id_table_cleared : 1;

	
	u8 payload_count;

	
	u8 next_start_slot;

	
	struct drm_dp_mst_branch *mst_primary;

	
	u8 dpcd[DP_RECEIVER_CAP_SIZE];
	
	u8 sink_count;

	
	const struct drm_private_state_funcs *funcs;

	
	struct mutex qlock;

	
	struct list_head tx_msg_downq;

	
	wait_queue_head_t tx_waitq;
	
	struct work_struct work;
	
	struct work_struct tx_work;

	
	struct list_head destroy_port_list;
	
	struct list_head destroy_branch_device_list;
	
	struct mutex delayed_destroy_lock;

	
	struct workqueue_struct *delayed_destroy_wq;

	
	struct work_struct delayed_destroy_work;

	
	struct list_head up_req_list;
	
	struct mutex up_req_lock;
	
	struct work_struct up_req_work;

#if IS_ENABLED(CONFIG_DRM_DEBUG_DP_MST_TOPOLOGY_REFS)
	
	struct mutex topology_ref_history_lock;
#endif
};

int drm_dp_mst_topology_mgr_init(struct drm_dp_mst_topology_mgr *mgr,
				 struct drm_device *dev, struct drm_dp_aux *aux,
				 int max_dpcd_transaction_bytes,
				 int max_payloads, int conn_base_id);

void drm_dp_mst_topology_mgr_destroy(struct drm_dp_mst_topology_mgr *mgr);

bool drm_dp_read_mst_cap(struct drm_dp_aux *aux, const u8 dpcd[DP_RECEIVER_CAP_SIZE]);
int drm_dp_mst_topology_mgr_set_mst(struct drm_dp_mst_topology_mgr *mgr, bool mst_state);

int drm_dp_mst_hpd_irq_handle_event(struct drm_dp_mst_topology_mgr *mgr,
				    const u8 *esi,
				    u8 *ack,
				    bool *handled);
void drm_dp_mst_hpd_irq_send_new_request(struct drm_dp_mst_topology_mgr *mgr);

int
drm_dp_mst_detect_port(struct drm_connector *connector,
		       struct drm_modeset_acquire_ctx *ctx,
		       struct drm_dp_mst_topology_mgr *mgr,
		       struct drm_dp_mst_port *port);

const struct drm_edid *drm_dp_mst_edid_read(struct drm_connector *connector,
					    struct drm_dp_mst_topology_mgr *mgr,
					    struct drm_dp_mst_port *port);
struct edid *drm_dp_mst_get_edid(struct drm_connector *connector,
				 struct drm_dp_mst_topology_mgr *mgr,
				 struct drm_dp_mst_port *port);

int drm_dp_get_vc_payload_bw(const struct drm_dp_mst_topology_mgr *mgr,
			     int link_rate, int link_lane_count);

int drm_dp_calc_pbn_mode(int clock, int bpp);

void drm_dp_mst_update_slots(struct drm_dp_mst_topology_state *mst_state, uint8_t link_encoding_cap);

int drm_dp_add_payload_part1(struct drm_dp_mst_topology_mgr *mgr,
			     struct drm_dp_mst_topology_state *mst_state,
			     struct drm_dp_mst_atomic_payload *payload);
int drm_dp_add_payload_part2(struct drm_dp_mst_topology_mgr *mgr,
			     struct drm_atomic_state *state,
			     struct drm_dp_mst_atomic_payload *payload);
void drm_dp_remove_payload(struct drm_dp_mst_topology_mgr *mgr,
			   struct drm_dp_mst_topology_state *mst_state,
			   const struct drm_dp_mst_atomic_payload *old_payload,
			   struct drm_dp_mst_atomic_payload *new_payload);

int drm_dp_check_act_status(struct drm_dp_mst_topology_mgr *mgr);

void drm_dp_mst_dump_topology(struct seq_file *m,
			      struct drm_dp_mst_topology_mgr *mgr);

void drm_dp_mst_topology_mgr_suspend(struct drm_dp_mst_topology_mgr *mgr);
int __must_check
drm_dp_mst_topology_mgr_resume(struct drm_dp_mst_topology_mgr *mgr,
			       bool sync);

ssize_t drm_dp_mst_dpcd_read(struct drm_dp_aux *aux,
			     unsigned int offset, void *buffer, size_t size);
ssize_t drm_dp_mst_dpcd_write(struct drm_dp_aux *aux,
			      unsigned int offset, void *buffer, size_t size);

int drm_dp_mst_connector_late_register(struct drm_connector *connector,
				       struct drm_dp_mst_port *port);
void drm_dp_mst_connector_early_unregister(struct drm_connector *connector,
					   struct drm_dp_mst_port *port);

struct drm_dp_mst_topology_state *
drm_atomic_get_mst_topology_state(struct drm_atomic_state *state,
				  struct drm_dp_mst_topology_mgr *mgr);
struct drm_dp_mst_topology_state *
drm_atomic_get_old_mst_topology_state(struct drm_atomic_state *state,
				      struct drm_dp_mst_topology_mgr *mgr);
struct drm_dp_mst_topology_state *
drm_atomic_get_new_mst_topology_state(struct drm_atomic_state *state,
				      struct drm_dp_mst_topology_mgr *mgr);
struct drm_dp_mst_atomic_payload *
drm_atomic_get_mst_payload_state(struct drm_dp_mst_topology_state *state,
				 struct drm_dp_mst_port *port);
int __must_check
drm_dp_atomic_find_time_slots(struct drm_atomic_state *state,
			      struct drm_dp_mst_topology_mgr *mgr,
			      struct drm_dp_mst_port *port, int pbn);
int drm_dp_mst_atomic_enable_dsc(struct drm_atomic_state *state,
				 struct drm_dp_mst_port *port,
				 int pbn, bool enable);
int __must_check
drm_dp_mst_add_affected_dsc_crtcs(struct drm_atomic_state *state,
				  struct drm_dp_mst_topology_mgr *mgr);
int __must_check
drm_dp_atomic_release_time_slots(struct drm_atomic_state *state,
				 struct drm_dp_mst_topology_mgr *mgr,
				 struct drm_dp_mst_port *port);
void drm_dp_mst_atomic_wait_for_dependencies(struct drm_atomic_state *state);
int __must_check drm_dp_mst_atomic_setup_commit(struct drm_atomic_state *state);
int drm_dp_send_power_updown_phy(struct drm_dp_mst_topology_mgr *mgr,
				 struct drm_dp_mst_port *port, bool power_up);
int drm_dp_send_query_stream_enc_status(struct drm_dp_mst_topology_mgr *mgr,
		struct drm_dp_mst_port *port,
		struct drm_dp_query_stream_enc_status_ack_reply *status);
int __must_check drm_dp_mst_atomic_check(struct drm_atomic_state *state);
int __must_check drm_dp_mst_root_conn_atomic_check(struct drm_connector_state *new_conn_state,
						   struct drm_dp_mst_topology_mgr *mgr);

void drm_dp_mst_get_port_malloc(struct drm_dp_mst_port *port);
void drm_dp_mst_put_port_malloc(struct drm_dp_mst_port *port);

struct drm_dp_aux *drm_dp_mst_dsc_aux_for_port(struct drm_dp_mst_port *port);

static inline struct drm_dp_mst_topology_state *
to_drm_dp_mst_topology_state(struct drm_private_state *state)
{
	return container_of(state, struct drm_dp_mst_topology_state, base);
}

extern const struct drm_private_state_funcs drm_dp_mst_topology_state_funcs;


static inline bool
__drm_dp_mst_state_iter_get(struct drm_atomic_state *state,
			    struct drm_dp_mst_topology_mgr **mgr,
			    struct drm_dp_mst_topology_state **old_state,
			    struct drm_dp_mst_topology_state **new_state,
			    int i)
{
	struct __drm_private_objs_state *objs_state = &state->private_objs[i];

	if (objs_state->ptr->funcs != &drm_dp_mst_topology_state_funcs)
		return false;

	*mgr = to_dp_mst_topology_mgr(objs_state->ptr);
	if (old_state)
		*old_state = to_dp_mst_topology_state(objs_state->old_state);
	if (new_state)
		*new_state = to_dp_mst_topology_state(objs_state->new_state);

	return true;
}


#define for_each_oldnew_mst_mgr_in_state(__state, mgr, old_state, new_state, __i) \
	for ((__i) = 0; (__i) < (__state)->num_private_objs; (__i)++) \
		for_each_if(__drm_dp_mst_state_iter_get((__state), &(mgr), &(old_state), &(new_state), (__i)))


#define for_each_old_mst_mgr_in_state(__state, mgr, old_state, __i) \
	for ((__i) = 0; (__i) < (__state)->num_private_objs; (__i)++) \
		for_each_if(__drm_dp_mst_state_iter_get((__state), &(mgr), &(old_state), NULL, (__i)))


#define for_each_new_mst_mgr_in_state(__state, mgr, new_state, __i) \
	for ((__i) = 0; (__i) < (__state)->num_private_objs; (__i)++) \
		for_each_if(__drm_dp_mst_state_iter_get((__state), &(mgr), NULL, &(new_state), (__i)))

#endif
