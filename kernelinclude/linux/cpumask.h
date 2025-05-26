/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_CPUMASK_H
#define __LINUX_CPUMASK_H


#include <linux/kernel.h>
#include <linux/threads.h>
#include <linux/bitmap.h>
#include <linux/atomic.h>
#include <linux/bug.h>
#include <linux/gfp_types.h>
#include <linux/numa.h>


typedef struct cpumask { DECLARE_BITMAP(bits, NR_CPUS); } cpumask_t;


#define cpumask_bits(maskp) ((maskp)->bits)


#define cpumask_pr_args(maskp)		nr_cpu_ids, cpumask_bits(maskp)

#if (NR_CPUS == 1) || defined(CONFIG_FORCE_NR_CPUS)
#define nr_cpu_ids ((unsigned int)NR_CPUS)
#else
extern unsigned int nr_cpu_ids;
#endif

static inline void set_nr_cpu_ids(unsigned int nr)
{
#if (NR_CPUS == 1) || defined(CONFIG_FORCE_NR_CPUS)
	WARN_ON(nr != nr_cpu_ids);
#else
	nr_cpu_ids = nr;
#endif
}


#if NR_CPUS <= BITS_PER_LONG
  #define small_cpumask_bits ((unsigned int)NR_CPUS)
  #define large_cpumask_bits ((unsigned int)NR_CPUS)
#elif NR_CPUS <= 4*BITS_PER_LONG
  #define small_cpumask_bits nr_cpu_ids
  #define large_cpumask_bits ((unsigned int)NR_CPUS)
#else
  #define small_cpumask_bits nr_cpu_ids
  #define large_cpumask_bits nr_cpu_ids
#endif
#define nr_cpumask_bits nr_cpu_ids



extern struct cpumask __cpu_possible_mask;
extern struct cpumask __cpu_online_mask;
extern struct cpumask __cpu_present_mask;
extern struct cpumask __cpu_active_mask;
extern struct cpumask __cpu_dying_mask;
#define cpu_possible_mask ((const struct cpumask *)&__cpu_possible_mask)
#define cpu_online_mask   ((const struct cpumask *)&__cpu_online_mask)
#define cpu_present_mask  ((const struct cpumask *)&__cpu_present_mask)
#define cpu_active_mask   ((const struct cpumask *)&__cpu_active_mask)
#define cpu_dying_mask    ((const struct cpumask *)&__cpu_dying_mask)

extern atomic_t __num_online_cpus;

extern cpumask_t cpus_booted_once_mask;

static __always_inline void cpu_max_bits_warn(unsigned int cpu, unsigned int bits)
{
#ifdef CONFIG_DEBUG_PER_CPU_MAPS
	WARN_ON_ONCE(cpu >= bits);
#endif 
}


static __always_inline unsigned int cpumask_check(unsigned int cpu)
{
	cpu_max_bits_warn(cpu, small_cpumask_bits);
	return cpu;
}


static inline unsigned int cpumask_first(const struct cpumask *srcp)
{
	return find_first_bit(cpumask_bits(srcp), small_cpumask_bits);
}


static inline unsigned int cpumask_first_zero(const struct cpumask *srcp)
{
	return find_first_zero_bit(cpumask_bits(srcp), small_cpumask_bits);
}


static inline
unsigned int cpumask_first_and(const struct cpumask *srcp1, const struct cpumask *srcp2)
{
	return find_first_and_bit(cpumask_bits(srcp1), cpumask_bits(srcp2), small_cpumask_bits);
}


static inline unsigned int cpumask_last(const struct cpumask *srcp)
{
	return find_last_bit(cpumask_bits(srcp), small_cpumask_bits);
}


static inline
unsigned int cpumask_next(int n, const struct cpumask *srcp)
{
	
	if (n != -1)
		cpumask_check(n);
	return find_next_bit(cpumask_bits(srcp), small_cpumask_bits, n + 1);
}


