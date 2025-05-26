/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACTBL1_H__
#define __ACTBL1_H__




#define ACPI_SIG_AEST           "AEST"	
#define ACPI_SIG_ASF            "ASF!"	
#define ACPI_SIG_ASPT           "ASPT"	
#define ACPI_SIG_BERT           "BERT"	
#define ACPI_SIG_BGRT           "BGRT"	
#define ACPI_SIG_BOOT           "BOOT"	
#define ACPI_SIG_CEDT           "CEDT"	
#define ACPI_SIG_CPEP           "CPEP"	
#define ACPI_SIG_CSRT           "CSRT"	
#define ACPI_SIG_DBG2           "DBG2"	
#define ACPI_SIG_DBGP           "DBGP"	
#define ACPI_SIG_DMAR           "DMAR"	
#define ACPI_SIG_DRTM           "DRTM"	
#define ACPI_SIG_ECDT           "ECDT"	
#define ACPI_SIG_EINJ           "EINJ"	
#define ACPI_SIG_ERST           "ERST"	
#define ACPI_SIG_FPDT           "FPDT"	
#define ACPI_SIG_GTDT           "GTDT"	
#define ACPI_SIG_HEST           "HEST"	
#define ACPI_SIG_HMAT           "HMAT"	
#define ACPI_SIG_HPET           "HPET"	
#define ACPI_SIG_IBFT           "IBFT"	
#define ACPI_SIG_MSCT           "MSCT"	

#define ACPI_SIG_S3PT           "S3PT"	
#define ACPI_SIG_PCCS           "PCC"	

#define ACPI_SIG_NBFT		"NBFT"	



#define ACPI_SIG_MATR           "MATR"	
#define ACPI_SIG_MSDM           "MSDM"	


#ifdef ACPI_UNDEFINED_TABLES
#define ACPI_SIG_ATKG           "ATKG"
#define ACPI_SIG_GSCI           "GSCI"	
#define ACPI_SIG_IEIT           "IEIT"
#endif


#pragma pack(1)







struct acpi_subtable_header {
	u8 type;
	u8 length;
};



struct acpi_whea_header {
	u8 action;
	u8 instruction;
	u8 flags;
	u8 reserved;
	struct acpi_generic_address register_region;
	u64 value;		
	u64 mask;		
};


#define ASPT_REVISION_ID 0x01
struct acpi_table_aspt {
	struct acpi_table_header header;
	u32 num_entries;
};

struct acpi_aspt_header {
	u16 type;
	u16 length;
};

enum acpi_aspt_type {
	ACPI_ASPT_TYPE_GLOBAL_REGS = 0,
	ACPI_ASPT_TYPE_SEV_MBOX_REGS = 1,
	ACPI_ASPT_TYPE_ACPI_MBOX_REGS = 2,
};


struct acpi_aspt_global_regs {
	struct acpi_aspt_header header;
	u32 reserved;
	u64 feature_reg_addr;
	u64 irq_en_reg_addr;
	u64 irq_st_reg_addr;
};


struct acpi_aspt_sev_mbox_regs {
	struct acpi_aspt_header header;
	u8 mbox_irq_id;
	u8 reserved[3];
	u64 cmd_resp_reg_addr;
	u64 cmd_buf_lo_reg_addr;
	u64 cmd_buf_hi_reg_addr;
};


struct acpi_aspt_acpi_mbox_regs {
	struct acpi_aspt_header header;
	u32 reserved1;
	u64 cmd_resp_reg_addr;
	u64 reserved2[2];
};



struct acpi_table_asf {
	struct acpi_table_header header;	
};



struct acpi_asf_header {
	u8 type;
	u8 reserved;
	u16 length;
};



enum acpi_asf_type {
	ACPI_ASF_TYPE_INFO = 0,
	ACPI_ASF_TYPE_ALERT = 1,
	ACPI_ASF_TYPE_CONTROL = 2,
	ACPI_ASF_TYPE_BOOT = 3,
	ACPI_ASF_TYPE_ADDRESS = 4,
	ACPI_ASF_TYPE_RESERVED = 5
};





struct acpi_asf_info {
	struct acpi_asf_header header;
	u8 min_reset_value;
	u8 min_poll_interval;
	u16 system_id;
	u32 mfg_id;
	u8 flags;
	u8 reserved2[3];
};



#define ACPI_ASF_SMBUS_PROTOCOLS    (1)



struct acpi_asf_alert {
	struct acpi_asf_header header;
	u8 assert_mask;
	u8 deassert_mask;
	u8 alerts;
	u8 data_length;
};

struct acpi_asf_alert_data {
	u8 address;
	u8 command;
	u8 mask;
	u8 value;
	u8 sensor_type;
	u8 type;
	u8 offset;
	u8 source_type;
	u8 severity;
	u8 sensor_number;
	u8 entity;
	u8 instance;
};



struct acpi_asf_remote {
	struct acpi_asf_header header;
	u8 controls;
	u8 data_length;
	u16 reserved2;
};

struct acpi_asf_control_data {
	u8 function;
	u8 address;
	u8 command;
	u8 value;
};



struct acpi_asf_rmcp {
	struct acpi_asf_header header;
	u8 capabilities[7];
	u8 completion_code;
	u32 enterprise_id;
	u8 command;
	u16 parameter;
	u16 boot_options;
	u16 oem_parameters;
};



struct acpi_asf_address {
	struct acpi_asf_header header;
	u8 eprom_address;
	u8 devices;
};



struct acpi_table_bert {
	struct acpi_table_header header;	
	u32 region_length;	
	u64 address;		
};



