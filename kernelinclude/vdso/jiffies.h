/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __VDSO_JIFFIES_H
#define __VDSO_JIFFIES_H

#include <asm/param.h>			
#include <vdso/time64.h>


#define TICK_NSEC ((NSEC_PER_SEC+HZ/2)/HZ)

#endif 
