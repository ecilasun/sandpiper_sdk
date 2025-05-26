// SPDX-License-Identifier: GPL-2.0+
// Copyright 2017 IBM Corp.
#ifndef _MISC_OCXL_H_
#define _MISC_OCXL_H_

#include <linux/pci.h>



#define OCXL_AFU_NAME_SZ      (24+1)  


struct ocxl_afu_config {
	u8 idx;
	int dvsec_afu_control_pos; 
	char name[OCXL_AFU_NAME_SZ];
	u8 version_major;
	u8 version_minor;
	u8 afuc_type;
	u8 afum_type;
	u8 profile;
	u8 global_mmio_bar;     
	u64 global_mmio_offset;
	u32 global_mmio_size;
	u8 pp_mmio_bar;         
	u64 pp_mmio_offset;
	u32 pp_mmio_stride;
	u64 lpc_mem_offset;
	u64 lpc_mem_size;
	u64 special_purpose_mem_offset;
	u64 special_purpose_mem_size;
	u8 pasid_supported_log;
	u16 actag_supported;
};

struct ocxl_fn_config {
	int dvsec_tl_pos;       
	int dvsec_function_pos; 
	int dvsec_afu_info_pos; 
	s8 max_pasid_log;
	s8 max_afu_index;
};

enum ocxl_endian {
	OCXL_BIG_ENDIAN = 0,    
	OCXL_LITTLE_ENDIAN = 1, 
	OCXL_HOST_ENDIAN = 2,   
};

// These are opaque outside the ocxl driver
struct ocxl_afu;
struct ocxl_fn;
struct ocxl_context;

// Device detection & initialisation


struct ocxl_fn *ocxl_function_open(struct pci_dev *dev);


struct list_head *ocxl_function_afu_list(struct ocxl_fn *fn);


struct ocxl_afu *ocxl_function_fetch_afu(struct ocxl_fn *fn, u8 afu_idx);


void ocxl_afu_get(struct ocxl_afu *afu);


void ocxl_afu_put(struct ocxl_afu *afu);



const struct ocxl_fn_config *ocxl_function_config(struct ocxl_fn *fn);


void ocxl_function_close(struct ocxl_fn *fn);

// Context allocation


int ocxl_context_alloc(struct ocxl_context **context, struct ocxl_afu *afu,
			struct address_space *mapping);


void ocxl_context_free(struct ocxl_context *ctx);


int ocxl_context_attach(struct ocxl_context *ctx, u64 amr,
				struct mm_struct *mm);


int ocxl_context_detach(struct ocxl_context *ctx);

// AFU IRQs


int ocxl_afu_irq_alloc(struct ocxl_context *ctx, int *irq_id);


int ocxl_afu_irq_free(struct ocxl_context *ctx, int irq_id);


u64 ocxl_afu_irq_get_addr(struct ocxl_context *ctx, int irq_id);


int ocxl_irq_set_handler(struct ocxl_context *ctx, int irq_id,
		irqreturn_t (*handler)(void *private),
		void (*free_private)(void *private),
		void *private);

// AFU Metadata


struct ocxl_afu_config *ocxl_afu_config(struct ocxl_afu *afu);


void ocxl_afu_set_private(struct ocxl_afu *afu, void *private);


void *ocxl_afu_get_private(struct ocxl_afu *afu);

// Global MMIO

int ocxl_global_mmio_read32(struct ocxl_afu *afu, size_t offset,
			    enum ocxl_endian endian, u32 *val);


int ocxl_global_mmio_read64(struct ocxl_afu *afu, size_t offset,
			    enum ocxl_endian endian, u64 *val);


int ocxl_global_mmio_write32(struct ocxl_afu *afu, size_t offset,
			     enum ocxl_endian endian, u32 val);


int ocxl_global_mmio_write64(struct ocxl_afu *afu, size_t offset,
			     enum ocxl_endian endian, u64 val);


int ocxl_global_mmio_set32(struct ocxl_afu *afu, size_t offset,
			   enum ocxl_endian endian, u32 mask);


int ocxl_global_mmio_set64(struct ocxl_afu *afu, size_t offset,
			   enum ocxl_endian endian, u64 mask);


int ocxl_global_mmio_clear32(struct ocxl_afu *afu, size_t offset,
			     enum ocxl_endian endian, u32 mask);


int ocxl_global_mmio_clear64(struct ocxl_afu *afu, size_t offset,
			     enum ocxl_endian endian, u64 mask);

// Functions left here are for compatibility with the cxlflash driver


int ocxl_config_read_afu(struct pci_dev *dev,
				struct ocxl_fn_config *fn,
				struct ocxl_afu_config *afu,
				u8 afu_idx);


void ocxl_config_set_afu_pasid(struct pci_dev *dev,
				int afu_control_offset,
				int pasid_base, u32 pasid_count_log);


int ocxl_config_get_actag_info(struct pci_dev *dev,
				u16 *base, u16 *enabled, u16 *supported);


void ocxl_config_set_actag(struct pci_dev *dev, int func_offset,
				u32 actag_base, u32 actag_count);


void ocxl_config_set_afu_actag(struct pci_dev *dev,
				int afu_control_offset,
				int actag_base, int actag_count);


void ocxl_config_set_afu_state(struct pci_dev *dev,
				int afu_control_offset, int enable);


int ocxl_config_set_TL(struct pci_dev *dev, int tl_dvsec);


int ocxl_config_terminate_pasid(struct pci_dev *dev,
				int afu_control_offset, int pasid);


int ocxl_config_read_function(struct pci_dev *dev,
				struct ocxl_fn_config *fn);


int ocxl_link_setup(struct pci_dev *dev, int PE_mask,
			void **link_handle);


void ocxl_link_release(struct pci_dev *dev, void *link_handle);


int ocxl_link_add_pe(void *link_handle, int pasid, u32 pidr, u32 tidr,
		u64 amr, u16 bdf, struct mm_struct *mm,
		void (*xsl_err_cb)(void *data, u64 addr, u64 dsisr),
		void *xsl_err_data);


int ocxl_link_remove_pe(void *link_handle, int pasid);


int ocxl_link_irq_alloc(void *link_handle, int *hw_irq);


void ocxl_link_free_irq(void *link_handle, int hw_irq);

#endif 