struct acpi_bert_region {
	u32 block_status;	
	u32 raw_data_offset;	
	u32 raw_data_length;	
	u32 data_length;	
	u32 error_severity;	
};



#define ACPI_BERT_UNCORRECTABLE             (1)
#define ACPI_BERT_CORRECTABLE               (1<<1)
#define ACPI_BERT_MULTIPLE_UNCORRECTABLE    (1<<2)
#define ACPI_BERT_MULTIPLE_CORRECTABLE      (1<<3)
#define ACPI_BERT_ERROR_ENTRY_COUNT         (0xFF<<4)	



enum acpi_bert_error_severity {
	ACPI_BERT_ERROR_CORRECTABLE = 0,
	ACPI_BERT_ERROR_FATAL = 1,
	ACPI_BERT_ERROR_CORRECTED = 2,
	ACPI_BERT_ERROR_NONE = 3,
	ACPI_BERT_ERROR_RESERVED = 4	
};





struct acpi_table_bgrt {
	struct acpi_table_header header;	
	u16 version;
	u8 status;
	u8 image_type;
	u64 image_address;
	u32 image_offset_x;
	u32 image_offset_y;
};



#define ACPI_BGRT_DISPLAYED                 (1)
#define ACPI_BGRT_ORIENTATION_OFFSET        (3 << 1)



struct acpi_table_boot {
	struct acpi_table_header header;	
	u8 cmos_index;		
	u8 reserved[3];
};



struct acpi_table_cdat {
	u32 length;		
	u8 revision;		
	u8 checksum;		
	u8 reserved[6];
	u32 sequence;		
};



struct acpi_cdat_header {
	u8 type;
	u8 reserved;
	u16 length;
};



enum acpi_cdat_type {
	ACPI_CDAT_TYPE_DSMAS = 0,
	ACPI_CDAT_TYPE_DSLBIS = 1,
	ACPI_CDAT_TYPE_DSMSCIS = 2,
	ACPI_CDAT_TYPE_DSIS = 3,
	ACPI_CDAT_TYPE_DSEMTS = 4,
	ACPI_CDAT_TYPE_SSLBIS = 5,
	ACPI_CDAT_TYPE_RESERVED = 6	
};



struct acpi_cdat_dsmas {
	u8 dsmad_handle;
	u8 flags;
	u16 reserved;
	u64 dpa_base_address;
	u64 dpa_length;
};



#define ACPI_CDAT_DSMAS_NON_VOLATILE        (1 << 2)



struct acpi_cdat_dslbis {
	u8 handle;
	u8 flags;		
	u8 data_type;
	u8 reserved;
	u64 entry_base_unit;
	u16 entry[3];
	u16 reserved2;
};



struct acpi_cdat_dsmscis {
	u8 dsmas_handle;
	u8 reserved[3];
	u64 side_cache_size;
	u32 cache_attributes;
};



struct acpi_cdat_dsis {
	u8 flags;
	u8 handle;
	u16 reserved;
};



#define ACPI_CDAT_DSIS_MEM_ATTACHED         (1 << 0)



struct acpi_cdat_dsemts {
	u8 dsmas_handle;
	u8 memory_type;
	u16 reserved;
	u64 dpa_offset;
	u64 range_length;
};



struct acpi_cdat_sslbis {
	u8 data_type;
	u8 reserved[3];
	u64 entry_base_unit;
};



struct acpi_cdat_sslbe {
	u16 portx_id;
	u16 porty_id;
	u16 latency_or_bandwidth;
	u16 reserved;
};



struct acpi_table_cedt {
	struct acpi_table_header header;	
};



struct acpi_cedt_header {
	u8 type;
	u8 reserved;
	u16 length;
};



enum acpi_cedt_type {
	ACPI_CEDT_TYPE_CHBS = 0,
	ACPI_CEDT_TYPE_CFMWS = 1,
	ACPI_CEDT_TYPE_CXIMS = 2,
	ACPI_CEDT_TYPE_RDPAS = 3,
	ACPI_CEDT_TYPE_RESERVED = 4,
};



#define ACPI_CEDT_CHBS_VERSION_CXL11    (0)
#define ACPI_CEDT_CHBS_VERSION_CXL20    (1)



#define ACPI_CEDT_CHBS_LENGTH_CXL11     (0x2000)
#define ACPI_CEDT_CHBS_LENGTH_CXL20     (0x10000)





struct acpi_cedt_chbs {
	struct acpi_cedt_header header;
	u32 uid;
	u32 cxl_version;
	u32 reserved;
	u64 base;
	u64 length;
};



struct acpi_cedt_cfmws {
	struct acpi_cedt_header header;
	u32 reserved1;
	u64 base_hpa;
	u64 window_size;
	u8 interleave_ways;
	u8 interleave_arithmetic;
	u16 reserved2;
	u32 granularity;
	u16 restrictions;
	u16 qtg_id;
	u32 interleave_targets[];
};

struct acpi_cedt_cfmws_target_element {
	u32 interleave_target;
};



#define ACPI_CEDT_CFMWS_ARITHMETIC_MODULO   (0)
#define ACPI_CEDT_CFMWS_ARITHMETIC_XOR      (1)



#define ACPI_CEDT_CFMWS_RESTRICT_TYPE2      (1)
#define ACPI_CEDT_CFMWS_RESTRICT_TYPE3      (1<<1)
#define ACPI_CEDT_CFMWS_RESTRICT_VOLATILE   (1<<2)
#define ACPI_CEDT_CFMWS_RESTRICT_PMEM       (1<<3)
#define ACPI_CEDT_CFMWS_RESTRICT_FIXED      (1<<4)



struct acpi_cedt_cxims {
	struct acpi_cedt_header header;
	u16 reserved1;
	u8 hbig;
	u8 nr_xormaps;
	u64 xormap_list[];
};



