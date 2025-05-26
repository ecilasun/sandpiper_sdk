/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _LINUX_AUDIT_ARCH_H_
#define _LINUX_AUDIT_ARCH_H_

enum auditsc_class_t {
	AUDITSC_NATIVE = 0,
	AUDITSC_COMPAT,
	AUDITSC_OPEN,
	AUDITSC_OPENAT,
	AUDITSC_SOCKETCALL,
	AUDITSC_EXECVE,
	AUDITSC_OPENAT2,

	AUDITSC_NVALS 
};

extern int audit_classify_compat_syscall(int abi, unsigned syscall);

#endif
