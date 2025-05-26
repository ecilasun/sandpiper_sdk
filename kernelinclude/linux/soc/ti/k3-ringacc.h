/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __SOC_TI_K3_RINGACC_API_H_
#define __SOC_TI_K3_RINGACC_API_H_

#include <linux/types.h>

struct device_node;


enum k3_ring_mode {
	K3_RINGACC_RING_MODE_RING = 0,
	K3_RINGACC_RING_MODE_MESSAGE,
	K3_RINGACC_RING_MODE_CREDENTIALS,
	K3_RINGACC_RING_MODE_INVALID
};


enum k3_ring_size {
	K3_RINGACC_RING_ELSIZE_4 = 0,
	K3_RINGACC_RING_ELSIZE_8,
	K3_RINGACC_RING_ELSIZE_16,
	K3_RINGACC_RING_ELSIZE_32,
	K3_RINGACC_RING_ELSIZE_64,
	K3_RINGACC_RING_ELSIZE_128,
	K3_RINGACC_RING_ELSIZE_256,
	K3_RINGACC_RING_ELSIZE_INVALID
};

struct k3_ringacc;
struct k3_ring;


struct k3_ring_cfg {
	u32 size;
	enum k3_ring_size elm_size;
	enum k3_ring_mode mode;
#define K3_RINGACC_RING_SHARED BIT(1)
	u32 flags;

	struct device *dma_dev;
	u32 asel;
};

#define K3_RINGACC_RING_ID_ANY (-1)


struct k3_ringacc *of_k3_ringacc_get_by_phandle(struct device_node *np,
						const char *property);

#define K3_RINGACC_RING_USE_PROXY BIT(1)


struct k3_ring *k3_ringacc_request_ring(struct k3_ringacc *ringacc,
					int id, u32 flags);

int k3_ringacc_request_rings_pair(struct k3_ringacc *ringacc,
				  int fwd_id, int compl_id,
				  struct k3_ring **fwd_ring,
				  struct k3_ring **compl_ring);

void k3_ringacc_ring_reset(struct k3_ring *ring);

void k3_ringacc_ring_reset_dma(struct k3_ring *ring, u32 occ);


int k3_ringacc_ring_free(struct k3_ring *ring);


u32 k3_ringacc_get_ring_id(struct k3_ring *ring);


int k3_ringacc_get_ring_irq_num(struct k3_ring *ring);


int k3_ringacc_ring_cfg(struct k3_ring *ring, struct k3_ring_cfg *cfg);


u32 k3_ringacc_ring_get_size(struct k3_ring *ring);


u32 k3_ringacc_ring_get_free(struct k3_ring *ring);


u32 k3_ringacc_ring_get_occ(struct k3_ring *ring);


u32 k3_ringacc_ring_is_full(struct k3_ring *ring);


int k3_ringacc_ring_push(struct k3_ring *ring, void *elem);


int k3_ringacc_ring_pop(struct k3_ring *ring, void *elem);


int k3_ringacc_ring_push_head(struct k3_ring *ring, void *elem);


int k3_ringacc_ring_pop_tail(struct k3_ring *ring, void *elem);

u32 k3_ringacc_get_tisci_dev_id(struct k3_ring *ring);


struct ti_sci_handle;


struct k3_ringacc_init_data {
	const struct ti_sci_handle *tisci;
	u32 tisci_dev_id;
	u32 num_rings;
};

struct k3_ringacc *k3_ringacc_dmarings_init(struct platform_device *pdev,
					    struct k3_ringacc_init_data *data);

#endif 
