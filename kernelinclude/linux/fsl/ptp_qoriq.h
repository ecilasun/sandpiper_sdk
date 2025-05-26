// SPDX-License-Identifier: GPL-2.0

#ifndef __PTP_QORIQ_H__
#define __PTP_QORIQ_H__

#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/ptp_clock_kernel.h>


struct ctrl_regs {
	u32 tmr_ctrl;     
	u32 tmr_tevent;   
	u32 tmr_temask;   
	u32 tmr_pevent;   
	u32 tmr_pemask;   
	u32 tmr_stat;     
	u32 tmr_cnt_h;    
	u32 tmr_cnt_l;    
	u32 tmr_add;      
	u32 tmr_acc;      
	u32 tmr_prsc;     
	u8  res1[4];
	u32 tmroff_h;     
	u32 tmroff_l;     
};

struct alarm_regs {
	u32 tmr_alarm1_h; 
	u32 tmr_alarm1_l; 
	u32 tmr_alarm2_h; 
	u32 tmr_alarm2_l; 
};

struct fiper_regs {
	u32 tmr_fiper1;   
	u32 tmr_fiper2;   
	u32 tmr_fiper3;   
};

struct etts_regs {
	u32 tmr_etts1_h;  
	u32 tmr_etts1_l;  
	u32 tmr_etts2_h;  
	u32 tmr_etts2_l;  
};

struct ptp_qoriq_registers {
	struct ctrl_regs __iomem *ctrl_regs;
	struct alarm_regs __iomem *alarm_regs;
	struct fiper_regs __iomem *fiper_regs;
	struct etts_regs __iomem *etts_regs;
};


#define ETSEC_CTRL_REGS_OFFSET	0x0
#define ETSEC_ALARM_REGS_OFFSET	0x40
#define ETSEC_FIPER_REGS_OFFSET	0x80
#define ETSEC_ETTS_REGS_OFFSET	0xa0

#define CTRL_REGS_OFFSET	0x80
#define ALARM_REGS_OFFSET	0xb8
#define FIPER_REGS_OFFSET	0xd0
#define ETTS_REGS_OFFSET	0xe0



#define ALM1P                 (1<<31) 
#define ALM2P                 (1<<30) 
#define FIPERST               (1<<28) 
#define PP1L                  (1<<27) 
#define PP2L                  (1<<26) 
#define TCLK_PERIOD_SHIFT     (16) 
#define TCLK_PERIOD_MASK      (0x3ff)
#define RTPE                  (1<<15) 
#define FRD                   (1<<14) 
#define ESFDP                 (1<<11) 
#define ESFDE                 (1<<10) 
#define ETEP2                 (1<<9) 
#define ETEP1                 (1<<8) 
#define COPH                  (1<<7) 
#define CIPH                  (1<<6) 
#define TMSR                  (1<<5) 
#define BYP                   (1<<3) 
#define TE                    (1<<2) 
#define CKSEL_SHIFT           (0)    
#define CKSEL_MASK            (0x3)


#define ETS2                  (1<<25) 
#define ETS1                  (1<<24) 
#define ALM2                  (1<<17) 
#define ALM1                  (1<<16) 
#define PP1                   (1<<7)  
#define PP2                   (1<<6)  
#define PP3                   (1<<5)  


#define ETS2EN                (1<<25) 
#define ETS1EN                (1<<24) 
#define ALM2EN                (1<<17) 
#define ALM1EN                (1<<16) 
#define PP1EN                 (1<<7) 
#define PP2EN                 (1<<6) 


#define TXP2                  (1<<9) 
#define TXP1                  (1<<8) 
#define RXP                   (1<<0) 


#define TXP2EN                (1<<9) 
#define TXP1EN                (1<<8) 
#define RXPEN                 (1<<0) 


#define STAT_VEC_SHIFT        (0) 
#define STAT_VEC_MASK         (0x3f)
#define ETS1_VLD              (1<<24)
#define ETS2_VLD              (1<<25)


#define PRSC_OCK_SHIFT        (0) 
#define PRSC_OCK_MASK         (0xffff)


#define DRIVER		"ptp_qoriq"
#define N_EXT_TS	2

#define DEFAULT_CKSEL		1
#define DEFAULT_TMR_PRSC	2
#define DEFAULT_FIPER1_PERIOD	1000000000
#define DEFAULT_FIPER2_PERIOD	1000000000
#define DEFAULT_FIPER3_PERIOD	1000000000

struct ptp_qoriq {
	void __iomem *base;
	struct ptp_qoriq_registers regs;
	spinlock_t lock; 
	struct ptp_clock *clock;
	struct ptp_clock_info caps;
	struct resource *rsrc;
	struct dentry *debugfs_root;
	struct device *dev;
	bool extts_fifo_support;
	bool fiper3_support;
	bool etsec;
	int irq;
	int phc_index;
	u32 tclk_period;  
	u32 tmr_prsc;
	u32 tmr_add;
	u32 cksel;
	u32 tmr_fiper1;
	u32 tmr_fiper2;
	u32 tmr_fiper3;
	u32 (*read)(unsigned __iomem *addr);
	void (*write)(unsigned __iomem *addr, u32 val);
};

static inline u32 qoriq_read_be(unsigned __iomem *addr)
{
	return ioread32be(addr);
}

static inline void qoriq_write_be(unsigned __iomem *addr, u32 val)
{
	iowrite32be(val, addr);
}

static inline u32 qoriq_read_le(unsigned __iomem *addr)
{
	return ioread32(addr);
}

static inline void qoriq_write_le(unsigned __iomem *addr, u32 val)
{
	iowrite32(val, addr);
}

irqreturn_t ptp_qoriq_isr(int irq, void *priv);
int ptp_qoriq_init(struct ptp_qoriq *ptp_qoriq, void __iomem *base,
		   const struct ptp_clock_info *caps);
void ptp_qoriq_free(struct ptp_qoriq *ptp_qoriq);
int ptp_qoriq_adjfine(struct ptp_clock_info *ptp, long scaled_ppm);
int ptp_qoriq_adjtime(struct ptp_clock_info *ptp, s64 delta);
int ptp_qoriq_gettime(struct ptp_clock_info *ptp, struct timespec64 *ts);
int ptp_qoriq_settime(struct ptp_clock_info *ptp,
		      const struct timespec64 *ts);
int ptp_qoriq_enable(struct ptp_clock_info *ptp,
		     struct ptp_clock_request *rq, int on);
int extts_clean_up(struct ptp_qoriq *ptp_qoriq, int index, bool update_event);
#ifdef CONFIG_DEBUG_FS
void ptp_qoriq_create_debugfs(struct ptp_qoriq *ptp_qoriq);
void ptp_qoriq_remove_debugfs(struct ptp_qoriq *ptp_qoriq);
#else
static inline void ptp_qoriq_create_debugfs(struct ptp_qoriq *ptp_qoriq)
{ }
static inline void ptp_qoriq_remove_debugfs(struct ptp_qoriq *ptp_qoriq)
{ }
#endif

#endif