static inline unsigned int cpumask_next_zero(int n, const struct cpumask *srcp)
{
	
	if (n != -1)
		cpumask_check(n);
	return find_next_zero_bit(cpumask_bits(srcp), small_cpumask_bits, n+1);
}

#if NR_CPUS == 1

static inline unsigned int cpumask_local_spread(unsigned int i, int node)
{
	return 0;
}

static inline unsigned int cpumask_any_and_distribute(const struct cpumask *src1p,
						      const struct cpumask *src2p)
{
	return cpumask_first_and(src1p, src2p);
}

static inline unsigned int cpumask_any_distribute(const struct cpumask *srcp)
{
	return cpumask_first(srcp);
}
#else
unsigned int cpumask_local_spread(unsigned int i, int node);
unsigned int cpumask_any_and_distribute(const struct cpumask *src1p,
			       const struct cpumask *src2p);
unsigned int cpumask_any_distribute(const struct cpumask *srcp);
#endif 


static inline
unsigned int cpumask_next_and(int n, const struct cpumask *src1p,
		     const struct cpumask *src2p)
{
	
	if (n != -1)
		cpumask_check(n);
	return find_next_and_bit(cpumask_bits(src1p), cpumask_bits(src2p),
		small_cpumask_bits, n + 1);
}


#define for_each_cpu(cpu, mask)				\
	for_each_set_bit(cpu, cpumask_bits(mask), small_cpumask_bits)

#if NR_CPUS == 1
static inline
unsigned int cpumask_next_wrap(int n, const struct cpumask *mask, int start, bool wrap)
{
	cpumask_check(start);
	if (n != -1)
		cpumask_check(n);

	
	if (wrap && n >= 0)
		return nr_cpumask_bits;

	return cpumask_first(mask);
}
#else
unsigned int __pure cpumask_next_wrap(int n, const struct cpumask *mask, int start, bool wrap);
#endif


#define for_each_cpu_wrap(cpu, mask, start)				\
	for_each_set_bit_wrap(cpu, cpumask_bits(mask), small_cpumask_bits, start)


#define for_each_cpu_and(cpu, mask1, mask2)				\
	for_each_and_bit(cpu, cpumask_bits(mask1), cpumask_bits(mask2), small_cpumask_bits)


#define for_each_cpu_andnot(cpu, mask1, mask2)				\
	for_each_andnot_bit(cpu, cpumask_bits(mask1), cpumask_bits(mask2), small_cpumask_bits)


#define for_each_cpu_or(cpu, mask1, mask2)				\
	for_each_or_bit(cpu, cpumask_bits(mask1), cpumask_bits(mask2), small_cpumask_bits)


static inline
unsigned int cpumask_any_but(const struct cpumask *mask, unsigned int cpu)
{
	unsigned int i;

	cpumask_check(cpu);
	for_each_cpu(i, mask)
		if (i != cpu)
			break;
	return i;
}


static inline unsigned int cpumask_nth(unsigned int cpu, const struct cpumask *srcp)
{
	return find_nth_bit(cpumask_bits(srcp), small_cpumask_bits, cpumask_check(cpu));
}


static inline
unsigned int cpumask_nth_and(unsigned int cpu, const struct cpumask *srcp1,
							const struct cpumask *srcp2)
{
	return find_nth_and_bit(cpumask_bits(srcp1), cpumask_bits(srcp2),
				small_cpumask_bits, cpumask_check(cpu));
}


static inline
unsigned int cpumask_nth_andnot(unsigned int cpu, const struct cpumask *srcp1,
							const struct cpumask *srcp2)
{
	return find_nth_andnot_bit(cpumask_bits(srcp1), cpumask_bits(srcp2),
				small_cpumask_bits, cpumask_check(cpu));
}


static __always_inline
unsigned int cpumask_nth_and_andnot(unsigned int cpu, const struct cpumask *srcp1,
							const struct cpumask *srcp2,
							const struct cpumask *srcp3)
{
	return find_nth_and_andnot_bit(cpumask_bits(srcp1),
					cpumask_bits(srcp2),
					cpumask_bits(srcp3),
					small_cpumask_bits, cpumask_check(cpu));
}

