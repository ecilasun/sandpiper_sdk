

#ifndef _LINUX_CONSOLE_H_
#define _LINUX_CONSOLE_H_ 1

#include <linux/atomic.h>
#include <linux/bits.h>
#include <linux/rculist.h>
#include <linux/types.h>

struct vc_data;
struct console_font_op;
struct console_font;
struct module;
struct tty_struct;
struct notifier_block;

enum con_scroll {
	SM_UP,
	SM_DOWN,
};

enum vc_intensity;


struct consw {
	struct module *owner;
	const char *(*con_startup)(void);
	void	(*con_init)(struct vc_data *vc, int init);
	void	(*con_deinit)(struct vc_data *vc);
	void	(*con_clear)(struct vc_data *vc, int sy, int sx, int height,
			int width);
	void	(*con_putc)(struct vc_data *vc, int c, int ypos, int xpos);
	void	(*con_putcs)(struct vc_data *vc, const unsigned short *s,
			int count, int ypos, int xpos);
	void	(*con_cursor)(struct vc_data *vc, int mode);
	bool	(*con_scroll)(struct vc_data *vc, unsigned int top,
			unsigned int bottom, enum con_scroll dir,
			unsigned int lines);
	int	(*con_switch)(struct vc_data *vc);
	int	(*con_blank)(struct vc_data *vc, int blank, int mode_switch);
	int	(*con_font_set)(struct vc_data *vc, struct console_font *font,
			unsigned int vpitch, unsigned int flags);
	int	(*con_font_get)(struct vc_data *vc, struct console_font *font,
			unsigned int vpitch);
	int	(*con_font_default)(struct vc_data *vc,
			struct console_font *font, char *name);
	int     (*con_resize)(struct vc_data *vc, unsigned int width,
			unsigned int height, unsigned int user);
	void	(*con_set_palette)(struct vc_data *vc,
			const unsigned char *table);
	void	(*con_scrolldelta)(struct vc_data *vc, int lines);
	int	(*con_set_origin)(struct vc_data *vc);
	void	(*con_save_screen)(struct vc_data *vc);
	u8	(*con_build_attr)(struct vc_data *vc, u8 color,
			enum vc_intensity intensity,
			bool blink, bool underline, bool reverse, bool italic);
	void	(*con_invert_region)(struct vc_data *vc, u16 *p, int count);
	u16    *(*con_screen_pos)(const struct vc_data *vc, int offset);
	unsigned long (*con_getxy)(struct vc_data *vc, unsigned long position,
			int *px, int *py);
	
	void	(*con_flush_scrollback)(struct vc_data *vc);
	
	int	(*con_debug_enter)(struct vc_data *vc);
	
	int	(*con_debug_leave)(struct vc_data *vc);
};

extern const struct consw *conswitchp;

extern const struct consw dummy_con;	
extern const struct consw vga_con;	
extern const struct consw newport_con;	

int con_is_bound(const struct consw *csw);
int do_unregister_con_driver(const struct consw *csw);
int do_take_over_console(const struct consw *sw, int first, int last, int deflt);
void give_up_console(const struct consw *sw);
#ifdef CONFIG_HW_CONSOLE
int con_debug_enter(struct vc_data *vc);
int con_debug_leave(void);
#else
static inline int con_debug_enter(struct vc_data *vc)
{
	return 0;
}
static inline int con_debug_leave(void)
{
	return 0;
}
#endif


#define CM_DRAW     (1)
#define CM_ERASE    (2)
#define CM_MOVE     (3)




enum cons_flags {
	CON_PRINTBUFFER		= BIT(0),
	CON_CONSDEV		= BIT(1),
	CON_ENABLED		= BIT(2),
	CON_BOOT		= BIT(3),
	CON_ANYTIME		= BIT(4),
	CON_BRL			= BIT(5),
	CON_EXTENDED		= BIT(6),
	CON_SUSPENDED		= BIT(7),
};


