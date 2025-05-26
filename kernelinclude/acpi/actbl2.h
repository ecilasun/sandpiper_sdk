/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACTBL2_H__
#define __ACTBL2_H__




#define ACPI_SIG_AGDI           "AGDI"	
#define ACPI_SIG_APMT           "APMT"	
#define ACPI_SIG_BDAT           "BDAT"	
#define ACPI_SIG_CCEL           "CCEL"	
#define ACPI_SIG_CDAT           "CDAT"	
#define ACPI_SIG_IORT           "IORT"	
#define ACPI_SIG_IVRS           "IVRS"	
#define ACPI_SIG_LPIT           "LPIT"	
#define ACPI_SIG_MADT           "APIC"	
#define ACPI_SIG_MCFG           "MCFG"	
#define ACPI_SIG_MCHI           "MCHI"	
#define ACPI_SIG_MPAM           "MPAM"	
#define ACPI_SIG_MPST           "MPST"	
#define ACPI_SIG_MSDM           "MSDM"	
#define ACPI_SIG_NFIT           "NFIT"	
#define ACPI_SIG_NHLT           "NHLT"	
#define ACPI_SIG_PCCT           "PCCT"	
#define ACPI_SIG_PDTT           "PDTT"	
#define ACPI_SIG_PHAT           "PHAT"	
#define ACPI_SIG_PMTT           "PMTT"	
#define ACPI_SIG_PPTT           "PPTT"	
#define ACPI_SIG_PRMT           "PRMT"	
#define ACPI_SIG_RASF           "RASF"	
#define ACPI_SIG_RGRT           "RGRT"	
#define ACPI_SIG_RHCT           "RHCT"	
#define ACPI_SIG_SBST           "SBST"	
#define ACPI_SIG_SDEI           "SDEI"	
#define ACPI_SIG_SDEV           "SDEV"	
#define ACPI_SIG_SVKL           "SVKL"	
#define ACPI_SIG_TDEL           "TDEL"	


#pragma pack(1)





struct acpi_table_aest {
	struct acpi_table_header header;
};



struct acpi_aest_hdr {
	u8 type;
	u16 length;
	u8 reserved;
	u32 node_specific_offset;
	u32 node_interface_offset;
	u32 node_interrupt_offset;
	u32 node_interrupt_count;
	u64 timestamp_rate;
	u64 reserved1;
	u64 error_injection_rate;
};



#define ACPI_AEST_PROCESSOR_ERROR_NODE      0
#define ACPI_AEST_MEMORY_ERROR_NODE         1
#define ACPI_AEST_SMMU_ERROR_NODE           2
#define ACPI_AEST_VENDOR_ERROR_NODE         3
#define ACPI_AEST_GIC_ERROR_NODE            4
#define ACPI_AEST_NODE_TYPE_RESERVED        5	





typedef struct acpi_aest_processor {
	u32 processor_id;
	u8 resource_type;
	u8 reserved;
	u8 flags;
	u8 revision;
	u64 processor_affinity;

} acpi_aest_processor;



#define ACPI_AEST_CACHE_RESOURCE            0
#define ACPI_AEST_TLB_RESOURCE              1
#define ACPI_AEST_GENERIC_RESOURCE          2
#define ACPI_AEST_RESOURCE_RESERVED         3	



typedef struct acpi_aest_processor_cache {
	u32 cache_reference;
	u32 reserved;

} acpi_aest_processor_cache;



#define ACPI_AEST_CACHE_DATA                0
#define ACPI_AEST_CACHE_INSTRUCTION         1
#define ACPI_AEST_CACHE_UNIFIED             2
#define ACPI_AEST_CACHE_RESERVED            3	



typedef struct acpi_aest_processor_tlb {
	u32 tlb_level;
	u32 reserved;

} acpi_aest_processor_tlb;



typedef struct acpi_aest_processor_generic {
	u32 resource;

} acpi_aest_processor_generic;



typedef struct acpi_aest_memory {
	u32 srat_proximity_domain;

} acpi_aest_memory;



typedef struct acpi_aest_smmu {
	u32 iort_node_reference;
	u32 subcomponent_reference;

} acpi_aest_smmu;



typedef struct acpi_aest_vendor {
	u32 acpi_hid;
	u32 acpi_uid;
	u8 vendor_specific_data[16];

} acpi_aest_vendor;



typedef struct acpi_aest_gic {
	u32 interface_type;
	u32 instance_id;

} acpi_aest_gic;



#define ACPI_AEST_GIC_CPU                   0
#define ACPI_AEST_GIC_DISTRIBUTOR           1
#define ACPI_AEST_GIC_REDISTRIBUTOR         2
#define ACPI_AEST_GIC_ITS                   3
#define ACPI_AEST_GIC_RESERVED              4	



typedef struct acpi_aest_node_interface {
	u8 type;
	u8 reserved[3];
	u32 flags;
	u64 address;
	u32 error_record_index;
	u32 error_record_count;
	u64 error_record_implemented;
	u64 error_status_reporting;
	u64 addressing_mode;

} acpi_aest_node_interface;



#define ACPI_AEST_NODE_SYSTEM_REGISTER      0
#define ACPI_AEST_NODE_MEMORY_MAPPED        1
#define ACPI_AEST_XFACE_RESERVED            2	



typedef struct acpi_aest_node_interrupt {
	u8 type;
	u8 reserved[2];
	u8 flags;
	u32 gsiv;
	u8 iort_id;
	u8 reserved1[3];

} acpi_aest_node_interrupt;



#define ACPI_AEST_NODE_FAULT_HANDLING       0
#define ACPI_AEST_NODE_ERROR_RECOVERY       1
#define ACPI_AEST_XRUPT_RESERVED            2	


struct acpi_table_agdi {
	struct acpi_table_header header;	
	u8 flags;
	u8 reserved[3];
	u32 sdei_event;
	u32 gsiv;
};



#define ACPI_AGDI_SIGNALING_MODE (1)



struct acpi_table_apmt {
	struct acpi_table_header header;	
};

#define ACPI_APMT_NODE_ID_LENGTH                4


struct acpi_apmt_node {
	u16 length;
	u8 flags;
	u8 type;
	u32 id;
	u64 inst_primary;
	u32 inst_secondary;
	u64 base_address0;
	u64 base_address1;
	u32 ovflw_irq;
	u32 reserved;
	u32 ovflw_irq_flags;
	u32 proc_affinity;
	u32 impl_id;
};



#define ACPI_APMT_FLAGS_DUAL_PAGE               (1<<0)
#define ACPI_APMT_FLAGS_AFFINITY                (1<<1)
#define ACPI_APMT_FLAGS_ATOMIC                  (1<<2)



#define ACPI_APMT_FLAGS_DUAL_PAGE_NSUPP         (0<<0)
#define ACPI_APMT_FLAGS_DUAL_PAGE_SUPP          (1<<0)


#define ACPI_APMT_FLAGS_AFFINITY_PROC           (0<<1)
#define ACPI_APMT_FLAGS_AFFINITY_PROC_CONTAINER (1<<1)


#define ACPI_APMT_FLAGS_ATOMIC_NSUPP            (0<<2)
#define ACPI_APMT_FLAGS_ATOMIC_SUPP             (1<<2)



enum acpi_apmt_node_type {
	ACPI_APMT_NODE_TYPE_MC = 0x00,
	ACPI_APMT_NODE_TYPE_SMMU = 0x01,
	ACPI_APMT_NODE_TYPE_PCIE_ROOT = 0x02,
	ACPI_APMT_NODE_TYPE_ACPI = 0x03,
	ACPI_APMT_NODE_TYPE_CACHE = 0x04,
	ACPI_APMT_NODE_TYPE_COUNT
};



#define ACPI_APMT_OVFLW_IRQ_FLAGS_MODE          (1<<0)
#define ACPI_APMT_OVFLW_IRQ_FLAGS_TYPE          (1<<1)



#define ACPI_APMT_OVFLW_IRQ_FLAGS_MODE_LEVEL    (0<<0)
#define ACPI_APMT_OVFLW_IRQ_FLAGS_MODE_EDGE     (1<<0)



#define ACPI_APMT_OVFLW_IRQ_FLAGS_TYPE_WIRED    (0<<1)



struct acpi_table_bdat {
	struct acpi_table_header header;
	struct acpi_generic_address gas;
};



struct acpi_table_ccel {
	struct acpi_table_header header;	
	u8 CCtype;
	u8 Ccsub_type;
	u16 reserved;
	u64 log_area_minimum_length;
	u64 log_area_start_address;
};



struct acpi_table_iort {
	struct acpi_table_header header;
	u32 node_count;
	u32 node_offset;
	u32 reserved;
};


struct acpi_iort_node {
	u8 type;
	u16 length;
	u8 revision;
	u32 identifier;
	u32 mapping_count;
	u32 mapping_offset;
	char node_data[];
};



