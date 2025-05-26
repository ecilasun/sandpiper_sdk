

#ifndef DRM_SCHEDULER_SPSC_QUEUE_H_
#define DRM_SCHEDULER_SPSC_QUEUE_H_

#include <linux/atomic.h>
#include <linux/preempt.h>



struct spsc_node {

	
	struct spsc_node *next;
};

struct spsc_queue {

	 struct spsc_node *head;

	
	atomic_long_t tail;

	atomic_t job_count;
};

static inline void spsc_queue_init(struct spsc_queue *queue)
{
	queue->head = NULL;
	atomic_long_set(&queue->tail, (long)&queue->head);
	atomic_set(&queue->job_count, 0);
}

static inline struct spsc_node *spsc_queue_peek(struct spsc_queue *queue)
{
	return queue->head;
}

static inline int spsc_queue_count(struct spsc_queue *queue)
{
	return atomic_read(&queue->job_count);
}

static inline bool spsc_queue_push(struct spsc_queue *queue, struct spsc_node *node)
{
	struct spsc_node **tail;

	node->next = NULL;

	preempt_disable();

	tail = (struct spsc_node **)atomic_long_xchg(&queue->tail, (long)&node->next);
	WRITE_ONCE(*tail, node);
	atomic_inc(&queue->job_count);

	
	smp_wmb();

	preempt_enable();

	return tail == &queue->head;
}


static inline struct spsc_node *spsc_queue_pop(struct spsc_queue *queue)
{
	struct spsc_node *next, *node;

	
	smp_rmb();

	node = READ_ONCE(queue->head);

	if (!node)
		return NULL;

	next = READ_ONCE(node->next);
	WRITE_ONCE(queue->head, next);

	if (unlikely(!next)) {
		

		if (atomic_long_cmpxchg(&queue->tail,
				(long)&node->next, (long) &queue->head) != (long)&node->next) {
			
			do {
				smp_rmb();
			} while (unlikely(!(queue->head = READ_ONCE(node->next))));
		}
	}

	atomic_dec(&queue->job_count);
	return node;
}



#endif 
