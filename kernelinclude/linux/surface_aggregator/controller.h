/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SURFACE_AGGREGATOR_CONTROLLER_H
#define _LINUX_SURFACE_AGGREGATOR_CONTROLLER_H

#include <linux/completion.h>
#include <linux/device.h>
#include <linux/types.h>

#include <linux/surface_aggregator/serial_hub.h>





enum ssam_event_flags {
	SSAM_EVENT_SEQUENCED = BIT(0),
};


struct ssam_event {
	u8 target_category;
	u8 target_id;
	u8 command_id;
	u8 instance_id;
	u16 length;
	u8 data[];
};


enum ssam_request_flags {
	SSAM_REQUEST_HAS_RESPONSE = BIT(0),
	SSAM_REQUEST_UNSEQUENCED  = BIT(1),
};


struct ssam_request {
	u8 target_category;
	u8 target_id;
	u8 command_id;
	u8 instance_id;
	u16 flags;
	u16 length;
	const u8 *payload;
};


struct ssam_response {
	size_t capacity;
	size_t length;
	u8 *pointer;
};

struct ssam_controller;

struct ssam_controller *ssam_get_controller(void);
struct ssam_controller *ssam_client_bind(struct device *client);
int ssam_client_link(struct ssam_controller *ctrl, struct device *client);

struct device *ssam_controller_device(struct ssam_controller *c);

struct ssam_controller *ssam_controller_get(struct ssam_controller *c);
void ssam_controller_put(struct ssam_controller *c);

void ssam_controller_statelock(struct ssam_controller *c);
void ssam_controller_stateunlock(struct ssam_controller *c);

ssize_t ssam_request_write_data(struct ssam_span *buf,
				struct ssam_controller *ctrl,
				const struct ssam_request *spec);





struct ssam_request_sync {
	struct ssh_request base;
	struct completion comp;
	struct ssam_response *resp;
	int status;
};

int ssam_request_sync_alloc(size_t payload_len, gfp_t flags,
			    struct ssam_request_sync **rqst,
			    struct ssam_span *buffer);

void ssam_request_sync_free(struct ssam_request_sync *rqst);

int ssam_request_sync_init(struct ssam_request_sync *rqst,
			   enum ssam_request_flags flags);


static inline void ssam_request_sync_set_data(struct ssam_request_sync *rqst,
					      u8 *ptr, size_t len)
{
	ssh_request_set_data(&rqst->base, ptr, len);
}


static inline void ssam_request_sync_set_resp(struct ssam_request_sync *rqst,
					      struct ssam_response *resp)
{
	rqst->resp = resp;
}

int ssam_request_sync_submit(struct ssam_controller *ctrl,
			     struct ssam_request_sync *rqst);


static inline int ssam_request_sync_wait(struct ssam_request_sync *rqst)
{
	wait_for_completion(&rqst->comp);
	return rqst->status;
}

int ssam_request_do_sync(struct ssam_controller *ctrl,
			 const struct ssam_request *spec,
			 struct ssam_response *rsp);

int ssam_request_do_sync_with_buffer(struct ssam_controller *ctrl,
				     const struct ssam_request *spec,
				     struct ssam_response *rsp,
				     struct ssam_span *buf);


#define ssam_request_do_sync_onstack(ctrl, rqst, rsp, payload_len)		\
	({									\
		u8 __data[SSH_COMMAND_MESSAGE_LENGTH(payload_len)];		\
		struct ssam_span __buf = { &__data[0], ARRAY_SIZE(__data) };	\
										\
		ssam_request_do_sync_with_buffer(ctrl, rqst, rsp, &__buf);	\
	})


#define __ssam_retry(request, n, args...)				\
	({								\
		int __i, __s = 0;					\
									\
		for (__i = (n); __i > 0; __i--) {			\
			__s = request(args);				\
			if (__s != -ETIMEDOUT && __s != -EREMOTEIO)	\
				break;					\
		}							\
		__s;							\
	})


