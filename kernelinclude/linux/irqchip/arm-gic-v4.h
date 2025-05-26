/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_IRQCHIP_ARM_GIC_V4_H
#define __LINUX_IRQCHIP_ARM_GIC_V4_H

struct its_vpe;


#define GICv4_ITS_LIST_MAX		16


struct its_vm {
	struct fwnode_handle	*fwnode;
	struct irq_domain	*domain;
	struct page		*vprop_page;
	struct its_vpe		**vpes;
	int			nr_vpes;
	irq_hw_number_t		db_lpi_base;
	unsigned long		*db_bitmap;
	int			nr_db_lpis;
	u32			vlpi_count[GICv4_ITS_LIST_MAX];
};


struct its_vpe {
	struct page 		*vpt_page;
	struct its_vm		*its_vm;
	
	atomic_t		vlpi_count;
	
	int			irq;
	irq_hw_number_t		vpe_db_lpi;
	
	bool			resident;
	
	bool			ready;
	union {
		
		struct {
			
			int	vpe_proxy_event;
			
			bool	idai;
		};
		
		struct {
			struct fwnode_handle	*fwnode;
			struct irq_domain	*sgi_domain;
			struct {
				u8	priority;
				bool	enabled;
				bool	group;
			}			sgi_config[16];
			atomic_t vmapp_count;
		};
	};

	
	raw_spinlock_t		vpe_lock;
	
	u16			col_idx;
	
	u16			vpe_id;
	
	bool			pending_last;
};


struct its_vlpi_map {
	struct its_vm		*vm;
	struct its_vpe		*vpe;
	u32			vintid;
	u8			properties;
	bool			db_enabled;
};

enum its_vcpu_info_cmd_type {
	MAP_VLPI,
	GET_VLPI,
	PROP_UPDATE_VLPI,
	PROP_UPDATE_AND_INV_VLPI,
	SCHEDULE_VPE,
	DESCHEDULE_VPE,
	COMMIT_VPE,
	INVALL_VPE,
	PROP_UPDATE_VSGI,
};

struct its_cmd_info {
	enum its_vcpu_info_cmd_type	cmd_type;
	union {
		struct its_vlpi_map	*map;
		u8			config;
		bool			req_db;
		struct {
			bool		g0en;
			bool		g1en;
		};
		struct {
			u8		priority;
			bool		group;
		};
	};
};

int its_alloc_vcpu_irqs(struct its_vm *vm);
void its_free_vcpu_irqs(struct its_vm *vm);
int its_make_vpe_resident(struct its_vpe *vpe, bool g0en, bool g1en);
int its_make_vpe_non_resident(struct its_vpe *vpe, bool db);
int its_commit_vpe(struct its_vpe *vpe);
int its_invall_vpe(struct its_vpe *vpe);
int its_map_vlpi(int irq, struct its_vlpi_map *map);
int its_get_vlpi(int irq, struct its_vlpi_map *map);
int its_unmap_vlpi(int irq);
int its_prop_update_vlpi(int irq, u8 config, bool inv);
int its_prop_update_vsgi(int irq, u8 priority, bool group);

struct irq_domain_ops;
int its_init_v4(struct irq_domain *domain,
		const struct irq_domain_ops *vpe_ops,
		const struct irq_domain_ops *sgi_ops);

bool gic_cpuif_has_vsgi(void);

#endif
