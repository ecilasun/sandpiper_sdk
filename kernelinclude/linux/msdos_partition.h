/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MSDOS_PARTITION_H
#define _LINUX_MSDOS_PARTITION_H

#define MSDOS_LABEL_MAGIC		0xAA55

struct msdos_partition {
	u8 boot_ind;		
	u8 head;		
	u8 sector;		
	u8 cyl;			
	u8 sys_ind;		
	u8 end_head;		
	u8 end_sector;		
	u8 end_cyl;		
	__le32 start_sect;	
	__le32 nr_sects;	
} __packed;

enum msdos_sys_ind {
	
	DOS_EXTENDED_PARTITION = 5,
	LINUX_EXTENDED_PARTITION = 0x85,
	WIN98_EXTENDED_PARTITION = 0x0f,

	LINUX_DATA_PARTITION = 0x83,
	LINUX_LVM_PARTITION = 0x8e,
	LINUX_RAID_PARTITION = 0xfd,	

	SOLARIS_X86_PARTITION =	0x82,	
	NEW_SOLARIS_X86_PARTITION = 0xbf,

	DM6_AUX1PARTITION = 0x51,	
	DM6_AUX3PARTITION = 0x53,	
	DM6_PARTITION =	0x54,		
	EZD_PARTITION =	0x55,		

	FREEBSD_PARTITION = 0xa5,	
	OPENBSD_PARTITION = 0xa6,	
	NETBSD_PARTITION = 0xa9,	
	BSDI_PARTITION = 0xb7,		
	MINIX_PARTITION = 0x81,		
	UNIXWARE_PARTITION = 0x63,	
};

#endif 
