/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef NET_9P_TRANSPORT_H
#define NET_9P_TRANSPORT_H

#include <linux/module.h>

#define P9_DEF_MIN_RESVPORT	(665U)
#define P9_DEF_MAX_RESVPORT	(1023U)



struct p9_trans_module {
	struct list_head list;
	char *name;		
	int maxsize;		
	bool pooled_rbuffers;
	int def;		
	struct module *owner;
	int (*create)(struct p9_client *client,
		      const char *devname, char *args);
	void (*close)(struct p9_client *client);
	int (*request)(struct p9_client *client, struct p9_req_t *req);
	int (*cancel)(struct p9_client *client, struct p9_req_t *req);
	int (*cancelled)(struct p9_client *client, struct p9_req_t *req);
	int (*zc_request)(struct p9_client *client, struct p9_req_t *req,
			  struct iov_iter *uidata, struct iov_iter *uodata,
			  int inlen, int outlen, int in_hdr_len);
	int (*show_options)(struct seq_file *m, struct p9_client *client);
};

void v9fs_register_trans(struct p9_trans_module *m);
void v9fs_unregister_trans(struct p9_trans_module *m);
struct p9_trans_module *v9fs_get_trans_by_name(const char *s);
struct p9_trans_module *v9fs_get_default_trans(void);
void v9fs_put_trans(struct p9_trans_module *m);

#define MODULE_ALIAS_9P(transport) \
	MODULE_ALIAS("9p-" transport)

#endif 
