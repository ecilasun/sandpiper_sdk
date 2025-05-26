/* SPDX-License-Identifier: GPL-2.0 */


#ifndef LINUX_NUBUS_H
#define LINUX_NUBUS_H

#include <linux/device.h>
#include <asm/nubus.h>
#include <uapi/linux/nubus.h>

struct proc_dir_entry;
struct seq_file;

struct nubus_dir {
	unsigned char *base;
	unsigned char *ptr;
	int done;
	int mask;
	struct proc_dir_entry *procdir;
};

struct nubus_dirent {
	unsigned char *base;
	unsigned char type;
	__u32 data;	
	int mask;
};

struct nubus_board {
	struct device dev;

	
	int slot;
	
	char name[64];

	
	unsigned char *fblock;
	
	unsigned char *directory;

	unsigned long slot_addr;
	
	unsigned long doffset;
	
	unsigned long rom_length;
	
	unsigned long crc;
	unsigned char rev;
	unsigned char format;
	unsigned char lanes;

	
	struct proc_dir_entry *procdir;
};

struct nubus_rsrc {
	struct list_head list;

	
	unsigned char resid;
	
	unsigned short category;
	unsigned short type;
	unsigned short dr_sw;
	unsigned short dr_hw;

	
	unsigned char *directory;
	
	struct nubus_board *board;
};


extern struct list_head nubus_func_rsrcs;

struct nubus_driver {
	struct device_driver driver;
	int (*probe)(struct nubus_board *board);
	void (*remove)(struct nubus_board *board);
};

extern struct bus_type nubus_bus_type;


#ifdef CONFIG_PROC_FS
extern bool nubus_populate_procfs;
void nubus_proc_init(void);
struct proc_dir_entry *nubus_proc_add_board(struct nubus_board *board);
struct proc_dir_entry *nubus_proc_add_rsrc_dir(struct proc_dir_entry *procdir,
					       const struct nubus_dirent *ent,
					       struct nubus_board *board);
void nubus_proc_add_rsrc_mem(struct proc_dir_entry *procdir,
			     const struct nubus_dirent *ent,
			     unsigned int size);
void nubus_proc_add_rsrc(struct proc_dir_entry *procdir,
			 const struct nubus_dirent *ent);
#else
static inline void nubus_proc_init(void) {}
static inline
struct proc_dir_entry *nubus_proc_add_board(struct nubus_board *board)
{ return NULL; }
static inline
struct proc_dir_entry *nubus_proc_add_rsrc_dir(struct proc_dir_entry *procdir,
					       const struct nubus_dirent *ent,
					       struct nubus_board *board)
{ return NULL; }
static inline void nubus_proc_add_rsrc_mem(struct proc_dir_entry *procdir,
					   const struct nubus_dirent *ent,
					   unsigned int size) {}
static inline void nubus_proc_add_rsrc(struct proc_dir_entry *procdir,
				       const struct nubus_dirent *ent) {}
#endif

struct nubus_rsrc *nubus_first_rsrc_or_null(void);
struct nubus_rsrc *nubus_next_rsrc_or_null(struct nubus_rsrc *from);

#define for_each_func_rsrc(f) \
	for (f = nubus_first_rsrc_or_null(); f; f = nubus_next_rsrc_or_null(f))

#define for_each_board_func_rsrc(b, f) \
	for_each_func_rsrc(f) if (f->board != b) {} else




int nubus_get_root_dir(const struct nubus_board *board,
		       struct nubus_dir *dir);

int nubus_get_board_dir(const struct nubus_board *board,
			struct nubus_dir *dir);

int nubus_get_func_dir(const struct nubus_rsrc *fres, struct nubus_dir *dir);


int nubus_readdir(struct nubus_dir *dir,
		  struct nubus_dirent *ent);
int nubus_find_rsrc(struct nubus_dir *dir,
		    unsigned char rsrc_type,
		    struct nubus_dirent *ent);
int nubus_rewinddir(struct nubus_dir *dir);


int nubus_get_subdir(const struct nubus_dirent *ent,
		     struct nubus_dir *dir);
void nubus_get_rsrc_mem(void *dest, const struct nubus_dirent *dirent,
			unsigned int len);
unsigned int nubus_get_rsrc_str(char *dest, const struct nubus_dirent *dirent,
				unsigned int len);
void nubus_seq_write_rsrc_mem(struct seq_file *m,
			      const struct nubus_dirent *dirent,
			      unsigned int len);
unsigned char *nubus_dirptr(const struct nubus_dirent *nd);


int nubus_parent_device_register(void);
int nubus_device_register(struct nubus_board *board);
int nubus_driver_register(struct nubus_driver *ndrv);
void nubus_driver_unregister(struct nubus_driver *ndrv);
int nubus_proc_show(struct seq_file *m, void *data);

static inline void nubus_set_drvdata(struct nubus_board *board, void *data)
{
	dev_set_drvdata(&board->dev, data);
}

static inline void *nubus_get_drvdata(struct nubus_board *board)
{
	return dev_get_drvdata(&board->dev);
}


static inline void *nubus_slot_addr(int slot)
{
	return (void *)(0xF0000000 | (slot << 24));
}

#endif 
