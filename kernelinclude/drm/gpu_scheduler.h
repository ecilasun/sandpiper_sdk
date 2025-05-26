

#ifndef _DRM_GPU_SCHEDULER_H_
#define _DRM_GPU_SCHEDULER_H_

#include <drm/spsc_queue.h>
#include <linux/dma-fence.h>
#include <linux/completion.h>
#include <linux/xarray.h>
#include <linux/workqueue.h>

#define MAX_WAIT_SCHED_ENTITY_Q_EMPTY msecs_to_jiffies(1000)


#define DRM_SCHED_FENCE_DONT_PIPELINE	DMA_FENCE_FLAG_USER_BITS


#define DRM_SCHED_FENCE_FLAG_HAS_DEADLINE_BIT	(DMA_FENCE_FLAG_USER_BITS + 1)

enum dma_resv_usage;
struct dma_resv;
struct drm_gem_object;

struct drm_gpu_scheduler;
struct drm_sched_rq;

struct drm_file;


enum drm_sched_priority {
	DRM_SCHED_PRIORITY_MIN,
	DRM_SCHED_PRIORITY_NORMAL,
	DRM_SCHED_PRIORITY_HIGH,
	DRM_SCHED_PRIORITY_KERNEL,

	DRM_SCHED_PRIORITY_COUNT
};


extern int drm_sched_policy;

#define DRM_SCHED_POLICY_RR    0
#define DRM_SCHED_POLICY_FIFO  1


struct drm_sched_entity {
	
	struct list_head		list;

	
	struct drm_sched_rq		*rq;

	
	struct drm_gpu_scheduler        **sched_list;

	
	unsigned int                    num_sched_list;

	
	enum drm_sched_priority         priority;

	
	spinlock_t			rq_lock;

	
	struct spsc_queue		job_queue;

	
	atomic_t			fence_seq;

	
	uint64_t			fence_context;

	
	struct dma_fence		*dependency;

	
	struct dma_fence_cb		cb;

	
	atomic_t			*guilty;

	
	struct dma_fence __rcu		*last_scheduled;

	
	struct task_struct		*last_user;

	
	bool 				stopped;

	
	struct completion		entity_idle;

	
	ktime_t				oldest_job_waiting;

	
	struct rb_node			rb_tree_node;

};


struct drm_sched_rq {
	spinlock_t			lock;
	struct drm_gpu_scheduler	*sched;
	struct list_head		entities;
	struct drm_sched_entity		*current_entity;
	struct rb_root_cached		rb_tree_root;
};


struct drm_sched_fence {
        
	struct dma_fence		scheduled;

        
	struct dma_fence		finished;

	
	ktime_t				deadline;

        
	struct dma_fence		*parent;
        
	struct drm_gpu_scheduler	*sched;
        
	spinlock_t			lock;
        
	void				*owner;
};

struct drm_sched_fence *to_drm_sched_fence(struct dma_fence *f);


struct drm_sched_job {
	struct spsc_node		queue_node;
	struct list_head		list;
	struct drm_gpu_scheduler	*sched;
	struct drm_sched_fence		*s_fence;

	
	union {
		struct dma_fence_cb		finish_cb;
		struct work_struct		work;
	};

	uint64_t			id;
	atomic_t			karma;
	enum drm_sched_priority		s_priority;
	struct drm_sched_entity         *entity;
	struct dma_fence_cb		cb;
	
	struct xarray			dependencies;

	
	unsigned long			last_dependency;

	
	ktime_t                         submit_ts;
};

static inline bool drm_sched_invalidate_job(struct drm_sched_job *s_job,
					    int threshold)
{
	return s_job && atomic_inc_return(&s_job->karma) > threshold;
}

enum drm_gpu_sched_stat {
	DRM_GPU_SCHED_STAT_NONE, 
	DRM_GPU_SCHED_STAT_NOMINAL,
	DRM_GPU_SCHED_STAT_ENODEV,
};


struct drm_sched_backend_ops {
	
	struct dma_fence *(*prepare_job)(struct drm_sched_job *sched_job,
					 struct drm_sched_entity *s_entity);

	
	struct dma_fence *(*run_job)(struct drm_sched_job *sched_job);

	
	enum drm_gpu_sched_stat (*timedout_job)(struct drm_sched_job *sched_job);

	
	void (*free_job)(struct drm_sched_job *sched_job);
};