enum acpi_iort_node_type {
	ACPI_IORT_NODE_ITS_GROUP = 0x00,
	ACPI_IORT_NODE_NAMED_COMPONENT = 0x01,
	ACPI_IORT_NODE_PCI_ROOT_COMPLEX = 0x02,
	ACPI_IORT_NODE_SMMU = 0x03,
	ACPI_IORT_NODE_SMMU_V3 = 0x04,
	ACPI_IORT_NODE_PMCG = 0x05,
	ACPI_IORT_NODE_RMR = 0x06,
};

struct acpi_iort_id_mapping {
	u32 input_base;		
	u32 id_count;		
	u32 output_base;	
	u32 output_reference;	
	u32 flags;
};



#define ACPI_IORT_ID_SINGLE_MAPPING (1)

struct acpi_iort_memory_access {
	u32 cache_coherency;
	u8 hints;
	u16 reserved;
	u8 memory_flags;
};



#define ACPI_IORT_NODE_COHERENT         0x00000001	
#define ACPI_IORT_NODE_NOT_COHERENT     0x00000000	



#define ACPI_IORT_HT_TRANSIENT          (1)
#define ACPI_IORT_HT_WRITE              (1<<1)
#define ACPI_IORT_HT_READ               (1<<2)
#define ACPI_IORT_HT_OVERRIDE           (1<<3)



#define ACPI_IORT_MF_COHERENCY          (1)
#define ACPI_IORT_MF_ATTRIBUTES         (1<<1)


struct acpi_iort_its_group {
	u32 its_count;
	u32 identifiers[];	
};

struct acpi_iort_named_component {
	u32 node_flags;
	u64 memory_properties;	
	u8 memory_address_limit;	
	char device_name[];	
};



#define ACPI_IORT_NC_STALL_SUPPORTED    (1)
#define ACPI_IORT_NC_PASID_BITS         (31<<1)

struct acpi_iort_root_complex {
	u64 memory_properties;	
	u32 ats_attribute;
	u32 pci_segment_number;
	u8 memory_address_limit;	
	u16 pasid_capabilities;	
	u8 reserved[];		
};



#define ACPI_IORT_ATS_SUPPORTED         (1)	
#define ACPI_IORT_PRI_SUPPORTED         (1<<1)	
#define ACPI_IORT_PASID_FWD_SUPPORTED   (1<<2)	


#define ACPI_IORT_PASID_MAX_WIDTH       (0x1F)	

struct acpi_iort_smmu {
	u64 base_address;	
	u64 span;		
	u32 model;
	u32 flags;
	u32 global_interrupt_offset;
	u32 context_interrupt_count;
	u32 context_interrupt_offset;
	u32 pmu_interrupt_count;
	u32 pmu_interrupt_offset;
	u64 interrupts[];	
};



#define ACPI_IORT_SMMU_V1               0x00000000	
#define ACPI_IORT_SMMU_V2               0x00000001	
#define ACPI_IORT_SMMU_CORELINK_MMU400  0x00000002	
#define ACPI_IORT_SMMU_CORELINK_MMU500  0x00000003	
#define ACPI_IORT_SMMU_CORELINK_MMU401  0x00000004	
#define ACPI_IORT_SMMU_CAVIUM_THUNDERX  0x00000005	



#define ACPI_IORT_SMMU_DVM_SUPPORTED    (1)
#define ACPI_IORT_SMMU_COHERENT_WALK    (1<<1)



struct acpi_iort_smmu_gsi {
	u32 nsg_irpt;
	u32 nsg_irpt_flags;
	u32 nsg_cfg_irpt;
	u32 nsg_cfg_irpt_flags;
};

struct acpi_iort_smmu_v3 {
	u64 base_address;	
	u32 flags;
	u32 reserved;
	u64 vatos_address;
	u32 model;
	u32 event_gsiv;
	u32 pri_gsiv;
	u32 gerr_gsiv;
	u32 sync_gsiv;
	u32 pxm;
	u32 id_mapping_index;
};



#define ACPI_IORT_SMMU_V3_GENERIC           0x00000000	
#define ACPI_IORT_SMMU_V3_HISILICON_HI161X  0x00000001	
#define ACPI_IORT_SMMU_V3_CAVIUM_CN99XX     0x00000002	



#define ACPI_IORT_SMMU_V3_COHACC_OVERRIDE   (1)
#define ACPI_IORT_SMMU_V3_HTTU_OVERRIDE     (3<<1)
#define ACPI_IORT_SMMU_V3_PXM_VALID         (1<<3)
#define ACPI_IORT_SMMU_V3_DEVICEID_VALID    (1<<4)

struct acpi_iort_pmcg {
	u64 page0_base_address;
	u32 overflow_gsiv;
	u32 node_reference;
	u64 page1_base_address;
};

struct acpi_iort_rmr {
	u32 flags;
	u32 rmr_count;
	u32 rmr_offset;
};


#define ACPI_IORT_RMR_REMAP_PERMITTED      (1)
#define ACPI_IORT_RMR_ACCESS_PRIVILEGE     (1<<1)


#define ACPI_IORT_RMR_ACCESS_ATTRIBUTES(flags)          (((flags) >> 2) & 0xFF)



#define ACPI_IORT_RMR_ATTR_DEVICE_NGNRNE   0x00
#define ACPI_IORT_RMR_ATTR_DEVICE_NGNRE    0x01
#define ACPI_IORT_RMR_ATTR_DEVICE_NGRE     0x02
#define ACPI_IORT_RMR_ATTR_DEVICE_GRE      0x03
#define ACPI_IORT_RMR_ATTR_NORMAL_NC       0x04
#define ACPI_IORT_RMR_ATTR_NORMAL_IWB_OWB  0x05

struct acpi_iort_rmr_desc {
	u64 base_address;
	u64 length;
	u32 reserved;
};



struct acpi_table_ivrs {
	struct acpi_table_header header;	
	u32 info;		
	u64 reserved;
};



#define ACPI_IVRS_PHYSICAL_SIZE     0x00007F00	
#define ACPI_IVRS_VIRTUAL_SIZE      0x003F8000	
#define ACPI_IVRS_ATS_RESERVED      0x00400000	



struct acpi_ivrs_header {
	u8 type;		
	u8 flags;
	u16 length;		
	u16 device_id;		
};



enum acpi_ivrs_type {
	ACPI_IVRS_TYPE_HARDWARE1 = 0x10,
	ACPI_IVRS_TYPE_HARDWARE2 = 0x11,
	ACPI_IVRS_TYPE_HARDWARE3 = 0x40,
	ACPI_IVRS_TYPE_MEMORY1 = 0x20,
	ACPI_IVRS_TYPE_MEMORY2 = 0x21,
	ACPI_IVRS_TYPE_MEMORY3 = 0x22
};



#define ACPI_IVHD_TT_ENABLE         (1)
#define ACPI_IVHD_PASS_PW           (1<<1)
#define ACPI_IVHD_RES_PASS_PW       (1<<2)
#define ACPI_IVHD_ISOC              (1<<3)
#define ACPI_IVHD_IOTLB             (1<<4)



#define ACPI_IVMD_UNITY             (1)
#define ACPI_IVMD_READ              (1<<1)
#define ACPI_IVMD_WRITE             (1<<2)
#define ACPI_IVMD_EXCLUSION_RANGE   (1<<3)





struct acpi_ivrs_hardware_10 {
	struct acpi_ivrs_header header;
	u16 capability_offset;	
	u64 base_address;	
	u16 pci_segment_group;
	u16 info;		
	u32 feature_reporting;
};



struct acpi_ivrs_hardware_11 {
	struct acpi_ivrs_header header;
	u16 capability_offset;	
	u64 base_address;	
	u16 pci_segment_group;
	u16 info;		
	u32 attributes;
	u64 efr_register_image;
	u64 reserved;
};



#define ACPI_IVHD_MSI_NUMBER_MASK   0x001F	
#define ACPI_IVHD_UNIT_ID_MASK      0x1F00	


struct acpi_ivrs_de_header {
	u8 type;
	u16 id;
	u8 data_setting;
};



#define ACPI_IVHD_ENTRY_LENGTH      0xC0



enum acpi_ivrs_device_entry_type {
	

	ACPI_IVRS_TYPE_PAD4 = 0,
	ACPI_IVRS_TYPE_ALL = 1,
	ACPI_IVRS_TYPE_SELECT = 2,
	ACPI_IVRS_TYPE_START = 3,
	ACPI_IVRS_TYPE_END = 4,

	

	ACPI_IVRS_TYPE_PAD8 = 64,
	ACPI_IVRS_TYPE_NOT_USED = 65,
	ACPI_IVRS_TYPE_ALIAS_SELECT = 66,	
	ACPI_IVRS_TYPE_ALIAS_START = 67,	
	ACPI_IVRS_TYPE_EXT_SELECT = 70,	
	ACPI_IVRS_TYPE_EXT_START = 71,	
	ACPI_IVRS_TYPE_SPECIAL = 72,	

	

