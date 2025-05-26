/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_SPI_MEM_H
#define __LINUX_SPI_MEM_H

#include <linux/spi/spi.h>

#define SPI_MEM_OP_CMD(__opcode, __buswidth)			\
	{							\
		.buswidth = __buswidth,				\
		.opcode = __opcode,				\
		.nbytes = 1,					\
	}

#define SPI_MEM_OP_ADDR(__nbytes, __val, __buswidth)		\
	{							\
		.nbytes = __nbytes,				\
		.val = __val,					\
		.buswidth = __buswidth,				\
	}

#define SPI_MEM_OP_NO_ADDR	{ }

#define SPI_MEM_OP_DUMMY(__nbytes, __buswidth)			\
	{							\
		.nbytes = __nbytes,				\
		.buswidth = __buswidth,				\
	}

#define SPI_MEM_OP_NO_DUMMY	{ }

#define SPI_MEM_OP_DATA_IN(__nbytes, __buf, __buswidth)		\
	{							\
		.dir = SPI_MEM_DATA_IN,				\
		.nbytes = __nbytes,				\
		.buf.in = __buf,				\
		.buswidth = __buswidth,				\
	}

#define SPI_MEM_OP_DATA_OUT(__nbytes, __buf, __buswidth)	\
	{							\
		.dir = SPI_MEM_DATA_OUT,			\
		.nbytes = __nbytes,				\
		.buf.out = __buf,				\
		.buswidth = __buswidth,				\
	}

#define SPI_MEM_OP_NO_DATA	{ }

#define SPI_MEM_DEV_MAX_ID_LEN		6


enum spi_mem_data_dir {
	SPI_MEM_NO_DATA,
	SPI_MEM_DATA_IN,
	SPI_MEM_DATA_OUT,
};


struct spi_mem_op {
	struct {
		u8 nbytes;
		u8 buswidth;
		u8 dtr : 1;
		u8 __pad : 7;
		u16 opcode;
	} cmd;

	struct {
		u8 nbytes;
		u8 buswidth;
		u8 dtr : 1;
		u8 __pad : 7;
		u64 val;
	} addr;

	struct {
		u8 nbytes;
		u8 buswidth;
		u8 dtr : 1;
		u8 __pad : 7;
	} dummy;

	struct {
		u8 buswidth;
		u8 dtr : 1;
		u8 ecc : 1;
		u8 __pad : 6;
		enum spi_mem_data_dir dir;
		unsigned int nbytes;
		union {
			void *in;
			const void *out;
		} buf;
	} data;
};

#define SPI_MEM_OP(__cmd, __addr, __dummy, __data)		\
	{							\
		.cmd = __cmd,					\
		.addr = __addr,					\
		.dummy = __dummy,				\
		.data = __data,					\
	}


struct spi_mem_dirmap_info {
	struct spi_mem_op op_tmpl;
	u64 offset;
	u64 length;
};


struct spi_mem_dirmap_desc {
	struct spi_mem *mem;
	struct spi_mem_dirmap_info info;
	unsigned int nodirmap;
	void *priv;
};


struct spi_mem {
	struct spi_device *spi;
	void *drvpriv;
	const char *name;
	u8 device_id[SPI_MEM_DEV_MAX_ID_LEN];
	struct delayed_work	complete_work;
	struct completion	request_completion;
};


static inline void spi_mem_set_drvdata(struct spi_mem *mem, void *data)
{
	mem->drvpriv = data;
}


static inline void *spi_mem_get_drvdata(struct spi_mem *mem)
{
	return mem->drvpriv;
}