#define ssam_retry(request, args...) \
	__ssam_retry(request, 3, args)


struct ssam_request_spec {
	u8 target_category;
	u8 target_id;
	u8 command_id;
	u8 instance_id;
	u8 flags;
};


struct ssam_request_spec_md {
	u8 target_category;
	u8 command_id;
	u8 flags;
};


#define SSAM_DEFINE_SYNC_REQUEST_N(name, spec...)				\
	static int name(struct ssam_controller *ctrl)				\
	{									\
		struct ssam_request_spec s = (struct ssam_request_spec)spec;	\
		struct ssam_request rqst;					\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = s.target_id;					\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = s.instance_id;				\
		rqst.flags = s.flags;						\
		rqst.length = 0;						\
		rqst.payload = NULL;						\
										\
		return ssam_request_do_sync_onstack(ctrl, &rqst, NULL, 0);	\
	}


#define SSAM_DEFINE_SYNC_REQUEST_W(name, atype, spec...)			\
	static int name(struct ssam_controller *ctrl, const atype *arg)		\
	{									\
		struct ssam_request_spec s = (struct ssam_request_spec)spec;	\
		struct ssam_request rqst;					\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = s.target_id;					\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = s.instance_id;				\
		rqst.flags = s.flags;						\
		rqst.length = sizeof(atype);					\
		rqst.payload = (u8 *)arg;					\
										\
		return ssam_request_do_sync_onstack(ctrl, &rqst, NULL,		\
						    sizeof(atype));		\
	}


#define SSAM_DEFINE_SYNC_REQUEST_R(name, rtype, spec...)			\
	static int name(struct ssam_controller *ctrl, rtype *ret)		\
	{									\
		struct ssam_request_spec s = (struct ssam_request_spec)spec;	\
		struct ssam_request rqst;					\
		struct ssam_response rsp;					\
		int status;							\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = s.target_id;					\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = s.instance_id;				\
		rqst.flags = s.flags | SSAM_REQUEST_HAS_RESPONSE;		\
		rqst.length = 0;						\
		rqst.payload = NULL;						\
										\
		rsp.capacity = sizeof(rtype);					\
		rsp.length = 0;							\
		rsp.pointer = (u8 *)ret;					\
										\
		status = ssam_request_do_sync_onstack(ctrl, &rqst, &rsp, 0);	\
		if (status)							\
			return status;						\
										\
		if (rsp.length != sizeof(rtype)) {				\
			struct device *dev = ssam_controller_device(ctrl);	\
			dev_err(dev,						\
				"rqst: invalid response length, expected %zu, got %zu (tc: %#04x, cid: %#04x)", \
				sizeof(rtype), rsp.length, rqst.target_category,\
				rqst.command_id);				\
			return -EIO;						\
		}								\
										\
		return 0;							\
	}


#define SSAM_DEFINE_SYNC_REQUEST_WR(name, atype, rtype, spec...)		\
	static int name(struct ssam_controller *ctrl, const atype *arg, rtype *ret) \
	{									\
		struct ssam_request_spec s = (struct ssam_request_spec)spec;	\
		struct ssam_request rqst;					\
		struct ssam_response rsp;					\
		int status;							\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = s.target_id;					\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = s.instance_id;				\
		rqst.flags = s.flags | SSAM_REQUEST_HAS_RESPONSE;		\
		rqst.length = sizeof(atype);					\
		rqst.payload = (u8 *)arg;					\
										\
		rsp.capacity = sizeof(rtype);					\
		rsp.length = 0;							\
		rsp.pointer = (u8 *)ret;					\
										\
		status = ssam_request_do_sync_onstack(ctrl, &rqst, &rsp, sizeof(atype)); \
		if (status)							\
			return status;						\
										\
		if (rsp.length != sizeof(rtype)) {				\
			struct device *dev = ssam_controller_device(ctrl);	\
			dev_err(dev,						\
				"rqst: invalid response length, expected %zu, got %zu (tc: %#04x, cid: %#04x)", \
				sizeof(rtype), rsp.length, rqst.target_category,\
				rqst.command_id);				\
			return -EIO;						\
		}								\
										\
		return 0;							\
	}