#define CPU_BITS_NONE						\
{								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-1] = 0UL			\
}

#define CPU_BITS_CPU0						\
{								\
	[0] =  1UL						\
}


static __always_inline void cpumask_set_cpu(unsigned int cpu, struct cpumask *dstp)
{
	set_bit(cpumask_check(cpu), cpumask_bits(dstp));
}

static __always_inline void __cpumask_set_cpu(unsigned int cpu, struct cpumask *dstp)
{
	__set_bit(cpumask_check(cpu), cpumask_bits(dstp));
}



static __always_inline void cpumask_clear_cpu(int cpu, struct cpumask *dstp)
{
	clear_bit(cpumask_check(cpu), cpumask_bits(dstp));
}

static __always_inline void __cpumask_clear_cpu(int cpu, struct cpumask *dstp)
{
	__clear_bit(cpumask_check(cpu), cpumask_bits(dstp));
}


static __always_inline bool cpumask_test_cpu(int cpu, const struct cpumask *cpumask)
{
	return test_bit(cpumask_check(cpu), cpumask_bits((cpumask)));
}


static __always_inline bool cpumask_test_and_set_cpu(int cpu, struct cpumask *cpumask)
{
	return test_and_set_bit(cpumask_check(cpu), cpumask_bits(cpumask));
}


static __always_inline bool cpumask_test_and_clear_cpu(int cpu, struct cpumask *cpumask)
{
	return test_and_clear_bit(cpumask_check(cpu), cpumask_bits(cpumask));
}


static inline void cpumask_setall(struct cpumask *dstp)
{
	if (small_const_nbits(small_cpumask_bits)) {
		cpumask_bits(dstp)[0] = BITMAP_LAST_WORD_MASK(nr_cpumask_bits);
		return;
	}
	bitmap_fill(cpumask_bits(dstp), nr_cpumask_bits);
}


static inline void cpumask_clear(struct cpumask *dstp)
{
	bitmap_zero(cpumask_bits(dstp), large_cpumask_bits);
}


static inline bool cpumask_and(struct cpumask *dstp,
			       const struct cpumask *src1p,
			       const struct cpumask *src2p)
{
	return bitmap_and(cpumask_bits(dstp), cpumask_bits(src1p),
				       cpumask_bits(src2p), small_cpumask_bits);
}


static inline void cpumask_or(struct cpumask *dstp, const struct cpumask *src1p,
			      const struct cpumask *src2p)
{
	bitmap_or(cpumask_bits(dstp), cpumask_bits(src1p),
				      cpumask_bits(src2p), small_cpumask_bits);
}


static inline void cpumask_xor(struct cpumask *dstp,
			       const struct cpumask *src1p,
			       const struct cpumask *src2p)
{
	bitmap_xor(cpumask_bits(dstp), cpumask_bits(src1p),
				       cpumask_bits(src2p), small_cpumask_bits);
}


static inline bool cpumask_andnot(struct cpumask *dstp,
				  const struct cpumask *src1p,
				  const struct cpumask *src2p)
{
	return bitmap_andnot(cpumask_bits(dstp), cpumask_bits(src1p),
					  cpumask_bits(src2p), small_cpumask_bits);
}


static inline bool cpumask_equal(const struct cpumask *src1p,
				const struct cpumask *src2p)
{
	return bitmap_equal(cpumask_bits(src1p), cpumask_bits(src2p),
						 small_cpumask_bits);
}


static inline bool cpumask_or_equal(const struct cpumask *src1p,
				    const struct cpumask *src2p,
				    const struct cpumask *src3p)
{
	return bitmap_or_equal(cpumask_bits(src1p), cpumask_bits(src2p),
			       cpumask_bits(src3p), small_cpumask_bits);
}


static inline bool cpumask_intersects(const struct cpumask *src1p,
				     const struct cpumask *src2p)
{
	return bitmap_intersects(cpumask_bits(src1p), cpumask_bits(src2p),
						      small_cpumask_bits);
}


