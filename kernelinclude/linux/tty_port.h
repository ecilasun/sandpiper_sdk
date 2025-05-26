/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TTY_PORT_H
#define _LINUX_TTY_PORT_H

#include <linux/kfifo.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/tty_buffer.h>
#include <linux/wait.h>

struct attribute_group;
struct tty_driver;
struct tty_port;
struct tty_struct;


struct tty_port_operations {
	bool (*carrier_raised)(struct tty_port *port);
	void (*dtr_rts)(struct tty_port *port, bool active);
	void (*shutdown)(struct tty_port *port);
	int (*activate)(struct tty_port *port, struct tty_struct *tty);
	void (*destruct)(struct tty_port *port);
};

struct tty_port_client_operations {
	size_t (*receive_buf)(struct tty_port *port, const u8 *cp, const u8 *fp,
			      size_t count);
	void (*lookahead_buf)(struct tty_port *port, const u8 *cp,
			      const u8 *fp, size_t count);
	void (*write_wakeup)(struct tty_port *port);
};

extern const struct tty_port_client_operations tty_port_default_client_ops;


struct tty_port {
	struct tty_bufhead	buf;
	struct tty_struct	*tty;
	struct tty_struct	*itty;
	const struct tty_port_operations *ops;
	const struct tty_port_client_operations *client_ops;
	spinlock_t		lock;
	int			blocked_open;
	int			count;
	wait_queue_head_t	open_wait;
	wait_queue_head_t	delta_msr_wait;
	unsigned long		flags;
	unsigned long		iflags;
	unsigned char		console:1;
	struct mutex		mutex;
	struct mutex		buf_mutex;
	unsigned char		*xmit_buf;
	DECLARE_KFIFO_PTR(xmit_fifo, unsigned char);
	unsigned int		close_delay;
	unsigned int		closing_wait;
	int			drain_delay;
	struct kref		kref;
	void			*client_data;
};


#define TTY_PORT_INITIALIZED	0	
#define TTY_PORT_SUSPENDED	1	
#define TTY_PORT_ACTIVE		2	


#define TTY_PORT_CTS_FLOW	3	
#define TTY_PORT_CHECK_CD	4	
#define TTY_PORT_KOPENED	5	

void tty_port_init(struct tty_port *port);
void tty_port_link_device(struct tty_port *port, struct tty_driver *driver,
		unsigned index);
struct device *tty_port_register_device(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device);
struct device *tty_port_register_device_attr(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device, void *drvdata,
		const struct attribute_group **attr_grp);
struct device *tty_port_register_device_serdev(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device);
struct device *tty_port_register_device_attr_serdev(struct tty_port *port,
		struct tty_driver *driver, unsigned index,
		struct device *device, void *drvdata,
		const struct attribute_group **attr_grp);
void tty_port_unregister_device(struct tty_port *port,
		struct tty_driver *driver, unsigned index);
int tty_port_alloc_xmit_buf(struct tty_port *port);
void tty_port_free_xmit_buf(struct tty_port *port);
void tty_port_destroy(struct tty_port *port);
void tty_port_put(struct tty_port *port);

static inline struct tty_port *tty_port_get(struct tty_port *port)
{
	if (port && kref_get_unless_zero(&port->kref))
		return port;
	return NULL;
}


static inline bool tty_port_cts_enabled(const struct tty_port *port)
{
	return test_bit(TTY_PORT_CTS_FLOW, &port->iflags);
}

static inline void tty_port_set_cts_flow(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_CTS_FLOW, &port->iflags, val);
}

static inline bool tty_port_active(const struct tty_port *port)
{
	return test_bit(TTY_PORT_ACTIVE, &port->iflags);
}

static inline void tty_port_set_active(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_ACTIVE, &port->iflags, val);
}

static inline bool tty_port_check_carrier(const struct tty_port *port)
{
	return test_bit(TTY_PORT_CHECK_CD, &port->iflags);
}

static inline void tty_port_set_check_carrier(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_CHECK_CD, &port->iflags, val);
}

static inline bool tty_port_suspended(const struct tty_port *port)
{
	return test_bit(TTY_PORT_SUSPENDED, &port->iflags);
}

static inline void tty_port_set_suspended(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_SUSPENDED, &port->iflags, val);
}

static inline bool tty_port_initialized(const struct tty_port *port)
{
	return test_bit(TTY_PORT_INITIALIZED, &port->iflags);
}

static inline void tty_port_set_initialized(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_INITIALIZED, &port->iflags, val);
}

static inline bool tty_port_kopened(const struct tty_port *port)
{
	return test_bit(TTY_PORT_KOPENED, &port->iflags);
}

static inline void tty_port_set_kopened(struct tty_port *port, bool val)
{
	assign_bit(TTY_PORT_KOPENED, &port->iflags, val);
}

struct tty_struct *tty_port_tty_get(struct tty_port *port);
void tty_port_tty_set(struct tty_port *port, struct tty_struct *tty);
bool tty_port_carrier_raised(struct tty_port *port);
void tty_port_raise_dtr_rts(struct tty_port *port);
void tty_port_lower_dtr_rts(struct tty_port *port);
void tty_port_hangup(struct tty_port *port);
void tty_port_tty_hangup(struct tty_port *port, bool check_clocal);
void tty_port_tty_wakeup(struct tty_port *port);
int tty_port_block_til_ready(struct tty_port *port, struct tty_struct *tty,
		struct file *filp);
int tty_port_close_start(struct tty_port *port, struct tty_struct *tty,
		struct file *filp);
void tty_port_close_end(struct tty_port *port, struct tty_struct *tty);
void tty_port_close(struct tty_port *port, struct tty_struct *tty,
		struct file *filp);
int tty_port_install(struct tty_port *port, struct tty_driver *driver,
		struct tty_struct *tty);
int tty_port_open(struct tty_port *port, struct tty_struct *tty,
		struct file *filp);

static inline int tty_port_users(struct tty_port *port)
{
	return port->count + port->blocked_open;
}

#endif
