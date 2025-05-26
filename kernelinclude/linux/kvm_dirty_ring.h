#ifndef KVM_DIRTY_RING_H
#define KVM_DIRTY_RING_H

#include <linux/kvm.h>


struct kvm_dirty_ring {
	u32 dirty_index;
	u32 reset_index;
	u32 size;
	u32 soft_limit;
	struct kvm_dirty_gfn *dirty_gfns;
	int index;
};

#ifndef CONFIG_HAVE_KVM_DIRTY_RING

static inline u32 kvm_dirty_ring_get_rsvd_entries(void)
{
	return 0;
}

static inline bool kvm_use_dirty_bitmap(struct kvm *kvm)
{
	return true;
}

static inline int kvm_dirty_ring_alloc(struct kvm_dirty_ring *ring,
				       int index, u32 size)
{
	return 0;
}

static inline int kvm_dirty_ring_reset(struct kvm *kvm,
				       struct kvm_dirty_ring *ring)
{
	return 0;
}

static inline void kvm_dirty_ring_push(struct kvm_vcpu *vcpu,
				       u32 slot, u64 offset)
{
}

static inline struct page *kvm_dirty_ring_get_page(struct kvm_dirty_ring *ring,
						   u32 offset)
{
	return NULL;
}

static inline void kvm_dirty_ring_free(struct kvm_dirty_ring *ring)
{
}

#else 

int kvm_cpu_dirty_log_size(void);
bool kvm_use_dirty_bitmap(struct kvm *kvm);
bool kvm_arch_allow_write_without_running_vcpu(struct kvm *kvm);
u32 kvm_dirty_ring_get_rsvd_entries(void);
int kvm_dirty_ring_alloc(struct kvm_dirty_ring *ring, int index, u32 size);


int kvm_dirty_ring_reset(struct kvm *kvm, struct kvm_dirty_ring *ring);


void kvm_dirty_ring_push(struct kvm_vcpu *vcpu, u32 slot, u64 offset);

bool kvm_dirty_ring_check_request(struct kvm_vcpu *vcpu);


struct page *kvm_dirty_ring_get_page(struct kvm_dirty_ring *ring, u32 offset);

void kvm_dirty_ring_free(struct kvm_dirty_ring *ring);

#endif 

#endif	
