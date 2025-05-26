/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_MTD_RAWNAND_H
#define __LINUX_MTD_RAWNAND_H

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/flashchip.h>
#include <linux/mtd/bbm.h>
#include <linux/mtd/jedec.h>
#include <linux/mtd/onfi.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/types.h>

struct nand_chip;
struct gpio_desc;


#define NAND_MAX_CHIPS		8



#define NAND_NCE		0x01

#define NAND_CLE		0x02

#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80


#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff


#define NAND_CMD_READSTART	0x30
#define NAND_CMD_READCACHESEQ	0x31
#define NAND_CMD_READCACHEEND	0x3f
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

#define NAND_CMD_NONE		-1


#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

#define NAND_DATA_IFACE_CHECK_ONLY	-1



#define NAND_ECC_READ		0

#define NAND_ECC_WRITE		1

#define NAND_ECC_READSYN	2


#define NAND_ECC_GENERIC_ERASED_CHECK	BIT(0)




#define NAND_BUSWIDTH_16	BIT(1)


#define NAND_ECC_SOFT_HAMMING_SM_ORDER	BIT(2)


#define NAND_CACHEPRG		BIT(3)

#define NAND_SAMSUNG_LP_OPTIONS NAND_CACHEPRG


#define NAND_NEED_READRDY	BIT(8)


#define NAND_NO_SUBPAGE_WRITE	BIT(9)


#define NAND_BROKEN_XD		BIT(10)


#define NAND_ROM		BIT(11)


#define NAND_SUBPAGE_READ	BIT(12)

#define NAND_HAS_SUBPAGE_READ(chip) ((chip->options & NAND_SUBPAGE_READ))


#define NAND_NEED_SCRAMBLING	BIT(13)


#define NAND_ROW_ADDR_3		BIT(14)



#define NAND_SKIP_BBTSCAN	BIT(16)

#define NAND_SCAN_SILENT_NODEV	BIT(18)


#define NAND_BUSWIDTH_AUTO      BIT(19)


#define NAND_USES_DMA		BIT(20)


#define NAND_WAIT_TCCS		BIT(21)


#define NAND_IS_BOOT_MEDIUM	BIT(22)


#define NAND_KEEP_TIMINGS	BIT(23)


#define NAND_BBM_FIRSTPAGE	BIT(24)
#define NAND_BBM_SECONDPAGE	BIT(25)
#define NAND_BBM_LASTPAGE	BIT(26)


#define NAND_NO_BBM_QUIRK	BIT(27)


#define NAND_CI_CHIPNR_MSK	0x03
#define NAND_CI_CELLTYPE_MSK	0x0C
#define NAND_CI_CELLTYPE_SHIFT	2


#define NAND_BBM_POS_SMALL		5
#define NAND_BBM_POS_LARGE		0


struct nand_parameters {
	
	const char *model;
	bool supports_set_get_features;
	bool supports_read_cache;
	DECLARE_BITMAP(set_feature_list, ONFI_FEATURE_NUMBER);
	DECLARE_BITMAP(get_feature_list, ONFI_FEATURE_NUMBER);

	
	struct onfi_params *onfi;
};


#define NAND_MAX_ID_LEN 8


struct nand_id {
	u8 data[NAND_MAX_ID_LEN];
	int len;
};


struct nand_ecc_step_info {
	int stepsize;
	const int *strengths;
	int nstrengths;
};


struct nand_ecc_caps {
	const struct nand_ecc_step_info *stepinfos;
	int nstepinfos;
	int (*calc_ecc_bytes)(int step_size, int strength);
};


#define NAND_ECC_CAPS_SINGLE(__name, __calc, __step, ...)	\
static const int __name##_strengths[] = { __VA_ARGS__ };	\
static const struct nand_ecc_step_info __name##_stepinfo = {	\
	.stepsize = __step,					\
	.strengths = __name##_strengths,			\
	.nstrengths = ARRAY_SIZE(__name##_strengths),		\
};								\
static const struct nand_ecc_caps __name = {			\
	.stepinfos = &__name##_stepinfo,			\
	.nstepinfos = 1,					\
	.calc_ecc_bytes = __calc,				\
}