struct acpi_cedt_rdpas {
	struct acpi_cedt_header header;
	u8 reserved1;
	u16 length;
	u16 segment;
	u16 bdf;
	u8 protocol;
	u64 address;
};


#define ACPI_CEDT_RDPAS_BUS_MASK            0xff00
#define ACPI_CEDT_RDPAS_DEVICE_MASK         0x00f8
#define ACPI_CEDT_RDPAS_FUNCTION_MASK       0x0007

#define ACPI_CEDT_RDPAS_PROTOCOL_IO        (0)
#define ACPI_CEDT_RDPAS_PROTOCOL_CACHEMEM  (1)



struct acpi_table_cpep {
	struct acpi_table_header header;	
	u64 reserved;
};



struct acpi_cpep_polling {
	struct acpi_subtable_header header;
	u8 id;			
	u8 eid;			
	u32 interval;		
};



struct acpi_table_csrt {
	struct acpi_table_header header;	
};



struct acpi_csrt_group {
	u32 length;
	u32 vendor_id;
	u32 subvendor_id;
	u16 device_id;
	u16 subdevice_id;
	u16 revision;
	u16 reserved;
	u32 shared_info_length;

	
};



struct acpi_csrt_shared_info {
	u16 major_version;
	u16 minor_version;
	u32 mmio_base_low;
	u32 mmio_base_high;
	u32 gsi_interrupt;
	u8 interrupt_polarity;
	u8 interrupt_mode;
	u8 num_channels;
	u8 dma_address_width;
	u16 base_request_line;
	u16 num_handshake_signals;
	u32 max_block_size;

	
};



struct acpi_csrt_descriptor {
	u32 length;
	u16 type;
	u16 subtype;
	u32 uid;

	
};



#define ACPI_CSRT_TYPE_INTERRUPT    0x0001
#define ACPI_CSRT_TYPE_TIMER        0x0002
#define ACPI_CSRT_TYPE_DMA          0x0003



#define ACPI_CSRT_XRUPT_LINE        0x0000
#define ACPI_CSRT_XRUPT_CONTROLLER  0x0001
#define ACPI_CSRT_TIMER             0x0000
#define ACPI_CSRT_DMA_CHANNEL       0x0000
#define ACPI_CSRT_DMA_CONTROLLER    0x0001



struct acpi_table_dbg2 {
	struct acpi_table_header header;	
	u32 info_offset;
	u32 info_count;
};

struct acpi_dbg2_header {
	u32 info_offset;
	u32 info_count;
};



struct acpi_dbg2_device {
	u8 revision;
	u16 length;
	u8 register_count;	
	u16 namepath_length;
	u16 namepath_offset;
	u16 oem_data_length;
	u16 oem_data_offset;
	u16 port_type;
	u16 port_subtype;
	u16 reserved;
	u16 base_address_offset;
	u16 address_size_offset;
	
};



#define ACPI_DBG2_SERIAL_PORT       0x8000
#define ACPI_DBG2_1394_PORT         0x8001
#define ACPI_DBG2_USB_PORT          0x8002
#define ACPI_DBG2_NET_PORT          0x8003



#define ACPI_DBG2_16550_COMPATIBLE  0x0000
#define ACPI_DBG2_16550_SUBSET      0x0001
#define ACPI_DBG2_MAX311XE_SPI      0x0002
#define ACPI_DBG2_ARM_PL011         0x0003
#define ACPI_DBG2_MSM8X60           0x0004
#define ACPI_DBG2_16550_NVIDIA      0x0005
#define ACPI_DBG2_TI_OMAP           0x0006
#define ACPI_DBG2_APM88XXXX         0x0008
#define ACPI_DBG2_MSM8974           0x0009
#define ACPI_DBG2_SAM5250           0x000A
#define ACPI_DBG2_INTEL_USIF        0x000B
#define ACPI_DBG2_IMX6              0x000C
#define ACPI_DBG2_ARM_SBSA_32BIT    0x000D
#define ACPI_DBG2_ARM_SBSA_GENERIC  0x000E
#define ACPI_DBG2_ARM_DCC           0x000F
#define ACPI_DBG2_BCM2835           0x0010
#define ACPI_DBG2_SDM845_1_8432MHZ  0x0011
#define ACPI_DBG2_16550_WITH_GAS    0x0012
#define ACPI_DBG2_SDM845_7_372MHZ   0x0013
#define ACPI_DBG2_INTEL_LPSS        0x0014

#define ACPI_DBG2_1394_STANDARD     0x0000

#define ACPI_DBG2_USB_XHCI          0x0000
#define ACPI_DBG2_USB_EHCI          0x0001



struct acpi_table_dbgp {
	struct acpi_table_header header;	
	u8 type;		
	u8 reserved[3];
	struct acpi_generic_address debug_port;
};



struct acpi_table_dmar {
	struct acpi_table_header header;	
	u8 width;		
	u8 flags;
	u8 reserved[10];
};



#define ACPI_DMAR_INTR_REMAP        (1)
#define ACPI_DMAR_X2APIC_OPT_OUT    (1<<1)
#define ACPI_DMAR_X2APIC_MODE       (1<<2)



struct acpi_dmar_header {
	u16 type;
	u16 length;
};



enum acpi_dmar_type {
	ACPI_DMAR_TYPE_HARDWARE_UNIT = 0,
	ACPI_DMAR_TYPE_RESERVED_MEMORY = 1,
	ACPI_DMAR_TYPE_ROOT_ATS = 2,
	ACPI_DMAR_TYPE_HARDWARE_AFFINITY = 3,
	ACPI_DMAR_TYPE_NAMESPACE = 4,
	ACPI_DMAR_TYPE_SATC = 5,
	ACPI_DMAR_TYPE_RESERVED = 6	
};



