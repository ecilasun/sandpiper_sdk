/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __BPF_MPROG_H
#define __BPF_MPROG_H

#include <linux/bpf.h>



#define bpf_mprog_foreach_tuple(entry, fp, cp, t)			\
	for (fp = &entry->fp_items[0], cp = &entry->parent->cp_items[0];\
	     ({								\
		t.prog = READ_ONCE(fp->prog);				\
		t.link = cp->link;					\
		t.prog;							\
	      });							\
	     fp++, cp++)

#define bpf_mprog_foreach_prog(entry, fp, p)				\
	for (fp = &entry->fp_items[0];					\
	     (p = READ_ONCE(fp->prog));					\
	     fp++)

#define BPF_MPROG_MAX 64

struct bpf_mprog_fp {
	struct bpf_prog *prog;
};

struct bpf_mprog_cp {
	struct bpf_link *link;
};

struct bpf_mprog_entry {
	struct bpf_mprog_fp fp_items[BPF_MPROG_MAX];
	struct bpf_mprog_bundle *parent;
};

struct bpf_mprog_bundle {
	struct bpf_mprog_entry a;
	struct bpf_mprog_entry b;
	struct bpf_mprog_cp cp_items[BPF_MPROG_MAX];
	struct bpf_prog *ref;
	atomic64_t revision;
	u32 count;
};

struct bpf_tuple {
	struct bpf_prog *prog;
	struct bpf_link *link;
};

static inline struct bpf_mprog_entry *
bpf_mprog_peer(const struct bpf_mprog_entry *entry)
{
	if (entry == &entry->parent->a)
		return &entry->parent->b;
	else
		return &entry->parent->a;
}

static inline void bpf_mprog_bundle_init(struct bpf_mprog_bundle *bundle)
{
	BUILD_BUG_ON(sizeof(bundle->a.fp_items[0]) > sizeof(u64));
	BUILD_BUG_ON(ARRAY_SIZE(bundle->a.fp_items) !=
		     ARRAY_SIZE(bundle->cp_items));

	memset(bundle, 0, sizeof(*bundle));
	atomic64_set(&bundle->revision, 1);
	bundle->a.parent = bundle;
	bundle->b.parent = bundle;
}

static inline void bpf_mprog_inc(struct bpf_mprog_entry *entry)
{
	entry->parent->count++;
}

static inline void bpf_mprog_dec(struct bpf_mprog_entry *entry)
{
	entry->parent->count--;
}

static inline int bpf_mprog_max(void)
{
	return ARRAY_SIZE(((struct bpf_mprog_entry *)NULL)->fp_items) - 1;
}

static inline int bpf_mprog_total(struct bpf_mprog_entry *entry)
{
	int total = entry->parent->count;

	WARN_ON_ONCE(total > bpf_mprog_max());
	return total;
}

static inline bool bpf_mprog_exists(struct bpf_mprog_entry *entry,
				    struct bpf_prog *prog)
{
	const struct bpf_mprog_fp *fp;
	const struct bpf_prog *tmp;

	bpf_mprog_foreach_prog(entry, fp, tmp) {
		if (tmp == prog)
			return true;
	}
	return false;
}

static inline void bpf_mprog_mark_for_release(struct bpf_mprog_entry *entry,
					      struct bpf_tuple *tuple)
{
	WARN_ON_ONCE(entry->parent->ref);
	if (!tuple->link)
		entry->parent->ref = tuple->prog;
}

static inline void bpf_mprog_complete_release(struct bpf_mprog_entry *entry)
{
	
	if (entry->parent->ref) {
		bpf_prog_put(entry->parent->ref);
		entry->parent->ref = NULL;
	}
}

static inline void bpf_mprog_revision_new(struct bpf_mprog_entry *entry)
{
	atomic64_inc(&entry->parent->revision);
}

static inline void bpf_mprog_commit(struct bpf_mprog_entry *entry)
{
	bpf_mprog_complete_release(entry);
	bpf_mprog_revision_new(entry);
}

static inline u64 bpf_mprog_revision(struct bpf_mprog_entry *entry)
{
	return atomic64_read(&entry->parent->revision);
}

static inline void bpf_mprog_entry_copy(struct bpf_mprog_entry *dst,
					struct bpf_mprog_entry *src)
{
	memcpy(dst->fp_items, src->fp_items, sizeof(src->fp_items));
}

static inline void bpf_mprog_entry_clear(struct bpf_mprog_entry *dst)
{
	memset(dst->fp_items, 0, sizeof(dst->fp_items));
}

static inline void bpf_mprog_clear_all(struct bpf_mprog_entry *entry,
				       struct bpf_mprog_entry **entry_new)
{
	struct bpf_mprog_entry *peer;

	peer = bpf_mprog_peer(entry);
	bpf_mprog_entry_clear(peer);
	peer->parent->count = 0;
	*entry_new = peer;
}

static inline void bpf_mprog_entry_grow(struct bpf_mprog_entry *entry, int idx)
{
	int total = bpf_mprog_total(entry);

	memmove(entry->fp_items + idx + 1,
		entry->fp_items + idx,
		(total - idx) * sizeof(struct bpf_mprog_fp));

	memmove(entry->parent->cp_items + idx + 1,
		entry->parent->cp_items + idx,
		(total - idx) * sizeof(struct bpf_mprog_cp));
}

static inline void bpf_mprog_entry_shrink(struct bpf_mprog_entry *entry, int idx)
{
	
	int total = ARRAY_SIZE(entry->fp_items);

	memmove(entry->fp_items + idx,
		entry->fp_items + idx + 1,
		(total - idx - 1) * sizeof(struct bpf_mprog_fp));

	memmove(entry->parent->cp_items + idx,
		entry->parent->cp_items + idx + 1,
		(total - idx - 1) * sizeof(struct bpf_mprog_cp));
}

static inline void bpf_mprog_read(struct bpf_mprog_entry *entry, u32 idx,
				  struct bpf_mprog_fp **fp,
				  struct bpf_mprog_cp **cp)
{
	*fp = &entry->fp_items[idx];
	*cp = &entry->parent->cp_items[idx];
}

static inline void bpf_mprog_write(struct bpf_mprog_fp *fp,
				   struct bpf_mprog_cp *cp,
				   struct bpf_tuple *tuple)
{
	WRITE_ONCE(fp->prog, tuple->prog);
	cp->link = tuple->link;
}

int bpf_mprog_attach(struct bpf_mprog_entry *entry,
		     struct bpf_mprog_entry **entry_new,
		     struct bpf_prog *prog_new, struct bpf_link *link,
		     struct bpf_prog *prog_old,
		     u32 flags, u32 id_or_fd, u64 revision);

int bpf_mprog_detach(struct bpf_mprog_entry *entry,
		     struct bpf_mprog_entry **entry_new,
		     struct bpf_prog *prog, struct bpf_link *link,
		     u32 flags, u32 id_or_fd, u64 revision);

int bpf_mprog_query(const union bpf_attr *attr, union bpf_attr __user *uattr,
		    struct bpf_mprog_entry *entry);

static inline bool bpf_mprog_supported(enum bpf_prog_type type)
{
	switch (type) {
	case BPF_PROG_TYPE_SCHED_CLS:
		return true;
	default:
		return false;
	}
}
#endif 
