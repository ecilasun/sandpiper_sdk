/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _LIBPS2_H
#define _LIBPS2_H



#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/wait.h>

struct ps2dev;


enum ps2_disposition {
	PS2_PROCESS,
	PS2_IGNORE,
	PS2_ERROR,
};

typedef enum ps2_disposition (*ps2_pre_receive_handler_t)(struct ps2dev *, u8,
							  unsigned int);
typedef void (*ps2_receive_handler_t)(struct ps2dev *, u8);


struct ps2dev {
	struct serio *serio;
	struct mutex cmd_mutex;
	wait_queue_head_t wait;
	unsigned long flags;
	u8 cmdbuf[8];
	u8 cmdcnt;
	u8 nak;

	ps2_pre_receive_handler_t pre_receive_handler;
	ps2_receive_handler_t receive_handler;
};

void ps2_init(struct ps2dev *ps2dev, struct serio *serio,
	      ps2_pre_receive_handler_t pre_receive_handler,
	      ps2_receive_handler_t receive_handler);
int ps2_sendbyte(struct ps2dev *ps2dev, u8 byte, unsigned int timeout);
void ps2_drain(struct ps2dev *ps2dev, size_t maxbytes, unsigned int timeout);
void ps2_begin_command(struct ps2dev *ps2dev);
void ps2_end_command(struct ps2dev *ps2dev);
int __ps2_command(struct ps2dev *ps2dev, u8 *param, unsigned int command);
int ps2_command(struct ps2dev *ps2dev, u8 *param, unsigned int command);
int ps2_sliced_command(struct ps2dev *ps2dev, u8 command);
bool ps2_is_keyboard_id(u8 id);

irqreturn_t ps2_interrupt(struct serio *serio, u8 data, unsigned int flags);

#endif 