struct acpi_dmar_device_scope {
	u8 entry_type;
	u8 length;
	u16 reserved;
	u8 enumeration_id;
	u8 bus;
};



enum acpi_dmar_scope_type {
	ACPI_DMAR_SCOPE_TYPE_NOT_USED = 0,
	ACPI_DMAR_SCOPE_TYPE_ENDPOINT = 1,
	ACPI_DMAR_SCOPE_TYPE_BRIDGE = 2,
	ACPI_DMAR_SCOPE_TYPE_IOAPIC = 3,
	ACPI_DMAR_SCOPE_TYPE_HPET = 4,
	ACPI_DMAR_SCOPE_TYPE_NAMESPACE = 5,
	ACPI_DMAR_SCOPE_TYPE_RESERVED = 6	
};

struct acpi_dmar_pci_path {
	u8 device;
	u8 function;
};





struct acpi_dmar_hardware_unit {
	struct acpi_dmar_header header;
	u8 flags;
	u8 size;		
	u16 segment;
	u64 address;		
};



#define ACPI_DMAR_INCLUDE_ALL       (1)



struct acpi_dmar_reserved_memory {
	struct acpi_dmar_header header;
	u16 reserved;
	u16 segment;
	u64 base_address;	
	u64 end_address;	
};



#define ACPI_DMAR_ALLOW_ALL         (1)



struct acpi_dmar_atsr {
	struct acpi_dmar_header header;
	u8 flags;
	u8 reserved;
	u16 segment;
};



#define ACPI_DMAR_ALL_PORTS         (1)



struct acpi_dmar_rhsa {
	struct acpi_dmar_header header;
	u32 reserved;
	u64 base_address;
	u32 proximity_domain;
};



struct acpi_dmar_andd {
	struct acpi_dmar_header header;
	u8 reserved[3];
	u8 device_number;
	union {
		char __pad;
		 ACPI_FLEX_ARRAY(char, device_name);
	};
};



struct acpi_dmar_satc {
	struct acpi_dmar_header header;
	u8 flags;
	u8 reserved;
	u16 segment;
};


struct acpi_table_drtm {
	struct acpi_table_header header;	
	u64 entry_base_address;
	u64 entry_length;
	u32 entry_address32;
	u64 entry_address64;
	u64 exit_address;
	u64 log_area_address;
	u32 log_area_length;
	u64 arch_dependent_address;
	u32 flags;
};



#define ACPI_DRTM_ACCESS_ALLOWED            (1)
#define ACPI_DRTM_ENABLE_GAP_CODE           (1<<1)
#define ACPI_DRTM_INCOMPLETE_MEASUREMENTS   (1<<2)
#define ACPI_DRTM_AUTHORITY_ORDER           (1<<3)



struct acpi_drtm_vtable_list {
	u32 validated_table_count;
	u64 validated_tables[];
};





struct acpi_drtm_resource {
	u8 size[7];
	u8 type;
	u64 address;
};

struct acpi_drtm_resource_list {
	u32 resource_count;
	struct acpi_drtm_resource resources[];
};



struct acpi_drtm_dps_id {
	u32 dps_id_length;
	u8 dps_id[16];
};



struct acpi_table_ecdt {
	struct acpi_table_header header;	
	struct acpi_generic_address control;	
	struct acpi_generic_address data;	
	u32 uid;		
	u8 gpe;			
	u8 id[];		
};



struct acpi_table_einj {
	struct acpi_table_header header;	
	u32 header_length;
	u8 flags;
	u8 reserved[3];
	u32 entries;
};



struct acpi_einj_entry {
	struct acpi_whea_header whea_header;	
};



#define ACPI_EINJ_PRESERVE          (1)



enum acpi_einj_actions {
	ACPI_EINJ_BEGIN_OPERATION = 0,
	ACPI_EINJ_GET_TRIGGER_TABLE = 1,
	ACPI_EINJ_SET_ERROR_TYPE = 2,
	ACPI_EINJ_GET_ERROR_TYPE = 3,
	ACPI_EINJ_END_OPERATION = 4,
	ACPI_EINJ_EXECUTE_OPERATION = 5,
	ACPI_EINJ_CHECK_BUSY_STATUS = 6,
	ACPI_EINJ_GET_COMMAND_STATUS = 7,
	ACPI_EINJ_SET_ERROR_TYPE_WITH_ADDRESS = 8,
	ACPI_EINJ_GET_EXECUTE_TIMINGS = 9,
	ACPI_EINJ_ACTION_RESERVED = 10,	
	ACPI_EINJ_TRIGGER_ERROR = 0xFF	
};



enum acpi_einj_instructions {
	ACPI_EINJ_READ_REGISTER = 0,
	ACPI_EINJ_READ_REGISTER_VALUE = 1,
	ACPI_EINJ_WRITE_REGISTER = 2,
	ACPI_EINJ_WRITE_REGISTER_VALUE = 3,
	ACPI_EINJ_NOOP = 4,
	ACPI_EINJ_FLUSH_CACHELINE = 5,
	ACPI_EINJ_INSTRUCTION_RESERVED = 6	
};

struct acpi_einj_error_type_with_addr {
	u32 error_type;
	u32 vendor_struct_offset;
	u32 flags;
	u32 apic_id;
	u64 address;
	u64 range;
	u32 pcie_id;
};

struct acpi_einj_vendor {
	u32 length;
	u32 pcie_id;
	u16 vendor_id;
	u16 device_id;
	u8 revision_id;
	u8 reserved[3];
};