static inline bool cpumask_subset(const struct cpumask *src1p,
				 const struct cpumask *src2p)
{
	return bitmap_subset(cpumask_bits(src1p), cpumask_bits(src2p),
						  small_cpumask_bits);
}


static inline bool cpumask_empty(const struct cpumask *srcp)
{
	return bitmap_empty(cpumask_bits(srcp), small_cpumask_bits);
}


static inline bool cpumask_full(const struct cpumask *srcp)
{
	return bitmap_full(cpumask_bits(srcp), nr_cpumask_bits);
}


static inline unsigned int cpumask_weight(const struct cpumask *srcp)
{
	return bitmap_weight(cpumask_bits(srcp), small_cpumask_bits);
}


static inline unsigned int cpumask_weight_and(const struct cpumask *srcp1,
						const struct cpumask *srcp2)
{
	return bitmap_weight_and(cpumask_bits(srcp1), cpumask_bits(srcp2), small_cpumask_bits);
}


static inline void cpumask_shift_right(struct cpumask *dstp,
				       const struct cpumask *srcp, int n)
{
	bitmap_shift_right(cpumask_bits(dstp), cpumask_bits(srcp), n,
					       small_cpumask_bits);
}


static inline void cpumask_shift_left(struct cpumask *dstp,
				      const struct cpumask *srcp, int n)
{
	bitmap_shift_left(cpumask_bits(dstp), cpumask_bits(srcp), n,
					      nr_cpumask_bits);
}


static inline void cpumask_copy(struct cpumask *dstp,
				const struct cpumask *srcp)
{
	bitmap_copy(cpumask_bits(dstp), cpumask_bits(srcp), large_cpumask_bits);
}


#define cpumask_any(srcp) cpumask_first(srcp)


#define cpumask_any_and(mask1, mask2) cpumask_first_and((mask1), (mask2))


#define cpumask_of(cpu) (get_cpu_mask(cpu))


static inline int cpumask_parse_user(const char __user *buf, int len,
				     struct cpumask *dstp)
{
	return bitmap_parse_user(buf, len, cpumask_bits(dstp), nr_cpumask_bits);
}


static inline int cpumask_parselist_user(const char __user *buf, int len,
				     struct cpumask *dstp)
{
	return bitmap_parselist_user(buf, len, cpumask_bits(dstp),
				     nr_cpumask_bits);
}


static inline int cpumask_parse(const char *buf, struct cpumask *dstp)
{
	return bitmap_parse(buf, UINT_MAX, cpumask_bits(dstp), nr_cpumask_bits);
}


static inline int cpulist_parse(const char *buf, struct cpumask *dstp)
{
	return bitmap_parselist(buf, cpumask_bits(dstp), nr_cpumask_bits);
}


static inline unsigned int cpumask_size(void)
{
	return BITS_TO_LONGS(large_cpumask_bits) * sizeof(long);
}


#ifdef CONFIG_CPUMASK_OFFSTACK
typedef struct cpumask *cpumask_var_t;

#define this_cpu_cpumask_var_ptr(x)	this_cpu_read(x)
#define __cpumask_var_read_mostly	__read_mostly

bool alloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags, int node);

static inline
bool zalloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags, int node)
{
	return alloc_cpumask_var_node(mask, flags | __GFP_ZERO, node);
}


static inline
bool alloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	return alloc_cpumask_var_node(mask, flags, NUMA_NO_NODE);
}

static inline
bool zalloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	return alloc_cpumask_var(mask, flags | __GFP_ZERO);
}

void alloc_bootmem_cpumask_var(cpumask_var_t *mask);
void free_cpumask_var(cpumask_var_t mask);
void free_bootmem_cpumask_var(cpumask_var_t mask);

static inline bool cpumask_available(cpumask_var_t mask)
{
	return mask != NULL;
}

#else
typedef struct cpumask cpumask_var_t[1];

#define this_cpu_cpumask_var_ptr(x) this_cpu_ptr(x)
#define __cpumask_var_read_mostly