	ACPI_IVRS_TYPE_HID = 240	
};



#define ACPI_IVHD_INIT_PASS         (1)
#define ACPI_IVHD_EINT_PASS         (1<<1)
#define ACPI_IVHD_NMI_PASS          (1<<2)
#define ACPI_IVHD_SYSTEM_MGMT       (3<<4)
#define ACPI_IVHD_LINT0_PASS        (1<<6)
#define ACPI_IVHD_LINT1_PASS        (1<<7)



struct acpi_ivrs_device4 {
	struct acpi_ivrs_de_header header;
};



struct acpi_ivrs_device8a {
	struct acpi_ivrs_de_header header;
	u8 reserved1;
	u16 used_id;
	u8 reserved2;
};



struct acpi_ivrs_device8b {
	struct acpi_ivrs_de_header header;
	u32 extended_data;
};



#define ACPI_IVHD_ATS_DISABLED      (1<<31)



struct acpi_ivrs_device8c {
	struct acpi_ivrs_de_header header;
	u8 handle;
	u16 used_id;
	u8 variety;
};



#define ACPI_IVHD_IOAPIC            1
#define ACPI_IVHD_HPET              2



struct acpi_ivrs_device_hid {
	struct acpi_ivrs_de_header header;
	u64 acpi_hid;
	u64 acpi_cid;
	u8 uid_type;
	u8 uid_length;
};



#define ACPI_IVRS_UID_NOT_PRESENT   0
#define ACPI_IVRS_UID_IS_INTEGER    1
#define ACPI_IVRS_UID_IS_STRING     2



struct acpi_ivrs_memory {
	struct acpi_ivrs_header header;
	u16 aux_data;
	u64 reserved;
	u64 start_address;
	u64 memory_length;
};



struct acpi_table_lpit {
	struct acpi_table_header header;	
};



struct acpi_lpit_header {
	u32 type;		
	u32 length;		
	u16 unique_id;
	u16 reserved;
	u32 flags;
};



enum acpi_lpit_type {
	ACPI_LPIT_TYPE_NATIVE_CSTATE = 0x00,
	ACPI_LPIT_TYPE_RESERVED = 0x01	
};



#define ACPI_LPIT_STATE_DISABLED    (1)
#define ACPI_LPIT_NO_COUNTER        (1<<1)





struct acpi_lpit_native {
	struct acpi_lpit_header header;
	struct acpi_generic_address entry_trigger;
	u32 residency;
	u32 latency;
	struct acpi_generic_address residency_counter;
	u64 counter_frequency;
};



struct acpi_table_madt {
	struct acpi_table_header header;	
	u32 address;		
	u32 flags;
};



#define ACPI_MADT_PCAT_COMPAT       (1)	



#define ACPI_MADT_DUAL_PIC          1
#define ACPI_MADT_MULTIPLE_APIC     0



enum acpi_madt_type {
	ACPI_MADT_TYPE_LOCAL_APIC = 0,
	ACPI_MADT_TYPE_IO_APIC = 1,
	ACPI_MADT_TYPE_INTERRUPT_OVERRIDE = 2,
	ACPI_MADT_TYPE_NMI_SOURCE = 3,
	ACPI_MADT_TYPE_LOCAL_APIC_NMI = 4,
	ACPI_MADT_TYPE_LOCAL_APIC_OVERRIDE = 5,
	ACPI_MADT_TYPE_IO_SAPIC = 6,
	ACPI_MADT_TYPE_LOCAL_SAPIC = 7,
	ACPI_MADT_TYPE_INTERRUPT_SOURCE = 8,
	ACPI_MADT_TYPE_LOCAL_X2APIC = 9,
	ACPI_MADT_TYPE_LOCAL_X2APIC_NMI = 10,
	ACPI_MADT_TYPE_GENERIC_INTERRUPT = 11,
	ACPI_MADT_TYPE_GENERIC_DISTRIBUTOR = 12,
	ACPI_MADT_TYPE_GENERIC_MSI_FRAME = 13,
	ACPI_MADT_TYPE_GENERIC_REDISTRIBUTOR = 14,
	ACPI_MADT_TYPE_GENERIC_TRANSLATOR = 15,
	ACPI_MADT_TYPE_MULTIPROC_WAKEUP = 16,
	ACPI_MADT_TYPE_CORE_PIC = 17,
	ACPI_MADT_TYPE_LIO_PIC = 18,
	ACPI_MADT_TYPE_HT_PIC = 19,
	ACPI_MADT_TYPE_EIO_PIC = 20,
	ACPI_MADT_TYPE_MSI_PIC = 21,
	ACPI_MADT_TYPE_BIO_PIC = 22,
	ACPI_MADT_TYPE_LPC_PIC = 23,
	ACPI_MADT_TYPE_RINTC = 24,
	ACPI_MADT_TYPE_IMSIC = 25,
	ACPI_MADT_TYPE_APLIC = 26,
	ACPI_MADT_TYPE_PLIC = 27,
	ACPI_MADT_TYPE_RESERVED = 28,	
	ACPI_MADT_TYPE_OEM_RESERVED = 0x80	
};





struct acpi_madt_local_apic {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u8 id;			
	u32 lapic_flags;
};



struct acpi_madt_io_apic {
	struct acpi_subtable_header header;
	u8 id;			
	u8 reserved;		
	u32 address;		
	u32 global_irq_base;	
};



struct acpi_madt_interrupt_override {
	struct acpi_subtable_header header;
	u8 bus;			
	u8 source_irq;		
	u32 global_irq;		
	u16 inti_flags;
};



struct acpi_madt_nmi_source {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 global_irq;		
};



struct acpi_madt_local_apic_nmi {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u16 inti_flags;
	u8 lint;		
};



struct acpi_madt_local_apic_override {
	struct acpi_subtable_header header;
	u16 reserved;		
	u64 address;		
};



struct acpi_madt_io_sapic {
	struct acpi_subtable_header header;
	u8 id;			
	u8 reserved;		
	u32 global_irq_base;	
	u64 address;		
};



struct acpi_madt_local_sapic {
	struct acpi_subtable_header header;
	u8 processor_id;	
	u8 id;			
	u8 eid;			
	u8 reserved[3];		
	u32 lapic_flags;
	u32 uid;		
	char uid_string[];	
};



struct acpi_madt_interrupt_source {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u8 type;		
	u8 id;			
	u8 eid;			
	u8 io_sapic_vector;	
	u32 global_irq;		
	u32 flags;		
};



#define ACPI_MADT_CPEI_OVERRIDE     (1)



struct acpi_madt_local_x2apic {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 local_apic_id;	
	u32 lapic_flags;
	u32 uid;		
};



struct acpi_madt_local_x2apic_nmi {
	struct acpi_subtable_header header;
	u16 inti_flags;
	u32 uid;		
	u8 lint;		
	u8 reserved[3];		
};



struct acpi_madt_generic_interrupt {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 cpu_interface_number;
	u32 uid;
	u32 flags;
	u32 parking_version;
	u32 performance_interrupt;
	u64 parked_address;
	u64 base_address;
	u64 gicv_base_address;
	u64 gich_base_address;
	u32 vgic_interrupt;
	u64 gicr_base_address;
	u64 arm_mpidr;
	u8 efficiency_class;
	u8 reserved2[1];
	u16 spe_interrupt;	
	u16 trbe_interrupt;	
};




#define ACPI_MADT_PERFORMANCE_IRQ_MODE  (1<<1)	
#define ACPI_MADT_VGIC_IRQ_MODE         (1<<2)	



struct acpi_madt_generic_distributor {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 gic_id;
	u64 base_address;
	u32 global_irq_base;
	u8 version;
	u8 reserved2[3];	
};



enum acpi_madt_gic_version {
	ACPI_MADT_GIC_VERSION_NONE = 0,
	ACPI_MADT_GIC_VERSION_V1 = 1,
	ACPI_MADT_GIC_VERSION_V2 = 2,
	ACPI_MADT_GIC_VERSION_V3 = 3,
	ACPI_MADT_GIC_VERSION_V4 = 4,
	ACPI_MADT_GIC_VERSION_RESERVED = 5	
};



struct acpi_madt_generic_msi_frame {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 msi_frame_id;
	u64 base_address;
	u32 flags;
	u16 spi_count;
	u16 spi_base;
};



#define ACPI_MADT_OVERRIDE_SPI_VALUES   (1)



struct acpi_madt_generic_redistributor {
	struct acpi_subtable_header header;
	u16 reserved;		
	u64 base_address;
	u32 length;
};



struct acpi_madt_generic_translator {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 translation_id;
	u64 base_address;
	u32 reserved2;
};



struct acpi_madt_multiproc_wakeup {
	struct acpi_subtable_header header;
	u16 mailbox_version;
	u32 reserved;		
	u64 base_address;
};

#define ACPI_MULTIPROC_WAKEUP_MB_OS_SIZE        2032
#define ACPI_MULTIPROC_WAKEUP_MB_FIRMWARE_SIZE  2048