struct acpi_einj_trigger {
	u32 header_size;
	u32 revision;
	u32 table_size;
	u32 entry_count;
};



enum acpi_einj_command_status {
	ACPI_EINJ_SUCCESS = 0,
	ACPI_EINJ_FAILURE = 1,
	ACPI_EINJ_INVALID_ACCESS = 2,
	ACPI_EINJ_STATUS_RESERVED = 3	
};



#define ACPI_EINJ_PROCESSOR_CORRECTABLE     (1)
#define ACPI_EINJ_PROCESSOR_UNCORRECTABLE   (1<<1)
#define ACPI_EINJ_PROCESSOR_FATAL           (1<<2)
#define ACPI_EINJ_MEMORY_CORRECTABLE        (1<<3)
#define ACPI_EINJ_MEMORY_UNCORRECTABLE      (1<<4)
#define ACPI_EINJ_MEMORY_FATAL              (1<<5)
#define ACPI_EINJ_PCIX_CORRECTABLE          (1<<6)
#define ACPI_EINJ_PCIX_UNCORRECTABLE        (1<<7)
#define ACPI_EINJ_PCIX_FATAL                (1<<8)
#define ACPI_EINJ_PLATFORM_CORRECTABLE      (1<<9)
#define ACPI_EINJ_PLATFORM_UNCORRECTABLE    (1<<10)
#define ACPI_EINJ_PLATFORM_FATAL            (1<<11)
#define ACPI_EINJ_VENDOR_DEFINED            (1<<31)



struct acpi_table_erst {
	struct acpi_table_header header;	
	u32 header_length;
	u32 reserved;
	u32 entries;
};



struct acpi_erst_entry {
	struct acpi_whea_header whea_header;	
};



#define ACPI_ERST_PRESERVE          (1)



enum acpi_erst_actions {
	ACPI_ERST_BEGIN_WRITE = 0,
	ACPI_ERST_BEGIN_READ = 1,
	ACPI_ERST_BEGIN_CLEAR = 2,
	ACPI_ERST_END = 3,
	ACPI_ERST_SET_RECORD_OFFSET = 4,
	ACPI_ERST_EXECUTE_OPERATION = 5,
	ACPI_ERST_CHECK_BUSY_STATUS = 6,
	ACPI_ERST_GET_COMMAND_STATUS = 7,
	ACPI_ERST_GET_RECORD_ID = 8,
	ACPI_ERST_SET_RECORD_ID = 9,
	ACPI_ERST_GET_RECORD_COUNT = 10,
	ACPI_ERST_BEGIN_DUMMY_WRIITE = 11,
	ACPI_ERST_NOT_USED = 12,
	ACPI_ERST_GET_ERROR_RANGE = 13,
	ACPI_ERST_GET_ERROR_LENGTH = 14,
	ACPI_ERST_GET_ERROR_ATTRIBUTES = 15,
	ACPI_ERST_EXECUTE_TIMINGS = 16,
	ACPI_ERST_ACTION_RESERVED = 17	
};



enum acpi_erst_instructions {
	ACPI_ERST_READ_REGISTER = 0,
	ACPI_ERST_READ_REGISTER_VALUE = 1,
	ACPI_ERST_WRITE_REGISTER = 2,
	ACPI_ERST_WRITE_REGISTER_VALUE = 3,
	ACPI_ERST_NOOP = 4,
	ACPI_ERST_LOAD_VAR1 = 5,
	ACPI_ERST_LOAD_VAR2 = 6,
	ACPI_ERST_STORE_VAR1 = 7,
	ACPI_ERST_ADD = 8,
	ACPI_ERST_SUBTRACT = 9,
	ACPI_ERST_ADD_VALUE = 10,
	ACPI_ERST_SUBTRACT_VALUE = 11,
	ACPI_ERST_STALL = 12,
	ACPI_ERST_STALL_WHILE_TRUE = 13,
	ACPI_ERST_SKIP_NEXT_IF_TRUE = 14,
	ACPI_ERST_GOTO = 15,
	ACPI_ERST_SET_SRC_ADDRESS_BASE = 16,
	ACPI_ERST_SET_DST_ADDRESS_BASE = 17,
	ACPI_ERST_MOVE_DATA = 18,
	ACPI_ERST_INSTRUCTION_RESERVED = 19	
};



enum acpi_erst_command_status {
	ACPI_ERST_SUCCESS = 0,
	ACPI_ERST_NO_SPACE = 1,
	ACPI_ERST_NOT_AVAILABLE = 2,
	ACPI_ERST_FAILURE = 3,
	ACPI_ERST_RECORD_EMPTY = 4,
	ACPI_ERST_NOT_FOUND = 5,
	ACPI_ERST_STATUS_RESERVED = 6	
};



struct acpi_erst_info {
	u16 signature;		
	u8 data[48];
};



struct acpi_table_fpdt {
	struct acpi_table_header header;	
};



struct acpi_fpdt_header {
	u16 type;
	u8 length;
	u8 revision;
};



enum acpi_fpdt_type {
	ACPI_FPDT_TYPE_BOOT = 0,
	ACPI_FPDT_TYPE_S3PERF = 1
};





struct acpi_fpdt_boot_pointer {
	struct acpi_fpdt_header header;
	u8 reserved[4];
	u64 address;
};



struct acpi_fpdt_s3pt_pointer {
	struct acpi_fpdt_header header;
	u8 reserved[4];
	u64 address;
};


struct acpi_table_s3pt {
	u8 signature[4];	
	u32 length;
};





enum acpi_s3pt_type {
	ACPI_S3PT_TYPE_RESUME = 0,
	ACPI_S3PT_TYPE_SUSPEND = 1,
	ACPI_FPDT_BOOT_PERFORMANCE = 2
};

