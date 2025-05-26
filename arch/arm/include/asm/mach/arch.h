/* SPDX-License-Identifier: GPL-2.0-only */


#include <linux/types.h>

#ifndef __ASSEMBLY__
#include <linux/reboot.h>

struct tag;
struct pt_regs;
struct smp_operations;
#ifdef CONFIG_SMP
#define smp_ops(ops) (&(ops))
#define smp_init_ops(ops) (&(ops))
#else
#define smp_ops(ops) (struct smp_operations *)NULL
#define smp_init_ops(ops) (bool (*)(void))NULL
#endif

struct machine_desc {
	unsigned int		nr;		
	const char		*name;		
	unsigned long		atag_offset;	
	const char *const 	*dt_compat;	

	unsigned int		nr_irqs;	

#ifdef CONFIG_ZONE_DMA
	phys_addr_t		dma_zone_size;	
#endif

	unsigned int		video_start;	
	unsigned int		video_end;	

	unsigned char		reserve_lp0 :1;	
	unsigned char		reserve_lp1 :1;	
	unsigned char		reserve_lp2 :1;	
	enum reboot_mode	reboot_mode;	
	unsigned		l2c_aux_val;	
	unsigned		l2c_aux_mask;	
	void			(*l2c_write_sec)(unsigned long, unsigned);
	const struct smp_operations	*smp;	
	bool			(*smp_init)(void);
	void			(*fixup)(struct tag *, char **);
	void			(*dt_fixup)(void);
	long long		(*pv_fixup)(void);
	void			(*reserve)(void);
	void			(*map_io)(void);
	void			(*init_early)(void);
	void			(*init_irq)(void);
	void			(*init_time)(void);
	void			(*init_machine)(void);
	void			(*init_late)(void);
	void			(*restart)(enum reboot_mode, const char *);
};


extern const struct machine_desc *machine_desc;


extern const struct machine_desc __arch_info_begin[], __arch_info_end[];
#define for_each_machine_desc(p)			\
	for (p = __arch_info_begin; p < __arch_info_end; p++)


#define MACHINE_START(_type,_name)			\
static const struct machine_desc __mach_desc_##_type	\
 __used							\
 __section(".arch.info.init") = {			\
	.nr		= MACH_TYPE_##_type,		\
	.name		= _name,

#define MACHINE_END				\
};

#define DT_MACHINE_START(_name, _namestr)		\
static const struct machine_desc __mach_desc_##_name	\
 __used							\
 __section(".arch.info.init") = {			\
	.nr		= ~0,				\
	.name		= _namestr,

#endif
