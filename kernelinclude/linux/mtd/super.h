/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __MTD_SUPER_H__
#define __MTD_SUPER_H__

#ifdef __KERNEL__

#include <linux/mtd/mtd.h>
#include <linux/fs.h>
#include <linux/mount.h>

extern int get_tree_mtd(struct fs_context *fc,
		     int (*fill_super)(struct super_block *sb,
				       struct fs_context *fc));
extern void kill_mtd_super(struct super_block *sb);


#endif 

#endif 