struct acpi_s3pt_resume {
	struct acpi_fpdt_header header;
	u32 resume_count;
	u64 full_resume;
	u64 average_resume;
};

struct acpi_s3pt_suspend {
	struct acpi_fpdt_header header;
	u64 suspend_start;
	u64 suspend_end;
};


struct acpi_fpdt_boot {
	struct acpi_fpdt_header header;
	u8 reserved[4];
	u64 reset_end;
	u64 load_start;
	u64 startup_start;
	u64 exit_services_entry;
	u64 exit_services_exit;
};



struct acpi_table_gtdt {
	struct acpi_table_header header;	
	u64 counter_block_addresss;
	u32 reserved;
	u32 secure_el1_interrupt;
	u32 secure_el1_flags;
	u32 non_secure_el1_interrupt;
	u32 non_secure_el1_flags;
	u32 virtual_timer_interrupt;
	u32 virtual_timer_flags;
	u32 non_secure_el2_interrupt;
	u32 non_secure_el2_flags;
	u64 counter_read_block_address;
	u32 platform_timer_count;
	u32 platform_timer_offset;
};



#define ACPI_GTDT_INTERRUPT_MODE        (1)
#define ACPI_GTDT_INTERRUPT_POLARITY    (1<<1)
#define ACPI_GTDT_ALWAYS_ON             (1<<2)

struct acpi_gtdt_el2 {
	u32 virtual_el2_timer_gsiv;
	u32 virtual_el2_timer_flags;
};



struct acpi_gtdt_header {
	u8 type;
	u16 length;
};



enum acpi_gtdt_type {
	ACPI_GTDT_TYPE_TIMER_BLOCK = 0,
	ACPI_GTDT_TYPE_WATCHDOG = 1,
	ACPI_GTDT_TYPE_RESERVED = 2	
};





struct acpi_gtdt_timer_block {
	struct acpi_gtdt_header header;
	u8 reserved;
	u64 block_address;
	u32 timer_count;
	u32 timer_offset;
};



struct acpi_gtdt_timer_entry {
	u8 frame_number;
	u8 reserved[3];
	u64 base_address;
	u64 el0_base_address;
	u32 timer_interrupt;
	u32 timer_flags;
	u32 virtual_timer_interrupt;
	u32 virtual_timer_flags;
	u32 common_flags;
};



#define ACPI_GTDT_GT_IRQ_MODE               (1)
#define ACPI_GTDT_GT_IRQ_POLARITY           (1<<1)



#define ACPI_GTDT_GT_IS_SECURE_TIMER        (1)
#define ACPI_GTDT_GT_ALWAYS_ON              (1<<1)



struct acpi_gtdt_watchdog {
	struct acpi_gtdt_header header;
	u8 reserved;
	u64 refresh_frame_address;
	u64 control_frame_address;
	u32 timer_interrupt;
	u32 timer_flags;
};



#define ACPI_GTDT_WATCHDOG_IRQ_MODE         (1)
#define ACPI_GTDT_WATCHDOG_IRQ_POLARITY     (1<<1)
#define ACPI_GTDT_WATCHDOG_SECURE           (1<<2)



struct acpi_table_hest {
	struct acpi_table_header header;	
	u32 error_source_count;
};



struct acpi_hest_header {
	u16 type;
	u16 source_id;
};



enum acpi_hest_types {
	ACPI_HEST_TYPE_IA32_CHECK = 0,
	ACPI_HEST_TYPE_IA32_CORRECTED_CHECK = 1,
	ACPI_HEST_TYPE_IA32_NMI = 2,
	ACPI_HEST_TYPE_NOT_USED3 = 3,
	ACPI_HEST_TYPE_NOT_USED4 = 4,
	ACPI_HEST_TYPE_NOT_USED5 = 5,
	ACPI_HEST_TYPE_AER_ROOT_PORT = 6,
	ACPI_HEST_TYPE_AER_ENDPOINT = 7,
	ACPI_HEST_TYPE_AER_BRIDGE = 8,
	ACPI_HEST_TYPE_GENERIC_ERROR = 9,
	ACPI_HEST_TYPE_GENERIC_ERROR_V2 = 10,
	ACPI_HEST_TYPE_IA32_DEFERRED_CHECK = 11,
	ACPI_HEST_TYPE_RESERVED = 12	
};




struct acpi_hest_ia_error_bank {
	u8 bank_number;
	u8 clear_status_on_init;
	u8 status_format;
	u8 reserved;
	u32 control_register;
	u64 control_data;
	u32 status_register;
	u32 address_register;
	u32 misc_register;
};



struct acpi_hest_aer_common {
	u16 reserved1;
	u8 flags;
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	u32 bus;		
	u16 device;
	u16 function;
	u16 device_control;
	u16 reserved2;
	u32 uncorrectable_mask;
	u32 uncorrectable_severity;
	u32 correctable_mask;
	u32 advanced_capabilities;
};



#define ACPI_HEST_FIRMWARE_FIRST        (1)
#define ACPI_HEST_GLOBAL                (1<<1)
#define ACPI_HEST_GHES_ASSIST           (1<<2)


#define ACPI_HEST_BUS(bus)              ((bus) & 0xFF)
#define ACPI_HEST_SEGMENT(bus)          (((bus) >> 8) & 0xFFFF)



struct acpi_hest_notify {
	u8 type;
	u8 length;
	u16 config_write_enable;
	u32 poll_interval;
	u32 vector;
	u32 polling_threshold_value;
	u32 polling_threshold_window;
	u32 error_threshold_value;
	u32 error_threshold_window;
};