struct acpi_madt_multiproc_wakeup_mailbox {
	u16 command;
	u16 reserved;		
	u32 apic_id;
	u64 wakeup_vector;
	u8 reserved_os[ACPI_MULTIPROC_WAKEUP_MB_OS_SIZE];	
	u8 reserved_firmware[ACPI_MULTIPROC_WAKEUP_MB_FIRMWARE_SIZE];	
};

#define ACPI_MP_WAKE_COMMAND_WAKEUP    1



struct acpi_madt_core_pic {
	struct acpi_subtable_header header;
	u8 version;
	u32 processor_id;
	u32 core_id;
	u32 flags;
};



enum acpi_madt_core_pic_version {
	ACPI_MADT_CORE_PIC_VERSION_NONE = 0,
	ACPI_MADT_CORE_PIC_VERSION_V1 = 1,
	ACPI_MADT_CORE_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_lio_pic {
	struct acpi_subtable_header header;
	u8 version;
	u64 address;
	u16 size;
	u8 cascade[2];
	u32 cascade_map[2];
};



enum acpi_madt_lio_pic_version {
	ACPI_MADT_LIO_PIC_VERSION_NONE = 0,
	ACPI_MADT_LIO_PIC_VERSION_V1 = 1,
	ACPI_MADT_LIO_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_ht_pic {
	struct acpi_subtable_header header;
	u8 version;
	u64 address;
	u16 size;
	u8 cascade[8];
};



enum acpi_madt_ht_pic_version {
	ACPI_MADT_HT_PIC_VERSION_NONE = 0,
	ACPI_MADT_HT_PIC_VERSION_V1 = 1,
	ACPI_MADT_HT_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_eio_pic {
	struct acpi_subtable_header header;
	u8 version;
	u8 cascade;
	u8 node;
	u64 node_map;
};



enum acpi_madt_eio_pic_version {
	ACPI_MADT_EIO_PIC_VERSION_NONE = 0,
	ACPI_MADT_EIO_PIC_VERSION_V1 = 1,
	ACPI_MADT_EIO_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_msi_pic {
	struct acpi_subtable_header header;
	u8 version;
	u64 msg_address;
	u32 start;
	u32 count;
};



enum acpi_madt_msi_pic_version {
	ACPI_MADT_MSI_PIC_VERSION_NONE = 0,
	ACPI_MADT_MSI_PIC_VERSION_V1 = 1,
	ACPI_MADT_MSI_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_bio_pic {
	struct acpi_subtable_header header;
	u8 version;
	u64 address;
	u16 size;
	u16 id;
	u16 gsi_base;
};



enum acpi_madt_bio_pic_version {
	ACPI_MADT_BIO_PIC_VERSION_NONE = 0,
	ACPI_MADT_BIO_PIC_VERSION_V1 = 1,
	ACPI_MADT_BIO_PIC_VERSION_RESERVED = 2	
};



struct acpi_madt_lpc_pic {
	struct acpi_subtable_header header;
	u8 version;
	u64 address;
	u16 size;
	u8 cascade;
};



enum acpi_madt_lpc_pic_version {
	ACPI_MADT_LPC_PIC_VERSION_NONE = 0,
	ACPI_MADT_LPC_PIC_VERSION_V1 = 1,
	ACPI_MADT_LPC_PIC_VERSION_RESERVED = 2	
};


struct acpi_madt_rintc {
	struct acpi_subtable_header header;
	u8 version;
	u8 reserved;
	u32 flags;
	u64 hart_id;
	u32 uid;		
	u32 ext_intc_id;	
	u64 imsic_addr;		
	u32 imsic_size;		
};



enum acpi_madt_rintc_version {
	ACPI_MADT_RINTC_VERSION_NONE = 0,
	ACPI_MADT_RINTC_VERSION_V1 = 1,
	ACPI_MADT_RINTC_VERSION_RESERVED = 2	
};


struct acpi_madt_imsic {
	struct acpi_subtable_header header;
	u8 version;
	u8 reserved;
	u32 flags;
	u16 num_ids;
	u16 num_guest_ids;
	u8 guest_index_bits;
	u8 hart_index_bits;
	u8 group_index_bits;
	u8 group_index_shift;
};


struct acpi_madt_aplic {
	struct acpi_subtable_header header;
	u8 version;
	u8 id;
	u32 flags;
	u8 hw_id[8];
	u16 num_idcs;
	u16 num_sources;
	u32 gsi_base;
	u64 base_addr;
	u32 size;
};


struct acpi_madt_plic {
	struct acpi_subtable_header header;
	u8 version;
	u8 id;
	u8 hw_id[8];
	u16 num_irqs;
	u16 max_prio;
	u32 flags;
	u32 size;
	u64 base_addr;
	u32 gsi_base;
};



struct acpi_madt_oem_data {
	ACPI_FLEX_ARRAY(u8, oem_data);
};





#define ACPI_MADT_ENABLED           (1)	
#define ACPI_MADT_ONLINE_CAPABLE    (2)	



#define ACPI_MADT_POLARITY_MASK     (3)	
#define ACPI_MADT_TRIGGER_MASK      (3<<2)	



#define ACPI_MADT_POLARITY_CONFORMS       0
#define ACPI_MADT_POLARITY_ACTIVE_HIGH    1
#define ACPI_MADT_POLARITY_RESERVED       2
#define ACPI_MADT_POLARITY_ACTIVE_LOW     3

#define ACPI_MADT_TRIGGER_CONFORMS        (0)
#define ACPI_MADT_TRIGGER_EDGE            (1<<2)
#define ACPI_MADT_TRIGGER_RESERVED        (2<<2)
#define ACPI_MADT_TRIGGER_LEVEL           (3<<2)



struct acpi_table_mcfg {
	struct acpi_table_header header;	
	u8 reserved[8];
};



struct acpi_mcfg_allocation {
	u64 address;		
	u16 pci_segment;	
	u8 start_bus_number;	
	u8 end_bus_number;	
	u32 reserved;
};



struct acpi_table_mchi {
	struct acpi_table_header header;	
	u8 interface_type;
	u8 protocol;
	u64 protocol_data;
	u8 interrupt_type;
	u8 gpe;
	u8 pci_device_flag;
	u32 global_interrupt;
	struct acpi_generic_address control_register;
	u8 pci_segment;
	u8 pci_bus;
	u8 pci_device;
	u8 pci_function;
};




enum acpi_mpam_locator_type {
	ACPI_MPAM_LOCATION_TYPE_PROCESSOR_CACHE = 0,
	ACPI_MPAM_LOCATION_TYPE_MEMORY = 1,
	ACPI_MPAM_LOCATION_TYPE_SMMU = 2,
	ACPI_MPAM_LOCATION_TYPE_MEMORY_CACHE = 3,
	ACPI_MPAM_LOCATION_TYPE_ACPI_DEVICE = 4,
	ACPI_MPAM_LOCATION_TYPE_INTERCONNECT = 5,
	ACPI_MPAM_LOCATION_TYPE_UNKNOWN = 0xFF
};


struct acpi_mpam_func_deps {
	u32 producer;
	u32 reserved;
};


struct acpi_mpam_resource_cache_locator {
	u64 cache_reference;
	u32 reserved;
};


struct acpi_mpam_resource_memory_locator {
	u64 proximity_domain;
	u32 reserved;
};


struct acpi_mpam_resource_smmu_locator {
	u64 smmu_interface;
	u32 reserved;
};


struct acpi_mpam_resource_memcache_locator {
	u8 reserved[7];
	u8 level;
	u32 reference;
};


struct acpi_mpam_resource_acpi_locator {
	u64 acpi_hw_id;
	u32 acpi_unique_id;
};


struct acpi_mpam_resource_interconnect_locator {
	u64 inter_connect_desc_tbl_off;
	u32 reserved;
};


struct acpi_mpam_resource_generic_locator {
	u64 descriptor1;
	u32 descriptor2;
};

union acpi_mpam_resource_locator {
	struct acpi_mpam_resource_cache_locator cache_locator;
	struct acpi_mpam_resource_memory_locator memory_locator;
	struct acpi_mpam_resource_smmu_locator smmu_locator;
	struct acpi_mpam_resource_memcache_locator mem_cache_locator;
	struct acpi_mpam_resource_acpi_locator acpi_locator;
	struct acpi_mpam_resource_interconnect_locator interconnect_ifc_locator;
	struct acpi_mpam_resource_generic_locator generic_locator;
};


struct acpi_mpam_resource_node {
	u32 identifier;
	u8 ris_index;
	u16 reserved1;
	u8 locator_type;
	union acpi_mpam_resource_locator locator;
	u32 num_functional_deps;
};


struct acpi_mpam_msc_node {
	u16 length;
	u8 interface_type;
	u8 reserved;
	u32 identifier;
	u64 base_address;
	u32 mmio_size;
	u32 overflow_interrupt;
	u32 overflow_interrupt_flags;
	u32 reserved1;
	u32 overflow_interrupt_affinity;
	u32 error_interrupt;
	u32 error_interrupt_flags;
	u32 reserved2;
	u32 error_interrupt_affinity;
	u32 max_nrdy_usec;
	u64 hardware_id_linked_device;
	u32 instance_id_linked_device;
	u32 num_resouce_nodes;
};

struct acpi_table_mpam {
	struct acpi_table_header header;	
};



#define ACPI_MPST_CHANNEL_INFO \
	u8                              channel_id; \
	u8                              reserved1[3]; \
	u16                             power_node_count; \
	u16                             reserved2;



struct acpi_table_mpst {
	struct acpi_table_header header;	
	 ACPI_MPST_CHANNEL_INFO	
};



struct acpi_mpst_channel {
	ACPI_MPST_CHANNEL_INFO	
};



struct acpi_mpst_power_node {
	u8 flags;
	u8 reserved1;
	u16 node_id;
	u32 length;
	u64 range_address;
	u64 range_length;
	u32 num_power_states;
	u32 num_physical_components;
};



#define ACPI_MPST_ENABLED               1
#define ACPI_MPST_POWER_MANAGED         2
#define ACPI_MPST_HOT_PLUG_CAPABLE      4



struct acpi_mpst_power_state {
	u8 power_state;
	u8 info_index;
};



struct acpi_mpst_component {
	u16 component_id;
};



struct acpi_mpst_data_hdr {
	u16 characteristics_count;
	u16 reserved;
};

struct acpi_mpst_power_data {
	u8 structure_id;
	u8 flags;
	u16 reserved1;
	u32 average_power;
	u32 power_saving;
	u64 exit_latency;
	u64 reserved2;
};



#define ACPI_MPST_PRESERVE              1
#define ACPI_MPST_AUTOENTRY             2
#define ACPI_MPST_AUTOEXIT              4



struct acpi_mpst_shared {
	u32 signature;
	u16 pcc_command;
	u16 pcc_status;
	u32 command_register;
	u32 status_register;
	u32 power_state_id;
	u32 power_node_id;
	u64 energy_consumed;
	u64 average_power;
};



struct acpi_table_msct {
	struct acpi_table_header header;	
	u32 proximity_offset;	
	u32 max_proximity_domains;	
	u32 max_clock_domains;	
	u64 max_address;	
};



struct acpi_msct_proximity {
	u8 revision;
	u8 length;
	u32 range_start;	
	u32 range_end;		
	u32 processor_capacity;
	u64 memory_capacity;	
};





struct acpi_table_msdm {
	struct acpi_table_header header;	
};



struct acpi_table_nfit {
	struct acpi_table_header header;	
	u32 reserved;		
};



struct acpi_nfit_header {
	u16 type;
	u16 length;
};



enum acpi_nfit_type {
	ACPI_NFIT_TYPE_SYSTEM_ADDRESS = 0,
	ACPI_NFIT_TYPE_MEMORY_MAP = 1,
	ACPI_NFIT_TYPE_INTERLEAVE = 2,
	ACPI_NFIT_TYPE_SMBIOS = 3,
	ACPI_NFIT_TYPE_CONTROL_REGION = 4,
	ACPI_NFIT_TYPE_DATA_REGION = 5,
	ACPI_NFIT_TYPE_FLUSH_ADDRESS = 6,
	ACPI_NFIT_TYPE_CAPABILITIES = 7,
	ACPI_NFIT_TYPE_RESERVED = 8	
};





struct acpi_nfit_system_address {
	struct acpi_nfit_header header;
	u16 range_index;
	u16 flags;
	u32 reserved;		
	u32 proximity_domain;
	u8 range_guid[16];
	u64 address;
	u64 length;
	u64 memory_mapping;
	u64 location_cookie;	
};



#define ACPI_NFIT_ADD_ONLINE_ONLY       (1)	
#define ACPI_NFIT_PROXIMITY_VALID       (1<<1)	
#define ACPI_NFIT_LOCATION_COOKIE_VALID (1<<2)	





struct acpi_nfit_memory_map {
	struct acpi_nfit_header header;
	u32 device_handle;
	u16 physical_id;
	u16 region_id;
	u16 range_index;
	u16 region_index;
	u64 region_size;
	u64 region_offset;
	u64 address;
	u16 interleave_index;
	u16 interleave_ways;
	u16 flags;
	u16 reserved;		
};



#define ACPI_NFIT_MEM_SAVE_FAILED       (1)	
#define ACPI_NFIT_MEM_RESTORE_FAILED    (1<<1)	
#define ACPI_NFIT_MEM_FLUSH_FAILED      (1<<2)	
#define ACPI_NFIT_MEM_NOT_ARMED         (1<<3)	
#define ACPI_NFIT_MEM_HEALTH_OBSERVED   (1<<4)	
#define ACPI_NFIT_MEM_HEALTH_ENABLED    (1<<5)	
#define ACPI_NFIT_MEM_MAP_FAILED        (1<<6)	



struct acpi_nfit_interleave {
	struct acpi_nfit_header header;
	u16 interleave_index;
	u16 reserved;		
	u32 line_count;
	u32 line_size;
	u32 line_offset[];	
};



struct acpi_nfit_smbios {
	struct acpi_nfit_header header;
	u32 reserved;		
	u8 data[];		
};



struct acpi_nfit_control_region {
	struct acpi_nfit_header header;
	u16 region_index;
	u16 vendor_id;
	u16 device_id;
	u16 revision_id;
	u16 subsystem_vendor_id;
	u16 subsystem_device_id;
	u16 subsystem_revision_id;
	u8 valid_fields;
	u8 manufacturing_location;
	u16 manufacturing_date;
	u8 reserved[2];		
	u32 serial_number;
	u16 code;
	u16 windows;
	u64 window_size;
	u64 command_offset;
	u64 command_size;
	u64 status_offset;
	u64 status_size;
	u16 flags;
	u8 reserved1[6];	
};



#define ACPI_NFIT_CONTROL_BUFFERED          (1)	



#define ACPI_NFIT_CONTROL_MFG_INFO_VALID    (1)	



struct acpi_nfit_data_region {
	struct acpi_nfit_header header;
	u16 region_index;
	u16 windows;
	u64 offset;
	u64 size;
	u64 capacity;
	u64 start_address;
};



struct acpi_nfit_flush_address {
	struct acpi_nfit_header header;
	u32 device_handle;
	u16 hint_count;
	u8 reserved[6];		
	u64 hint_address[];	
};



struct acpi_nfit_capabilities {
	struct acpi_nfit_header header;
	u8 highest_capability;
	u8 reserved[3];		
	u32 capabilities;
	u32 reserved2;
};



#define ACPI_NFIT_CAPABILITY_CACHE_FLUSH       (1)	
#define ACPI_NFIT_CAPABILITY_MEM_FLUSH         (1<<1)	
#define ACPI_NFIT_CAPABILITY_MEM_MIRRORING     (1<<2)	


struct nfit_device_handle {
	u32 handle;
};



#define ACPI_NFIT_DIMM_NUMBER_MASK              0x0000000F
#define ACPI_NFIT_CHANNEL_NUMBER_MASK           0x000000F0
#define ACPI_NFIT_MEMORY_ID_MASK                0x00000F00
#define ACPI_NFIT_SOCKET_ID_MASK                0x0000F000
#define ACPI_NFIT_NODE_ID_MASK                  0x0FFF0000

#define ACPI_NFIT_DIMM_NUMBER_OFFSET            0
#define ACPI_NFIT_CHANNEL_NUMBER_OFFSET         4
#define ACPI_NFIT_MEMORY_ID_OFFSET              8
#define ACPI_NFIT_SOCKET_ID_OFFSET              12
#define ACPI_NFIT_NODE_ID_OFFSET                16



#define ACPI_NFIT_BUILD_DEVICE_HANDLE(dimm, channel, memory, socket, node) \
	((dimm)                                         | \
	((channel) << ACPI_NFIT_CHANNEL_NUMBER_OFFSET)  | \
	((memory)  << ACPI_NFIT_MEMORY_ID_OFFSET)       | \
	((socket)  << ACPI_NFIT_SOCKET_ID_OFFSET)       | \
	((node)    << ACPI_NFIT_NODE_ID_OFFSET))



#define ACPI_NFIT_GET_DIMM_NUMBER(handle) \
	((handle) & ACPI_NFIT_DIMM_NUMBER_MASK)

#define ACPI_NFIT_GET_CHANNEL_NUMBER(handle) \
	(((handle) & ACPI_NFIT_CHANNEL_NUMBER_MASK) >> ACPI_NFIT_CHANNEL_NUMBER_OFFSET)

#define ACPI_NFIT_GET_MEMORY_ID(handle) \
	(((handle) & ACPI_NFIT_MEMORY_ID_MASK)      >> ACPI_NFIT_MEMORY_ID_OFFSET)

#define ACPI_NFIT_GET_SOCKET_ID(handle) \
	(((handle) & ACPI_NFIT_SOCKET_ID_MASK)      >> ACPI_NFIT_SOCKET_ID_OFFSET)

#define ACPI_NFIT_GET_NODE_ID(handle) \
	(((handle) & ACPI_NFIT_NODE_ID_MASK)        >> ACPI_NFIT_NODE_ID_OFFSET)





struct acpi_table_nhlt {
	struct acpi_table_header header;	
	u8 endpoint_count;
};

struct acpi_nhlt_endpoint {
	u32 descriptor_length;
	u8 link_type;
	u8 instance_id;
	u16 vendor_id;
	u16 device_id;
	u16 revision_id;
	u32 subsystem_id;
	u8 device_type;
	u8 direction;
	u8 virtual_bus_id;
};



#define ACPI_NHLT_RESERVED_HD_AUDIO         0
#define ACPI_NHLT_RESERVED_DSP              1
#define ACPI_NHLT_PDM                       2
#define ACPI_NHLT_SSP                       3
#define ACPI_NHLT_RESERVED_SLIMBUS          4
#define ACPI_NHLT_RESERVED_SOUNDWIRE        5
#define ACPI_NHLT_TYPE_RESERVED             6	





#define ACPI_NHLT_PDM_DMIC                  0xAE20
#define ACPI_NHLT_BT_SIDEBAND               0xAE30
#define ACPI_NHLT_I2S_TDM_CODECS            0xAE23





#define ACPI_NHLT_LINK_BT_SIDEBAND          0
#define ACPI_NHLT_LINK_FM                   1
#define ACPI_NHLT_LINK_MODEM                2

#define ACPI_NHLT_LINK_SSP_ANALOG_CODEC     4



#define ACPI_NHLT_PDM_ON_CAVS_1P8           0
#define ACPI_NHLT_PDM_ON_CAVS_1P5           1



#define ACPI_NHLT_DIR_RENDER                0
#define ACPI_NHLT_DIR_CAPTURE               1
#define ACPI_NHLT_DIR_RENDER_LOOPBACK       2
#define ACPI_NHLT_DIR_RENDER_FEEDBACK       3
#define ACPI_NHLT_DIR_RESERVED              4	

struct acpi_nhlt_device_specific_config {
	u32 capabilities_size;
	u8 virtual_slot;
	u8 config_type;
};

struct acpi_nhlt_device_specific_config_a {
	u32 capabilities_size;
	u8 virtual_slot;
	u8 config_type;
	u8 array_type;
};



#define ACPI_NHLT_CONFIG_TYPE_GENERIC              0x00
#define ACPI_NHLT_CONFIG_TYPE_MIC_ARRAY            0x01
#define ACPI_NHLT_CONFIG_TYPE_RENDER_FEEDBACK      0x03
#define ACPI_NHLT_CONFIG_TYPE_RESERVED             0x04	

struct acpi_nhlt_device_specific_config_b {
	u32 capabilities_size;
};

struct acpi_nhlt_device_specific_config_c {
	u32 capabilities_size;
	u8 virtual_slot;
};

struct acpi_nhlt_render_device_specific_config {
	u32 capabilities_size;
	u8 virtual_slot;
};

struct acpi_nhlt_wave_extensible {
	u16 format_tag;
	u16 channel_count;
	u32 samples_per_sec;
	u32 avg_bytes_per_sec;
	u16 block_align;
	u16 bits_per_sample;
	u16 extra_format_size;
	u16 valid_bits_per_sample;
	u32 channel_mask;
	u8 sub_format_guid[16];
};



#define ACPI_NHLT_SPKR_FRONT_LEFT             0x1
#define ACPI_NHLT_SPKR_FRONT_RIGHT            0x2
#define ACPI_NHLT_SPKR_FRONT_CENTER           0x4
#define ACPI_NHLT_SPKR_LOW_FREQ               0x8
#define ACPI_NHLT_SPKR_BACK_LEFT              0x10
#define ACPI_NHLT_SPKR_BACK_RIGHT             0x20
#define ACPI_NHLT_SPKR_FRONT_LEFT_OF_CENTER   0x40
#define ACPI_NHLT_SPKR_FRONT_RIGHT_OF_CENTER  0x80
#define ACPI_NHLT_SPKR_BACK_CENTER            0x100
#define ACPI_NHLT_SPKR_SIDE_LEFT              0x200
#define ACPI_NHLT_SPKR_SIDE_RIGHT             0x400
#define ACPI_NHLT_SPKR_TOP_CENTER             0x800
#define ACPI_NHLT_SPKR_TOP_FRONT_LEFT         0x1000
#define ACPI_NHLT_SPKR_TOP_FRONT_CENTER       0x2000
#define ACPI_NHLT_SPKR_TOP_FRONT_RIGHT        0x4000
#define ACPI_NHLT_SPKR_TOP_BACK_LEFT          0x8000
#define ACPI_NHLT_SPKR_TOP_BACK_CENTER        0x10000
#define ACPI_NHLT_SPKR_TOP_BACK_RIGHT         0x20000

struct acpi_nhlt_format_config {
	struct acpi_nhlt_wave_extensible format;
	u32 capability_size;
	u8 capabilities[];
};

struct acpi_nhlt_formats_config {
	u8 formats_count;
};

struct acpi_nhlt_device_specific_hdr {
	u8 virtual_slot;
	u8 config_type;
};



#define ACPI_NHLT_GENERIC                   0
#define ACPI_NHLT_MIC                       1
#define ACPI_NHLT_RENDER                    3

struct acpi_nhlt_mic_device_specific_config {
	struct acpi_nhlt_device_specific_hdr device_config;
	u8 array_type_ext;
};



#define ACPI_NHLT_ARRAY_TYPE_RESERVED               0x09	
#define ACPI_NHLT_SMALL_LINEAR_2ELEMENT             0x0A
#define ACPI_NHLT_BIG_LINEAR_2ELEMENT               0x0B
#define ACPI_NHLT_FIRST_GEOMETRY_LINEAR_4ELEMENT    0x0C
#define ACPI_NHLT_PLANAR_LSHAPED_4ELEMENT           0x0D
#define ACPI_NHLT_SECOND_GEOMETRY_LINEAR_4ELEMENT   0x0E
#define ACPI_NHLT_VENDOR_DEFINED                    0x0F
#define ACPI_NHLT_ARRAY_TYPE_MASK                   0x0F
#define ACPI_NHLT_ARRAY_TYPE_EXT_MASK               0x10

#define ACPI_NHLT_NO_EXTENSION                      0x0
#define ACPI_NHLT_MIC_SNR_SENSITIVITY_EXT           (1<<4)

struct acpi_nhlt_vendor_mic_count {
	u8 microphone_count;
};

struct acpi_nhlt_vendor_mic_config {
	u8 type;
	u8 panel;
	u16 speaker_position_distance;	
	u16 horizontal_offset;	
	u16 vertical_offset;	
	u8 frequency_low_band;	
	u8 frequency_high_band;	
	u16 direction_angle;	
	u16 elevation_angle;	
	u16 work_vertical_angle_begin;	
	u16 work_vertical_angle_end;	
	u16 work_horizontal_angle_begin;	
	u16 work_horizontal_angle_end;	
};



#define ACPI_NHLT_MIC_OMNIDIRECTIONAL       0
#define ACPI_NHLT_MIC_SUBCARDIOID           1
#define ACPI_NHLT_MIC_CARDIOID              2
#define ACPI_NHLT_MIC_SUPER_CARDIOID        3
#define ACPI_NHLT_MIC_HYPER_CARDIOID        4
#define ACPI_NHLT_MIC_8_SHAPED              5
#define ACPI_NHLT_MIC_RESERVED6             6	
#define ACPI_NHLT_MIC_VENDOR_DEFINED        7
#define ACPI_NHLT_MIC_RESERVED              8	



#define ACPI_NHLT_MIC_POSITION_TOP          0
#define ACPI_NHLT_MIC_POSITION_BOTTOM       1
#define ACPI_NHLT_MIC_POSITION_LEFT         2
#define ACPI_NHLT_MIC_POSITION_RIGHT        3
#define ACPI_NHLT_MIC_POSITION_FRONT        4
#define ACPI_NHLT_MIC_POSITION_BACK         5
#define ACPI_NHLT_MIC_POSITION_RESERVED     6	

struct acpi_nhlt_vendor_mic_device_specific_config {
	struct acpi_nhlt_mic_device_specific_config mic_array_device_config;
	u8 number_of_microphones;
	struct acpi_nhlt_vendor_mic_config mic_config[];	
};



struct acpi_nhlt_mic_snr_sensitivity_extension {
	u32 SNR;
	u32 sensitivity;
};



struct acpi_nhlt_render_feedback_device_specific_config {
	u8 feedback_virtual_slot;	
	u16 feedback_channels;	
	u16 feedback_valid_bits_per_sample;
};



struct acpi_nhlt_device_info_count {
	u8 structure_count;
};

struct acpi_nhlt_device_info {
	u8 device_id[16];
	u8 device_instance_id;
	u8 device_port_id;
};



struct acpi_table_pcct {
	struct acpi_table_header header;	
	u32 flags;
	u64 reserved;
};



#define ACPI_PCCT_DOORBELL              1



enum acpi_pcct_type {
	ACPI_PCCT_TYPE_GENERIC_SUBSPACE = 0,
	ACPI_PCCT_TYPE_HW_REDUCED_SUBSPACE = 1,
	ACPI_PCCT_TYPE_HW_REDUCED_SUBSPACE_TYPE2 = 2,	
	ACPI_PCCT_TYPE_EXT_PCC_MASTER_SUBSPACE = 3,	
	ACPI_PCCT_TYPE_EXT_PCC_SLAVE_SUBSPACE = 4,	
	ACPI_PCCT_TYPE_HW_REG_COMM_SUBSPACE = 5,	
	ACPI_PCCT_TYPE_RESERVED = 6	
};





struct acpi_pcct_subspace {
	struct acpi_subtable_header header;
	u8 reserved[6];
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
};



struct acpi_pcct_hw_reduced {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved;
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
};



struct acpi_pcct_hw_reduced_type2 {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved;
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u16 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_write_mask;
};



struct acpi_pcct_ext_pcc_master {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved1;
	u64 base_address;
	u32 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u32 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_set_mask;
	u64 reserved2;
	struct acpi_generic_address cmd_complete_register;
	u64 cmd_complete_mask;
	struct acpi_generic_address cmd_update_register;
	u64 cmd_update_preserve_mask;
	u64 cmd_update_set_mask;
	struct acpi_generic_address error_status_register;
	u64 error_status_mask;
};



struct acpi_pcct_ext_pcc_slave {
	struct acpi_subtable_header header;
	u32 platform_interrupt;
	u8 flags;
	u8 reserved1;
	u64 base_address;
	u32 length;
	struct acpi_generic_address doorbell_register;
	u64 preserve_mask;
	u64 write_mask;
	u32 latency;
	u32 max_access_rate;
	u32 min_turnaround_time;
	struct acpi_generic_address platform_ack_register;
	u64 ack_preserve_mask;
	u64 ack_set_mask;
	u64 reserved2;
	struct acpi_generic_address cmd_complete_register;
	u64 cmd_complete_mask;
	struct acpi_generic_address cmd_update_register;
	u64 cmd_update_preserve_mask;
	u64 cmd_update_set_mask;
	struct acpi_generic_address error_status_register;
	u64 error_status_mask;
};



struct acpi_pcct_hw_reg {
	struct acpi_subtable_header header;
	u16 version;
	u64 base_address;
	u64 length;
	struct acpi_generic_address doorbell_register;
	u64 doorbell_preserve;
	u64 doorbell_write;
	struct acpi_generic_address cmd_complete_register;
	u64 cmd_complete_mask;
	struct acpi_generic_address error_status_register;
	u64 error_status_mask;
	u32 nominal_latency;
	u32 min_turnaround_time;
};



#define ACPI_PCCT_INTERRUPT_POLARITY    (1)
#define ACPI_PCCT_INTERRUPT_MODE        (1<<1)





struct acpi_pcct_shared_memory {
	u32 signature;
	u16 command;
	u16 status;
};



struct acpi_pcct_ext_pcc_shared_memory {
	u32 signature;
	u32 flags;
	u32 length;
	u32 command;
};



struct acpi_table_pdtt {
	struct acpi_table_header header;	
	u8 trigger_count;
	u8 reserved[3];
	u32 array_offset;
};


struct acpi_pdtt_channel {
	u8 subchannel_id;
	u8 flags;
};



#define ACPI_PDTT_RUNTIME_TRIGGER           (1)
#define ACPI_PDTT_WAIT_COMPLETION           (1<<1)
#define ACPI_PDTT_TRIGGER_ORDER             (1<<2)



struct acpi_table_phat {
	struct acpi_table_header header;	
};



struct acpi_phat_header {
	u16 type;
	u16 length;
	u8 revision;
};



#define ACPI_PHAT_TYPE_FW_VERSION_DATA  0
#define ACPI_PHAT_TYPE_FW_HEALTH_DATA   1
#define ACPI_PHAT_TYPE_RESERVED         2	





struct acpi_phat_version_data {
	struct acpi_phat_header header;
	u8 reserved[3];
	u32 element_count;
};

struct acpi_phat_version_element {
	u8 guid[16];
	u64 version_value;
	u32 producer_id;
};



struct acpi_phat_health_data {
	struct acpi_phat_header header;
	u8 reserved[2];
	u8 health;
	u8 device_guid[16];
	u32 device_specific_offset;	
};



#define ACPI_PHAT_ERRORS_FOUND          0
#define ACPI_PHAT_NO_ERRORS             1
#define ACPI_PHAT_UNKNOWN_ERRORS        2
#define ACPI_PHAT_ADVISORY              3



struct acpi_table_pmtt {
	struct acpi_table_header header;	
	u32 memory_device_count;
	
};



struct acpi_pmtt_header {
	u8 type;
	u8 reserved1;
	u16 length;
	u16 flags;
	u16 reserved2;
	u32 memory_device_count;	
	
};



#define ACPI_PMTT_TYPE_SOCKET           0
#define ACPI_PMTT_TYPE_CONTROLLER       1
#define ACPI_PMTT_TYPE_DIMM             2
#define ACPI_PMTT_TYPE_RESERVED         3	
#define ACPI_PMTT_TYPE_VENDOR           0xFF



#define ACPI_PMTT_TOP_LEVEL             0x0001
#define ACPI_PMTT_PHYSICAL              0x0002
#define ACPI_PMTT_MEMORY_TYPE           0x000C





struct acpi_pmtt_socket {
	struct acpi_pmtt_header header;
	u16 socket_id;
	u16 reserved;
};
	



struct acpi_pmtt_controller {
	struct acpi_pmtt_header header;
	u16 controller_id;
	u16 reserved;
};
	



struct acpi_pmtt_physical_component {
	struct acpi_pmtt_header header;
	u32 bios_handle;
};



struct acpi_pmtt_vendor_specific {
	struct acpi_pmtt_header header;
	u8 type_uuid[16];
	u8 specific[];
	
};



struct acpi_table_pptt {
	struct acpi_table_header header;	
};



enum acpi_pptt_type {
	ACPI_PPTT_TYPE_PROCESSOR = 0,
	ACPI_PPTT_TYPE_CACHE = 1,
	ACPI_PPTT_TYPE_ID = 2,
	ACPI_PPTT_TYPE_RESERVED = 3
};



struct acpi_pptt_processor {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 flags;
	u32 parent;
	u32 acpi_processor_id;
	u32 number_of_priv_resources;
};



#define ACPI_PPTT_PHYSICAL_PACKAGE          (1)
#define ACPI_PPTT_ACPI_PROCESSOR_ID_VALID   (1<<1)
#define ACPI_PPTT_ACPI_PROCESSOR_IS_THREAD  (1<<2)	
#define ACPI_PPTT_ACPI_LEAF_NODE            (1<<3)	
#define ACPI_PPTT_ACPI_IDENTICAL            (1<<4)	



struct acpi_pptt_cache {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 flags;
	u32 next_level_of_cache;
	u32 size;
	u32 number_of_sets;
	u8 associativity;
	u8 attributes;
	u16 line_size;
};



struct acpi_pptt_cache_v1 {
	u32 cache_id;
};



#define ACPI_PPTT_SIZE_PROPERTY_VALID       (1)	
#define ACPI_PPTT_NUMBER_OF_SETS_VALID      (1<<1)	
#define ACPI_PPTT_ASSOCIATIVITY_VALID       (1<<2)	
#define ACPI_PPTT_ALLOCATION_TYPE_VALID     (1<<3)	
#define ACPI_PPTT_CACHE_TYPE_VALID          (1<<4)	
#define ACPI_PPTT_WRITE_POLICY_VALID        (1<<5)	
#define ACPI_PPTT_LINE_SIZE_VALID           (1<<6)	
#define ACPI_PPTT_CACHE_ID_VALID            (1<<7)	



#define ACPI_PPTT_MASK_ALLOCATION_TYPE      (0x03)	
#define ACPI_PPTT_MASK_CACHE_TYPE           (0x0C)	
#define ACPI_PPTT_MASK_WRITE_POLICY         (0x10)	


#define ACPI_PPTT_CACHE_READ_ALLOCATE       (0x0)	
#define ACPI_PPTT_CACHE_WRITE_ALLOCATE      (0x01)	
#define ACPI_PPTT_CACHE_RW_ALLOCATE         (0x02)	
#define ACPI_PPTT_CACHE_RW_ALLOCATE_ALT     (0x03)	

#define ACPI_PPTT_CACHE_TYPE_DATA           (0x0)	
#define ACPI_PPTT_CACHE_TYPE_INSTR          (1<<2)	
#define ACPI_PPTT_CACHE_TYPE_UNIFIED        (2<<2)	
#define ACPI_PPTT_CACHE_TYPE_UNIFIED_ALT    (3<<2)	

#define ACPI_PPTT_CACHE_POLICY_WB           (0x0)	
#define ACPI_PPTT_CACHE_POLICY_WT           (1<<4)	



struct acpi_pptt_id {
	struct acpi_subtable_header header;
	u16 reserved;
	u32 vendor_id;
	u64 level1_id;
	u64 level2_id;
	u16 major_rev;
	u16 minor_rev;
	u16 spin_rev;
};



struct acpi_table_prmt {
	struct acpi_table_header header;	
};

struct acpi_table_prmt_header {
	u8 platform_guid[16];
	u32 module_info_offset;
	u32 module_info_count;
};

struct acpi_prmt_module_header {
	u16 revision;
	u16 length;
};

struct acpi_prmt_module_info {
	u16 revision;
	u16 length;
	u8 module_guid[16];
	u16 major_rev;
	u16 minor_rev;
	u16 handler_info_count;
	u32 handler_info_offset;
	u64 mmio_list_pointer;
};

struct acpi_prmt_handler_info {
	u16 revision;
	u16 length;
	u8 handler_guid[16];
	u64 handler_address;
	u64 static_data_buffer_address;
	u64 acpi_param_buffer_address;
};



struct acpi_table_rasf {
	struct acpi_table_header header;	
	u8 channel_id[12];
};



struct acpi_rasf_shared_memory {
	u32 signature;
	u16 command;
	u16 status;
	u16 version;
	u8 capabilities[16];
	u8 set_capabilities[16];
	u16 num_parameter_blocks;
	u32 set_capabilities_status;
};



struct acpi_rasf_parameter_block {
	u16 type;
	u16 version;
	u16 length;
};



struct acpi_rasf_patrol_scrub_parameter {
	struct acpi_rasf_parameter_block header;
	u16 patrol_scrub_command;
	u64 requested_address_range[2];
	u64 actual_address_range[2];
	u16 flags;
	u8 requested_speed;
};



#define ACPI_RASF_SCRUBBER_RUNNING      1
#define ACPI_RASF_SPEED                 (7<<1)
#define ACPI_RASF_SPEED_SLOW            (0<<1)
#define ACPI_RASF_SPEED_MEDIUM          (4<<1)
#define ACPI_RASF_SPEED_FAST            (7<<1)



enum acpi_rasf_commands {
	ACPI_RASF_EXECUTE_RASF_COMMAND = 1
};



enum acpi_rasf_capabiliities {
	ACPI_HW_PATROL_SCRUB_SUPPORTED = 0,
	ACPI_SW_PATROL_SCRUB_EXPOSED = 1
};



enum acpi_rasf_patrol_scrub_commands {
	ACPI_RASF_GET_PATROL_PARAMETERS = 1,
	ACPI_RASF_START_PATROL_SCRUBBER = 2,
	ACPI_RASF_STOP_PATROL_SCRUBBER = 3
};



#define ACPI_RASF_GENERATE_SCI          (1<<15)



enum acpi_rasf_status {
	ACPI_RASF_SUCCESS = 0,
	ACPI_RASF_NOT_VALID = 1,
	ACPI_RASF_NOT_SUPPORTED = 2,
	ACPI_RASF_BUSY = 3,
	ACPI_RASF_FAILED = 4,
	ACPI_RASF_ABORTED = 5,
	ACPI_RASF_INVALID_DATA = 6
};



#define ACPI_RASF_COMMAND_COMPLETE      (1)
#define ACPI_RASF_SCI_DOORBELL          (1<<1)
#define ACPI_RASF_ERROR                 (1<<2)
#define ACPI_RASF_STATUS                (0x1F<<3)



struct acpi_table_rgrt {
	struct acpi_table_header header;	
	u16 version;
	u8 image_type;
	u8 reserved;
	u8 image[];
};



enum acpi_rgrt_image_type {
	ACPI_RGRT_TYPE_RESERVED0 = 0,
	ACPI_RGRT_IMAGE_TYPE_PNG = 1,
	ACPI_RGRT_TYPE_RESERVED = 2	
};



struct acpi_table_rhct {
	struct acpi_table_header header;	
	u32 flags;		
	u64 time_base_freq;
	u32 node_count;
	u32 node_offset;
};



#define ACPI_RHCT_TIMER_CANNOT_WAKEUP_CPU       (1)

struct acpi_rhct_node_header {
	u16 type;
	u16 length;
	u16 revision;
};



enum acpi_rhct_node_type {
	ACPI_RHCT_NODE_TYPE_ISA_STRING = 0x0000,
	ACPI_RHCT_NODE_TYPE_CMO = 0x0001,
	ACPI_RHCT_NODE_TYPE_MMU = 0x0002,
	ACPI_RHCT_NODE_TYPE_RESERVED = 0x0003,
	ACPI_RHCT_NODE_TYPE_HART_INFO = 0xFFFF,
};




struct acpi_rhct_isa_string {
	u16 isa_length;
	char isa[];
};

struct acpi_rhct_cmo_node {
	u8 reserved;		
	u8 cbom_size;		
	u8 cbop_size;		
	u8 cboz_size;		
};

struct acpi_rhct_mmu_node {
	u8 reserved;		
	u8 mmu_type;		
};

enum acpi_rhct_mmu_type {
	ACPI_RHCT_MMU_TYPE_SV39 = 0,
	ACPI_RHCT_MMU_TYPE_SV48 = 1,
	ACPI_RHCT_MMU_TYPE_SV57 = 2
};


struct acpi_rhct_hart_info {
	u16 num_offsets;
	u32 uid;		
};



struct acpi_table_sbst {
	struct acpi_table_header header;	
	u32 warning_level;
	u32 low_level;
	u32 critical_level;
};



struct acpi_table_sdei {
	struct acpi_table_header header;	
};



struct acpi_table_sdev {
	struct acpi_table_header header;	
};

struct acpi_sdev_header {
	u8 type;
	u8 flags;
	u16 length;
};



enum acpi_sdev_type {
	ACPI_SDEV_TYPE_NAMESPACE_DEVICE = 0,
	ACPI_SDEV_TYPE_PCIE_ENDPOINT_DEVICE = 1,
	ACPI_SDEV_TYPE_RESERVED = 2	
};



#define ACPI_SDEV_HANDOFF_TO_UNSECURE_OS    (1)
#define ACPI_SDEV_SECURE_COMPONENTS_PRESENT (1<<1)





struct acpi_sdev_namespace {
	struct acpi_sdev_header header;
	u16 device_id_offset;
	u16 device_id_length;
	u16 vendor_data_offset;
	u16 vendor_data_length;
};

struct acpi_sdev_secure_component {
	u16 secure_component_offset;
	u16 secure_component_length;
};


struct acpi_sdev_component {
	struct acpi_sdev_header header;
};



enum acpi_sac_type {
	ACPI_SDEV_TYPE_ID_COMPONENT = 0,
	ACPI_SDEV_TYPE_MEM_COMPONENT = 1
};

struct acpi_sdev_id_component {
	struct acpi_sdev_header header;
	u16 hardware_id_offset;
	u16 hardware_id_length;
	u16 subsystem_id_offset;
	u16 subsystem_id_length;
	u16 hardware_revision;
	u8 hardware_rev_present;
	u8 class_code_present;
	u8 pci_base_class;
	u8 pci_sub_class;
	u8 pci_programming_xface;
};

struct acpi_sdev_mem_component {
	struct acpi_sdev_header header;
	u32 reserved;
	u64 memory_base_address;
	u64 memory_length;
};



struct acpi_sdev_pcie {
	struct acpi_sdev_header header;
	u16 segment;
	u16 start_bus;
	u16 path_offset;
	u16 path_length;
	u16 vendor_data_offset;
	u16 vendor_data_length;
};



struct acpi_sdev_pcie_path {
	u8 device;
	u8 function;
};



struct acpi_table_svkl {
	struct acpi_table_header header;	
	u32 count;
};

struct acpi_svkl_key {
	u16 type;
	u16 format;
	u32 size;
	u64 address;
};

enum acpi_svkl_type {
	ACPI_SVKL_TYPE_MAIN_STORAGE = 0,
	ACPI_SVKL_TYPE_RESERVED = 1	
};

enum acpi_svkl_format {
	ACPI_SVKL_FORMAT_RAW_BINARY = 0,
	ACPI_SVKL_FORMAT_RESERVED = 1	
};



struct acpi_table_tdel {
	struct acpi_table_header header;	
	u32 reserved;
	u64 log_area_minimum_length;
	u64 log_area_start_address;
};



#pragma pack()

#endif				
