/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _LINUX_BPF_VERIFIER_H
#define _LINUX_BPF_VERIFIER_H 1

#include <linux/bpf.h> 
#include <linux/btf.h> 
#include <linux/filter.h> 
#include <linux/tnum.h>


#define BPF_MAX_VAR_OFF	(1 << 29)

#define BPF_MAX_VAR_SIZ	(1 << 29)

#define TMP_STR_BUF_LEN 320


enum bpf_reg_liveness {
	REG_LIVE_NONE = 0, 
	REG_LIVE_READ32 = 0x1, 
	REG_LIVE_READ64 = 0x2, 
	REG_LIVE_READ = REG_LIVE_READ32 | REG_LIVE_READ64,
	REG_LIVE_WRITTEN = 0x4, 
	REG_LIVE_DONE = 0x8, 
};


struct bpf_active_lock {
	
	void *ptr;
	
	u32 id;
};

#define ITER_PREFIX "bpf_iter_"

enum bpf_iter_state {
	BPF_ITER_STATE_INVALID, 
	BPF_ITER_STATE_ACTIVE,
	BPF_ITER_STATE_DRAINED,
};

struct bpf_reg_state {
	
	enum bpf_reg_type type;
	
	s32 off;
	union {
		
		int range;

		
		struct {
			struct bpf_map *map_ptr;
			
			u32 map_uid;
		};

		
		struct {
			struct btf *btf;
			u32 btf_id;
		};

		struct { 
			u32 mem_size;
			u32 dynptr_id; 
		};

		
		struct {
			enum bpf_dynptr_type type;
			
			bool first_slot;
		} dynptr;

		
		struct {
			
			struct btf *btf;
			u32 btf_id;
			
			enum bpf_iter_state state:2;
			int depth:30;
		} iter;

		
		struct {
			unsigned long raw1;
			unsigned long raw2;
		} raw;

		u32 subprogno; 
	};
	
	struct tnum var_off;
	
	s64 smin_value; 
	s64 smax_value; 
	u64 umin_value; 
	u64 umax_value; 
	s32 s32_min_value; 
	s32 s32_max_value; 
	u32 u32_min_value; 
	u32 u32_max_value; 
	
	u32 id;
	
	u32 ref_obj_id;
	
	struct bpf_reg_state *parent;
	
	u32 frameno;
	
	s32 subreg_def;
	enum bpf_reg_liveness live;
	
	bool precise;
};

enum bpf_stack_slot_type {
	STACK_INVALID,    
	STACK_SPILL,      
	STACK_MISC,	  
	STACK_ZERO,	  
	
	STACK_DYNPTR,
	STACK_ITER,
};

#define BPF_REG_SIZE 8	

#define BPF_REGMASK_ARGS ((1 << BPF_REG_1) | (1 << BPF_REG_2) | \
			  (1 << BPF_REG_3) | (1 << BPF_REG_4) | \
			  (1 << BPF_REG_5))

#define BPF_DYNPTR_SIZE		sizeof(struct bpf_dynptr_kern)
#define BPF_DYNPTR_NR_SLOTS		(BPF_DYNPTR_SIZE / BPF_REG_SIZE)

struct bpf_stack_state {
	struct bpf_reg_state spilled_ptr;
	u8 slot_type[BPF_REG_SIZE];
};

struct bpf_reference_state {
	
	int id;
	
	int insn_idx;
	
	int callback_ref;
};


struct bpf_func_state {
	struct bpf_reg_state regs[MAX_BPF_REG];
	
	int callsite;
	
	u32 frameno;
	
	u32 subprogno;
	
	u32 async_entry_cnt;
	bool in_callback_fn;
	struct tnum callback_ret_range;
	bool in_async_callback_fn;
	
	u32 callback_depth;

	
	int acquired_refs;
	struct bpf_reference_state *refs;
	int allocated_stack;
	struct bpf_stack_state *stack;
};

struct bpf_idx_pair {
	u32 prev_idx;
	u32 idx;
};

#define MAX_CALL_FRAMES 8

#define BPF_ID_MAP_SIZE ((MAX_BPF_REG + MAX_BPF_STACK / BPF_REG_SIZE) * MAX_CALL_FRAMES)
struct bpf_verifier_state {
	
