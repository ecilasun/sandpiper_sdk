/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ASMARM_SPARSEMEM_H
#define ASMARM_SPARSEMEM_H

#include <asm/page.h>


#if !defined(MAX_PHYSMEM_BITS) || !defined(SECTION_SIZE_BITS)
#define MAX_PHYSMEM_BITS	36
#define SECTION_SIZE_BITS	28
#endif

#endif
