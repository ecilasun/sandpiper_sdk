/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __MEM_ENCRYPT_H__
#define __MEM_ENCRYPT_H__

#ifndef __ASSEMBLY__

#ifdef CONFIG_ARCH_HAS_MEM_ENCRYPT

#include <asm/mem_encrypt.h>

#endif	

#ifdef CONFIG_AMD_MEM_ENCRYPT

#define __sme_set(x)		((x) | sme_me_mask)
#define __sme_clr(x)		((x) & ~sme_me_mask)
#else
#define __sme_set(x)		(x)
#define __sme_clr(x)		(x)
#endif

#endif	

#endif	
