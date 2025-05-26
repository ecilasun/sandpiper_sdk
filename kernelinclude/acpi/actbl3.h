/* SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0 */


#ifndef __ACTBL3_H__
#define __ACTBL3_H__




#define ACPI_SIG_SLIC           "SLIC"	
#define ACPI_SIG_SLIT           "SLIT"	
#define ACPI_SIG_SPCR           "SPCR"	
#define ACPI_SIG_SPMI           "SPMI"	
#define ACPI_SIG_SRAT           "SRAT"	
#define ACPI_SIG_STAO           "STAO"	
#define ACPI_SIG_TCPA           "TCPA"	
#define ACPI_SIG_TPM2           "TPM2"	
#define ACPI_SIG_UEFI           "UEFI"	
#define ACPI_SIG_VIOT           "VIOT"	
#define ACPI_SIG_WAET           "WAET"	
#define ACPI_SIG_WDAT           "WDAT"	
#define ACPI_SIG_WDDT           "WDDT"	
#define ACPI_SIG_WDRT           "WDRT"	
#define ACPI_SIG_WPBT           "WPBT"	
#define ACPI_SIG_WSMT           "WSMT"	
#define ACPI_SIG_XENV           "XENV"	
#define ACPI_SIG_XXXX           "XXXX"	


#pragma pack(1)







struct acpi_table_slic {
	struct acpi_table_header header;	
};



struct acpi_table_slit {
	struct acpi_table_header header;	
	u64 locality_count;
	u8 entry[];				
};



struct acpi_table_spcr {
	struct acpi_table_header header;	
	u8 interface_type;	
	u8 reserved[3];
	struct acpi_generic_address serial_port;
	u8 interrupt_type;
	u8 pc_interrupt;
	u32 interrupt;
	u8 baud_rate;
	u8 parity;
	u8 stop_bits;
	u8 flow_control;
	u8 terminal_type;
	u8 reserved1;
	u16 pci_device_id;
	u16 pci_vendor_id;
	u8 pci_bus;
	u8 pci_device;
	u8 pci_function;
	u32 pci_flags;
	u8 pci_segment;
	u32 reserved2;
};



#define ACPI_SPCR_DO_NOT_DISABLE    (1)





struct acpi_table_spmi {
	struct acpi_table_header header;	
	u8 interface_type;
	u8 reserved;		
	u16 spec_revision;	
	u8 interrupt_type;
	u8 gpe_number;		
	u8 reserved1;
	u8 pci_device_flag;
	u32 interrupt;
	struct acpi_generic_address ipmi_register;
	u8 pci_segment;
	u8 pci_bus;
	u8 pci_device;
	u8 pci_function;
	u8 reserved2;
};



enum acpi_spmi_interface_types {
	ACPI_SPMI_NOT_USED = 0,
	ACPI_SPMI_KEYBOARD = 1,
	ACPI_SPMI_SMI = 2,
	ACPI_SPMI_BLOCK_TRANSFER = 3,
	ACPI_SPMI_SMBUS = 4,
	ACPI_SPMI_RESERVED = 5	
};



struct acpi_table_srat {
	struct acpi_table_header header;	
	u32 table_revision;	
	u64 reserved;		
};



enum acpi_srat_type {
	ACPI_SRAT_TYPE_CPU_AFFINITY = 0,
	ACPI_SRAT_TYPE_MEMORY_AFFINITY = 1,
	ACPI_SRAT_TYPE_X2APIC_CPU_AFFINITY = 2,
	ACPI_SRAT_TYPE_GICC_AFFINITY = 3,
	ACPI_SRAT_TYPE_GIC_ITS_AFFINITY = 4,	
	ACPI_SRAT_TYPE_GENERIC_AFFINITY = 5,	
	ACPI_SRAT_TYPE_GENERIC_PORT_AFFINITY = 6,	
	ACPI_SRAT_TYPE_RESERVED = 7	
};