	struct bpf_func_state *frame[MAX_CALL_FRAMES];
	struct bpf_verifier_state *parent;
	
	u32 branches;
	u32 insn_idx;
	u32 curframe;

	struct bpf_active_lock active_lock;
	bool speculative;
	bool active_rcu_lock;
	
	bool used_as_loop_entry;

	
	u32 first_insn_idx;
	u32 last_insn_idx;
	
	struct bpf_verifier_state *loop_entry;
	
	struct bpf_idx_pair *jmp_history;
	u32 jmp_history_cnt;
	u32 dfs_depth;
	u32 callback_unroll_depth;
};

#define bpf_get_spilled_reg(slot, frame)				\
	(((slot < frame->allocated_stack / BPF_REG_SIZE) &&		\
	  (frame->stack[slot].slot_type[0] == STACK_SPILL))		\
	 ? &frame->stack[slot].spilled_ptr : NULL)


#define bpf_for_each_spilled_reg(iter, frame, reg)			\
	for (iter = 0, reg = bpf_get_spilled_reg(iter, frame);		\
	     iter < frame->allocated_stack / BPF_REG_SIZE;		\
	     iter++, reg = bpf_get_spilled_reg(iter, frame))


#define bpf_for_each_reg_in_vstate(__vst, __state, __reg, __expr)   \
	({                                                               \
		struct bpf_verifier_state *___vstate = __vst;            \
		int ___i, ___j;                                          \
		for (___i = 0; ___i <= ___vstate->curframe; ___i++) {    \
			struct bpf_reg_state *___regs;                   \
			__state = ___vstate->frame[___i];                \
			___regs = __state->regs;                         \
			for (___j = 0; ___j < MAX_BPF_REG; ___j++) {     \
				__reg = &___regs[___j];                  \
				(void)(__expr);                          \
			}                                                \
			bpf_for_each_spilled_reg(___j, __state, __reg) { \
				if (!__reg)                              \
					continue;                        \
				(void)(__expr);                          \
			}                                                \
		}                                                        \
	})


struct bpf_verifier_state_list {
	struct bpf_verifier_state state;
	struct bpf_verifier_state_list *next;
	int miss_cnt, hit_cnt;
};

struct bpf_loop_inline_state {
	unsigned int initialized:1; 
	unsigned int fit_for_inline:1; 
	u32 callback_subprogno; 
};


#define BPF_ALU_SANITIZE_SRC		(1U << 0)
#define BPF_ALU_SANITIZE_DST		(1U << 1)
#define BPF_ALU_NEG_VALUE		(1U << 2)
#define BPF_ALU_NON_POINTER		(1U << 3)
#define BPF_ALU_IMMEDIATE		(1U << 4)
#define BPF_ALU_SANITIZE		(BPF_ALU_SANITIZE_SRC | \
					 BPF_ALU_SANITIZE_DST)

struct bpf_insn_aux_data {
	union {
		enum bpf_reg_type ptr_type;	
		unsigned long map_ptr_state;	
		s32 call_imm;			
		u32 alu_limit;			
		struct {
			u32 map_index;		
			u32 map_off;		
		};
		struct {
			enum bpf_reg_type reg_type;	
			union {
				struct {
					struct btf *btf;
					u32 btf_id;	
				};
				u32 mem_size;	
			};
		} btf_var;
		
		struct bpf_loop_inline_state loop_inline_state;
	};
	union {
		
		u64 obj_new_size;
		
		u64 insert_off;
	};
	struct btf_struct_meta *kptr_struct_meta;
	u64 map_key_state; 
	int ctx_field_size; 
	u32 seen; 
	bool sanitize_stack_spill; 
	bool zext_dst; 
	bool storage_get_func_atomic; 
	bool is_iter_next; 
	u8 alu_state; 

	
	unsigned int orig_idx; 
	bool jmp_point;
	bool prune_point;
	
	bool force_checkpoint;
	
	bool calls_callback;
};

#define MAX_USED_MAPS 64 
#define MAX_USED_BTFS 64 