struct console {
	char			name[16];
	void			(*write)(struct console *co, const char *s, unsigned int count);
	int			(*read)(struct console *co, char *s, unsigned int count);
	struct tty_driver	*(*device)(struct console *co, int *index);
	void			(*unblank)(void);
	int			(*setup)(struct console *co, char *options);
	int			(*exit)(struct console *co);
	int			(*match)(struct console *co, char *name, int idx, char *options);
	short			flags;
	short			index;
	int			cflag;
	uint			ispeed;
	uint			ospeed;
	u64			seq;
	unsigned long		dropped;
	void			*data;
	struct hlist_node	node;
};

#ifdef CONFIG_LOCKDEP
extern void lockdep_assert_console_list_lock_held(void);
#else
static inline void lockdep_assert_console_list_lock_held(void)
{
}
#endif

#ifdef CONFIG_DEBUG_LOCK_ALLOC
extern bool console_srcu_read_lock_is_held(void);
#else
static inline bool console_srcu_read_lock_is_held(void)
{
	return 1;
}
#endif

extern int console_srcu_read_lock(void);
extern void console_srcu_read_unlock(int cookie);

extern void console_list_lock(void) __acquires(console_mutex);
extern void console_list_unlock(void) __releases(console_mutex);

extern struct hlist_head console_list;


static inline short console_srcu_read_flags(const struct console *con)
{
	WARN_ON_ONCE(!console_srcu_read_lock_is_held());

	
	return data_race(READ_ONCE(con->flags));
}


static inline void console_srcu_write_flags(struct console *con, short flags)
{
	lockdep_assert_console_list_lock_held();

	
	WRITE_ONCE(con->flags, flags);
}


static inline bool console_is_registered_locked(const struct console *con)
{
	lockdep_assert_console_list_lock_held();
	return !hlist_unhashed(&con->node);
}


static inline bool console_is_registered(const struct console *con)
{
	bool ret;

	console_list_lock();
	ret = console_is_registered_locked(con);
	console_list_unlock();
	return ret;
}


#define for_each_console_srcu(con)					\
	hlist_for_each_entry_srcu(con, &console_list, node,		\
				  console_srcu_read_lock_is_held())


#define for_each_console(con)						\
	lockdep_assert_console_list_lock_held();			\
	hlist_for_each_entry(con, &console_list, node)

extern int console_set_on_cmdline;
extern struct console *early_console;

enum con_flush_mode {
	CONSOLE_FLUSH_PENDING,
	CONSOLE_REPLAY_ALL,
};

extern int add_preferred_console(char *name, int idx, char *options);
extern void console_force_preferred_locked(struct console *con);
extern void register_console(struct console *);
extern int unregister_console(struct console *);
extern void console_lock(void);
extern int console_trylock(void);
extern void console_unlock(void);
extern void console_conditional_schedule(void);
extern void console_unblank(void);
extern void console_flush_on_panic(enum con_flush_mode mode);
extern struct tty_driver *console_device(int *);
extern void console_stop(struct console *);
extern void console_start(struct console *);
extern int is_console_locked(void);
extern int braille_register_console(struct console *, int index,
		char *console_options, char *braille_options);
extern int braille_unregister_console(struct console *);
#ifdef CONFIG_TTY
extern void console_sysfs_notify(void);
#else
static inline void console_sysfs_notify(void)
{ }
#endif
extern bool console_suspend_enabled;


extern void suspend_console(void);
extern void resume_console(void);

int mda_console_init(void);

void vcs_make_sysfs(int index);
void vcs_remove_sysfs(int index);


#define WARN_CONSOLE_UNLOCKED()						\
	WARN_ON(!atomic_read(&ignore_console_lock_warning) &&		\
		!is_console_locked() && !oops_in_progress)

extern atomic_t ignore_console_lock_warning;


#define VESA_NO_BLANKING        0
#define VESA_VSYNC_SUSPEND      1
#define VESA_HSYNC_SUSPEND      2
#define VESA_POWERDOWN          3

extern void console_init(void);


void dummycon_register_output_notifier(struct notifier_block *nb);
void dummycon_unregister_output_notifier(struct notifier_block *nb);

#endif 