static inline bool alloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	return true;
}

static inline bool alloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags,
					  int node)
{
	return true;
}

static inline bool zalloc_cpumask_var(cpumask_var_t *mask, gfp_t flags)
{
	cpumask_clear(*mask);
	return true;
}

static inline bool zalloc_cpumask_var_node(cpumask_var_t *mask, gfp_t flags,
					  int node)
{
	cpumask_clear(*mask);
	return true;
}

static inline void alloc_bootmem_cpumask_var(cpumask_var_t *mask)
{
}

static inline void free_cpumask_var(cpumask_var_t mask)
{
}

static inline void free_bootmem_cpumask_var(cpumask_var_t mask)
{
}

static inline bool cpumask_available(cpumask_var_t mask)
{
	return true;
}
#endif 


extern const DECLARE_BITMAP(cpu_all_bits, NR_CPUS);
#define cpu_all_mask to_cpumask(cpu_all_bits)


#define cpu_none_mask to_cpumask(cpu_bit_bitmap[0])

#if NR_CPUS == 1

#define for_each_possible_cpu(cpu)	for ((cpu) = 0; (cpu) < 1; (cpu)++)
#define for_each_online_cpu(cpu)	for ((cpu) = 0; (cpu) < 1; (cpu)++)
#define for_each_present_cpu(cpu)	for ((cpu) = 0; (cpu) < 1; (cpu)++)
#else
#define for_each_possible_cpu(cpu) for_each_cpu((cpu), cpu_possible_mask)
#define for_each_online_cpu(cpu)   for_each_cpu((cpu), cpu_online_mask)
#define for_each_present_cpu(cpu)  for_each_cpu((cpu), cpu_present_mask)
#endif


void init_cpu_present(const struct cpumask *src);
void init_cpu_possible(const struct cpumask *src);
void init_cpu_online(const struct cpumask *src);

static inline void reset_cpu_possible_mask(void)
{
	bitmap_zero(cpumask_bits(&__cpu_possible_mask), NR_CPUS);
}

static inline void
set_cpu_possible(unsigned int cpu, bool possible)
{
	if (possible)
		cpumask_set_cpu(cpu, &__cpu_possible_mask);
	else
		cpumask_clear_cpu(cpu, &__cpu_possible_mask);
}

static inline void
set_cpu_present(unsigned int cpu, bool present)
{
	if (present)
		cpumask_set_cpu(cpu, &__cpu_present_mask);
	else
		cpumask_clear_cpu(cpu, &__cpu_present_mask);
}

void set_cpu_online(unsigned int cpu, bool online);

static inline void
set_cpu_active(unsigned int cpu, bool active)
{
	if (active)
		cpumask_set_cpu(cpu, &__cpu_active_mask);
	else
		cpumask_clear_cpu(cpu, &__cpu_active_mask);
}

static inline void
set_cpu_dying(unsigned int cpu, bool dying)
{
	if (dying)
		cpumask_set_cpu(cpu, &__cpu_dying_mask);
	else
		cpumask_clear_cpu(cpu, &__cpu_dying_mask);
}


#define to_cpumask(bitmap)						\
	((struct cpumask *)(1 ? (bitmap)				\
			    : (void *)sizeof(__check_is_bitmap(bitmap))))

static inline int __check_is_bitmap(const unsigned long *bitmap)
{
	return 1;
}


extern const unsigned long
	cpu_bit_bitmap[BITS_PER_LONG+1][BITS_TO_LONGS(NR_CPUS)];

static inline const struct cpumask *get_cpu_mask(unsigned int cpu)
{
	const unsigned long *p = cpu_bit_bitmap[1 + cpu % BITS_PER_LONG];
	p -= cpu / BITS_PER_LONG;
	return to_cpumask(p);
}

#if NR_CPUS > 1

static __always_inline unsigned int num_online_cpus(void)
{
	return raw_atomic_read(&__num_online_cpus);
}
#define num_possible_cpus()	cpumask_weight(cpu_possible_mask)
#define num_present_cpus()	cpumask_weight(cpu_present_mask)
#define num_active_cpus()	cpumask_weight(cpu_active_mask)

