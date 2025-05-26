/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __SAMA7_DDR_H__
#define __SAMA7_DDR_H__


#define DDR3PHY_PIR				(0x04)		
#define	DDR3PHY_PIR_DLLBYP			(1 << 17)	
#define		DDR3PHY_PIR_ITMSRST		(1 << 4)	
#define	DDR3PHY_PIR_DLLLOCK			(1 << 2)	
#define		DDR3PHY_PIR_DLLSRST		(1 << 1)	
#define	DDR3PHY_PIR_INIT			(1 << 0)	

#define DDR3PHY_PGCR				(0x08)		
#define		DDR3PHY_PGCR_CKDV1		(1 << 13)	
#define		DDR3PHY_PGCR_CKDV0		(1 << 12)	

#define	DDR3PHY_PGSR				(0x0C)		
#define		DDR3PHY_PGSR_IDONE		(1 << 0)	

#define	DDR3PHY_ACDLLCR				(0x14)		
#define		DDR3PHY_ACDLLCR_DLLSRST		(1 << 30)	

#define DDR3PHY_ACIOCR				(0x24)		
#define		DDR3PHY_ACIOCR_CSPDD_CS0	(1 << 18)	
#define		DDR3PHY_ACIOCR_CKPDD_CK0	(1 << 8)	
#define		DDR3PHY_ACIORC_ACPDD		(1 << 3)	

#define DDR3PHY_DXCCR				(0x28)		
#define		DDR3PHY_DXCCR_DXPDR		(1 << 3)	

#define DDR3PHY_DSGCR				(0x2C)		
#define		DDR3PHY_DSGCR_ODTPDD_ODT0	(1 << 20)	

#define DDR3PHY_ZQ0SR0				(0x188)		
#define DDR3PHY_ZQ0SR0_PDO_OFF			(0)		
#define DDR3PHY_ZQ0SR0_PUO_OFF			(5)		
#define DDR3PHY_ZQ0SR0_PDODT_OFF		(10)		
#define DDR3PHY_ZQ0SRO_PUODT_OFF		(15)		

#define	DDR3PHY_DX0DLLCR			(0x1CC)		
#define	DDR3PHY_DX1DLLCR			(0x20C)		
#define		DDR3PHY_DXDLLCR_DLLDIS		(1 << 31)	


#define UDDRC_STAT				(0x04)		
#define		UDDRC_STAT_SELFREF_TYPE_DIS	(0x0 << 4)	
#define		UDDRC_STAT_SELFREF_TYPE_PHY	(0x1 << 4)	
#define		UDDRC_STAT_SELFREF_TYPE_SW	(0x2 << 4)	
#define		UDDRC_STAT_SELFREF_TYPE_AUTO	(0x3 << 4)	
#define		UDDRC_STAT_SELFREF_TYPE_MSK	(0x3 << 4)	
#define		UDDRC_STAT_OPMODE_INIT		(0x0 << 0)	
#define		UDDRC_STAT_OPMODE_NORMAL	(0x1 << 0)	
#define		UDDRC_STAT_OPMODE_PWRDOWN	(0x2 << 0)	
#define		UDDRC_STAT_OPMODE_SELF_REFRESH	(0x3 << 0)	
#define		UDDRC_STAT_OPMODE_MSK		(0x7 << 0)	

#define UDDRC_PWRCTL				(0x30)		
#define		UDDRC_PWRCTL_SELFREF_EN		(1 << 0)	
#define		UDDRC_PWRCTL_SELFREF_SW		(1 << 5)	

#define UDDRC_DFIMISC				(0x1B0)		
#define		UDDRC_DFIMISC_DFI_INIT_COMPLETE_EN (1 << 0)	

#define UDDRC_SWCTRL				(0x320)		
#define		UDDRC_SWCTRL_SW_DONE		(1 << 0)	

#define UDDRC_SWSTAT				(0x324)		
#define		UDDRC_SWSTAT_SW_DONE_ACK	(1 << 0)	

#define UDDRC_PSTAT				(0x3FC)		
#define	UDDRC_PSTAT_ALL_PORTS			(0x1F001F)	

#define UDDRC_PCTRL_0				(0x490)		
#define UDDRC_PCTRL_1				(0x540)		
#define UDDRC_PCTRL_2				(0x5F0)		
#define UDDRC_PCTRL_3				(0x6A0)		
#define UDDRC_PCTRL_4				(0x750)		

#endif 
