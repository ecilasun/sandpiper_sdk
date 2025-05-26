/* SPDX-License-Identifier: MIT */


#ifndef __XEN_PUBLIC_ARCH_X86_HVM_START_INFO_H__
#define __XEN_PUBLIC_ARCH_X86_HVM_START_INFO_H__


#define XEN_HVM_START_MAGIC_VALUE 0x336ec578


#define XEN_HVM_MEMMAP_TYPE_RAM       1
#define XEN_HVM_MEMMAP_TYPE_RESERVED  2
#define XEN_HVM_MEMMAP_TYPE_ACPI      3
#define XEN_HVM_MEMMAP_TYPE_NVS       4
#define XEN_HVM_MEMMAP_TYPE_UNUSABLE  5
#define XEN_HVM_MEMMAP_TYPE_DISABLED  6
#define XEN_HVM_MEMMAP_TYPE_PMEM      7


struct hvm_start_info {
    uint32_t magic;             
                                
    uint32_t version;           
    uint32_t flags;             
    uint32_t nr_modules;        
    uint64_t modlist_paddr;     
                                
    uint64_t cmdline_paddr;     
    uint64_t rsdp_paddr;        
                                
    
    uint64_t memmap_paddr;      
                                
    uint32_t memmap_entries;    
                                
                                
    uint32_t reserved;          
};

struct hvm_modlist_entry {
    uint64_t paddr;             
    uint64_t size;              
    uint64_t cmdline_paddr;     
    uint64_t reserved;
};

struct hvm_memmap_table_entry {
    uint64_t addr;              
    uint64_t size;              
    uint32_t type;              
    uint32_t reserved;          
};

#endif 
