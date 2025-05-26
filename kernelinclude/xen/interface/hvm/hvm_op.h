/* SPDX-License-Identifier: MIT */

#ifndef __XEN_PUBLIC_HVM_HVM_OP_H__
#define __XEN_PUBLIC_HVM_HVM_OP_H__

#include <xen/interface/xen.h>


#define HVMOP_set_param           0
#define HVMOP_get_param           1
struct xen_hvm_param {
    domid_t  domid;    
    uint32_t index;    
    uint64_t value;    
};
DEFINE_GUEST_HANDLE_STRUCT(xen_hvm_param);


#define HVMOP_pagetable_dying       9
struct xen_hvm_pagetable_dying {
    
    domid_t  domid;
    
    aligned_u64 gpa;
};
typedef struct xen_hvm_pagetable_dying xen_hvm_pagetable_dying_t;
DEFINE_GUEST_HANDLE_STRUCT(xen_hvm_pagetable_dying_t);
 
enum hvmmem_type_t {
    HVMMEM_ram_rw,             
    HVMMEM_ram_ro,             
    HVMMEM_mmio_dm,            
};

#define HVMOP_get_mem_type    15

struct xen_hvm_get_mem_type {
    
    domid_t domid;
    
    uint16_t mem_type;
    uint16_t pad[2]; 
    
    uint64_t pfn;
};
DEFINE_GUEST_HANDLE_STRUCT(xen_hvm_get_mem_type);

#if defined(__i386__) || defined(__x86_64__)


#define HVMOP_set_evtchn_upcall_vector 23
struct xen_hvm_evtchn_upcall_vector {
    uint32_t vcpu;
    uint8_t vector;
};
typedef struct xen_hvm_evtchn_upcall_vector xen_hvm_evtchn_upcall_vector_t;
DEFINE_GUEST_HANDLE_STRUCT(xen_hvm_evtchn_upcall_vector_t);

#endif 

#endif 