struct nand_ecc_ctrl {
	enum nand_ecc_engine_type engine_type;
	enum nand_ecc_placement placement;
	enum nand_ecc_algo algo;
	int steps;
	int size;
	int bytes;
	int total;
	int strength;
	int prepad;
	int postpad;
	unsigned int options;
	u8 *calc_buf;
	u8 *code_buf;
	void (*hwctl)(struct nand_chip *chip, int mode);
	int (*calculate)(struct nand_chip *chip, const uint8_t *dat,
			 uint8_t *ecc_code);
	int (*correct)(struct nand_chip *chip, uint8_t *dat, uint8_t *read_ecc,
		       uint8_t *calc_ecc);
	int (*read_page_raw)(struct nand_chip *chip, uint8_t *buf,
			     int oob_required, int page);
	int (*write_page_raw)(struct nand_chip *chip, const uint8_t *buf,
			      int oob_required, int page);
	int (*read_page)(struct nand_chip *chip, uint8_t *buf,
			 int oob_required, int page);
	int (*read_subpage)(struct nand_chip *chip, uint32_t offs,
			    uint32_t len, uint8_t *buf, int page);
	int (*write_subpage)(struct nand_chip *chip, uint32_t offset,
			     uint32_t data_len, const uint8_t *data_buf,
			     int oob_required, int page);
	int (*write_page)(struct nand_chip *chip, const uint8_t *buf,
			  int oob_required, int page);
	int (*write_oob_raw)(struct nand_chip *chip, int page);
	int (*read_oob_raw)(struct nand_chip *chip, int page);
	int (*read_oob)(struct nand_chip *chip, int page);
	int (*write_oob)(struct nand_chip *chip, int page);
};


struct nand_sdr_timings {
	u64 tBERS_max;
	u32 tCCS_min;
	u64 tPROG_max;
	u64 tR_max;
	u32 tALH_min;
	u32 tADL_min;
	u32 tALS_min;
	u32 tAR_min;
	u32 tCEA_max;
	u32 tCEH_min;
	u32 tCH_min;
	u32 tCHZ_max;
	u32 tCLH_min;
	u32 tCLR_min;
	u32 tCLS_min;
	u32 tCOH_min;
	u32 tCS_min;
	u32 tDH_min;
	u32 tDS_min;
	u32 tFEAT_max;
	u32 tIR_min;
	u32 tITC_max;
	u32 tRC_min;
	u32 tREA_max;
	u32 tREH_min;
	u32 tRHOH_min;
	u32 tRHW_min;
	u32 tRHZ_max;
	u32 tRLOH_min;
	u32 tRP_min;
	u32 tRR_min;
	u64 tRST_max;
	u32 tWB_max;
	u32 tWC_min;
	u32 tWH_min;
	u32 tWHR_min;
	u32 tWP_min;
	u32 tWW_min;
};


struct nand_nvddr_timings {
	u64 tBERS_max;
	u32 tCCS_min;
	u64 tPROG_max;
	u64 tR_max;
	u32 tAC_min;
	u32 tAC_max;
	u32 tADL_min;
	u32 tCAD_min;
	u32 tCAH_min;
	u32 tCALH_min;
	u32 tCALS_min;
	u32 tCAS_min;
	u32 tCEH_min;
	u32 tCH_min;
	u32 tCK_min;
	u32 tCS_min;
	u32 tDH_min;
	u32 tDQSCK_min;
	u32 tDQSCK_max;
	u32 tDQSD_min;
	u32 tDQSD_max;
	u32 tDQSHZ_max;
	u32 tDQSQ_max;
	u32 tDS_min;
	u32 tDSC_min;
	u32 tFEAT_max;
	u32 tITC_max;
	u32 tQHS_max;
	u32 tRHW_min;
	u32 tRR_min;
	u32 tRST_max;
	u32 tWB_max;
	u32 tWHR_min;
	u32 tWRCK_min;
	u32 tWW_min;
};


#define NAND_COMMON_TIMING_PS(conf, timing_name)		\
	nand_interface_is_sdr(conf) ?				\
		nand_get_sdr_timings(conf)->timing_name :	\
		nand_get_nvddr_timings(conf)->timing_name

#define NAND_COMMON_TIMING_MS(conf, timing_name) \
	PSEC_TO_MSEC(NAND_COMMON_TIMING_PS((conf), timing_name))

#define NAND_COMMON_TIMING_NS(conf, timing_name) \
	PSEC_TO_NSEC(NAND_COMMON_TIMING_PS((conf), timing_name))


