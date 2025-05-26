/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __KVM_ARM_VGIC_H
#define __KVM_ARM_VGIC_H

#include <linux/bits.h>
#include <linux/kvm.h>
#include <linux/irqreturn.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/static_key.h>
#include <linux/types.h>
#include <kvm/iodev.h>
#include <linux/list.h>
#include <linux/jump_label.h>

#include <linux/irqchip/arm-gic-v4.h>

#define VGIC_V3_MAX_CPUS	512
#define VGIC_V2_MAX_CPUS	8
#define VGIC_NR_IRQS_LEGACY     256
#define VGIC_NR_SGIS		16
#define VGIC_NR_PPIS		16
#define VGIC_NR_PRIVATE_IRQS	(VGIC_NR_SGIS + VGIC_NR_PPIS)
#define VGIC_MAX_PRIVATE	(VGIC_NR_PRIVATE_IRQS - 1)
#define VGIC_MAX_SPI		1019
#define VGIC_MAX_RESERVED	1023
#define VGIC_MIN_LPI		8192
#define KVM_IRQCHIP_NUM_PINS	(1020 - 32)

#define irq_is_ppi(irq) ((irq) >= VGIC_NR_SGIS && (irq) < VGIC_NR_PRIVATE_IRQS)
#define irq_is_spi(irq) ((irq) >= VGIC_NR_PRIVATE_IRQS && \
			 (irq) <= VGIC_MAX_SPI)

enum vgic_type {
	VGIC_V2,		
	VGIC_V3,		
};


struct vgic_global {
	
	enum vgic_type		type;

	
	phys_addr_t		vcpu_base;

	
	void __iomem		*vcpu_base_va;
	
	void __iomem		*vcpu_hyp_va;

	
	void __iomem		*vctrl_base;
	
	void __iomem		*vctrl_hyp;

	
	int			nr_lr;

	
	unsigned int		maint_irq;

	
	int			max_gic_vcpus;

	
	bool			can_emulate_gicv2;

	
	bool			has_gicv4;
	bool			has_gicv4_1;

	
	bool			no_hw_deactivation;

	
	struct static_key_false gicv3_cpuif;

	u32			ich_vtr_el2;
};

extern struct vgic_global kvm_vgic_global_state;

#define VGIC_V2_MAX_LRS		(1 << 6)
#define VGIC_V3_MAX_LRS		16
#define VGIC_V3_LR_INDEX(lr)	(VGIC_V3_MAX_LRS - 1 - lr)

enum vgic_irq_config {
	VGIC_CONFIG_EDGE = 0,
	VGIC_CONFIG_LEVEL
};


struct irq_ops {
	
	unsigned long flags;

#define VGIC_IRQ_SW_RESAMPLE	BIT(0)	

	
	bool (*get_input_level)(int vintid);
};

struct vgic_irq {
	raw_spinlock_t irq_lock;	
	struct list_head lpi_list;	
	struct list_head ap_list;

	struct kvm_vcpu *vcpu;		

	struct kvm_vcpu *target_vcpu;	

	u32 intid;			
	bool line_level;		
	bool pending_latch;		
	bool active;			
	bool enabled;
	bool hw;			
	struct kref refcount;		
	u32 hwintid;			
	unsigned int host_irq;		
	union {
		u8 targets;			
		u32 mpidr;			
	};
	u8 source;			
	u8 active_source;		
	u8 priority;
	u8 group;			
	enum vgic_irq_config config;	

	struct irq_ops *ops;

	void *owner;			
};

static inline bool vgic_irq_needs_resampling(struct vgic_irq *irq)
{
	return irq->ops && (irq->ops->flags & VGIC_IRQ_SW_RESAMPLE);
}

struct vgic_register_region;
struct vgic_its;

enum iodev_type {
	IODEV_CPUIF,
	IODEV_DIST,
	IODEV_REDIST,
	IODEV_ITS
};

struct vgic_io_device {
	gpa_t base_addr;
	union {
		struct kvm_vcpu *redist_vcpu;
		struct vgic_its *its;
	};
	const struct vgic_register_region *regions;
	enum iodev_type iodev_type;
	int nr_regions;
	struct kvm_io_device dev;
};

struct vgic_its {
	
	gpa_t			vgic_its_base;

	bool			enabled;
	struct vgic_io_device	iodev;
	struct kvm_device	*dev;

	
	u64			baser_device_table;
	u64			baser_coll_table;

	
	struct mutex		cmd_lock;
	u64			cbaser;
	u32			creadr;
	u32			cwriter;

	
	u32			abi_rev;

	
	struct mutex		its_lock;
	struct list_head	device_list;
	struct list_head	collection_list;
};

struct vgic_state_iter;

struct vgic_redist_region {
	u32 index;
	gpa_t base;
	u32 count; 
	u32 free_index; 
	struct list_head list;
};

struct vgic_dist {
	bool			in_kernel;
	bool			ready;
	bool			initialized;

	
	u32			vgic_model;

	
	u32			implementation_rev;
#define KVM_VGIC_IMP_REV_2	2 
#define KVM_VGIC_IMP_REV_3	3 
#define KVM_VGIC_IMP_REV_LATEST	KVM_VGIC_IMP_REV_3

	
	bool			v2_groups_user_writable;

	
	bool			msis_require_devid;

	int			nr_spis;

	
	gpa_t			vgic_dist_base;		
	union {
		
		gpa_t			vgic_cpu_base;
		
