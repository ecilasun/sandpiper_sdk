/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_ARM_KVM_ARCH_TIMER_H
#define __ASM_ARM_KVM_ARCH_TIMER_H

#include <linux/clocksource.h>
#include <linux/hrtimer.h>

enum kvm_arch_timers {
	TIMER_PTIMER,
	TIMER_VTIMER,
	NR_KVM_EL0_TIMERS,
	TIMER_HVTIMER = NR_KVM_EL0_TIMERS,
	TIMER_HPTIMER,
	NR_KVM_TIMERS
};

enum kvm_arch_timer_regs {
	TIMER_REG_CNT,
	TIMER_REG_CVAL,
	TIMER_REG_TVAL,
	TIMER_REG_CTL,
	TIMER_REG_VOFF,
};

struct arch_timer_offset {
	
	u64	*vm_offset;
	
	u64	*vcpu_offset;
};

struct arch_timer_vm_data {
	
	u64	voffset;
	
	u64	poffset;

	
	u8	ppi[NR_KVM_TIMERS];
};

struct arch_timer_context {
	struct kvm_vcpu			*vcpu;

	
	struct hrtimer			hrtimer;
	u64				ns_frac;

	
	struct arch_timer_offset	offset;
	
	bool				loaded;

	
	struct {
		bool			level;
	} irq;

	
	u32				host_timer_irq;
};

struct timer_map {
	struct arch_timer_context *direct_vtimer;
	struct arch_timer_context *direct_ptimer;
	struct arch_timer_context *emul_vtimer;
	struct arch_timer_context *emul_ptimer;
};

void get_timer_map(struct kvm_vcpu *vcpu, struct timer_map *map);

struct arch_timer_cpu {
	struct arch_timer_context timers[NR_KVM_TIMERS];

	
	struct hrtimer			bg_timer;

	
	bool			enabled;
};

int __init kvm_timer_hyp_init(bool has_gic);
int kvm_timer_enable(struct kvm_vcpu *vcpu);
int kvm_timer_vcpu_reset(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_init(struct kvm_vcpu *vcpu);
void kvm_timer_sync_user(struct kvm_vcpu *vcpu);
bool kvm_timer_should_notify_user(struct kvm_vcpu *vcpu);
void kvm_timer_update_run(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_terminate(struct kvm_vcpu *vcpu);

void kvm_timer_init_vm(struct kvm *kvm);

u64 kvm_arm_timer_get_reg(struct kvm_vcpu *, u64 regid);
int kvm_arm_timer_set_reg(struct kvm_vcpu *, u64 regid, u64 value);

int kvm_arm_timer_set_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);
int kvm_arm_timer_get_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);
int kvm_arm_timer_has_attr(struct kvm_vcpu *vcpu, struct kvm_device_attr *attr);

u64 kvm_phys_timer_read(void);

void kvm_timer_vcpu_load(struct kvm_vcpu *vcpu);
void kvm_timer_vcpu_put(struct kvm_vcpu *vcpu);

void kvm_timer_init_vhe(void);

#define vcpu_timer(v)	(&(v)->arch.timer_cpu)
#define vcpu_get_timer(v,t)	(&vcpu_timer(v)->timers[(t)])
#define vcpu_vtimer(v)	(&(v)->arch.timer_cpu.timers[TIMER_VTIMER])
#define vcpu_ptimer(v)	(&(v)->arch.timer_cpu.timers[TIMER_PTIMER])
#define vcpu_hvtimer(v)	(&(v)->arch.timer_cpu.timers[TIMER_HVTIMER])
#define vcpu_hptimer(v)	(&(v)->arch.timer_cpu.timers[TIMER_HPTIMER])

#define arch_timer_ctx_index(ctx)	((ctx) - vcpu_timer((ctx)->vcpu)->timers)

#define timer_vm_data(ctx)		(&(ctx)->vcpu->kvm->arch.timer_data)
#define timer_irq(ctx)			(timer_vm_data(ctx)->ppi[arch_timer_ctx_index(ctx)])

u64 kvm_arm_timer_read_sysreg(struct kvm_vcpu *vcpu,
			      enum kvm_arch_timers tmr,
			      enum kvm_arch_timer_regs treg);
void kvm_arm_timer_write_sysreg(struct kvm_vcpu *vcpu,
				enum kvm_arch_timers tmr,
				enum kvm_arch_timer_regs treg,
				u64 val);


u32 timer_get_ctl(struct arch_timer_context *ctxt);
u64 timer_get_cval(struct arch_timer_context *ctxt);


void kvm_timer_cpu_up(void);
void kvm_timer_cpu_down(void);

static inline bool has_cntpoff(void)
{
	return (has_vhe() && cpus_have_final_cap(ARM64_HAS_ECV_CNTPOFF));
}

#endif
