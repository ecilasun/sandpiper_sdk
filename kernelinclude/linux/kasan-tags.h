/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_KASAN_TAGS_H
#define _LINUX_KASAN_TAGS_H

#define KASAN_TAG_KERNEL	0xFF 
#define KASAN_TAG_INVALID	0xFE 
#define KASAN_TAG_MAX		0xFD 

#ifdef CONFIG_KASAN_HW_TAGS
#define KASAN_TAG_MIN		0xF0 
#else
#define KASAN_TAG_MIN		0x00 
#endif

#endif 
