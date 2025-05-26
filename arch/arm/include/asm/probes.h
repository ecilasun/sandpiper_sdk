/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _ASM_PROBES_H
#define _ASM_PROBES_H

#ifndef __ASSEMBLY__

typedef u32 probes_opcode_t;

struct arch_probes_insn;
typedef void (probes_insn_handler_t)(probes_opcode_t,
				     struct arch_probes_insn *,
				     struct pt_regs *);
typedef unsigned long (probes_check_cc)(unsigned long);
typedef void (probes_insn_singlestep_t)(probes_opcode_t,
					struct arch_probes_insn *,
					struct pt_regs *);
typedef void (probes_insn_fn_t)(void);


struct arch_probes_insn {
	probes_opcode_t			*insn;
	probes_insn_handler_t		*insn_handler;
	probes_check_cc			*insn_check_cc;
	probes_insn_singlestep_t	*insn_singlestep;
	probes_insn_fn_t		*insn_fn;
	int				stack_space;
	unsigned long			register_usage_flags;
	bool				kprobe_direct_exec;
};

#endif 


#define MAX_STACK_SIZE			64

#endif
