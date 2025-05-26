// SPDX-License-Identifier: GPL-2.0

#ifndef __QMI_HELPERS_H__
#define __QMI_HELPERS_H__

#include <linux/completion.h>
#include <linux/idr.h>
#include <linux/list.h>
#include <linux/qrtr.h>
#include <linux/types.h>
#include <linux/workqueue.h>

struct socket;


struct qmi_header {
	u8 type;
	u16 txn_id;
	u16 msg_id;
	u16 msg_len;
} __packed;

#define QMI_REQUEST	0
#define QMI_RESPONSE	2
#define QMI_INDICATION	4

#define QMI_COMMON_TLV_TYPE 0

enum qmi_elem_type {
	QMI_EOTI,
	QMI_OPT_FLAG,
	QMI_DATA_LEN,
	QMI_UNSIGNED_1_BYTE,
	QMI_UNSIGNED_2_BYTE,
	QMI_UNSIGNED_4_BYTE,
	QMI_UNSIGNED_8_BYTE,
	QMI_SIGNED_2_BYTE_ENUM,
	QMI_SIGNED_4_BYTE_ENUM,
	QMI_STRUCT,
	QMI_STRING,
};

enum qmi_array_type {
	NO_ARRAY,
	STATIC_ARRAY,
	VAR_LEN_ARRAY,
};


struct qmi_elem_info {
	enum qmi_elem_type data_type;
	u32 elem_len;
	u32 elem_size;
	enum qmi_array_type array_type;
	u8 tlv_type;
	u32 offset;
	const struct qmi_elem_info *ei_array;
};

#define QMI_RESULT_SUCCESS_V01			0
#define QMI_RESULT_FAILURE_V01			1

#define QMI_ERR_NONE_V01			0
#define QMI_ERR_MALFORMED_MSG_V01		1
#define QMI_ERR_NO_MEMORY_V01			2
#define QMI_ERR_INTERNAL_V01			3
#define QMI_ERR_CLIENT_IDS_EXHAUSTED_V01	5
#define QMI_ERR_INVALID_ID_V01			41
#define QMI_ERR_ENCODING_V01			58
#define QMI_ERR_DISABLED_V01                    69
#define QMI_ERR_INCOMPATIBLE_STATE_V01		90
#define QMI_ERR_NOT_SUPPORTED_V01		94


struct qmi_response_type_v01 {
	u16 result;
	u16 error;
};

extern const struct qmi_elem_info qmi_response_type_v01_ei[];


struct qmi_service {
	unsigned int service;
	unsigned int version;
	unsigned int instance;

	unsigned int node;
	unsigned int port;

	void *priv;
	struct list_head list_node;
};

struct qmi_handle;


struct qmi_ops {
	int (*new_server)(struct qmi_handle *qmi, struct qmi_service *svc);
	void (*del_server)(struct qmi_handle *qmi, struct qmi_service *svc);
	void (*net_reset)(struct qmi_handle *qmi);
	void (*msg_handler)(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
			    const void *data, size_t count);
	void (*bye)(struct qmi_handle *qmi, unsigned int node);
	void (*del_client)(struct qmi_handle *qmi,
			   unsigned int node, unsigned int port);
};


struct qmi_txn {
	struct qmi_handle *qmi;

	u16 id;

	struct mutex lock;
	struct completion completion;
	int result;

	const struct qmi_elem_info *ei;
	void *dest;
};


struct qmi_msg_handler {
	unsigned int type;
	unsigned int msg_id;

	const struct qmi_elem_info *ei;

	size_t decoded_size;
	void (*fn)(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
		   struct qmi_txn *txn, const void *decoded);
};


struct qmi_handle {
	struct socket *sock;
	struct mutex sock_lock;

	struct sockaddr_qrtr sq;

	struct work_struct work;
	struct workqueue_struct *wq;

	void *recv_buf;
	size_t recv_buf_size;

	struct list_head lookups;
	struct list_head lookup_results;
	struct list_head services;

	struct qmi_ops ops;

	struct idr txns;
	struct mutex txn_lock;

	const struct qmi_msg_handler *handlers;
};

int qmi_add_lookup(struct qmi_handle *qmi, unsigned int service,
		   unsigned int version, unsigned int instance);
int qmi_add_server(struct qmi_handle *qmi, unsigned int service,
		   unsigned int version, unsigned int instance);

int qmi_handle_init(struct qmi_handle *qmi, size_t max_msg_len,
		    const struct qmi_ops *ops,
		    const struct qmi_msg_handler *handlers);
void qmi_handle_release(struct qmi_handle *qmi);

ssize_t qmi_send_request(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
			 struct qmi_txn *txn, int msg_id, size_t len,
			 const struct qmi_elem_info *ei, const void *c_struct);
ssize_t qmi_send_response(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
			  struct qmi_txn *txn, int msg_id, size_t len,
			  const struct qmi_elem_info *ei, const void *c_struct);
ssize_t qmi_send_indication(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
			    int msg_id, size_t len, const struct qmi_elem_info *ei,
			    const void *c_struct);

void *qmi_encode_message(int type, unsigned int msg_id, size_t *len,
			 unsigned int txn_id, const struct qmi_elem_info *ei,
			 const void *c_struct);

int qmi_decode_message(const void *buf, size_t len,
		       const struct qmi_elem_info *ei, void *c_struct);

int qmi_txn_init(struct qmi_handle *qmi, struct qmi_txn *txn,
		 const struct qmi_elem_info *ei, void *c_struct);
int qmi_txn_wait(struct qmi_txn *txn, unsigned long timeout);
void qmi_txn_cancel(struct qmi_txn *txn);

#endif