enum nand_interface_type {
	NAND_SDR_IFACE,
	NAND_NVDDR_IFACE,
};


struct nand_interface_config {
	enum nand_interface_type type;
	struct nand_timings {
		unsigned int mode;
		union {
			struct nand_sdr_timings sdr;
			struct nand_nvddr_timings nvddr;
		};
	} timings;
};


static bool nand_interface_is_sdr(const struct nand_interface_config *conf)
{
	return conf->type == NAND_SDR_IFACE;
}


static bool nand_interface_is_nvddr(const struct nand_interface_config *conf)
{
	return conf->type == NAND_NVDDR_IFACE;
}


static inline const struct nand_sdr_timings *
nand_get_sdr_timings(const struct nand_interface_config *conf)
{
	if (!nand_interface_is_sdr(conf))
		return ERR_PTR(-EINVAL);

	return &conf->timings.sdr;
}


static inline const struct nand_nvddr_timings *
nand_get_nvddr_timings(const struct nand_interface_config *conf)
{
	if (!nand_interface_is_nvddr(conf))
		return ERR_PTR(-EINVAL);

	return &conf->timings.nvddr;
}


struct nand_op_cmd_instr {
	u8 opcode;
};


struct nand_op_addr_instr {
	unsigned int naddrs;
	const u8 *addrs;
};


struct nand_op_data_instr {
	unsigned int len;
	union {
		void *in;
		const void *out;
	} buf;
	bool force_8bit;
};


struct nand_op_waitrdy_instr {
	unsigned int timeout_ms;
};


enum nand_op_instr_type {
	NAND_OP_CMD_INSTR,
	NAND_OP_ADDR_INSTR,
	NAND_OP_DATA_IN_INSTR,
	NAND_OP_DATA_OUT_INSTR,
	NAND_OP_WAITRDY_INSTR,
};


struct nand_op_instr {
	enum nand_op_instr_type type;
	union {
		struct nand_op_cmd_instr cmd;
		struct nand_op_addr_instr addr;
		struct nand_op_data_instr data;
		struct nand_op_waitrdy_instr waitrdy;
	} ctx;
	unsigned int delay_ns;
};


#define __DIVIDE(dividend, divisor) ({						\
	(__typeof__(dividend))(sizeof(dividend) <= sizeof(unsigned long) ?	\
			       DIV_ROUND_UP(dividend, divisor) :		\
			       DIV_ROUND_UP_ULL(dividend, divisor)); 		\
	})
#define PSEC_TO_NSEC(x) __DIVIDE(x, 1000)
#define PSEC_TO_MSEC(x) __DIVIDE(x, 1000000000)

#define NAND_OP_CMD(id, ns)						\
	{								\
		.type = NAND_OP_CMD_INSTR,				\
		.ctx.cmd.opcode = id,					\
		.delay_ns = ns,						\
	}

#define NAND_OP_ADDR(ncycles, cycles, ns)				\
	{								\
		.type = NAND_OP_ADDR_INSTR,				\
		.ctx.addr = {						\
			.naddrs = ncycles,				\
			.addrs = cycles,				\
		},							\
		.delay_ns = ns,						\
	}

#define NAND_OP_DATA_IN(l, b, ns)					\
	{								\
		.type = NAND_OP_DATA_IN_INSTR,				\
		.ctx.data = {						\
			.len = l,					\
			.buf.in = b,					\
			.force_8bit = false,				\
		},							\
		.delay_ns = ns,						\
	}

#define NAND_OP_DATA_OUT(l, b, ns)					\
	{								\
		.type = NAND_OP_DATA_OUT_INSTR,				\
		.ctx.data = {						\
			.len = l,					\
			.buf.out = b,					\
			.force_8bit = false,				\
		},							\
		.delay_ns = ns,						\
	}

#define NAND_OP_8BIT_DATA_IN(l, b, ns)					\
	{								\
		.type = NAND_OP_DATA_IN_INSTR,				\
		.ctx.data = {						\
			.len = l,					\
			.buf.in = b,					\
			.force_8bit = true,				\
		},							\
		.delay_ns = ns,						\
	}

#define NAND_OP_8BIT_DATA_OUT(l, b, ns)					\
	{								\
		.type = NAND_OP_DATA_OUT_INSTR,				\
		.ctx.data = {						\
			.len = l,					\
			.buf.out = b,					\
			.force_8bit = true,				\
		},							\
		.delay_ns = ns,						\
	}

