/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _MISC_CXL_H
#define _MISC_CXL_H

#include <linux/pci.h>
#include <linux/poll.h>
#include <linux/interrupt.h>
#include <uapi/misc/cxl.h>




struct cxl_afu *cxl_pci_to_afu(struct pci_dev *dev);


unsigned int cxl_pci_to_cfg_record(struct pci_dev *dev);





struct cxl_context *cxl_get_context(struct pci_dev *dev);

struct cxl_context *cxl_dev_context_init(struct pci_dev *dev);

int cxl_release_context(struct cxl_context *ctx);


int cxl_set_priv(struct cxl_context *ctx, void *priv);
void *cxl_get_priv(struct cxl_context *ctx);


int cxl_allocate_afu_irqs(struct cxl_context *cxl, int num);

void cxl_free_afu_irqs(struct cxl_context *cxl);


int cxl_map_afu_irq(struct cxl_context *cxl, int num,
		    irq_handler_t handler, void *cookie, char *name);

void cxl_unmap_afu_irq(struct cxl_context *cxl, int num, void *cookie);


int cxl_start_context(struct cxl_context *ctx, u64 wed,
		      struct task_struct *task);

int cxl_stop_context(struct cxl_context *ctx);


int cxl_afu_reset(struct cxl_context *ctx);


void cxl_set_master(struct cxl_context *ctx);


void __iomem *cxl_psa_map(struct cxl_context *ctx);
void cxl_psa_unmap(void __iomem *addr);


int cxl_process_element(struct cxl_context *ctx);




struct file *cxl_get_fd(struct cxl_context *ctx, struct file_operations *fops,
			int *fd);

struct cxl_context *cxl_fops_get_context(struct file *file);

int cxl_start_work(struct cxl_context *ctx,
		   struct cxl_ioctl_start_work *work);

int cxl_fd_open(struct inode *inode, struct file *file);
int cxl_fd_release(struct inode *inode, struct file *file);
long cxl_fd_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int cxl_fd_mmap(struct file *file, struct vm_area_struct *vm);
__poll_t cxl_fd_poll(struct file *file, struct poll_table_struct *poll);
ssize_t cxl_fd_read(struct file *file, char __user *buf, size_t count,
			   loff_t *off);


void cxl_perst_reloads_same_image(struct cxl_afu *afu,
				  bool perst_reloads_same_image);


ssize_t cxl_read_adapter_vpd(struct pci_dev *dev, void *buf, size_t count);


struct cxl_afu_driver_ops {
	struct cxl_event_afu_driver_reserved *(*fetch_event) (
						struct cxl_context *ctx);
	void (*event_delivered) (struct cxl_context *ctx,
				 struct cxl_event_afu_driver_reserved *event,
				 int rc);
};


void cxl_set_driver_ops(struct cxl_context *ctx,
			struct cxl_afu_driver_ops *ops);


void cxl_context_events_pending(struct cxl_context *ctx,
				unsigned int new_events);

#endif 