		struct list_head rd_regions;
	};

	
	bool			enabled;

	
	bool			nassgireq;

	struct vgic_irq		*spis;

	struct vgic_io_device	dist_iodev;

	bool			has_its;
	bool			table_write_in_progress;

	
	u64			propbaser;

	
	raw_spinlock_t		lpi_list_lock;
	struct list_head	lpi_list_head;
	int			lpi_list_count;

	
	struct list_head	lpi_translation_cache;

	
	struct vgic_state_iter *iter;

	
	struct its_vm		its_vm;
};

struct vgic_v2_cpu_if {
	u32		vgic_hcr;
	u32		vgic_vmcr;
	u32		vgic_apr;
	u32		vgic_lr[VGIC_V2_MAX_LRS];

	unsigned int used_lrs;
};

struct vgic_v3_cpu_if {
	u32		vgic_hcr;
	u32		vgic_vmcr;
	u32		vgic_sre;	
	u32		vgic_ap0r[4];
	u32		vgic_ap1r[4];
	u64		vgic_lr[VGIC_V3_MAX_LRS];

	
	struct its_vpe	its_vpe;

	unsigned int used_lrs;
};

struct vgic_cpu {
	
	union {
		struct vgic_v2_cpu_if	vgic_v2;
		struct vgic_v3_cpu_if	vgic_v3;
	};

	struct vgic_irq private_irqs[VGIC_NR_PRIVATE_IRQS];

	raw_spinlock_t ap_list_lock;	

	
	struct list_head ap_list_head;

	
	struct vgic_io_device	rd_iodev;
	struct vgic_redist_region *rdreg;
	u32 rdreg_index;
	atomic_t syncr_busy;

	
	u64 pendbaser;
	
	atomic_t ctlr;

	
	u32 num_pri_bits;

	
	u32 num_id_bits;
};

extern struct static_key_false vgic_v2_cpuif_trap;
extern struct static_key_false vgic_v3_cpuif_trap;

int kvm_set_legacy_vgic_v2_addr(struct kvm *kvm, struct kvm_arm_device_addr *dev_addr);
void kvm_vgic_early_init(struct kvm *kvm);
int kvm_vgic_vcpu_init(struct kvm_vcpu *vcpu);
int kvm_vgic_create(struct kvm *kvm, u32 type);
void kvm_vgic_destroy(struct kvm *kvm);
void kvm_vgic_vcpu_destroy(struct kvm_vcpu *vcpu);
int kvm_vgic_map_resources(struct kvm *kvm);
int kvm_vgic_hyp_init(void);
void kvm_vgic_init_cpu_hardware(void);

int kvm_vgic_inject_irq(struct kvm *kvm, int cpuid, unsigned int intid,
			bool level, void *owner);
int kvm_vgic_map_phys_irq(struct kvm_vcpu *vcpu, unsigned int host_irq,
			  u32 vintid, struct irq_ops *ops);
int kvm_vgic_unmap_phys_irq(struct kvm_vcpu *vcpu, unsigned int vintid);
int kvm_vgic_get_map(struct kvm_vcpu *vcpu, unsigned int vintid);
bool kvm_vgic_map_is_active(struct kvm_vcpu *vcpu, unsigned int vintid);

int kvm_vgic_vcpu_pending_irq(struct kvm_vcpu *vcpu);

void kvm_vgic_load(struct kvm_vcpu *vcpu);
void kvm_vgic_put(struct kvm_vcpu *vcpu);
void kvm_vgic_vmcr_sync(struct kvm_vcpu *vcpu);

#define irqchip_in_kernel(k)	(!!((k)->arch.vgic.in_kernel))
#define vgic_initialized(k)	((k)->arch.vgic.initialized)
#define vgic_ready(k)		((k)->arch.vgic.ready)
#define vgic_valid_spi(k, i)	(((i) >= VGIC_NR_PRIVATE_IRQS) && \
			((i) < (k)->arch.vgic.nr_spis + VGIC_NR_PRIVATE_IRQS))

bool kvm_vcpu_has_pending_irqs(struct kvm_vcpu *vcpu);
void kvm_vgic_sync_hwstate(struct kvm_vcpu *vcpu);
void kvm_vgic_flush_hwstate(struct kvm_vcpu *vcpu);
void kvm_vgic_reset_mapped_irq(struct kvm_vcpu *vcpu, u32 vintid);

void vgic_v3_dispatch_sgi(struct kvm_vcpu *vcpu, u64 reg, bool allow_group1);


static inline int kvm_vgic_get_max_vcpus(void)
{
	return kvm_vgic_global_state.max_gic_vcpus;
}


int kvm_vgic_setup_default_irq_routing(struct kvm *kvm);

int kvm_vgic_set_owner(struct kvm_vcpu *vcpu, unsigned int intid, void *owner);

struct kvm_kernel_irq_routing_entry;

int kvm_vgic_v4_set_forwarding(struct kvm *kvm, int irq,
			       struct kvm_kernel_irq_routing_entry *irq_entry);

int kvm_vgic_v4_unset_forwarding(struct kvm *kvm, int irq,
				 struct kvm_kernel_irq_routing_entry *irq_entry);

int vgic_v4_load(struct kvm_vcpu *vcpu);
void vgic_v4_commit(struct kvm_vcpu *vcpu);
int vgic_v4_put(struct kvm_vcpu *vcpu);


void kvm_vgic_cpu_up(void);
void kvm_vgic_cpu_down(void);

#endif 