struct spi_controller_mem_ops {
	int (*adjust_op_size)(struct spi_mem *mem, struct spi_mem_op *op);
	bool (*supports_op)(struct spi_mem *mem,
			    const struct spi_mem_op *op);
	int (*exec_op)(struct spi_mem *mem,
		       const struct spi_mem_op *op);
	const char *(*get_name)(struct spi_mem *mem);
	int (*dirmap_create)(struct spi_mem_dirmap_desc *desc);
	void (*dirmap_destroy)(struct spi_mem_dirmap_desc *desc);
	ssize_t (*dirmap_read)(struct spi_mem_dirmap_desc *desc,
			       u64 offs, size_t len, void *buf);
	ssize_t (*dirmap_write)(struct spi_mem_dirmap_desc *desc,
				u64 offs, size_t len, const void *buf);
	int (*poll_status)(struct spi_mem *mem,
			   const struct spi_mem_op *op,
			   u16 mask, u16 match,
			   unsigned long initial_delay_us,
			   unsigned long polling_rate_us,
			   unsigned long timeout_ms);
};


struct spi_controller_mem_caps {
	bool dtr;
	bool ecc;
};

#define spi_mem_controller_is_capable(ctlr, cap)	\
	((ctlr)->mem_caps && (ctlr)->mem_caps->cap)


struct spi_mem_driver {
	struct spi_driver spidrv;
	int (*probe)(struct spi_mem *mem);
	int (*remove)(struct spi_mem *mem);
	void (*shutdown)(struct spi_mem *mem);
};

#if IS_ENABLED(CONFIG_SPI_MEM)
int spi_controller_dma_map_mem_op_data(struct spi_controller *ctlr,
				       const struct spi_mem_op *op,
				       struct sg_table *sg);

void spi_controller_dma_unmap_mem_op_data(struct spi_controller *ctlr,
					  const struct spi_mem_op *op,
					  struct sg_table *sg);

bool spi_mem_default_supports_op(struct spi_mem *mem,
				 const struct spi_mem_op *op);
#else
static inline int
spi_controller_dma_map_mem_op_data(struct spi_controller *ctlr,
				   const struct spi_mem_op *op,
				   struct sg_table *sg)
{
	return -ENOTSUPP;
}

static inline void
spi_controller_dma_unmap_mem_op_data(struct spi_controller *ctlr,
				     const struct spi_mem_op *op,
				     struct sg_table *sg)
{
}

static inline
bool spi_mem_default_supports_op(struct spi_mem *mem,
				 const struct spi_mem_op *op)
{
	return false;
}
#endif 

int spi_mem_adjust_op_size(struct spi_mem *mem, struct spi_mem_op *op);

bool spi_mem_supports_op(struct spi_mem *mem,
			 const struct spi_mem_op *op);

int spi_mem_exec_op(struct spi_mem *mem,
		    const struct spi_mem_op *op);

const char *spi_mem_get_name(struct spi_mem *mem);

struct spi_mem_dirmap_desc *
spi_mem_dirmap_create(struct spi_mem *mem,
		      const struct spi_mem_dirmap_info *info);
void spi_mem_dirmap_destroy(struct spi_mem_dirmap_desc *desc);
ssize_t spi_mem_dirmap_read(struct spi_mem_dirmap_desc *desc,
			    u64 offs, size_t len, void *buf);
ssize_t spi_mem_dirmap_write(struct spi_mem_dirmap_desc *desc,
			     u64 offs, size_t len, const void *buf);
struct spi_mem_dirmap_desc *
devm_spi_mem_dirmap_create(struct device *dev, struct spi_mem *mem,
			   const struct spi_mem_dirmap_info *info);
void devm_spi_mem_dirmap_destroy(struct device *dev,
				 struct spi_mem_dirmap_desc *desc);

int spi_mem_poll_status(struct spi_mem *mem,
			const struct spi_mem_op *op,
			u16 mask, u16 match,
			unsigned long initial_delay_us,
			unsigned long polling_delay_us,
			u16 timeout_ms);

int spi_mem_driver_register_with_owner(struct spi_mem_driver *drv,
				       struct module *owner);

void spi_mem_driver_unregister(struct spi_mem_driver *drv);

#define spi_mem_driver_register(__drv)                                  \
	spi_mem_driver_register_with_owner(__drv, THIS_MODULE)

#define module_spi_mem_driver(__drv)                                    \
	module_driver(__drv, spi_mem_driver_register,                   \
		      spi_mem_driver_unregister)

#endif 
