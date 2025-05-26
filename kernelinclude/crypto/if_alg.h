/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _CRYPTO_IF_ALG_H
#define _CRYPTO_IF_ALG_H

#include <linux/compiler.h>
#include <linux/completion.h>
#include <linux/if_alg.h>
#include <linux/scatterlist.h>
#include <linux/types.h>
#include <linux/atomic.h>
#include <net/sock.h>

#include <crypto/aead.h>
#include <crypto/skcipher.h>

#define ALG_MAX_PAGES			16

struct alg_sock {
	
	struct sock sk;

	struct sock *parent;

	atomic_t refcnt;
	atomic_t nokey_refcnt;

	const struct af_alg_type *type;
	void *private;
};

struct af_alg_control {
	struct af_alg_iv *iv;
	int op;
	unsigned int aead_assoclen;
};

struct af_alg_type {
	void *(*bind)(const char *name, u32 type, u32 mask);
	void (*release)(void *private);
	int (*setkey)(void *private, const u8 *key, unsigned int keylen);
	int (*setentropy)(void *private, sockptr_t entropy, unsigned int len);
	int (*accept)(void *private, struct sock *sk);
	int (*accept_nokey)(void *private, struct sock *sk);
	int (*setauthsize)(void *private, unsigned int authsize);

	struct proto_ops *ops;
	struct proto_ops *ops_nokey;
	struct module *owner;
	char name[14];
};

struct af_alg_sgl {
	struct sg_table sgt;
	struct scatterlist sgl[ALG_MAX_PAGES + 1];
	bool need_unpin;
};


struct af_alg_tsgl {
	struct list_head list;
	unsigned int cur;		
	struct scatterlist sg[];	
};

#define MAX_SGL_ENTS ((4096 - sizeof(struct af_alg_tsgl)) / \
		      sizeof(struct scatterlist) - 1)


struct af_alg_rsgl {
	struct af_alg_sgl sgl;
	struct list_head list;
	size_t sg_num_bytes;		
};


struct af_alg_async_req {
	struct kiocb *iocb;
	struct sock *sk;

	struct af_alg_rsgl first_rsgl;
	struct af_alg_rsgl *last_rsgl;
	struct list_head rsgl_list;

	struct scatterlist *tsgl;
	unsigned int tsgl_entries;

	unsigned int outlen;
	unsigned int areqlen;

	union {
		struct aead_request aead_req;
		struct skcipher_request skcipher_req;
	} cra_u;

	
};


struct af_alg_ctx {
	struct list_head tsgl_list;

	void *iv;
	size_t aead_assoclen;

	struct crypto_wait wait;

	size_t used;
	atomic_t rcvused;

	bool more;
	bool merge;
	bool enc;
	bool init;

	unsigned int len;

	unsigned int inflight;
};

int af_alg_register_type(const struct af_alg_type *type);
int af_alg_unregister_type(const struct af_alg_type *type);

int af_alg_release(struct socket *sock);
void af_alg_release_parent(struct sock *sk);
int af_alg_accept(struct sock *sk, struct socket *newsock, bool kern);

void af_alg_free_sg(struct af_alg_sgl *sgl);

static inline struct alg_sock *alg_sk(struct sock *sk)
{
	return (struct alg_sock *)sk;
}


static inline int af_alg_sndbuf(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;

	return max_t(int, max_t(int, sk->sk_sndbuf & PAGE_MASK, PAGE_SIZE) -
			  ctx->used, 0);
}


static inline bool af_alg_writable(struct sock *sk)
{
	return PAGE_SIZE <= af_alg_sndbuf(sk);
}


static inline int af_alg_rcvbuf(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;

	return max_t(int, max_t(int, sk->sk_rcvbuf & PAGE_MASK, PAGE_SIZE) -
		     atomic_read(&ctx->rcvused), 0);
}


static inline bool af_alg_readable(struct sock *sk)
{
	return PAGE_SIZE <= af_alg_rcvbuf(sk);
}

unsigned int af_alg_count_tsgl(struct sock *sk, size_t bytes, size_t offset);
void af_alg_pull_tsgl(struct sock *sk, size_t used, struct scatterlist *dst,
		      size_t dst_offset);
void af_alg_wmem_wakeup(struct sock *sk);
int af_alg_wait_for_data(struct sock *sk, unsigned flags, unsigned min);
int af_alg_sendmsg(struct socket *sock, struct msghdr *msg, size_t size,
		   unsigned int ivsize);
void af_alg_free_resources(struct af_alg_async_req *areq);
void af_alg_async_cb(void *data, int err);
__poll_t af_alg_poll(struct file *file, struct socket *sock,
			 poll_table *wait);
struct af_alg_async_req *af_alg_alloc_areq(struct sock *sk,
					   unsigned int areqlen);
int af_alg_get_rsgl(struct sock *sk, struct msghdr *msg, int flags,
		    struct af_alg_async_req *areq, size_t maxsize,
		    size_t *outlen);

#endif	