struct acpi_srat_cpu_affinity {
	struct acpi_subtable_header header;
	u8 proximity_domain_lo;
	u8 apic_id;
	u32 flags;
	u8 local_sapic_eid;
	u8 proximity_domain_hi[3];
	u32 clock_domain;
};



#define ACPI_SRAT_CPU_USE_AFFINITY  (1)	



struct acpi_srat_mem_affinity {
	struct acpi_subtable_header header;
	u32 proximity_domain;
	u16 reserved;		
	u64 base_address;
	u64 length;
	u32 reserved1;
	u32 flags;
	u64 reserved2;		
};



#define ACPI_SRAT_MEM_ENABLED       (1)	
#define ACPI_SRAT_MEM_HOT_PLUGGABLE (1<<1)	
#define ACPI_SRAT_MEM_NON_VOLATILE  (1<<2)	



struct acpi_srat_x2apic_cpu_affinity {
	struct acpi_subtable_header header;
	u16 reserved;		
	u32 proximity_domain;
	u32 apic_id;
	u32 flags;
	u32 clock_domain;
	u32 reserved2;
};



#define ACPI_SRAT_CPU_ENABLED       (1)	



struct acpi_srat_gicc_affinity {
	struct acpi_subtable_header header;
	u32 proximity_domain;
	u32 acpi_processor_uid;
	u32 flags;
	u32 clock_domain;
};



#define ACPI_SRAT_GICC_ENABLED     (1)	



struct acpi_srat_gic_its_affinity {
	struct acpi_subtable_header header;
	u32 proximity_domain;
	u16 reserved;
	u32 its_id;
};



#define ACPI_SRAT_DEVICE_HANDLE_SIZE	16

struct acpi_srat_generic_affinity {
	struct acpi_subtable_header header;
	u8 reserved;
	u8 device_handle_type;
	u32 proximity_domain;
	u8 device_handle[ACPI_SRAT_DEVICE_HANDLE_SIZE];
	u32 flags;
	u32 reserved1;
};



#define ACPI_SRAT_GENERIC_AFFINITY_ENABLED     (1)	
#define ACPI_SRAT_ARCHITECTURAL_TRANSACTIONS   (1<<1)	



struct acpi_table_stao {
	struct acpi_table_header header;	
	u8 ignore_uart;
};



struct acpi_table_tcpa_hdr {
	struct acpi_table_header header;	
	u16 platform_class;
};


#define ACPI_TCPA_CLIENT_TABLE          0
#define ACPI_TCPA_SERVER_TABLE          1

struct acpi_table_tcpa_client {
	u32 minimum_log_length;	
	u64 log_address;	
};

struct acpi_table_tcpa_server {
	u16 reserved;
	u64 minimum_log_length;	
	u64 log_address;	
	u16 spec_revision;
	u8 device_flags;
	u8 interrupt_flags;
	u8 gpe_number;
	u8 reserved2[3];
	u32 global_interrupt;
	struct acpi_generic_address address;
	u32 reserved3;
	struct acpi_generic_address config_address;
	u8 group;
	u8 bus;			
	u8 device;
	u8 function;
};



#define ACPI_TCPA_PCI_DEVICE            (1)
#define ACPI_TCPA_BUS_PNP               (1<<1)
#define ACPI_TCPA_ADDRESS_VALID         (1<<2)



#define ACPI_TCPA_INTERRUPT_MODE        (1)
#define ACPI_TCPA_INTERRUPT_POLARITY    (1<<1)
#define ACPI_TCPA_SCI_VIA_GPE           (1<<2)
#define ACPI_TCPA_GLOBAL_INTERRUPT      (1<<3)





struct acpi_table_tpm23 {
	struct acpi_table_header header;	
	u32 reserved;
	u64 control_address;
	u32 start_method;
};



#define ACPI_TPM23_ACPI_START_METHOD                 2


struct acpi_tmp23_trailer {
	u32 reserved;
};