struct drm_gpu_scheduler {
	const struct drm_sched_backend_ops	*ops;
	uint32_t			hw_submission_limit;
	long				timeout;
	const char			*name;
	struct drm_sched_rq		sched_rq[DRM_SCHED_PRIORITY_COUNT];
	wait_queue_head_t		wake_up_worker;
	wait_queue_head_t		job_scheduled;
	atomic_t			hw_rq_count;
	atomic64_t			job_id_count;
	struct workqueue_struct		*timeout_wq;
	struct delayed_work		work_tdr;
	struct task_struct		*thread;
	struct list_head		pending_list;
	spinlock_t			job_list_lock;
	int				hang_limit;
	atomic_t                        *score;
	atomic_t                        _score;
	bool				ready;
	bool				free_guilty;
	struct device			*dev;
};

int drm_sched_init(struct drm_gpu_scheduler *sched,
		   const struct drm_sched_backend_ops *ops,
		   uint32_t hw_submission, unsigned hang_limit,
		   long timeout, struct workqueue_struct *timeout_wq,
		   atomic_t *score, const char *name, struct device *dev);

void drm_sched_fini(struct drm_gpu_scheduler *sched);
int drm_sched_job_init(struct drm_sched_job *job,
		       struct drm_sched_entity *entity,
		       void *owner);
void drm_sched_job_arm(struct drm_sched_job *job);
int drm_sched_job_add_dependency(struct drm_sched_job *job,
				 struct dma_fence *fence);
int drm_sched_job_add_syncobj_dependency(struct drm_sched_job *job,
					 struct drm_file *file,
					 u32 handle,
					 u32 point);
int drm_sched_job_add_resv_dependencies(struct drm_sched_job *job,
					struct dma_resv *resv,
					enum dma_resv_usage usage);
int drm_sched_job_add_implicit_dependencies(struct drm_sched_job *job,
					    struct drm_gem_object *obj,
					    bool write);


void drm_sched_entity_modify_sched(struct drm_sched_entity *entity,
				    struct drm_gpu_scheduler **sched_list,
                                   unsigned int num_sched_list);

void drm_sched_job_cleanup(struct drm_sched_job *job);
void drm_sched_wakeup_if_can_queue(struct drm_gpu_scheduler *sched);
void drm_sched_stop(struct drm_gpu_scheduler *sched, struct drm_sched_job *bad);
void drm_sched_start(struct drm_gpu_scheduler *sched, bool full_recovery);
void drm_sched_resubmit_jobs(struct drm_gpu_scheduler *sched);
void drm_sched_increase_karma(struct drm_sched_job *bad);
void drm_sched_reset_karma(struct drm_sched_job *bad);
void drm_sched_increase_karma_ext(struct drm_sched_job *bad, int type);
bool drm_sched_dependency_optimized(struct dma_fence* fence,
				    struct drm_sched_entity *entity);
void drm_sched_fault(struct drm_gpu_scheduler *sched);

void drm_sched_rq_add_entity(struct drm_sched_rq *rq,
			     struct drm_sched_entity *entity);
void drm_sched_rq_remove_entity(struct drm_sched_rq *rq,
				struct drm_sched_entity *entity);

void drm_sched_rq_update_fifo(struct drm_sched_entity *entity, ktime_t ts);

int drm_sched_entity_init(struct drm_sched_entity *entity,
			  enum drm_sched_priority priority,
			  struct drm_gpu_scheduler **sched_list,
			  unsigned int num_sched_list,
			  atomic_t *guilty);
long drm_sched_entity_flush(struct drm_sched_entity *entity, long timeout);
void drm_sched_entity_fini(struct drm_sched_entity *entity);
void drm_sched_entity_destroy(struct drm_sched_entity *entity);
void drm_sched_entity_select_rq(struct drm_sched_entity *entity);
struct drm_sched_job *drm_sched_entity_pop_job(struct drm_sched_entity *entity);
void drm_sched_entity_push_job(struct drm_sched_job *sched_job);
void drm_sched_entity_set_priority(struct drm_sched_entity *entity,
				   enum drm_sched_priority priority);
bool drm_sched_entity_is_ready(struct drm_sched_entity *entity);
int drm_sched_entity_error(struct drm_sched_entity *entity);

struct drm_sched_fence *drm_sched_fence_alloc(
	struct drm_sched_entity *s_entity, void *owner);
void drm_sched_fence_init(struct drm_sched_fence *fence,
			  struct drm_sched_entity *entity);
void drm_sched_fence_free(struct drm_sched_fence *fence);

void drm_sched_fence_scheduled(struct drm_sched_fence *fence,
			       struct dma_fence *parent);
void drm_sched_fence_finished(struct drm_sched_fence *fence, int result);

unsigned long drm_sched_suspend_timeout(struct drm_gpu_scheduler *sched);
void drm_sched_resume_timeout(struct drm_gpu_scheduler *sched,
		                unsigned long remaining);
struct drm_gpu_scheduler *
drm_sched_pick_best(struct drm_gpu_scheduler **sched_list,
		     unsigned int num_sched_list);

#endif