#define NAND_OP_WAIT_RDY(tout_ms, ns)					\
	{								\
		.type = NAND_OP_WAITRDY_INSTR,				\
		.ctx.waitrdy.timeout_ms = tout_ms,			\
		.delay_ns = ns,						\
	}


struct nand_subop {
	unsigned int cs;
	const struct nand_op_instr *instrs;
	unsigned int ninstrs;
	unsigned int first_instr_start_off;
	unsigned int last_instr_end_off;
};

unsigned int nand_subop_get_addr_start_off(const struct nand_subop *subop,
					   unsigned int op_id);
unsigned int nand_subop_get_num_addr_cyc(const struct nand_subop *subop,
					 unsigned int op_id);
unsigned int nand_subop_get_data_start_off(const struct nand_subop *subop,
					   unsigned int op_id);
unsigned int nand_subop_get_data_len(const struct nand_subop *subop,
				     unsigned int op_id);


struct nand_op_parser_addr_constraints {
	unsigned int maxcycles;
};


struct nand_op_parser_data_constraints {
	unsigned int maxlen;
};


struct nand_op_parser_pattern_elem {
	enum nand_op_instr_type type;
	bool optional;
	union {
		struct nand_op_parser_addr_constraints addr;
		struct nand_op_parser_data_constraints data;
	} ctx;
};

#define NAND_OP_PARSER_PAT_CMD_ELEM(_opt)			\
	{							\
		.type = NAND_OP_CMD_INSTR,			\
		.optional = _opt,				\
	}

#define NAND_OP_PARSER_PAT_ADDR_ELEM(_opt, _maxcycles)		\
	{							\
		.type = NAND_OP_ADDR_INSTR,			\
		.optional = _opt,				\
		.ctx.addr.maxcycles = _maxcycles,		\
	}

#define NAND_OP_PARSER_PAT_DATA_IN_ELEM(_opt, _maxlen)		\
	{							\
		.type = NAND_OP_DATA_IN_INSTR,			\
		.optional = _opt,				\
		.ctx.data.maxlen = _maxlen,			\
	}

#define NAND_OP_PARSER_PAT_DATA_OUT_ELEM(_opt, _maxlen)		\
	{							\
		.type = NAND_OP_DATA_OUT_INSTR,			\
		.optional = _opt,				\
		.ctx.data.maxlen = _maxlen,			\
	}

#define NAND_OP_PARSER_PAT_WAITRDY_ELEM(_opt)			\
	{							\
		.type = NAND_OP_WAITRDY_INSTR,			\
		.optional = _opt,				\
	}


struct nand_op_parser_pattern {
	const struct nand_op_parser_pattern_elem *elems;
	unsigned int nelems;
	int (*exec)(struct nand_chip *chip, const struct nand_subop *subop);
};

#define NAND_OP_PARSER_PATTERN(_exec, ...)							\
	{											\
		.exec = _exec,									\
		.elems = (const struct nand_op_parser_pattern_elem[]) { __VA_ARGS__ },		\
		.nelems = sizeof((struct nand_op_parser_pattern_elem[]) { __VA_ARGS__ }) /	\
			  sizeof(struct nand_op_parser_pattern_elem),				\
	}


struct nand_op_parser {
	const struct nand_op_parser_pattern *patterns;
	unsigned int npatterns;
};

#define NAND_OP_PARSER(...)									\
	{											\
		.patterns = (const struct nand_op_parser_pattern[]) { __VA_ARGS__ },		\
		.npatterns = sizeof((struct nand_op_parser_pattern[]) { __VA_ARGS__ }) /	\
			     sizeof(struct nand_op_parser_pattern),				\
	}


struct nand_operation {
	unsigned int cs;
	const struct nand_op_instr *instrs;
	unsigned int ninstrs;
};

#define NAND_OPERATION(_cs, _instrs)				\
	{							\
		.cs = _cs,					\
		.instrs = _instrs,				\
		.ninstrs = ARRAY_SIZE(_instrs),			\
	}

int nand_op_parser_exec_op(struct nand_chip *chip,
			   const struct nand_op_parser *parser,
			   const struct nand_operation *op, bool check_only);

