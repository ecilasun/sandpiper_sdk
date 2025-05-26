/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_KMSAN_STRING_H
#define _LINUX_KMSAN_STRING_H


void *__msan_memcpy(void *dst, const void *src, size_t size);
void *__msan_memset(void *s, int c, size_t n);
void *__msan_memmove(void *dest, const void *src, size_t len);

#endif 
