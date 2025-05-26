/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TTY_DRIVER_H
#define _LINUX_TTY_DRIVER_H

#include <linux/export.h>
#include <linux/fs.h>
#include <linux/kref.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/termios.h>
#include <linux/seq_file.h>

struct tty_struct;
struct tty_driver;
struct serial_icounter_struct;
struct serial_struct;


struct tty_operations {
	struct tty_struct * (*lookup)(struct tty_driver *driver,
			struct file *filp, int idx);
	int  (*install)(struct tty_driver *driver, struct tty_struct *tty);
	void (*remove)(struct tty_driver *driver, struct tty_struct *tty);
	int  (*open)(struct tty_struct * tty, struct file * filp);
	void (*close)(struct tty_struct * tty, struct file * filp);
	void (*shutdown)(struct tty_struct *tty);
	void (*cleanup)(struct tty_struct *tty);
	ssize_t (*write)(struct tty_struct *tty, const u8 *buf, size_t count);
	int  (*put_char)(struct tty_struct *tty, u8 ch);
	void (*flush_chars)(struct tty_struct *tty);
	unsigned int (*write_room)(struct tty_struct *tty);
	unsigned int (*chars_in_buffer)(struct tty_struct *tty);
	int  (*ioctl)(struct tty_struct *tty,
		    unsigned int cmd, unsigned long arg);
	long (*compat_ioctl)(struct tty_struct *tty,
			     unsigned int cmd, unsigned long arg);
	void (*set_termios)(struct tty_struct *tty, const struct ktermios *old);
	void (*throttle)(struct tty_struct * tty);
	void (*unthrottle)(struct tty_struct * tty);
	void (*stop)(struct tty_struct *tty);
	void (*start)(struct tty_struct *tty);
	void (*hangup)(struct tty_struct *tty);
	int (*break_ctl)(struct tty_struct *tty, int state);
	void (*flush_buffer)(struct tty_struct *tty);
	int (*ldisc_ok)(struct tty_struct *tty, int ldisc);
	void (*set_ldisc)(struct tty_struct *tty);
	void (*wait_until_sent)(struct tty_struct *tty, int timeout);
	void (*send_xchar)(struct tty_struct *tty, char ch);
	int (*tiocmget)(struct tty_struct *tty);
	int (*tiocmset)(struct tty_struct *tty,
			unsigned int set, unsigned int clear);
	int (*resize)(struct tty_struct *tty, struct winsize *ws);
	int (*get_icount)(struct tty_struct *tty,
				struct serial_icounter_struct *icount);
	int  (*get_serial)(struct tty_struct *tty, struct serial_struct *p);
	int  (*set_serial)(struct tty_struct *tty, struct serial_struct *p);
	void (*show_fdinfo)(struct tty_struct *tty, struct seq_file *m);
#ifdef CONFIG_CONSOLE_POLL
	int (*poll_init)(struct tty_driver *driver, int line, char *options);
	int (*poll_get_char)(struct tty_driver *driver, int line);
	void (*poll_put_char)(struct tty_driver *driver, int line, char ch);
#endif
	int (*proc_show)(struct seq_file *m, void *driver);
} __randomize_layout;


struct tty_driver {
	struct kref kref;
	struct cdev **cdevs;
	struct module	*owner;
	const char	*driver_name;
	const char	*name;
	int	name_base;
	int	major;
	int	minor_start;
	unsigned int	num;
	short	type;
	short	subtype;
	struct ktermios init_termios;
	unsigned long	flags;
	struct proc_dir_entry *proc_entry;
	struct tty_driver *other;

	
	struct tty_struct **ttys;
	struct tty_port **ports;
	struct ktermios **termios;
	void *driver_state;

	

	const struct tty_operations *ops;
	struct list_head tty_drivers;
} __randomize_layout;

extern struct list_head tty_drivers;

struct tty_driver *__tty_alloc_driver(unsigned int lines, struct module *owner,
		unsigned long flags);
struct tty_driver *tty_find_polling_driver(char *name, int *line);

void tty_driver_kref_put(struct tty_driver *driver);


#define tty_alloc_driver(lines, flags) \
		__tty_alloc_driver(lines, THIS_MODULE, flags)

static inline struct tty_driver *tty_driver_kref_get(struct tty_driver *d)
{
	kref_get(&d->kref);
	return d;
}

static inline void tty_set_operations(struct tty_driver *driver,
		const struct tty_operations *op)
{
	driver->ops = op;
}


#define TTY_DRIVER_INSTALLED		0x0001
#define TTY_DRIVER_RESET_TERMIOS	0x0002
#define TTY_DRIVER_REAL_RAW		0x0004
#define TTY_DRIVER_DYNAMIC_DEV		0x0008
#define TTY_DRIVER_DEVPTS_MEM		0x0010
#define TTY_DRIVER_HARDWARE_BREAK	0x0020
#define TTY_DRIVER_DYNAMIC_ALLOC	0x0040
#define TTY_DRIVER_UNNUMBERED_NODE	0x0080


#define TTY_DRIVER_TYPE_SYSTEM		0x0001
#define TTY_DRIVER_TYPE_CONSOLE		0x0002
#define TTY_DRIVER_TYPE_SERIAL		0x0003
#define TTY_DRIVER_TYPE_PTY		0x0004
#define TTY_DRIVER_TYPE_SCC		0x0005	
#define TTY_DRIVER_TYPE_SYSCONS		0x0006


#define SYSTEM_TYPE_TTY			0x0001
#define SYSTEM_TYPE_CONSOLE		0x0002
#define SYSTEM_TYPE_SYSCONS		0x0003
#define SYSTEM_TYPE_SYSPTMX		0x0004


#define PTY_TYPE_MASTER			0x0001
#define PTY_TYPE_SLAVE			0x0002


#define SERIAL_TYPE_NORMAL	1

int tty_register_driver(struct tty_driver *driver);
void tty_unregister_driver(struct tty_driver *driver);
struct device *tty_register_device(struct tty_driver *driver, unsigned index,
		struct device *dev);
struct device *tty_register_device_attr(struct tty_driver *driver,
		unsigned index, struct device *device, void *drvdata,
		const struct attribute_group **attr_grp);
void tty_unregister_device(struct tty_driver *driver, unsigned index);

#ifdef CONFIG_PROC_FS
void proc_tty_register_driver(struct tty_driver *);
void proc_tty_unregister_driver(struct tty_driver *);
#else
static inline void proc_tty_register_driver(struct tty_driver *d) {}
static inline void proc_tty_unregister_driver(struct tty_driver *d) {}
#endif

#endif 
