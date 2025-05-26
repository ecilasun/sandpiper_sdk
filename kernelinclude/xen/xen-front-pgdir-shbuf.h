/* SPDX-License-Identifier: GPL-2.0 OR MIT */



#ifndef __XEN_FRONT_PGDIR_SHBUF_H_
#define __XEN_FRONT_PGDIR_SHBUF_H_

#include <linux/kernel.h>

#include <xen/grant_table.h>

struct xen_front_pgdir_shbuf_ops;

struct xen_front_pgdir_shbuf {
	
	int num_grefs;
	grant_ref_t *grefs;
	
	u8 *directory;

	
	int num_pages;
	
	struct page **pages;

	struct xenbus_device *xb_dev;

	
	const struct xen_front_pgdir_shbuf_ops *ops;

	
	grant_handle_t *backend_map_handles;
};

struct xen_front_pgdir_shbuf_cfg {
	struct xenbus_device *xb_dev;

	
	int num_pages;
	
	struct page **pages;

	
	struct xen_front_pgdir_shbuf *pgdir;
	
	int be_alloc;
};

int xen_front_pgdir_shbuf_alloc(struct xen_front_pgdir_shbuf_cfg *cfg);

grant_ref_t
xen_front_pgdir_shbuf_get_dir_start(struct xen_front_pgdir_shbuf *buf);

int xen_front_pgdir_shbuf_map(struct xen_front_pgdir_shbuf *buf);

int xen_front_pgdir_shbuf_unmap(struct xen_front_pgdir_shbuf *buf);

void xen_front_pgdir_shbuf_free(struct xen_front_pgdir_shbuf *buf);

#endif 
