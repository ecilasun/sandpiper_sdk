/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */


#ifndef RDMA_CM_H
#define RDMA_CM_H

#include <linux/socket.h>
#include <linux/in6.h>
#include <rdma/ib_addr.h>
#include <rdma/ib_sa.h>
#include <uapi/rdma/rdma_user_cm.h>


enum rdma_cm_event_type {
	RDMA_CM_EVENT_ADDR_RESOLVED,
	RDMA_CM_EVENT_ADDR_ERROR,
	RDMA_CM_EVENT_ROUTE_RESOLVED,
	RDMA_CM_EVENT_ROUTE_ERROR,
	RDMA_CM_EVENT_CONNECT_REQUEST,
	RDMA_CM_EVENT_CONNECT_RESPONSE,
	RDMA_CM_EVENT_CONNECT_ERROR,
	RDMA_CM_EVENT_UNREACHABLE,
	RDMA_CM_EVENT_REJECTED,
	RDMA_CM_EVENT_ESTABLISHED,
	RDMA_CM_EVENT_DISCONNECTED,
	RDMA_CM_EVENT_DEVICE_REMOVAL,
	RDMA_CM_EVENT_MULTICAST_JOIN,
	RDMA_CM_EVENT_MULTICAST_ERROR,
	RDMA_CM_EVENT_ADDR_CHANGE,
	RDMA_CM_EVENT_TIMEWAIT_EXIT
};

const char *__attribute_const__ rdma_event_msg(enum rdma_cm_event_type event);

#define RDMA_IB_IP_PS_MASK   0xFFFFFFFFFFFF0000ULL
#define RDMA_IB_IP_PS_TCP    0x0000000001060000ULL
#define RDMA_IB_IP_PS_UDP    0x0000000001110000ULL
#define RDMA_IB_IP_PS_IB     0x00000000013F0000ULL

struct rdma_addr {
	struct sockaddr_storage src_addr;
	struct sockaddr_storage dst_addr;
	struct rdma_dev_addr dev_addr;
};

struct rdma_route {
	struct rdma_addr addr;
	struct sa_path_rec *path_rec;

	
	struct sa_path_rec *path_rec_inbound;
	struct sa_path_rec *path_rec_outbound;

	
	int num_pri_alt_paths;
};

struct rdma_conn_param {
	const void *private_data;
	u8 private_data_len;
	u8 responder_resources;
	u8 initiator_depth;
	u8 flow_control;
	u8 retry_count;		
	u8 rnr_retry_count;
	
	u8 srq;
	u32 qp_num;
	u32 qkey;
};

struct rdma_ud_param {
	const void *private_data;
	u8 private_data_len;
	struct rdma_ah_attr ah_attr;
	u32 qp_num;
	u32 qkey;
};

struct rdma_cm_event {
	enum rdma_cm_event_type	 event;
	int			 status;
	union {
		struct rdma_conn_param	conn;
		struct rdma_ud_param	ud;
	} param;
	struct rdma_ucm_ece ece;
};

struct rdma_cm_id;


typedef int (*rdma_cm_event_handler)(struct rdma_cm_id *id,
				     struct rdma_cm_event *event);

struct rdma_cm_id {
	struct ib_device	*device;
	void			*context;
	struct ib_qp		*qp;
	rdma_cm_event_handler	 event_handler;
	struct rdma_route	 route;
	enum rdma_ucm_port_space ps;
	enum ib_qp_type		 qp_type;
	u32			 port_num;
	struct work_struct net_work;
};

struct rdma_cm_id *
__rdma_create_kernel_id(struct net *net, rdma_cm_event_handler event_handler,
			void *context, enum rdma_ucm_port_space ps,
			enum ib_qp_type qp_type, const char *caller);
struct rdma_cm_id *rdma_create_user_id(rdma_cm_event_handler event_handler,
				       void *context,
				       enum rdma_ucm_port_space ps,
				       enum ib_qp_type qp_type);


#define rdma_create_id(net, event_handler, context, ps, qp_type)               \
	__rdma_create_kernel_id(net, event_handler, context, ps, qp_type,      \
				KBUILD_MODNAME)


void rdma_destroy_id(struct rdma_cm_id *id);


int rdma_bind_addr(struct rdma_cm_id *id, struct sockaddr *addr);


int rdma_resolve_addr(struct rdma_cm_id *id, struct sockaddr *src_addr,
		      const struct sockaddr *dst_addr,
		      unsigned long timeout_ms);


int rdma_resolve_route(struct rdma_cm_id *id, unsigned long timeout_ms);


int rdma_create_qp(struct rdma_cm_id *id, struct ib_pd *pd,
		   struct ib_qp_init_attr *qp_init_attr);


void rdma_destroy_qp(struct rdma_cm_id *id);


int rdma_init_qp_attr(struct rdma_cm_id *id, struct ib_qp_attr *qp_attr,
		       int *qp_attr_mask);

int rdma_connect(struct rdma_cm_id *id, struct rdma_conn_param *conn_param);
int rdma_connect_locked(struct rdma_cm_id *id,
			struct rdma_conn_param *conn_param);

int rdma_connect_ece(struct rdma_cm_id *id, struct rdma_conn_param *conn_param,
		     struct rdma_ucm_ece *ece);


int rdma_listen(struct rdma_cm_id *id, int backlog);

int rdma_accept(struct rdma_cm_id *id, struct rdma_conn_param *conn_param);

void rdma_lock_handler(struct rdma_cm_id *id);
void rdma_unlock_handler(struct rdma_cm_id *id);
int rdma_accept_ece(struct rdma_cm_id *id, struct rdma_conn_param *conn_param,
		    struct rdma_ucm_ece *ece);


int rdma_notify(struct rdma_cm_id *id, enum ib_event_type event);


int rdma_reject(struct rdma_cm_id *id, const void *private_data,
		u8 private_data_len, u8 reason);


int rdma_disconnect(struct rdma_cm_id *id);


int rdma_join_multicast(struct rdma_cm_id *id, struct sockaddr *addr,
			u8 join_state, void *context);


void rdma_leave_multicast(struct rdma_cm_id *id, struct sockaddr *addr);


void rdma_set_service_type(struct rdma_cm_id *id, int tos);


int rdma_set_reuseaddr(struct rdma_cm_id *id, int reuse);


int rdma_set_afonly(struct rdma_cm_id *id, int afonly);

int rdma_set_ack_timeout(struct rdma_cm_id *id, u8 timeout);

int rdma_set_min_rnr_timer(struct rdma_cm_id *id, u8 min_rnr_timer);
 
__be64 rdma_get_service_id(struct rdma_cm_id *id, struct sockaddr *addr);


const char *__attribute_const__ rdma_reject_msg(struct rdma_cm_id *id,
						int reason);

const void *rdma_consumer_reject_data(struct rdma_cm_id *id,
				      struct rdma_cm_event *ev, u8 *data_len);



void rdma_read_gids(struct rdma_cm_id *cm_id, union ib_gid *sgid,
		    union ib_gid *dgid);

struct iw_cm_id *rdma_iw_cm_id(struct rdma_cm_id *cm_id);
struct rdma_cm_id *rdma_res_to_id(struct rdma_restrack_entry *res);

#endif 
