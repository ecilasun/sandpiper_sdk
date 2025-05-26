/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASMARM_SIGNAL_H
#define _ASMARM_SIGNAL_H

#include <uapi/asm/signal.h>



#define _NSIG		64
#define _NSIG_BPW	32
#define _NSIG_WORDS	(_NSIG / _NSIG_BPW)

typedef unsigned long old_sigset_t;		

typedef struct {
	unsigned long sig[_NSIG_WORDS];
} sigset_t;

#define __ARCH_UAPI_SA_FLAGS	(SA_THIRTYTWO | SA_RESTORER)

#define __ARCH_HAS_SA_RESTORER

#include <asm/sigcontext.h>

void do_rseq_syscall(struct pt_regs *regs);
int do_work_pending(struct pt_regs *regs, unsigned int thread_flags,
		    int syscall);

#endif