static inline void nand_op_trace(const char *prefix,
				 const struct nand_op_instr *instr)
{
#if IS_ENABLED(CONFIG_DYNAMIC_DEBUG) || defined(DEBUG)
	switch (instr->type) {
	case NAND_OP_CMD_INSTR:
		pr_debug("%sCMD      [0x%02x]\n", prefix,
			 instr->ctx.cmd.opcode);
		break;
	case NAND_OP_ADDR_INSTR:
		pr_debug("%sADDR     [%d cyc: %*ph]\n", prefix,
			 instr->ctx.addr.naddrs,
			 instr->ctx.addr.naddrs < 64 ?
			 instr->ctx.addr.naddrs : 64,
			 instr->ctx.addr.addrs);
		break;
	case NAND_OP_DATA_IN_INSTR:
		pr_debug("%sDATA_IN  [%d B%s]\n", prefix,
			 instr->ctx.data.len,
			 instr->ctx.data.force_8bit ?
			 ", force 8-bit" : "");
		break;
	case NAND_OP_DATA_OUT_INSTR:
		pr_debug("%sDATA_OUT [%d B%s]\n", prefix,
			 instr->ctx.data.len,
			 instr->ctx.data.force_8bit ?
			 ", force 8-bit" : "");
		break;
	case NAND_OP_WAITRDY_INSTR:
		pr_debug("%sWAITRDY  [max %d ms]\n", prefix,
			 instr->ctx.waitrdy.timeout_ms);
		break;
	}
#endif
}


struct nand_controller_ops {
	int (*attach_chip)(struct nand_chip *chip);
	void (*detach_chip)(struct nand_chip *chip);
	int (*exec_op)(struct nand_chip *chip,
		       const struct nand_operation *op,
		       bool check_only);
	int (*setup_interface)(struct nand_chip *chip, int chipnr,
			       const struct nand_interface_config *conf);
};


struct nand_controller {
	struct mutex lock;
	const struct nand_controller_ops *ops;
	struct {
		unsigned int data_only_read: 1;
		unsigned int cont_read: 1;
	} supported_op;
};

static inline void nand_controller_init(struct nand_controller *nfc)
{
	mutex_init(&nfc->lock);
}


struct nand_legacy {
	void __iomem *IO_ADDR_R;
	void __iomem *IO_ADDR_W;
	void (*select_chip)(struct nand_chip *chip, int cs);
	u8 (*read_byte)(struct nand_chip *chip);
	void (*write_byte)(struct nand_chip *chip, u8 byte);
	void (*write_buf)(struct nand_chip *chip, const u8 *buf, int len);
	void (*read_buf)(struct nand_chip *chip, u8 *buf, int len);
	void (*cmd_ctrl)(struct nand_chip *chip, int dat, unsigned int ctrl);
	void (*cmdfunc)(struct nand_chip *chip, unsigned command, int column,
			int page_addr);
	int (*dev_ready)(struct nand_chip *chip);
	int (*waitfunc)(struct nand_chip *chip);
	int (*block_bad)(struct nand_chip *chip, loff_t ofs);
	int (*block_markbad)(struct nand_chip *chip, loff_t ofs);
	int (*set_features)(struct nand_chip *chip, int feature_addr,
			    u8 *subfeature_para);
	int (*get_features)(struct nand_chip *chip, int feature_addr,
			    u8 *subfeature_para);
	int chip_delay;
	struct nand_controller dummy_controller;
};


struct nand_chip_ops {
	int (*suspend)(struct nand_chip *chip);
	void (*resume)(struct nand_chip *chip);
	int (*lock_area)(struct nand_chip *chip, loff_t ofs, uint64_t len);
	int (*unlock_area)(struct nand_chip *chip, loff_t ofs, uint64_t len);
	int (*setup_read_retry)(struct nand_chip *chip, int retry_mode);
	int (*choose_interface_config)(struct nand_chip *chip,
				       struct nand_interface_config *iface);
};


struct nand_manufacturer {
	const struct nand_manufacturer_desc *desc;
	void *priv;
};


struct nand_secure_region {
	u64 offset;
	u64 size;
};


