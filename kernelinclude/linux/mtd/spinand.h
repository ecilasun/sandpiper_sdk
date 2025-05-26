/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_MTD_SPINAND_H
#define __LINUX_MTD_SPINAND_H

#include <linux/mutex.h>
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-mem.h>



#define SPINAND_RESET_OP						\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xff, 1),				\
		   SPI_MEM_OP_NO_ADDR,					\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_WR_EN_DIS_OP(enable)					\
	SPI_MEM_OP(SPI_MEM_OP_CMD((enable) ? 0x06 : 0x04, 1),		\
		   SPI_MEM_OP_NO_ADDR,					\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_READID_OP(naddr, ndummy, buf, len)			\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x9f, 1),				\
		   SPI_MEM_OP_ADDR(naddr, 0, 1),			\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_SET_FEATURE_OP(reg, valptr)				\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x1f, 1),				\
		   SPI_MEM_OP_ADDR(1, reg, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_DATA_OUT(1, valptr, 1))

#define SPINAND_GET_FEATURE_OP(reg, valptr)				\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x0f, 1),				\
		   SPI_MEM_OP_ADDR(1, reg, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_DATA_IN(1, valptr, 1))

#define SPINAND_BLK_ERASE_OP(addr)					\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xd8, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PAGE_READ_OP(addr)					\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x13, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PAGE_READ_FROM_CACHE_OP(fast, addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   SPI_MEM_OP_ADDR(2, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_PAGE_READ_FROM_CACHE_OP_3A(fast, addr, ndummy, buf, len) \
	SPI_MEM_OP(SPI_MEM_OP_CMD(fast ? 0x0b : 0x03, 1),		\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 1))

#define SPINAND_PAGE_READ_FROM_CACHE_X2_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x3b, 1),				\
		   SPI_MEM_OP_ADDR(2, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_X2_OP_3A(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x3b, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_X4_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x6b, 1),				\
		   SPI_MEM_OP_ADDR(2, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_X4_OP_3A(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x6b, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_DUMMY(ndummy, 1),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xbb, 1),				\
		   SPI_MEM_OP_ADDR(2, addr, 2),				\
		   SPI_MEM_OP_DUMMY(ndummy, 2),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_DUALIO_OP_3A(addr, ndummy, buf, len) \
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xbb, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 2),				\
		   SPI_MEM_OP_DUMMY(ndummy, 2),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 2))

#define SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP(addr, ndummy, buf, len)	\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xeb, 1),				\
		   SPI_MEM_OP_ADDR(2, addr, 4),				\
		   SPI_MEM_OP_DUMMY(ndummy, 4),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PAGE_READ_FROM_CACHE_QUADIO_OP_3A(addr, ndummy, buf, len) \
	SPI_MEM_OP(SPI_MEM_OP_CMD(0xeb, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 4),				\
		   SPI_MEM_OP_DUMMY(ndummy, 4),				\
		   SPI_MEM_OP_DATA_IN(len, buf, 4))

