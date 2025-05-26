/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_MTD_ONFI_H
#define __LINUX_MTD_ONFI_H

#include <linux/types.h>
#include <linux/bitfield.h>


#define ONFI_VERSION_1_0		BIT(1)
#define ONFI_VERSION_2_0		BIT(2)
#define ONFI_VERSION_2_1		BIT(3)
#define ONFI_VERSION_2_2		BIT(4)
#define ONFI_VERSION_2_3		BIT(5)
#define ONFI_VERSION_3_0		BIT(6)
#define ONFI_VERSION_3_1		BIT(7)
#define ONFI_VERSION_3_2		BIT(8)
#define ONFI_VERSION_4_0		BIT(9)


#define ONFI_FEATURE_16_BIT_BUS		BIT(0)
#define ONFI_FEATURE_NV_DDR		BIT(5)
#define ONFI_FEATURE_EXT_PARAM_PAGE	BIT(7)


#define ONFI_DATA_INTERFACE_SDR		0
#define ONFI_DATA_INTERFACE_NVDDR	BIT(4)
#define ONFI_DATA_INTERFACE_NVDDR2	BIT(5)
#define ONFI_TIMING_MODE_0		BIT(0)
#define ONFI_TIMING_MODE_1		BIT(1)
#define ONFI_TIMING_MODE_2		BIT(2)
#define ONFI_TIMING_MODE_3		BIT(3)
#define ONFI_TIMING_MODE_4		BIT(4)
#define ONFI_TIMING_MODE_5		BIT(5)
#define ONFI_TIMING_MODE_UNKNOWN	BIT(6)
#define ONFI_TIMING_MODE_PARAM(x)	FIELD_GET(GENMASK(3, 0), (x))


#define ONFI_FEATURE_NUMBER		256
#define ONFI_FEATURE_ADDR_TIMING_MODE	0x1


#define ONFI_FEATURE_ADDR_READ_RETRY	0x89
#define ONFI_FEATURE_ON_DIE_ECC		0x90
#define   ONFI_FEATURE_ON_DIE_ECC_EN	BIT(3)


#define ONFI_SUBFEATURE_PARAM_LEN	4


#define ONFI_OPT_CMD_READ_CACHE		BIT(1)
#define ONFI_OPT_CMD_SET_GET_FEATURES	BIT(2)

struct nand_onfi_params {
	
	
	u8 sig[4];
	__le16 revision;
	__le16 features;
	__le16 opt_cmd;
	u8 reserved0[2];
	__le16 ext_param_page_length; 
	u8 num_of_param_pages;        
	u8 reserved1[17];

	
	char manufacturer[12];
	char model[20];
	u8 jedec_id;
	__le16 date_code;
	u8 reserved2[13];

	
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	__le32 data_bytes_per_ppage;
	__le16 spare_bytes_per_ppage;
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	u8 lun_count;
	u8 addr_cycles;
	u8 bits_per_cell;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	u8 programs_per_page;
	u8 ppage_attr;
	u8 ecc_bits;
	u8 interleaved_bits;
	u8 interleaved_ops;
	u8 reserved3[13];

	
	u8 io_pin_capacitance_max;
	__le16 sdr_timing_modes;
	__le16 program_cache_timing_mode;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_ccs;
	u8 nvddr_timing_modes;
	u8 nvddr2_timing_modes;
	u8 nvddr_nvddr2_features;
	__le16 clk_pin_capacitance_typ;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	u8 input_pin_capacitance_max;
	u8 driver_strength_support;
	__le16 t_int_r;
	__le16 t_adl;
	u8 reserved4[8];

	
	__le16 vendor_revision;
	u8 vendor[88];

	__le16 crc;
} __packed;

#define ONFI_CRC_BASE	0x4F4E


struct onfi_ext_ecc_info {
	u8 ecc_bits;
	u8 codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	u8 reserved[2];
} __packed;

#define ONFI_SECTION_TYPE_0	0	
#define ONFI_SECTION_TYPE_1	1	
#define ONFI_SECTION_TYPE_2	2	
struct onfi_ext_section {
	u8 type;
	u8 length;
} __packed;

#define ONFI_EXT_SECTION_MAX 8


struct onfi_ext_param_page {
	__le16 crc;
	u8 sig[4];             
	u8 reserved0[10];
	struct onfi_ext_section sections[ONFI_EXT_SECTION_MAX];

	
} __packed;


struct onfi_params {
	int version;
	u16 tPROG;
	u16 tBERS;
	u16 tR;
	u16 tCCS;
	bool fast_tCAD;
	u16 sdr_timing_modes;
	u16 nvddr_timing_modes;
	u16 vendor_revision;
	u8 vendor[88];
};

#endif 