#define BPF_VERIFIER_TMP_LOG_SIZE	1024

struct bpf_verifier_log {
	
	u64 start_pos;
	u64 end_pos;
	char __user *ubuf;
	u32 level;
	u32 len_total;
	u32 len_max;
	char kbuf[BPF_VERIFIER_TMP_LOG_SIZE];
};

#define BPF_LOG_LEVEL1	1
#define BPF_LOG_LEVEL2	2
#define BPF_LOG_STATS	4
#define BPF_LOG_FIXED	8
#define BPF_LOG_LEVEL	(BPF_LOG_LEVEL1 | BPF_LOG_LEVEL2)
#define BPF_LOG_MASK	(BPF_LOG_LEVEL | BPF_LOG_STATS | BPF_LOG_FIXED)
#define BPF_LOG_KERNEL	(BPF_LOG_MASK + 1) 
#define BPF_LOG_MIN_ALIGNMENT 8U
#define BPF_LOG_ALIGNMENT 40U

static inline bool bpf_verifier_log_needed(const struct bpf_verifier_log *log)
{
	return log && log->level;
}

#define BPF_MAX_SUBPROGS 256

struct bpf_subprog_info {
	
	u32 start; 
	u32 linfo_idx; 
	u16 stack_depth; 
	bool has_tail_call;
	bool tail_call_reachable;
	bool has_ld_abs;
	bool is_async_cb;
};

struct bpf_verifier_env;

struct backtrack_state {
	struct bpf_verifier_env *env;
	u32 frame;
	u32 reg_masks[MAX_CALL_FRAMES];
	u64 stack_masks[MAX_CALL_FRAMES];
};

struct bpf_id_pair {
	u32 old;
	u32 cur;
};

struct bpf_idmap {
	u32 tmp_id_gen;
	struct bpf_id_pair map[BPF_ID_MAP_SIZE];
};

struct bpf_idset {
	u32 count;
	u32 ids[BPF_ID_MAP_SIZE];
};


struct bpf_verifier_env {
	u32 insn_idx;
	u32 prev_insn_idx;
	struct bpf_prog *prog;		
	const struct bpf_verifier_ops *ops;
	struct bpf_verifier_stack_elem *head; 
	int stack_size;			
	bool strict_alignment;		
	bool test_state_freq;		
	struct bpf_verifier_state *cur_state; 
	struct bpf_verifier_state_list **explored_states; 
	struct bpf_verifier_state_list *free_list;
	struct bpf_map *used_maps[MAX_USED_MAPS]; 
	struct btf_mod_pair used_btfs[MAX_USED_BTFS]; 
	u32 used_map_cnt;		
	u32 used_btf_cnt;		
	u32 id_gen;			
	bool explore_alu_limits;
	bool allow_ptr_leaks;
	bool allow_uninit_stack;
	bool bpf_capable;
	bool bypass_spec_v1;
	bool bypass_spec_v4;
	bool seen_direct_write;
	struct bpf_insn_aux_data *insn_aux_data; 
	const struct bpf_line_info *prev_linfo;
	struct bpf_verifier_log log;
	struct bpf_subprog_info subprog_info[BPF_MAX_SUBPROGS + 1];
	union {
		struct bpf_idmap idmap_scratch;
		struct bpf_idset idset_scratch;
	};
	struct {
		int *insn_state;
		int *insn_stack;
		int cur_stack;
	} cfg;
	struct backtrack_state bt;
	u32 pass_cnt; 
	u32 subprog_cnt;
	
	u32 prev_insn_processed, insn_processed;
	
	u32 prev_jmps_processed, jmps_processed;
	
	u64 verification_time;
	
	u32 max_states_per_insn;
	
	u32 total_states;
	
	u32 peak_states;
	
	u32 longest_mark_read_walk;
	bpfptr_t fd_array;

	
	u32 scratched_regs;
	
	u64 scratched_stack_slots;
	u64 prev_log_pos, prev_insn_print_pos;
	
	char tmp_str_buf[TMP_STR_BUF_LEN];
};

__printf(2, 0) void bpf_verifier_vlog(struct bpf_verifier_log *log,
				      const char *fmt, va_list args);
