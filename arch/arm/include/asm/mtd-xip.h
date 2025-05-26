/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ARM_MTD_XIP_H__
#define __ARM_MTD_XIP_H__

#include <mach/mtd-xip.h>


#define xip_iprefetch() 	do { asm volatile (".rep 8; nop; .endr"); } while (0)

#endif 