#define SSAM_DEFINE_SYNC_REQUEST_MD_N(name, spec...)				\
	static int name(struct ssam_controller *ctrl, u8 tid, u8 iid)		\
	{									\
		struct ssam_request_spec_md s = (struct ssam_request_spec_md)spec; \
		struct ssam_request rqst;					\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = tid;						\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = iid;						\
		rqst.flags = s.flags;						\
		rqst.length = 0;						\
		rqst.payload = NULL;						\
										\
		return ssam_request_do_sync_onstack(ctrl, &rqst, NULL, 0);	\
	}


#define SSAM_DEFINE_SYNC_REQUEST_MD_W(name, atype, spec...)			\
	static int name(struct ssam_controller *ctrl, u8 tid, u8 iid, const atype *arg) \
	{									\
		struct ssam_request_spec_md s = (struct ssam_request_spec_md)spec; \
		struct ssam_request rqst;					\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = tid;						\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = iid;						\
		rqst.flags = s.flags;						\
		rqst.length = sizeof(atype);					\
		rqst.payload = (u8 *)arg;					\
										\
		return ssam_request_do_sync_onstack(ctrl, &rqst, NULL,		\
						 sizeof(atype));		\
	}


#define SSAM_DEFINE_SYNC_REQUEST_MD_R(name, rtype, spec...)			\
	static int name(struct ssam_controller *ctrl, u8 tid, u8 iid, rtype *ret) \
	{									\
		struct ssam_request_spec_md s = (struct ssam_request_spec_md)spec; \
		struct ssam_request rqst;					\
		struct ssam_response rsp;					\
		int status;							\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = tid;						\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = iid;						\
		rqst.flags = s.flags | SSAM_REQUEST_HAS_RESPONSE;		\
		rqst.length = 0;						\
		rqst.payload = NULL;						\
										\
		rsp.capacity = sizeof(rtype);					\
		rsp.length = 0;							\
		rsp.pointer = (u8 *)ret;					\
										\
		status = ssam_request_do_sync_onstack(ctrl, &rqst, &rsp, 0);	\
		if (status)							\
			return status;						\
										\
		if (rsp.length != sizeof(rtype)) {				\
			struct device *dev = ssam_controller_device(ctrl);	\
			dev_err(dev,						\
				"rqst: invalid response length, expected %zu, got %zu (tc: %#04x, cid: %#04x)", \
				sizeof(rtype), rsp.length, rqst.target_category,\
				rqst.command_id);				\
			return -EIO;						\
		}								\
										\
		return 0;							\
	}


#define SSAM_DEFINE_SYNC_REQUEST_MD_WR(name, atype, rtype, spec...)		\
	static int name(struct ssam_controller *ctrl, u8 tid, u8 iid,		\
			const atype *arg, rtype *ret)				\
	{									\
		struct ssam_request_spec_md s = (struct ssam_request_spec_md)spec; \
		struct ssam_request rqst;					\
		struct ssam_response rsp;					\
		int status;							\
										\
		rqst.target_category = s.target_category;			\
		rqst.target_id = tid;						\
		rqst.command_id = s.command_id;					\
		rqst.instance_id = iid;						\
		rqst.flags = s.flags | SSAM_REQUEST_HAS_RESPONSE;		\
		rqst.length = sizeof(atype);					\
		rqst.payload = (u8 *)arg;					\
										\
		rsp.capacity = sizeof(rtype);					\
		rsp.length = 0;							\
		rsp.pointer = (u8 *)ret;					\
										\
		status = ssam_request_do_sync_onstack(ctrl, &rqst, &rsp, sizeof(atype)); \
		if (status)							\
			return status;						\
										\
		if (rsp.length != sizeof(rtype)) {				\
			struct device *dev = ssam_controller_device(ctrl);	\
			dev_err(dev,						\
				"rqst: invalid response length, expected %zu, got %zu (tc: %#04x, cid: %#04x)", \
				sizeof(rtype), rsp.length, rqst.target_category,\
				rqst.command_id);				\
			return -EIO;						\
		}								\
										\
		return 0;							\
	}




