/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef ISCSI_IBFT_H
#define ISCSI_IBFT_H

#include <linux/types.h>


extern phys_addr_t ibft_phys_addr;

#ifdef CONFIG_ISCSI_IBFT_FIND


void reserve_ibft_region(void);


#define IBFT_START 0x80000 
#define IBFT_END 0x100000 

#else
static inline void reserve_ibft_region(void) {}
#endif

#endif 
