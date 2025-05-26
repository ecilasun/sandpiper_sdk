/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_ARM_SMP_H
#define __ASM_ARM_SMP_H

#include <linux/threads.h>
#include <linux/cpumask.h>
#include <linux/thread_info.h>

#ifndef CONFIG_SMP
# error "<asm/smp.h> included in non-SMP build"
#endif

#define raw_smp_processor_id() (current_thread_info()->cpu)

struct seq_file;


extern void show_ipi_list(struct seq_file *, int);


void handle_IPI(int ipinr, struct pt_regs *regs);


extern void smp_init_cpus(void);


extern void set_smp_ipi_range(int ipi_base, int nr_ipi);


asmlinkage void secondary_start_kernel(struct task_struct *task);



struct secondary_data {
	union {
		struct mpu_rgn_info *mpu_rgn_info;
		u64 pgdir;
	};
	unsigned long swapper_pg_dir;
	void *stack;
	struct task_struct *task;
};
extern struct secondary_data secondary_data;
extern void secondary_startup(void);
extern void secondary_startup_arm(void);

extern int __cpu_disable(void);

static inline void __cpu_die(unsigned int cpu) { }

extern void arch_send_call_function_single_ipi(int cpu);
extern void arch_send_call_function_ipi_mask(const struct cpumask *mask);
extern void arch_send_wakeup_ipi_mask(const struct cpumask *mask);

extern int register_ipi_completion(struct completion *completion, int cpu);

struct smp_operations {
#ifdef CONFIG_SMP
	
	void (*smp_init_cpus)(void);
	
	void (*smp_prepare_cpus)(unsigned int max_cpus);

	
	void (*smp_secondary_init)(unsigned int cpu);
	
	int  (*smp_boot_secondary)(unsigned int cpu, struct task_struct *idle);
#ifdef CONFIG_HOTPLUG_CPU
	int  (*cpu_kill)(unsigned int cpu);
	void (*cpu_die)(unsigned int cpu);
	bool  (*cpu_can_disable)(unsigned int cpu);
	int  (*cpu_disable)(unsigned int cpu);
#endif
#endif
};

struct of_cpu_method {
	const char *method;
	const struct smp_operations *ops;
};

#define CPU_METHOD_OF_DECLARE(name, _method, _ops)			\
	static const struct of_cpu_method __cpu_method_of_table_##name	\
		__used __section("__cpu_method_of_table")		\
		= { .method = _method, .ops = _ops }

extern void smp_set_ops(const struct smp_operations *);

#endif 