static inline bool cpu_online(unsigned int cpu)
{
	return cpumask_test_cpu(cpu, cpu_online_mask);
}

static inline bool cpu_possible(unsigned int cpu)
{
	return cpumask_test_cpu(cpu, cpu_possible_mask);
}

static inline bool cpu_present(unsigned int cpu)
{
	return cpumask_test_cpu(cpu, cpu_present_mask);
}

static inline bool cpu_active(unsigned int cpu)
{
	return cpumask_test_cpu(cpu, cpu_active_mask);
}

static inline bool cpu_dying(unsigned int cpu)
{
	return cpumask_test_cpu(cpu, cpu_dying_mask);
}

#else

#define num_online_cpus()	1U
#define num_possible_cpus()	1U
#define num_present_cpus()	1U
#define num_active_cpus()	1U

static inline bool cpu_online(unsigned int cpu)
{
	return cpu == 0;
}

static inline bool cpu_possible(unsigned int cpu)
{
	return cpu == 0;
}

static inline bool cpu_present(unsigned int cpu)
{
	return cpu == 0;
}

static inline bool cpu_active(unsigned int cpu)
{
	return cpu == 0;
}

static inline bool cpu_dying(unsigned int cpu)
{
	return false;
}

#endif 

#define cpu_is_offline(cpu)	unlikely(!cpu_online(cpu))

#if NR_CPUS <= BITS_PER_LONG
#define CPU_BITS_ALL						\
{								\
	[BITS_TO_LONGS(NR_CPUS)-1] = BITMAP_LAST_WORD_MASK(NR_CPUS)	\
}

#else 

#define CPU_BITS_ALL						\
{								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-2] = ~0UL,		\
	[BITS_TO_LONGS(NR_CPUS)-1] = BITMAP_LAST_WORD_MASK(NR_CPUS)	\
}
#endif 


static inline ssize_t
cpumap_print_to_pagebuf(bool list, char *buf, const struct cpumask *mask)
{
	return bitmap_print_to_pagebuf(list, buf, cpumask_bits(mask),
				      nr_cpu_ids);
}


static inline ssize_t
cpumap_print_bitmask_to_buf(char *buf, const struct cpumask *mask,
		loff_t off, size_t count)
{
	return bitmap_print_bitmask_to_buf(buf, cpumask_bits(mask),
				   nr_cpu_ids, off, count) - 1;
}


static inline ssize_t
cpumap_print_list_to_buf(char *buf, const struct cpumask *mask,
		loff_t off, size_t count)
{
	return bitmap_print_list_to_buf(buf, cpumask_bits(mask),
				   nr_cpu_ids, off, count) - 1;
}

#if NR_CPUS <= BITS_PER_LONG
#define CPU_MASK_ALL							\
(cpumask_t) { {								\
	[BITS_TO_LONGS(NR_CPUS)-1] = BITMAP_LAST_WORD_MASK(NR_CPUS)	\
} }
#else
#define CPU_MASK_ALL							\
(cpumask_t) { {								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-2] = ~0UL,			\
	[BITS_TO_LONGS(NR_CPUS)-1] = BITMAP_LAST_WORD_MASK(NR_CPUS)	\
} }
#endif 

#define CPU_MASK_NONE							\
(cpumask_t) { {								\
	[0 ... BITS_TO_LONGS(NR_CPUS)-1] =  0UL				\
} }

#define CPU_MASK_CPU0							\
(cpumask_t) { {								\
	[0] =  1UL							\
} }


#define CPUMAP_FILE_MAX_BYTES  (((NR_CPUS * 9)/32 > PAGE_SIZE) \
					? (NR_CPUS * 9)/32 - 1 : PAGE_SIZE)
#define CPULIST_FILE_MAX_BYTES  (((NR_CPUS * 7)/2 > PAGE_SIZE) ? (NR_CPUS * 7)/2 : PAGE_SIZE)

#endif 
