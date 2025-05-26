/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_FLAT_H
#define _LINUX_FLAT_H

#define	FLAT_VERSION			0x00000004L



struct flat_hdr {
	char	magic[4];
	__be32	rev;          
	__be32	entry;        
	__be32	data_start;   
	__be32	data_end;     
	__be32	bss_end;      

	

	__be32	stack_size;   
	__be32	reloc_start;  
	__be32	reloc_count;  
	__be32	flags;
	__be32	build_date;   
	__u32	filler[5];    
};

#define FLAT_FLAG_RAM    0x0001 
#define FLAT_FLAG_GOTPIC 0x0002 
#define FLAT_FLAG_GZIP   0x0004 
#define FLAT_FLAG_GZDATA 0x0008 
#define FLAT_FLAG_KTRACE 0x0010 



#define	OLD_FLAT_VERSION			0x00000002L
#define OLD_FLAT_RELOC_TYPE_TEXT	0
#define OLD_FLAT_RELOC_TYPE_DATA	1
#define OLD_FLAT_RELOC_TYPE_BSS		2

typedef union {
	u32		value;
	struct {
#if defined(__LITTLE_ENDIAN_BITFIELD) || \
    (defined(mc68000) && !defined(CONFIG_COLDFIRE))
		s32	offset : 30;
		u32	type : 2;
# elif defined(__BIG_ENDIAN_BITFIELD)
		u32	type : 2;
		s32	offset : 30;
# else
#   	error "Unknown bitfield order for flat files."
# endif
	} reloc;
} flat_v2_reloc_t;

#endif 
