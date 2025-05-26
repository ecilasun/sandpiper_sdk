/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _CRYPTO_INTERNAL_ENGINE_H
#define _CRYPTO_INTERNAL_ENGINE_H

#include <crypto/algapi.h>
#include <crypto/engine.h>
#include <linux/kthread.h>
#include <linux/spinlock_types.h>
#include <linux/types.h>

#define ENGINE_NAME_LEN	30

struct device;


struct crypto_engine {
	char			name[ENGINE_NAME_LEN];
	bool			idling;
	bool			busy;
	bool			running;

	bool			retry_support;

	struct list_head	list;
	spinlock_t		queue_lock;
	struct crypto_queue	queue;
	struct device		*dev;

	bool			rt;

	int (*prepare_crypt_hardware)(struct crypto_engine *engine);
	int (*unprepare_crypt_hardware)(struct crypto_engine *engine);
	int (*do_batch_requests)(struct crypto_engine *engine);


	struct kthread_worker           *kworker;
	struct kthread_work             pump_requests;

	void				*priv_data;
	struct crypto_async_request	*cur_req;
};

#endif
