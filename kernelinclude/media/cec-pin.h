/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef LINUX_CEC_PIN_H
#define LINUX_CEC_PIN_H

#include <linux/types.h>
#include <media/cec.h>


struct cec_pin_ops {
	int  (*read)(struct cec_adapter *adap);
	void (*low)(struct cec_adapter *adap);
	void (*high)(struct cec_adapter *adap);
	bool (*enable_irq)(struct cec_adapter *adap);
	void (*disable_irq)(struct cec_adapter *adap);
	void (*free)(struct cec_adapter *adap);
	void (*status)(struct cec_adapter *adap, struct seq_file *file);
	int  (*read_hpd)(struct cec_adapter *adap);
	int  (*read_5v)(struct cec_adapter *adap);

	
	int (*received)(struct cec_adapter *adap, struct cec_msg *msg);
};


void cec_pin_changed(struct cec_adapter *adap, bool value);


struct cec_adapter *cec_pin_allocate_adapter(const struct cec_pin_ops *pin_ops,
					void *priv, const char *name, u32 caps);

#endif
