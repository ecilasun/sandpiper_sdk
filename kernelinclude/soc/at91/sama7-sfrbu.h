/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SAMA7_SFRBU_H__
#define __SAMA7_SFRBU_H__

#ifdef CONFIG_SOC_SAMA7

#define AT91_SFRBU_PSWBU			(0x00)		
#define		AT91_SFRBU_PSWBU_PSWKEY		(0x4BD20C << 8)	
#define		AT91_SFRBU_PSWBU_STATE		(1 << 2)	
#define		AT91_SFRBU_PSWBU_SOFTSWITCH	(1 << 1)	
#define		AT91_SFRBU_PSWBU_CTRL		(1 << 0)	

#define AT91_SFRBU_25LDOCR			(0x0C)		
#define		AT91_SFRBU_25LDOCR_LDOANAKEY	(0x3B6E18 << 8)	
#define		AT91_SFRBU_25LDOCR_STATE	(1 << 3)	
#define		AT91_SFRBU_25LDOCR_LP		(1 << 2)	
#define		AT91_SFRBU_PD_VALUE_MSK		(0x3)
#define		AT91_SFRBU_25LDOCR_PD_VALUE(v)	((v) & AT91_SFRBU_PD_VALUE_MSK)	

#define AT91_FRBU_DDRPWR			(0x10)		
#define		AT91_FRBU_DDRPWR_STATE		(1 << 0)	

#endif 

#endif 