struct acpi_table_tpm2 {
	struct acpi_table_header header;	
	u16 platform_class;
	u16 reserved;
	u64 control_address;
	u32 start_method;

	
};


struct acpi_tpm2_phy {
	u8  start_method_specific[12];
	u32 log_area_minimum_length;
	u64 log_area_start_address;
};



#define ACPI_TPM2_NOT_ALLOWED                       0
#define ACPI_TPM2_RESERVED1                         1
#define ACPI_TPM2_START_METHOD                      2
#define ACPI_TPM2_RESERVED3                         3
#define ACPI_TPM2_RESERVED4                         4
#define ACPI_TPM2_RESERVED5                         5
#define ACPI_TPM2_MEMORY_MAPPED                     6
#define ACPI_TPM2_COMMAND_BUFFER                    7
#define ACPI_TPM2_COMMAND_BUFFER_WITH_START_METHOD  8
#define ACPI_TPM2_RESERVED9                         9
#define ACPI_TPM2_RESERVED10                        10
#define ACPI_TPM2_COMMAND_BUFFER_WITH_ARM_SMC       11	
#define ACPI_TPM2_RESERVED                          12
#define ACPI_TPM2_COMMAND_BUFFER_WITH_PLUTON        13



struct acpi_tpm2_trailer {
	u8 method_parameters[12];
	u32 minimum_log_length;	
	u64 log_address;	
};





struct acpi_tpm2_arm_smc {
	u32 global_interrupt;
	u8 interrupt_flags;
	u8 operation_flags;
	u16 reserved;
	u32 function_id;
};



#define ACPI_TPM2_INTERRUPT_SUPPORT     (1)



#define ACPI_TPM2_IDLE_SUPPORT          (1)



struct acpi_table_uefi {
	struct acpi_table_header header;	
	u8 identifier[16];	
	u16 data_offset;	
};



struct acpi_table_viot {
	struct acpi_table_header header;	
	u16 node_count;
	u16 node_offset;
	u8 reserved[8];
};



struct acpi_viot_header {
	u8 type;
	u8 reserved;
	u16 length;
};



enum acpi_viot_node_type {
	ACPI_VIOT_NODE_PCI_RANGE = 0x01,
	ACPI_VIOT_NODE_MMIO = 0x02,
	ACPI_VIOT_NODE_VIRTIO_IOMMU_PCI = 0x03,
	ACPI_VIOT_NODE_VIRTIO_IOMMU_MMIO = 0x04,
	ACPI_VIOT_RESERVED = 0x05
};



struct acpi_viot_pci_range {
	struct acpi_viot_header header;
	u32 endpoint_start;
	u16 segment_start;
	u16 segment_end;
	u16 bdf_start;
	u16 bdf_end;
	u16 output_node;
	u8 reserved[6];
};

struct acpi_viot_mmio {
	struct acpi_viot_header header;
	u32 endpoint;
	u64 base_address;
	u16 output_node;
	u8 reserved[6];
};

struct acpi_viot_virtio_iommu_pci {
	struct acpi_viot_header header;
	u16 segment;
	u16 bdf;
	u8 reserved[8];
};

struct acpi_viot_virtio_iommu_mmio {
	struct acpi_viot_header header;
	u8 reserved[4];
	u64 base_address;
};



struct acpi_table_waet {
	struct acpi_table_header header;	
	u32 flags;
};



#define ACPI_WAET_RTC_NO_ACK        (1)	
#define ACPI_WAET_TIMER_ONE_READ    (1<<1)	



struct acpi_table_wdat {
	struct acpi_table_header header;	
	u32 header_length;	
	u16 pci_segment;	
	u8 pci_bus;		
	u8 pci_device;		
	u8 pci_function;	
	u8 reserved[3];
	u32 timer_period;	
	u32 max_count;		
	u32 min_count;		
	u8 flags;
	u8 reserved2[3];
	u32 entries;		
};



