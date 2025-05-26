/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _COMEDI_8254_H
#define _COMEDI_8254_H

#include <linux/types.h>

struct comedi_device;
struct comedi_insn;
struct comedi_subdevice;


#define I8254_OSC_BASE_10MHZ	100
#define I8254_OSC_BASE_5MHZ	200
#define I8254_OSC_BASE_4MHZ	250
#define I8254_OSC_BASE_2MHZ	500
#define I8254_OSC_BASE_1MHZ	1000
#define I8254_OSC_BASE_100KHZ	10000
#define I8254_OSC_BASE_10KHZ	100000
#define I8254_OSC_BASE_1KHZ	1000000


#define I8254_IO8		1
#define I8254_IO16		2
#define I8254_IO32		4


#define I8254_COUNTER0_REG		0x00
#define I8254_COUNTER1_REG		0x01
#define I8254_COUNTER2_REG		0x02
#define I8254_CTRL_REG			0x03
#define I8254_CTRL_SEL_CTR(x)		((x) << 6)
#define I8254_CTRL_READBACK(x)		(I8254_CTRL_SEL_CTR(3) | BIT(x))
#define I8254_CTRL_READBACK_COUNT	I8254_CTRL_READBACK(4)
#define I8254_CTRL_READBACK_STATUS	I8254_CTRL_READBACK(5)
#define I8254_CTRL_READBACK_SEL_CTR(x)	(2 << (x))
#define I8254_CTRL_RW(x)		(((x) & 0x3) << 4)
#define I8254_CTRL_LATCH		I8254_CTRL_RW(0)
#define I8254_CTRL_LSB_ONLY		I8254_CTRL_RW(1)
#define I8254_CTRL_MSB_ONLY		I8254_CTRL_RW(2)
#define I8254_CTRL_LSB_MSB		I8254_CTRL_RW(3)


#define I8254_MAX_COUNT			0x10000


struct comedi_8254 {
	unsigned long iobase;
	void __iomem *mmio;
	unsigned int iosize;
	unsigned int regshift;
	unsigned int osc_base;
	unsigned int divisor;
	unsigned int divisor1;
	unsigned int divisor2;
	unsigned int next_div;
	unsigned int next_div1;
	unsigned int next_div2;
	unsigned int clock_src[3];
	unsigned int gate_src[3];
	bool busy[3];

	int (*insn_config)(struct comedi_device *dev,
			   struct comedi_subdevice *s,
			   struct comedi_insn *insn, unsigned int *data);
};

unsigned int comedi_8254_status(struct comedi_8254 *i8254,
				unsigned int counter);
unsigned int comedi_8254_read(struct comedi_8254 *i8254, unsigned int counter);
void comedi_8254_write(struct comedi_8254 *i8254,
		       unsigned int counter, unsigned int val);

int comedi_8254_set_mode(struct comedi_8254 *i8254,
			 unsigned int counter, unsigned int mode);
int comedi_8254_load(struct comedi_8254 *i8254,
		     unsigned int counter, unsigned int val, unsigned int mode);

void comedi_8254_pacer_enable(struct comedi_8254 *i8254,
			      unsigned int counter1, unsigned int counter2,
			      bool enable);
void comedi_8254_update_divisors(struct comedi_8254 *i8254);
void comedi_8254_cascade_ns_to_timer(struct comedi_8254 *i8254,
				     unsigned int *nanosec, unsigned int flags);
void comedi_8254_ns_to_timer(struct comedi_8254 *i8254,
			     unsigned int *nanosec, unsigned int flags);

void comedi_8254_set_busy(struct comedi_8254 *i8254,
			  unsigned int counter, bool busy);

void comedi_8254_subdevice_init(struct comedi_subdevice *s,
				struct comedi_8254 *i8254);

struct comedi_8254 *comedi_8254_init(unsigned long iobase,
				     unsigned int osc_base,
				     unsigned int iosize,
				     unsigned int regshift);
struct comedi_8254 *comedi_8254_mm_init(void __iomem *mmio,
					unsigned int osc_base,
					unsigned int iosize,
					unsigned int regshift);

#endif	
