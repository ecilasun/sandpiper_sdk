/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_IRQDESC_H
#define _LINUX_IRQDESC_H

#include <linux/rcupdate.h>
#include <linux/kobject.h>
#include <linux/mutex.h>



struct irq_affinity_notify;
struct proc_dir_entry;
struct module;
struct irq_desc;
struct irq_domain;
struct pt_regs;


struct irq_desc {
	struct irq_common_data	irq_common_data;
	struct irq_data		irq_data;
	unsigned int __percpu	*kstat_irqs;
	irq_flow_handler_t	handle_irq;
	struct irqaction	*action;	
	unsigned int		status_use_accessors;
	unsigned int		core_internal_state__do_not_mess_with_it;
	unsigned int		depth;		
	unsigned int		wake_depth;	
	unsigned int		tot_count;
	unsigned int		irq_count;	
	unsigned long		last_unhandled;	
	unsigned int		irqs_unhandled;
	atomic_t		threads_handled;
	int			threads_handled_last;
	raw_spinlock_t		lock;
	struct cpumask		*percpu_enabled;
	const struct cpumask	*percpu_affinity;
#ifdef CONFIG_SMP
	const struct cpumask	*affinity_hint;
	struct irq_affinity_notify *affinity_notify;
#ifdef CONFIG_GENERIC_PENDING_IRQ
	cpumask_var_t		pending_mask;
#endif
#endif
	unsigned long		threads_oneshot;
	atomic_t		threads_active;
	wait_queue_head_t       wait_for_threads;
#ifdef CONFIG_PM_SLEEP
	unsigned int		nr_actions;
	unsigned int		no_suspend_depth;
	unsigned int		cond_suspend_depth;
	unsigned int		force_resume_depth;
#endif
#ifdef CONFIG_PROC_FS
	struct proc_dir_entry	*dir;
#endif
#ifdef CONFIG_GENERIC_IRQ_DEBUGFS
	struct dentry		*debugfs_file;
	const char		*dev_name;
#endif
#ifdef CONFIG_SPARSE_IRQ
	struct rcu_head		rcu;
	struct kobject		kobj;
#endif
	struct mutex		request_mutex;
	int			parent_irq;
	struct module		*owner;
	const char		*name;
#ifdef CONFIG_HARDIRQS_SW_RESEND
	struct hlist_node	resend_node;
#endif
} ____cacheline_internodealigned_in_smp;

#ifdef CONFIG_SPARSE_IRQ
extern void irq_lock_sparse(void);
extern void irq_unlock_sparse(void);
#else
static inline void irq_lock_sparse(void) { }
static inline void irq_unlock_sparse(void) { }
extern struct irq_desc irq_desc[NR_IRQS];
#endif

static inline unsigned int irq_desc_kstat_cpu(struct irq_desc *desc,
					      unsigned int cpu)
{
	return desc->kstat_irqs ? *per_cpu_ptr(desc->kstat_irqs, cpu) : 0;
}

static inline struct irq_desc *irq_data_to_desc(struct irq_data *data)
{
	return container_of(data->common, struct irq_desc, irq_common_data);
}

static inline unsigned int irq_desc_get_irq(struct irq_desc *desc)
{
	return desc->irq_data.irq;
}

static inline struct irq_data *irq_desc_get_irq_data(struct irq_desc *desc)
{
	return &desc->irq_data;
}

static inline struct irq_chip *irq_desc_get_chip(struct irq_desc *desc)
{
	return desc->irq_data.chip;
}

static inline void *irq_desc_get_chip_data(struct irq_desc *desc)
{
	return desc->irq_data.chip_data;
}

static inline void *irq_desc_get_handler_data(struct irq_desc *desc)
{
	return desc->irq_common_data.handler_data;
}


static inline void generic_handle_irq_desc(struct irq_desc *desc)
{
	desc->handle_irq(desc);
}

int handle_irq_desc(struct irq_desc *desc);
int generic_handle_irq(unsigned int irq);
int generic_handle_irq_safe(unsigned int irq);

#ifdef CONFIG_IRQ_DOMAIN

int generic_handle_domain_irq(struct irq_domain *domain, unsigned int hwirq);
int generic_handle_domain_irq_safe(struct irq_domain *domain, unsigned int hwirq);
int generic_handle_domain_nmi(struct irq_domain *domain, unsigned int hwirq);
#endif


static inline int irq_desc_has_action(struct irq_desc *desc)
{
	return desc && desc->action != NULL;
}


static inline void irq_set_handler_locked(struct irq_data *data,
					  irq_flow_handler_t handler)
{
	struct irq_desc *desc = irq_data_to_desc(data);

	desc->handle_irq = handler;
}


static inline void
irq_set_chip_handler_name_locked(struct irq_data *data,
				 const struct irq_chip *chip,
				 irq_flow_handler_t handler, const char *name)
{
	struct irq_desc *desc = irq_data_to_desc(data);

	desc->handle_irq = handler;
	desc->name = name;
	data->chip = (struct irq_chip *)chip;
}

bool irq_check_status_bit(unsigned int irq, unsigned int bitmask);

static inline bool irq_balancing_disabled(unsigned int irq)
{
	return irq_check_status_bit(irq, IRQ_NO_BALANCING_MASK);
}

static inline bool irq_is_percpu(unsigned int irq)
{
	return irq_check_status_bit(irq, IRQ_PER_CPU);
}

static inline bool irq_is_percpu_devid(unsigned int irq)
{
	return irq_check_status_bit(irq, IRQ_PER_CPU_DEVID);
}

void __irq_set_lockdep_class(unsigned int irq, struct lock_class_key *lock_class,
			     struct lock_class_key *request_class);
static inline void
irq_set_lockdep_class(unsigned int irq, struct lock_class_key *lock_class,
		      struct lock_class_key *request_class)
{
	if (IS_ENABLED(CONFIG_LOCKDEP))
		__irq_set_lockdep_class(irq, lock_class, request_class);
}

#endif