#define ACPI_WDAT_ENABLED           (1)
#define ACPI_WDAT_STOPPED           0x80



struct acpi_wdat_entry {
	u8 action;
	u8 instruction;
	u16 reserved;
	struct acpi_generic_address register_region;
	u32 value;		
	u32 mask;		
};



enum acpi_wdat_actions {
	ACPI_WDAT_RESET = 1,
	ACPI_WDAT_GET_CURRENT_COUNTDOWN = 4,
	ACPI_WDAT_GET_COUNTDOWN = 5,
	ACPI_WDAT_SET_COUNTDOWN = 6,
	ACPI_WDAT_GET_RUNNING_STATE = 8,
	ACPI_WDAT_SET_RUNNING_STATE = 9,
	ACPI_WDAT_GET_STOPPED_STATE = 10,
	ACPI_WDAT_SET_STOPPED_STATE = 11,
	ACPI_WDAT_GET_REBOOT = 16,
	ACPI_WDAT_SET_REBOOT = 17,
	ACPI_WDAT_GET_SHUTDOWN = 18,
	ACPI_WDAT_SET_SHUTDOWN = 19,
	ACPI_WDAT_GET_STATUS = 32,
	ACPI_WDAT_SET_STATUS = 33,
	ACPI_WDAT_ACTION_RESERVED = 34	
};



enum acpi_wdat_instructions {
	ACPI_WDAT_READ_VALUE = 0,
	ACPI_WDAT_READ_COUNTDOWN = 1,
	ACPI_WDAT_WRITE_VALUE = 2,
	ACPI_WDAT_WRITE_COUNTDOWN = 3,
	ACPI_WDAT_INSTRUCTION_RESERVED = 4,	
	ACPI_WDAT_PRESERVE_REGISTER = 0x80	
};



struct acpi_table_wddt {
	struct acpi_table_header header;	
	u16 spec_version;
	u16 table_version;
	u16 pci_vendor_id;
	struct acpi_generic_address address;
	u16 max_count;		
	u16 min_count;		
	u16 period;
	u16 status;
	u16 capability;
};



#define ACPI_WDDT_AVAILABLE     (1)
#define ACPI_WDDT_ACTIVE        (1<<1)
#define ACPI_WDDT_TCO_OS_OWNED  (1<<2)
#define ACPI_WDDT_USER_RESET    (1<<11)
#define ACPI_WDDT_WDT_RESET     (1<<12)
#define ACPI_WDDT_POWER_FAIL    (1<<13)
#define ACPI_WDDT_UNKNOWN_RESET (1<<14)



#define ACPI_WDDT_AUTO_RESET    (1)
#define ACPI_WDDT_ALERT_SUPPORT (1<<1)



struct acpi_table_wdrt {
	struct acpi_table_header header;	
	struct acpi_generic_address control_register;
	struct acpi_generic_address count_register;
	u16 pci_device_id;
	u16 pci_vendor_id;
	u8 pci_bus;		
	u8 pci_device;		
	u8 pci_function;	
	u8 pci_segment;		
	u16 max_count;		
	u8 units;
};



struct acpi_table_wpbt {
	struct acpi_table_header header;	
	u32 handoff_size;
	u64 handoff_address;
	u8 layout;
	u8 type;
	u16 arguments_length;
};

struct acpi_wpbt_unicode {
	u16 *unicode_string;
};



struct acpi_table_wsmt {
	struct acpi_table_header header;	
	u32 protection_flags;
};



#define ACPI_WSMT_FIXED_COMM_BUFFERS                (1)
#define ACPI_WSMT_COMM_BUFFER_NESTED_PTR_PROTECTION (2)
#define ACPI_WSMT_SYSTEM_RESOURCE_PROTECTION        (4)



struct acpi_table_xenv {
	struct acpi_table_header header;	
	u64 grant_table_address;
	u64 grant_table_size;
	u32 event_interrupt;
	u8 event_flags;
};



#pragma pack()

#endif				