#define SPINAND_PROG_EXEC_OP(addr)					\
	SPI_MEM_OP(SPI_MEM_OP_CMD(0x10, 1),				\
		   SPI_MEM_OP_ADDR(3, addr, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_NO_DATA)

#define SPINAND_PROG_LOAD(reset, addr, buf, len)			\
	SPI_MEM_OP(SPI_MEM_OP_CMD(reset ? 0x02 : 0x84, 1),		\
		   SPI_MEM_OP_ADDR(2, addr, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_DATA_OUT(len, buf, 1))

#define SPINAND_PROG_LOAD_X4(reset, addr, buf, len)			\
	SPI_MEM_OP(SPI_MEM_OP_CMD(reset ? 0x32 : 0x34, 1),		\
		   SPI_MEM_OP_ADDR(2, addr, 1),				\
		   SPI_MEM_OP_NO_DUMMY,					\
		   SPI_MEM_OP_DATA_OUT(len, buf, 4))


#define SPINAND_CMD_PROG_LOAD_X4		0x32
#define SPINAND_CMD_PROG_LOAD_RDM_DATA_X4	0x34


#define REG_BLOCK_LOCK		0xa0
#define BL_ALL_UNLOCKED		0x00


#define REG_CFG			0xb0
#define CFG_OTP_ENABLE		BIT(6)
#define CFG_ECC_ENABLE		BIT(4)
#define CFG_QUAD_ENABLE		BIT(0)


#define REG_STATUS		0xc0
#define STATUS_BUSY		BIT(0)
#define STATUS_ERASE_FAILED	BIT(2)
#define STATUS_PROG_FAILED	BIT(3)
#define STATUS_ECC_MASK		GENMASK(5, 4)
#define STATUS_ECC_NO_BITFLIPS	(0 << 4)
#define STATUS_ECC_HAS_BITFLIPS	(1 << 4)
#define STATUS_ECC_UNCOR_ERROR	(2 << 4)

struct spinand_op;
struct spinand_device;

#define SPINAND_MAX_ID_LEN	5

#define SPINAND_READ_INITIAL_DELAY_US	6
#define SPINAND_READ_POLL_DELAY_US	5
#define SPINAND_RESET_INITIAL_DELAY_US	5
#define SPINAND_RESET_POLL_DELAY_US	5
#define SPINAND_WRITE_INITIAL_DELAY_US	75
#define SPINAND_WRITE_POLL_DELAY_US	15
#define SPINAND_ERASE_INITIAL_DELAY_US	250
#define SPINAND_ERASE_POLL_DELAY_US	50

#define SPINAND_WAITRDY_TIMEOUT_MS	400


struct spinand_id {
	u8 data[SPINAND_MAX_ID_LEN];
	int len;
};

enum spinand_readid_method {
	SPINAND_READID_METHOD_OPCODE,
	SPINAND_READID_METHOD_OPCODE_ADDR,
	SPINAND_READID_METHOD_OPCODE_DUMMY,
};


struct spinand_devid {
	const u8 *id;
	const u8 len;
	const enum spinand_readid_method method;
};


struct spinand_manufacturer_ops {
	int (*init)(struct spinand_device *spinand);
	void (*cleanup)(struct spinand_device *spinand);
};


struct spinand_manufacturer {
	u8 id;
	char *name;
	const struct spinand_info *chips;
	const size_t nchips;
	const struct spinand_manufacturer_ops *ops;
};


extern const struct spinand_manufacturer alliancememory_spinand_manufacturer;
extern const struct spinand_manufacturer ato_spinand_manufacturer;
extern const struct spinand_manufacturer esmt_c8_spinand_manufacturer;
extern const struct spinand_manufacturer gigadevice_spinand_manufacturer;
extern const struct spinand_manufacturer macronix_spinand_manufacturer;
extern const struct spinand_manufacturer micron_spinand_manufacturer;
extern const struct spinand_manufacturer paragon_spinand_manufacturer;
extern const struct spinand_manufacturer toshiba_spinand_manufacturer;
extern const struct spinand_manufacturer winbond_spinand_manufacturer;
extern const struct spinand_manufacturer xtx_spinand_manufacturer;


struct spinand_op_variants {
	const struct spi_mem_op *ops;
	unsigned int nops;
};

#define SPINAND_OP_VARIANTS(name, ...)					\
	const struct spinand_op_variants name = {			\
		.ops = (struct spi_mem_op[]) { __VA_ARGS__ },		\
		.nops = sizeof((struct spi_mem_op[]){ __VA_ARGS__ }) /	\
			sizeof(struct spi_mem_op),			\
	}


struct spinand_ecc_info {
	int (*get_status)(struct spinand_device *spinand, u8 status);
	const struct mtd_ooblayout_ops *ooblayout;
};

#define SPINAND_HAS_QE_BIT		BIT(0)
#define SPINAND_HAS_CR_FEAT_BIT		BIT(1)


struct spinand_ondie_ecc_conf {
	u8 status;
};


struct spinand_info {
	const char *model;
	struct spinand_devid devid;
	u32 flags;
	struct nand_memory_organization memorg;
	struct nand_ecc_props eccreq;
	struct spinand_ecc_info eccinfo;
	struct {
		const struct spinand_op_variants *read_cache;
		const struct spinand_op_variants *write_cache;
		const struct spinand_op_variants *update_cache;
	} op_variants;
	int (*select_target)(struct spinand_device *spinand,
			     unsigned int target);
};

#define SPINAND_ID(__method, ...)					\
	{								\
		.id = (const u8[]){ __VA_ARGS__ },			\
		.len = sizeof((u8[]){ __VA_ARGS__ }),			\
		.method = __method,					\
	}

#define SPINAND_INFO_OP_VARIANTS(__read, __write, __update)		\
	{								\
		.read_cache = __read,					\
		.write_cache = __write,					\
		.update_cache = __update,				\
	}

#define SPINAND_ECCINFO(__ooblayout, __get_status)			\
	.eccinfo = {							\
		.ooblayout = __ooblayout,				\
		.get_status = __get_status,				\
	}

#define SPINAND_SELECT_TARGET(__func)					\
	.select_target = __func,

#define SPINAND_INFO(__model, __id, __memorg, __eccreq, __op_variants,	\
		     __flags, ...)					\
	{								\
		.model = __model,					\
		.devid = __id,						\
		.memorg = __memorg,					\
		.eccreq = __eccreq,					\
		.op_variants = __op_variants,				\
		.flags = __flags,					\
		__VA_ARGS__						\
	}

struct spinand_dirmap {
	struct spi_mem_dirmap_desc *wdesc;
	struct spi_mem_dirmap_desc *rdesc;
	struct spi_mem_dirmap_desc *wdesc_ecc;
	struct spi_mem_dirmap_desc *rdesc_ecc;
};


struct spinand_device {
	struct nand_device base;
	struct spi_mem *spimem;
	struct mutex lock;
	struct spinand_id id;
	u32 flags;

	struct {
		const struct spi_mem_op *read_cache;
		const struct spi_mem_op *write_cache;
		const struct spi_mem_op *update_cache;
	} op_templates;

	struct spinand_dirmap *dirmaps;

	int (*select_target)(struct spinand_device *spinand,
			     unsigned int target);
	unsigned int cur_target;

	struct spinand_ecc_info eccinfo;

	u8 *cfg_cache;
	u8 *databuf;
	u8 *oobbuf;
	u8 *scratchbuf;
	const struct spinand_manufacturer *manufacturer;
	void *priv;
};


static inline struct spinand_device *mtd_to_spinand(struct mtd_info *mtd)
{
	return container_of(mtd_to_nanddev(mtd), struct spinand_device, base);
}


static inline struct mtd_info *spinand_to_mtd(struct spinand_device *spinand)
{
	return nanddev_to_mtd(&spinand->base);
}


static inline struct spinand_device *nand_to_spinand(struct nand_device *nand)
{
	return container_of(nand, struct spinand_device, base);
}


static inline struct nand_device *
spinand_to_nand(struct spinand_device *spinand)
{
	return &spinand->base;
}


static inline void spinand_set_of_node(struct spinand_device *spinand,
				       struct device_node *np)
{
	nanddev_set_of_node(&spinand->base, np);
}

int spinand_match_and_init(struct spinand_device *spinand,
			   const struct spinand_info *table,
			   unsigned int table_size,
			   enum spinand_readid_method rdid_method);

int spinand_upd_cfg(struct spinand_device *spinand, u8 mask, u8 val);
int spinand_select_target(struct spinand_device *spinand, unsigned int target);

#endif 
