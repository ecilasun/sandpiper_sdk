/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _DT_BINDINGS_CLK_AT91_H
#define _DT_BINDINGS_CLK_AT91_H

#define PMC_TYPE_CORE		0
#define PMC_TYPE_SYSTEM		1
#define PMC_TYPE_PERIPHERAL	2
#define PMC_TYPE_GCK		3
#define PMC_TYPE_PROGRAMMABLE	4

#define PMC_SLOW		0
#define PMC_MCK			1
#define PMC_UTMI		2
#define PMC_MAIN		3
#define PMC_MCK2		4
#define PMC_I2S0_MUX		5
#define PMC_I2S1_MUX		6
#define PMC_PLLACK		7
#define PMC_PLLBCK		8
#define PMC_AUDIOPLLCK		9
#define PMC_AUDIOPINCK		10


#define PMC_CPUPLL		(PMC_MAIN + 1)
#define PMC_SYSPLL		(PMC_MAIN + 2)
#define PMC_DDRPLL		(PMC_MAIN + 3)
#define PMC_IMGPLL		(PMC_MAIN + 4)
#define PMC_BAUDPLL		(PMC_MAIN + 5)
#define PMC_AUDIOPMCPLL		(PMC_MAIN + 6)
#define PMC_AUDIOIOPLL		(PMC_MAIN + 7)
#define PMC_ETHPLL		(PMC_MAIN + 8)
#define PMC_CPU			(PMC_MAIN + 9)
#define PMC_MCK1		(PMC_MAIN + 10)

#ifndef AT91_PMC_MOSCS
#define AT91_PMC_MOSCS		0		
#define AT91_PMC_LOCKA		1		
#define AT91_PMC_LOCKB		2		
#define AT91_PMC_MCKRDY		3		
#define AT91_PMC_LOCKU		6		
#define AT91_PMC_PCKRDY(id)	(8 + (id))	
#define AT91_PMC_MOSCSELS	16		
#define AT91_PMC_MOSCRCS	17		
#define AT91_PMC_CFDEV		18		
#define AT91_PMC_GCKRDY		24		
#endif

#endif