#define SSAM_NOTIF_STATE_SHIFT		2
#define SSAM_NOTIF_STATE_MASK		((1 << SSAM_NOTIF_STATE_SHIFT) - 1)


enum ssam_notif_flags {
	SSAM_NOTIF_HANDLED = BIT(0),
	SSAM_NOTIF_STOP    = BIT(1),
};

struct ssam_event_notifier;

typedef u32 (*ssam_notifier_fn_t)(struct ssam_event_notifier *nf,
				  const struct ssam_event *event);


struct ssam_notifier_block {
	struct list_head node;
	ssam_notifier_fn_t fn;
	int priority;
};


static inline u32 ssam_notifier_from_errno(int err)
{
	if (WARN_ON(err > 0) || err == 0)
		return 0;
	else
		return ((-err) << SSAM_NOTIF_STATE_SHIFT) | SSAM_NOTIF_STOP;
}


static inline int ssam_notifier_to_errno(u32 ret)
{
	return -(ret >> SSAM_NOTIF_STATE_SHIFT);
}





struct ssam_event_registry {
	u8 target_category;
	u8 target_id;
	u8 cid_enable;
	u8 cid_disable;
};


struct ssam_event_id {
	u8 target_category;
	u8 instance;
};


enum ssam_event_mask {
	SSAM_EVENT_MASK_TARGET   = BIT(0),
	SSAM_EVENT_MASK_INSTANCE = BIT(1),

	SSAM_EVENT_MASK_NONE = 0,
	SSAM_EVENT_MASK_STRICT =
		  SSAM_EVENT_MASK_TARGET
		| SSAM_EVENT_MASK_INSTANCE,
};


#define SSAM_EVENT_REGISTRY(tc, tid, cid_en, cid_dis)	\
	((struct ssam_event_registry) {			\
		.target_category = (tc),		\
		.target_id = (tid),			\
		.cid_enable = (cid_en),			\
		.cid_disable = (cid_dis),		\
	})

#define SSAM_EVENT_REGISTRY_SAM	\
	SSAM_EVENT_REGISTRY(SSAM_SSH_TC_SAM, SSAM_SSH_TID_SAM, 0x0b, 0x0c)

#define SSAM_EVENT_REGISTRY_KIP	\
	SSAM_EVENT_REGISTRY(SSAM_SSH_TC_KIP, SSAM_SSH_TID_KIP, 0x27, 0x28)

#define SSAM_EVENT_REGISTRY_REG(tid)\
	SSAM_EVENT_REGISTRY(SSAM_SSH_TC_REG, tid, 0x01, 0x02)


enum ssam_event_notifier_flags {
	SSAM_EVENT_NOTIFIER_OBSERVER = BIT(0),
};


struct ssam_event_notifier {
	struct ssam_notifier_block base;

	struct {
		struct ssam_event_registry reg;
		struct ssam_event_id id;
		enum ssam_event_mask mask;
		u8 flags;
	} event;

	unsigned long flags;
};

int ssam_notifier_register(struct ssam_controller *ctrl,
			   struct ssam_event_notifier *n);

int __ssam_notifier_unregister(struct ssam_controller *ctrl,
			       struct ssam_event_notifier *n, bool disable);


static inline int ssam_notifier_unregister(struct ssam_controller *ctrl,
					   struct ssam_event_notifier *n)
{
	return __ssam_notifier_unregister(ctrl, n, true);
}

int ssam_controller_event_enable(struct ssam_controller *ctrl,
				 struct ssam_event_registry reg,
				 struct ssam_event_id id, u8 flags);

int ssam_controller_event_disable(struct ssam_controller *ctrl,
				  struct ssam_event_registry reg,
				  struct ssam_event_id id, u8 flags);

#endif 
