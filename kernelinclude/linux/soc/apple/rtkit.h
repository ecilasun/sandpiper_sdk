/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef _LINUX_APPLE_RTKIT_H_
#define _LINUX_APPLE_RTKIT_H_

#include <linux/device.h>
#include <linux/types.h>
#include <linux/mailbox_client.h>



struct apple_rtkit_shmem {
	void *buffer;
	void __iomem *iomem;
	size_t size;
	dma_addr_t iova;
	bool is_mapped;
	void *private;
};


struct apple_rtkit_ops {
	void (*crashed)(void *cookie);
	void (*recv_message)(void *cookie, u8 endpoint, u64 message);
	bool (*recv_message_early)(void *cookie, u8 endpoint, u64 message);
	int (*shmem_setup)(void *cookie, struct apple_rtkit_shmem *bfr);
	void (*shmem_destroy)(void *cookie, struct apple_rtkit_shmem *bfr);
};

struct apple_rtkit;


struct apple_rtkit *devm_apple_rtkit_init(struct device *dev, void *cookie,
					  const char *mbox_name, int mbox_idx,
					  const struct apple_rtkit_ops *ops);


struct apple_rtkit *apple_rtkit_init(struct device *dev, void *cookie,
					  const char *mbox_name, int mbox_idx,
					  const struct apple_rtkit_ops *ops);


void apple_rtkit_free(struct apple_rtkit *rtk);


int apple_rtkit_reinit(struct apple_rtkit *rtk);


int apple_rtkit_boot(struct apple_rtkit *rtk);


int apple_rtkit_quiesce(struct apple_rtkit *rtk);


int apple_rtkit_wake(struct apple_rtkit *rtk);


int apple_rtkit_shutdown(struct apple_rtkit *rtk);


int apple_rtkit_idle(struct apple_rtkit *rtk);


bool apple_rtkit_is_running(struct apple_rtkit *rtk);


bool apple_rtkit_is_crashed(struct apple_rtkit *rtk);


int apple_rtkit_start_ep(struct apple_rtkit *rtk, u8 endpoint);


int apple_rtkit_send_message(struct apple_rtkit *rtk, u8 ep, u64 message,
			     struct completion *completion, bool atomic);


int apple_rtkit_send_message_wait(struct apple_rtkit *rtk, u8 ep, u64 message,
				  unsigned long timeout, bool atomic);


int apple_rtkit_poll(struct apple_rtkit *rtk);

#endif 
