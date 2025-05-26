/* SPDX-License-Identifier: (GPL-2.0+ OR BSD-3-Clause) */

#ifndef __FSL_DPAA2_IO_H
#define __FSL_DPAA2_IO_H

#include <linux/types.h>
#include <linux/cpumask.h>
#include <linux/irqreturn.h>

#include "dpaa2-fd.h"
#include "dpaa2-global.h"

struct dpaa2_io;
struct dpaa2_io_store;
struct device;



#define DPAA2_IO_ANY_CPU	-1


struct dpaa2_io_desc {
	int receives_notifications;
	int has_8prio;
	int cpu;
	void *regs_cena;
	void __iomem *regs_cinh;
	int dpio_id;
	u32 qman_version;
	u32 qman_clk;
};

struct dpaa2_io *dpaa2_io_create(const struct dpaa2_io_desc *desc,
				 struct device *dev);

void dpaa2_io_down(struct dpaa2_io *d);

irqreturn_t dpaa2_io_irq(struct dpaa2_io *obj);

struct dpaa2_io *dpaa2_io_service_select(int cpu);


struct dpaa2_io_notification_ctx {
	void (*cb)(struct dpaa2_io_notification_ctx *ctx);
	int is_cdan;
	u32 id;
	int desired_cpu;
	int dpio_id;
	u64 qman64;
	struct list_head node;
	void *dpio_private;
};

int dpaa2_io_get_cpu(struct dpaa2_io *d);

int dpaa2_io_service_register(struct dpaa2_io *service,
			      struct dpaa2_io_notification_ctx *ctx,
			      struct device *dev);
void dpaa2_io_service_deregister(struct dpaa2_io *service,
				 struct dpaa2_io_notification_ctx *ctx,
				 struct device *dev);
int dpaa2_io_service_rearm(struct dpaa2_io *service,
			   struct dpaa2_io_notification_ctx *ctx);

int dpaa2_io_service_pull_fq(struct dpaa2_io *d, u32 fqid,
			     struct dpaa2_io_store *s);
int dpaa2_io_service_pull_channel(struct dpaa2_io *d, u32 channelid,
				  struct dpaa2_io_store *s);

int dpaa2_io_service_enqueue_fq(struct dpaa2_io *d, u32 fqid,
				const struct dpaa2_fd *fd);
int dpaa2_io_service_enqueue_multiple_fq(struct dpaa2_io *d, u32 fqid,
				const struct dpaa2_fd *fd, int number_of_frame);
int dpaa2_io_service_enqueue_multiple_desc_fq(struct dpaa2_io *d, u32 *fqid,
				const struct dpaa2_fd *fd, int number_of_frame);
int dpaa2_io_service_enqueue_qd(struct dpaa2_io *d, u32 qdid, u8 prio,
				u16 qdbin, const struct dpaa2_fd *fd);
int dpaa2_io_service_release(struct dpaa2_io *d, u16 bpid,
			     const u64 *buffers, unsigned int num_buffers);
int dpaa2_io_service_acquire(struct dpaa2_io *d, u16 bpid,
			     u64 *buffers, unsigned int num_buffers);

struct dpaa2_io_store *dpaa2_io_store_create(unsigned int max_frames,
					     struct device *dev);
void dpaa2_io_store_destroy(struct dpaa2_io_store *s);
struct dpaa2_dq *dpaa2_io_store_next(struct dpaa2_io_store *s, int *is_last);

int dpaa2_io_query_fq_count(struct dpaa2_io *d, u32 fqid,
			    u32 *fcnt, u32 *bcnt);
int dpaa2_io_query_bp_count(struct dpaa2_io *d, u16 bpid,
			    u32 *num);

int dpaa2_io_set_irq_coalescing(struct dpaa2_io *d, u32 irq_holdoff);
void dpaa2_io_get_irq_coalescing(struct dpaa2_io *d, u32 *irq_holdoff);
void dpaa2_io_set_adaptive_coalescing(struct dpaa2_io *d,
				      int use_adaptive_rx_coalesce);
int dpaa2_io_get_adaptive_coalescing(struct dpaa2_io *d);
void dpaa2_io_update_net_dim(struct dpaa2_io *d, __u64 frames, __u64 bytes);
#endif 
