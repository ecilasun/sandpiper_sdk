// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2017 Arm Ltd.
#ifndef __LINUX_ARM_SDEI_H
#define __LINUX_ARM_SDEI_H

#include <uapi/linux/arm_sdei.h>

#include <acpi/ghes.h>

#ifdef CONFIG_ARM_SDE_INTERFACE
#include <asm/sdei.h>
#endif


#ifndef sdei_arch_get_entry_point
#define sdei_arch_get_entry_point(conduit)	(0)
#endif


typedef int (sdei_event_callback)(u32 event, struct pt_regs *regs, void *arg);


int sdei_event_register(u32 event_num, sdei_event_callback *cb, void *arg);


int sdei_event_unregister(u32 event_num);

int sdei_event_enable(u32 event_num);
int sdei_event_disable(u32 event_num);


int sdei_register_ghes(struct ghes *ghes, sdei_event_callback *normal_cb,
		       sdei_event_callback *critical_cb);
int sdei_unregister_ghes(struct ghes *ghes);

#ifdef CONFIG_ARM_SDE_INTERFACE

int sdei_mask_local_cpu(void);
int sdei_unmask_local_cpu(void);
void __init sdei_init(void);
void sdei_handler_abort(void);
#else
static inline int sdei_mask_local_cpu(void) { return 0; }
static inline int sdei_unmask_local_cpu(void) { return 0; }
static inline void sdei_init(void) { }
static inline void sdei_handler_abort(void) { }
#endif 



struct sdei_registered_event {
	
	struct pt_regs          interrupted_regs;

	sdei_event_callback	*callback;
	void			*callback_arg;
	u32			 event_num;
	u8			 priority;
};


int notrace sdei_event_handler(struct pt_regs *regs,
			       struct sdei_registered_event *arg);


int sdei_api_event_context(u32 query, u64 *result);

#endif 
