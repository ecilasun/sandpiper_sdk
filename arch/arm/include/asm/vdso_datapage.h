/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ASM_VDSO_DATAPAGE_H
#define __ASM_VDSO_DATAPAGE_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <vdso/datapage.h>
#include <asm/page.h>

union vdso_data_store {
	struct vdso_data	data[CS_BASES];
	u8			page[PAGE_SIZE];
};

#endif 

#endif 

#endif 