enum acpi_hest_notify_types {
	ACPI_HEST_NOTIFY_POLLED = 0,
	ACPI_HEST_NOTIFY_EXTERNAL = 1,
	ACPI_HEST_NOTIFY_LOCAL = 2,
	ACPI_HEST_NOTIFY_SCI = 3,
	ACPI_HEST_NOTIFY_NMI = 4,
	ACPI_HEST_NOTIFY_CMCI = 5,	
	ACPI_HEST_NOTIFY_MCE = 6,	
	ACPI_HEST_NOTIFY_GPIO = 7,	
	ACPI_HEST_NOTIFY_SEA = 8,	
	ACPI_HEST_NOTIFY_SEI = 9,	
	ACPI_HEST_NOTIFY_GSIV = 10,	
	ACPI_HEST_NOTIFY_SOFTWARE_DELEGATED = 11,	
	ACPI_HEST_NOTIFY_RESERVED = 12	
};



#define ACPI_HEST_TYPE                  (1)
#define ACPI_HEST_POLL_INTERVAL         (1<<1)
#define ACPI_HEST_POLL_THRESHOLD_VALUE  (1<<2)
#define ACPI_HEST_POLL_THRESHOLD_WINDOW (1<<3)
#define ACPI_HEST_ERR_THRESHOLD_VALUE   (1<<4)
#define ACPI_HEST_ERR_THRESHOLD_WINDOW  (1<<5)





struct acpi_hest_ia_machine_check {
	struct acpi_hest_header header;
	u16 reserved1;
	u8 flags;		
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	u64 global_capability_data;
	u64 global_control_data;
	u8 num_hardware_banks;
	u8 reserved3[7];
};



struct acpi_hest_ia_corrected {
	struct acpi_hest_header header;
	u16 reserved1;
	u8 flags;		
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	struct acpi_hest_notify notify;
	u8 num_hardware_banks;
	u8 reserved2[3];
};



struct acpi_hest_ia_nmi {
	struct acpi_hest_header header;
	u32 reserved;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	u32 max_raw_data_length;
};





struct acpi_hest_aer_root {
	struct acpi_hest_header header;
	struct acpi_hest_aer_common aer;
	u32 root_error_command;
};



struct acpi_hest_aer {
	struct acpi_hest_header header;
	struct acpi_hest_aer_common aer;
};



struct acpi_hest_aer_bridge {
	struct acpi_hest_header header;
	struct acpi_hest_aer_common aer;
	u32 uncorrectable_mask2;
	u32 uncorrectable_severity2;
	u32 advanced_capabilities2;
};



struct acpi_hest_generic {
	struct acpi_hest_header header;
	u16 related_source_id;
	u8 reserved;
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	u32 max_raw_data_length;
	struct acpi_generic_address error_status_address;
	struct acpi_hest_notify notify;
	u32 error_block_length;
};



struct acpi_hest_generic_v2 {
	struct acpi_hest_header header;
	u16 related_source_id;
	u8 reserved;
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	u32 max_raw_data_length;
	struct acpi_generic_address error_status_address;
	struct acpi_hest_notify notify;
	u32 error_block_length;
	struct acpi_generic_address read_ack_register;
	u64 read_ack_preserve;
	u64 read_ack_write;
};



struct acpi_hest_generic_status {
	u32 block_status;
	u32 raw_data_offset;
	u32 raw_data_length;
	u32 data_length;
	u32 error_severity;
};



#define ACPI_HEST_UNCORRECTABLE             (1)
#define ACPI_HEST_CORRECTABLE               (1<<1)
#define ACPI_HEST_MULTIPLE_UNCORRECTABLE    (1<<2)
#define ACPI_HEST_MULTIPLE_CORRECTABLE      (1<<3)
#define ACPI_HEST_ERROR_ENTRY_COUNT         (0xFF<<4)	



struct acpi_hest_generic_data {
	u8 section_type[16];
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;
	u32 error_data_length;
	u8 fru_id[16];
	u8 fru_text[20];
};



struct acpi_hest_generic_data_v300 {
	u8 section_type[16];
	u32 error_severity;
	u16 revision;
	u8 validation_bits;
	u8 flags;
	u32 error_data_length;
	u8 fru_id[16];
	u8 fru_text[20];
	u64 time_stamp;
};



#define ACPI_HEST_GEN_ERROR_RECOVERABLE     0
#define ACPI_HEST_GEN_ERROR_FATAL           1
#define ACPI_HEST_GEN_ERROR_CORRECTED       2
#define ACPI_HEST_GEN_ERROR_NONE            3



#define ACPI_HEST_GEN_VALID_FRU_ID          (1)
#define ACPI_HEST_GEN_VALID_FRU_STRING      (1<<1)
#define ACPI_HEST_GEN_VALID_TIMESTAMP       (1<<2)



struct acpi_hest_ia_deferred_check {
	struct acpi_hest_header header;
	u16 reserved1;
	u8 flags;		
	u8 enabled;
	u32 records_to_preallocate;
	u32 max_sections_per_record;
	struct acpi_hest_notify notify;
	u8 num_hardware_banks;
	u8 reserved2[3];
};



struct acpi_table_hmat {
	struct acpi_table_header header;	
	u32 reserved;
};



enum acpi_hmat_type {
	ACPI_HMAT_TYPE_PROXIMITY = 0,	
	ACPI_HMAT_TYPE_LOCALITY = 1,	
	ACPI_HMAT_TYPE_CACHE = 2,	
	ACPI_HMAT_TYPE_RESERVED = 3	
};

struct acpi_hmat_structure {
	u16 type;
	u16 reserved;
	u32 length;
};





