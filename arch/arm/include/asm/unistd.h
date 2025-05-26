/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_ARM_UNISTD_H
#define __ASM_ARM_UNISTD_H

#include <uapi/asm/unistd.h>
#include <asm/unistd-nr.h>

#define __ARCH_WANT_NEW_STAT
#define __ARCH_WANT_STAT64
#define __ARCH_WANT_SYS_GETHOSTNAME
#define __ARCH_WANT_SYS_PAUSE
#define __ARCH_WANT_SYS_GETPGRP
#define __ARCH_WANT_SYS_NICE
#define __ARCH_WANT_SYS_SIGPENDING
#define __ARCH_WANT_SYS_SIGPROCMASK
#define __ARCH_WANT_SYS_OLD_MMAP
#define __ARCH_WANT_SYS_OLD_SELECT
#define __ARCH_WANT_SYS_UTIME32

#if !defined(CONFIG_AEABI) || defined(CONFIG_OABI_COMPAT)
#define __ARCH_WANT_SYS_TIME32
#define __ARCH_WANT_SYS_IPC
#define __ARCH_WANT_SYS_OLDUMOUNT
#define __ARCH_WANT_SYS_ALARM
#define __ARCH_WANT_SYS_OLD_GETRLIMIT
#define __ARCH_WANT_OLD_READDIR
#define __ARCH_WANT_SYS_SOCKETCALL
#endif
#define __ARCH_WANT_SYS_FORK
#define __ARCH_WANT_SYS_VFORK
#define __ARCH_WANT_SYS_CLONE
#define __ARCH_WANT_SYS_CLONE3


#define __IGNORE_fadvise64_64

#ifdef __ARM_EABI__

#define __IGNORE_getrlimit
#endif

#endif 