__printf(2, 3) void bpf_verifier_log_write(struct bpf_verifier_env *env,
					   const char *fmt, ...);
__printf(2, 3) void bpf_log(struct bpf_verifier_log *log,
			    const char *fmt, ...);
int bpf_vlog_init(struct bpf_verifier_log *log, u32 log_level,
		  char __user *log_buf, u32 log_size);
void bpf_vlog_reset(struct bpf_verifier_log *log, u64 new_pos);
int bpf_vlog_finalize(struct bpf_verifier_log *log, u32 *log_size_actual);

static inline struct bpf_func_state *cur_func(struct bpf_verifier_env *env)
{
	struct bpf_verifier_state *cur = env->cur_state;

	return cur->frame[cur->curframe];
}

static inline struct bpf_reg_state *cur_regs(struct bpf_verifier_env *env)
{
	return cur_func(env)->regs;
}

int bpf_prog_offload_verifier_prep(struct bpf_prog *prog);
int bpf_prog_offload_verify_insn(struct bpf_verifier_env *env,
				 int insn_idx, int prev_insn_idx);
int bpf_prog_offload_finalize(struct bpf_verifier_env *env);
void
bpf_prog_offload_replace_insn(struct bpf_verifier_env *env, u32 off,
			      struct bpf_insn *insn);
void
bpf_prog_offload_remove_insns(struct bpf_verifier_env *env, u32 off, u32 cnt);

int check_ptr_off_reg(struct bpf_verifier_env *env,
		      const struct bpf_reg_state *reg, int regno);
int check_func_arg_reg_off(struct bpf_verifier_env *env,
			   const struct bpf_reg_state *reg, int regno,
			   enum bpf_arg_type arg_type);
int check_mem_reg(struct bpf_verifier_env *env, struct bpf_reg_state *reg,
		   u32 regno, u32 mem_size);


static inline u64 bpf_trampoline_compute_key(const struct bpf_prog *tgt_prog,
					     struct btf *btf, u32 btf_id)
{
	if (tgt_prog)
		return ((u64)tgt_prog->aux->id << 32) | btf_id;
	else
		return ((u64)btf_obj_id(btf) << 32) | 0x80000000 | btf_id;
}


static inline void bpf_trampoline_unpack_key(u64 key, u32 *obj_id, u32 *btf_id)
{
	if (obj_id)
		*obj_id = key >> 32;
	if (btf_id)
		*btf_id = key & 0x7FFFFFFF;
}

int bpf_check_attach_target(struct bpf_verifier_log *log,
			    const struct bpf_prog *prog,
			    const struct bpf_prog *tgt_prog,
			    u32 btf_id,
			    struct bpf_attach_target_info *tgt_info);
void bpf_free_kfunc_btf_tab(struct bpf_kfunc_btf_tab *tab);

int mark_chain_precision(struct bpf_verifier_env *env, int regno);

#define BPF_BASE_TYPE_MASK	GENMASK(BPF_BASE_TYPE_BITS - 1, 0)


static inline u32 base_type(u32 type)
{
	return type & BPF_BASE_TYPE_MASK;
}


static inline u32 type_flag(u32 type)
{
	return type & ~BPF_BASE_TYPE_MASK;
}


static inline enum bpf_prog_type resolve_prog_type(const struct bpf_prog *prog)
{
	return prog->type == BPF_PROG_TYPE_EXT ?
		prog->aux->dst_prog->type : prog->type;
}

static inline bool bpf_prog_check_recur(const struct bpf_prog *prog)
{
	switch (resolve_prog_type(prog)) {
	case BPF_PROG_TYPE_TRACING:
		return prog->expected_attach_type != BPF_TRACE_ITER;
	case BPF_PROG_TYPE_STRUCT_OPS:
	case BPF_PROG_TYPE_LSM:
		return false;
	default:
		return true;
	}
}

#define BPF_REG_TRUSTED_MODIFIERS (MEM_ALLOC | PTR_TRUSTED | NON_OWN_REF)

static inline bool bpf_type_has_unsafe_modifiers(u32 type)
{
	return type_flag(type) & ~BPF_REG_TRUSTED_MODIFIERS;
}

#endif 