struct nand_chip {
	struct nand_device base;
	struct nand_id id;
	struct nand_parameters parameters;
	struct nand_manufacturer manufacturer;
	struct nand_chip_ops ops;
	struct nand_legacy legacy;
	unsigned int options;

	
	const struct nand_interface_config *current_interface_config;
	struct nand_interface_config *best_interface_config;

	
	unsigned int bbt_erase_shift;
	unsigned int bbt_options;
	unsigned int badblockpos;
	unsigned int badblockbits;
	struct nand_bbt_descr *bbt_td;
	struct nand_bbt_descr *bbt_md;
	struct nand_bbt_descr *badblock_pattern;
	u8 *bbt;

	
	unsigned int page_shift;
	unsigned int phys_erase_shift;
	unsigned int chip_shift;
	unsigned int pagemask;
	unsigned int subpagesize;

	
	u8 *data_buf;
	u8 *oob_poi;
	struct {
		unsigned int bitflips;
		int page;
	} pagecache;
	unsigned long buf_align;

	
	struct mutex lock;
	unsigned int suspended : 1;
	wait_queue_head_t resume_wq;
	int cur_cs;
	int read_retries;
	struct nand_secure_region *secure_regions;
	u8 nr_secure_regions;
	struct {
		bool ongoing;
		unsigned int first_page;
		unsigned int pause_page;
		unsigned int last_page;
	} cont_read;

	
	struct nand_controller *controller;
	struct nand_ecc_ctrl ecc;
	void *priv;
};

static inline struct nand_chip *mtd_to_nand(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_chip, base.mtd);
}

static inline struct mtd_info *nand_to_mtd(struct nand_chip *chip)
{
	return &chip->base.mtd;
}

static inline void *nand_get_controller_data(struct nand_chip *chip)
{
	return chip->priv;
}

static inline void nand_set_controller_data(struct nand_chip *chip, void *priv)
{
	chip->priv = priv;
}

static inline void nand_set_manufacturer_data(struct nand_chip *chip,
					      void *priv)
{
	chip->manufacturer.priv = priv;
}

static inline void *nand_get_manufacturer_data(struct nand_chip *chip)
{
	return chip->manufacturer.priv;
}

static inline void nand_set_flash_node(struct nand_chip *chip,
				       struct device_node *np)
{
	mtd_set_of_node(nand_to_mtd(chip), np);
}

static inline struct device_node *nand_get_flash_node(struct nand_chip *chip)
{
	return mtd_get_of_node(nand_to_mtd(chip));
}


static inline const struct nand_interface_config *
nand_get_interface_config(struct nand_chip *chip)
{
	return chip->current_interface_config;
}


#define LEGACY_ID_NAND(nm, devid, chipsz, erasesz, opts)          \
	{ .name = (nm), {{ .dev_id = (devid) }}, .pagesize = 512, \
	  .chipsize = (chipsz), .erasesize = (erasesz), .options = (opts) }


#define EXTENDED_ID_NAND(nm, devid, chipsz, opts)                      \
	{ .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz), \
	  .options = (opts) }

#define NAND_ECC_INFO(_strength, _step)	\
			{ .strength_ds = (_strength), .step_ds = (_step) }
#define NAND_ECC_STRENGTH(type)		((type)->ecc.strength_ds)
#define NAND_ECC_STEP(type)		((type)->ecc.step_ds)


struct nand_flash_dev {
	char *name;
	union {
		struct {
			uint8_t mfr_id;
			uint8_t dev_id;
		};
		uint8_t id[NAND_MAX_ID_LEN];
	};
	unsigned int pagesize;
	unsigned int chipsize;
	unsigned int erasesize;
	unsigned int options;
	uint16_t id_len;
	uint16_t oobsize;
	struct {
		uint16_t strength_ds;
		uint16_t step_ds;
	} ecc;
};

int nand_create_bbt(struct nand_chip *chip);


static inline bool nand_is_slc(struct nand_chip *chip)
{
	WARN(nanddev_bits_per_cell(&chip->base) == 0,
	     "chip->bits_per_cell is used uninitialized\n");
	return nanddev_bits_per_cell(&chip->base) == 1;
}


static inline int nand_opcode_8bits(unsigned int command)
{
	switch (command) {
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
	case NAND_CMD_GET_FEATURES:
	case NAND_CMD_SET_FEATURES:
		return 1;
	default:
		break;
	}
	return 0;
}

int rawnand_sw_hamming_init(struct nand_chip *chip);
int rawnand_sw_hamming_calculate(struct nand_chip *chip,
				 const unsigned char *buf,
				 unsigned char *code);
