/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_INITRD_H
#define __LINUX_INITRD_H

#define INITRD_MINOR 250 


extern int rd_image_start;


extern unsigned long rd_size;


extern int initrd_below_start_ok;


extern unsigned long initrd_start, initrd_end;
extern void free_initrd_mem(unsigned long, unsigned long);

#ifdef CONFIG_BLK_DEV_INITRD
extern void __init reserve_initrd_mem(void);
extern void wait_for_initramfs(void);
#else
static inline void __init reserve_initrd_mem(void) {}
static inline void wait_for_initramfs(void) {}
#endif

extern phys_addr_t phys_initrd_start;
extern unsigned long phys_initrd_size;

extern char __initramfs_start[];
extern unsigned long __initramfs_size;

void console_on_rootfs(void);

#endif 