struct acpi_hmat_proximity_domain {
	struct acpi_hmat_structure header;
	u16 flags;
	u16 reserved1;
	u32 processor_PD;	
	u32 memory_PD;		
	u32 reserved2;
	u64 reserved3;
	u64 reserved4;
};



#define ACPI_HMAT_PROCESSOR_PD_VALID    (1)	
#define ACPI_HMAT_MEMORY_PD_VALID       (1<<1)	
#define ACPI_HMAT_RESERVATION_HINT      (1<<2)	



struct acpi_hmat_locality {
	struct acpi_hmat_structure header;
	u8 flags;
	u8 data_type;
	u8 min_transfer_size;
	u8 reserved1;
	u32 number_of_initiator_Pds;
	u32 number_of_target_Pds;
	u32 reserved2;
	u64 entry_base_unit;
};



#define ACPI_HMAT_MEMORY_HIERARCHY  (0x0F)     



#define ACPI_HMAT_MEMORY            0
#define ACPI_HMAT_LAST_LEVEL_CACHE  1
#define ACPI_HMAT_1ST_LEVEL_CACHE   2
#define ACPI_HMAT_2ND_LEVEL_CACHE   3
#define ACPI_HMAT_3RD_LEVEL_CACHE   4
#define ACPI_HMAT_MINIMUM_XFER_SIZE 0x10       
#define ACPI_HMAT_NON_SEQUENTIAL_XFERS 0x20    




#define ACPI_HMAT_ACCESS_LATENCY    0
#define ACPI_HMAT_READ_LATENCY      1
#define ACPI_HMAT_WRITE_LATENCY     2
#define ACPI_HMAT_ACCESS_BANDWIDTH  3
#define ACPI_HMAT_READ_BANDWIDTH    4
#define ACPI_HMAT_WRITE_BANDWIDTH   5



struct acpi_hmat_cache {
	struct acpi_hmat_structure header;
	u32 memory_PD;
	u32 reserved1;
	u64 cache_size;
	u32 cache_attributes;
	u16 reserved2;
	u16 number_of_SMBIOShandles;
};



#define ACPI_HMAT_TOTAL_CACHE_LEVEL     (0x0000000F)
#define ACPI_HMAT_CACHE_LEVEL           (0x000000F0)
#define ACPI_HMAT_CACHE_ASSOCIATIVITY   (0x00000F00)
#define ACPI_HMAT_WRITE_POLICY          (0x0000F000)
#define ACPI_HMAT_CACHE_LINE_SIZE       (0xFFFF0000)



#define ACPI_HMAT_CA_NONE                     (0)
#define ACPI_HMAT_CA_DIRECT_MAPPED            (1)
#define ACPI_HMAT_CA_COMPLEX_CACHE_INDEXING   (2)



#define ACPI_HMAT_CP_NONE   (0)
#define ACPI_HMAT_CP_WB     (1)
#define ACPI_HMAT_CP_WT     (2)



struct acpi_table_hpet {
	struct acpi_table_header header;	
	u32 id;			
	struct acpi_generic_address address;	
	u8 sequence;		
	u16 minimum_tick;	
	u8 flags;
};



#define ACPI_HPET_PAGE_PROTECT_MASK (3)



enum acpi_hpet_page_protect {
	ACPI_HPET_NO_PAGE_PROTECT = 0,
	ACPI_HPET_PAGE_PROTECT4 = 1,
	ACPI_HPET_PAGE_PROTECT64 = 2
};



struct acpi_table_ibft {
	struct acpi_table_header header;	
	u8 reserved[12];
};



struct acpi_ibft_header {
	u8 type;
	u8 version;
	u16 length;
	u8 index;
	u8 flags;
};



enum acpi_ibft_type {
	ACPI_IBFT_TYPE_NOT_USED = 0,
	ACPI_IBFT_TYPE_CONTROL = 1,
	ACPI_IBFT_TYPE_INITIATOR = 2,
	ACPI_IBFT_TYPE_NIC = 3,
	ACPI_IBFT_TYPE_TARGET = 4,
	ACPI_IBFT_TYPE_EXTENSIONS = 5,
	ACPI_IBFT_TYPE_RESERVED = 6	
};



struct acpi_ibft_control {
	struct acpi_ibft_header header;
	u16 extensions;
	u16 initiator_offset;
	u16 nic0_offset;
	u16 target0_offset;
	u16 nic1_offset;
	u16 target1_offset;
};

struct acpi_ibft_initiator {
	struct acpi_ibft_header header;
	u8 sns_server[16];
	u8 slp_server[16];
	u8 primary_server[16];
	u8 secondary_server[16];
	u16 name_length;
	u16 name_offset;
};

struct acpi_ibft_nic {
	struct acpi_ibft_header header;
	u8 ip_address[16];
	u8 subnet_mask_prefix;
	u8 origin;
	u8 gateway[16];
	u8 primary_dns[16];
	u8 secondary_dns[16];
	u8 dhcp[16];
	u16 vlan;
	u8 mac_address[6];
	u16 pci_address;
	u16 name_length;
	u16 name_offset;
};

struct acpi_ibft_target {
	struct acpi_ibft_header header;
	u8 target_ip_address[16];
	u16 target_ip_socket;
	u8 target_boot_lun[8];
	u8 chap_type;
	u8 nic_association;
	u16 target_name_length;
	u16 target_name_offset;
	u16 chap_name_length;
	u16 chap_name_offset;
	u16 chap_secret_length;
	u16 chap_secret_offset;
	u16 reverse_chap_name_length;
	u16 reverse_chap_name_offset;
	u16 reverse_chap_secret_length;
	u16 reverse_chap_secret_offset;
};



#pragma pack()

#endif				