int rawnand_sw_hamming_correct(struct nand_chip *chip,
			       unsigned char *buf,
			       unsigned char *read_ecc,
			       unsigned char *calc_ecc);
void rawnand_sw_hamming_cleanup(struct nand_chip *chip);
int rawnand_sw_bch_init(struct nand_chip *chip);
int rawnand_sw_bch_correct(struct nand_chip *chip, unsigned char *buf,
			   unsigned char *read_ecc, unsigned char *calc_ecc);
void rawnand_sw_bch_cleanup(struct nand_chip *chip);

int nand_check_erased_ecc_chunk(void *data, int datalen,
				void *ecc, int ecclen,
				void *extraoob, int extraooblen,
				int threshold);

int nand_ecc_choose_conf(struct nand_chip *chip,
			 const struct nand_ecc_caps *caps, int oobavail);


int nand_write_oob_std(struct nand_chip *chip, int page);


int nand_read_oob_std(struct nand_chip *chip, int page);


int nand_get_set_features_notsupp(struct nand_chip *chip, int addr,
				  u8 *subfeature_param);


int nand_read_page_raw(struct nand_chip *chip, uint8_t *buf, int oob_required,
		       int page);
int nand_monolithic_read_page_raw(struct nand_chip *chip, uint8_t *buf,
				  int oob_required, int page);


int nand_write_page_raw(struct nand_chip *chip, const uint8_t *buf,
			int oob_required, int page);
int nand_monolithic_write_page_raw(struct nand_chip *chip, const uint8_t *buf,
				   int oob_required, int page);


int nand_reset(struct nand_chip *chip, int chipnr);


int nand_reset_op(struct nand_chip *chip);
int nand_readid_op(struct nand_chip *chip, u8 addr, void *buf,
		   unsigned int len);
int nand_status_op(struct nand_chip *chip, u8 *status);
int nand_exit_status_op(struct nand_chip *chip);
int nand_erase_op(struct nand_chip *chip, unsigned int eraseblock);
int nand_read_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, void *buf, unsigned int len);
int nand_change_read_column_op(struct nand_chip *chip,
			       unsigned int offset_in_page, void *buf,
			       unsigned int len, bool force_8bit);
int nand_read_oob_op(struct nand_chip *chip, unsigned int page,
		     unsigned int offset_in_page, void *buf, unsigned int len);
int nand_prog_page_begin_op(struct nand_chip *chip, unsigned int page,
			    unsigned int offset_in_page, const void *buf,
			    unsigned int len);
int nand_prog_page_end_op(struct nand_chip *chip);
int nand_prog_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, const void *buf,
		      unsigned int len);
int nand_change_write_column_op(struct nand_chip *chip,
				unsigned int offset_in_page, const void *buf,
				unsigned int len, bool force_8bit);
int nand_read_data_op(struct nand_chip *chip, void *buf, unsigned int len,
		      bool force_8bit, bool check_only);
int nand_write_data_op(struct nand_chip *chip, const void *buf,
		       unsigned int len, bool force_8bit);
int nand_read_page_hwecc_oob_first(struct nand_chip *chip, uint8_t *buf,
				   int oob_required, int page);


int nand_scan_with_ids(struct nand_chip *chip, unsigned int max_chips,
		       struct nand_flash_dev *ids);

static inline int nand_scan(struct nand_chip *chip, unsigned int max_chips)
{
	return nand_scan_with_ids(chip, max_chips, NULL);
}


void nand_wait_ready(struct nand_chip *chip);


void nand_cleanup(struct nand_chip *chip);


int nand_soft_waitrdy(struct nand_chip *chip, unsigned long timeout_ms);
int nand_gpio_waitrdy(struct nand_chip *chip, struct gpio_desc *gpiod,
		      unsigned long timeout_ms);


void nand_select_target(struct nand_chip *chip, unsigned int cs);
void nand_deselect_target(struct nand_chip *chip);


void nand_extract_bits(u8 *dst, unsigned int dst_off, const u8 *src,
		       unsigned int src_off, unsigned int nbits);


static inline void *nand_get_data_buf(struct nand_chip *chip)
{
	chip->pagecache.page = -1;

	return chip->data_buf;
}


int rawnand_dt_parse_gpio_cs(struct device *dev, struct gpio_desc ***cs_array,
			     unsigned int *ncs_array);

#endif 
