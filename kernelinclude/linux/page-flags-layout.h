/* SPDX-License-Identifier: GPL-2.0 */
#ifndef PAGE_FLAGS_LAYOUT_H
#define PAGE_FLAGS_LAYOUT_H

#include <linux/numa.h>
#include <generated/bounds.h>


#if MAX_NR_ZONES < 2
#define ZONES_SHIFT 0
#elif MAX_NR_ZONES <= 2
#define ZONES_SHIFT 1
#elif MAX_NR_ZONES <= 4
#define ZONES_SHIFT 2
#elif MAX_NR_ZONES <= 8
#define ZONES_SHIFT 3
#else
#error ZONES_SHIFT "Too many zones configured"
#endif

#define ZONES_WIDTH		ZONES_SHIFT

#ifdef CONFIG_SPARSEMEM
#include <asm/sparsemem.h>
#define SECTIONS_SHIFT	(MAX_PHYSMEM_BITS - SECTION_SIZE_BITS)
#else
#define SECTIONS_SHIFT	0
#endif

#ifndef BUILD_VDSO32_64

#if defined(CONFIG_SPARSEMEM) && !defined(CONFIG_SPARSEMEM_VMEMMAP)
#define SECTIONS_WIDTH		SECTIONS_SHIFT
#else
#define SECTIONS_WIDTH		0
#endif

#if ZONES_WIDTH + LRU_GEN_WIDTH + SECTIONS_WIDTH + NODES_SHIFT \
	<= BITS_PER_LONG - NR_PAGEFLAGS
#define NODES_WIDTH		NODES_SHIFT
#elif defined(CONFIG_SPARSEMEM_VMEMMAP)
#error "Vmemmap: No space for nodes field in page flags"
#else
#define NODES_WIDTH		0
#endif


#if NODES_SHIFT != 0 && NODES_WIDTH == 0
#define NODE_NOT_IN_PAGE_FLAGS	1
#endif

#if defined(CONFIG_KASAN_SW_TAGS) || defined(CONFIG_KASAN_HW_TAGS)
#define KASAN_TAG_WIDTH 8
#else
#define KASAN_TAG_WIDTH 0
#endif

#ifdef CONFIG_NUMA_BALANCING
#define LAST__PID_SHIFT 8
#define LAST__PID_MASK  ((1 << LAST__PID_SHIFT)-1)

#define LAST__CPU_SHIFT NR_CPUS_BITS
#define LAST__CPU_MASK  ((1 << LAST__CPU_SHIFT)-1)

#define LAST_CPUPID_SHIFT (LAST__PID_SHIFT+LAST__CPU_SHIFT)
#else
#define LAST_CPUPID_SHIFT 0
#endif

#if ZONES_WIDTH + LRU_GEN_WIDTH + SECTIONS_WIDTH + NODES_WIDTH + \
	KASAN_TAG_WIDTH + LAST_CPUPID_SHIFT <= BITS_PER_LONG - NR_PAGEFLAGS
#define LAST_CPUPID_WIDTH LAST_CPUPID_SHIFT
#else
#define LAST_CPUPID_WIDTH 0
#endif

#if LAST_CPUPID_SHIFT != 0 && LAST_CPUPID_WIDTH == 0
#define LAST_CPUPID_NOT_IN_PAGE_FLAGS
#endif

#if ZONES_WIDTH + LRU_GEN_WIDTH + SECTIONS_WIDTH + NODES_WIDTH + \
	KASAN_TAG_WIDTH + LAST_CPUPID_WIDTH > BITS_PER_LONG - NR_PAGEFLAGS
#error "Not enough bits in page flags"
#endif


#define LRU_REFS_WIDTH	min(__LRU_REFS_WIDTH, BITS_PER_LONG - NR_PAGEFLAGS - \
			    ZONES_WIDTH - LRU_GEN_WIDTH - SECTIONS_WIDTH - \
			    NODES_WIDTH - KASAN_TAG_WIDTH - LAST_CPUPID_WIDTH)

#endif
#endif 
