#include <string.h>
#include <math.h>
#include <pthread.h>
#include "tinynet.h"

// ============================================================================
// Parallel Training Data Structures
// ============================================================================

#define TINYNET_NUM_THREADS 4

typedef struct {
	TinyNetC* net;
	const float* x;
	const float* hidden;
	const float* pre;
	float grad_out;
	float lr;
	int start_hidden;
	int end_hidden;
} TinyNetTrainingTask;

typedef struct {
	float sum;
	pthread_mutex_t lock;
} OutputReductionState;

float tinynet_predict(const TinyNetC* net, const float* x)
{
	float hidden[TINYNET_HIDDEN_SIZE];

	// Compute hidden layer activations
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float sum = net->b1[i];
		const float* row = net->w1 + i * TINYNET_INPUT_SIZE;
		
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			sum += row[j] * x[j];
		
		hidden[i] = sum > 0.0f ? sum : 0.0f;  // ReLU
	}

	// Compute output layer
	float out = net->b2;
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
		out += net->w2[i] * hidden[i];
	
	return out;
}

void tinynet_train_on_sample(TinyNetC* net, const float* x, float target, float lr)
{
	float hidden[TINYNET_HIDDEN_SIZE];
	float pre[TINYNET_HIDDEN_SIZE];
	
	// Forward pass for hidden layer
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float sum = net->b1[i];
		const float* row = net->w1 + i * TINYNET_INPUT_SIZE;
		
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			sum += row[j] * x[j];
		
		pre[i] = sum;
		hidden[i] = sum > 0.0f ? sum : 0.0f;
	}

	// Output layer
	float out = net->b2;
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
		out += net->w2[i] * hidden[i];

	// Compute loss and output gradient
	float loss = out - target;
	float grad_out = loss;

	// Backward pass
	net->b2 -= lr * grad_out;
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float w2_old = net->w2[i];
		net->w2[i] -= lr * grad_out * hidden[i];

		// Gradient for hidden layer
		float grad_hidden = grad_out * w2_old;
		if (pre[i] <= 0.0f)
			grad_hidden = 0.0f;
		net->b1[i] -= lr * grad_hidden;

		// Update weights for hidden layer
		float* row = net->w1 + i * TINYNET_INPUT_SIZE;
		float coeff = -lr * grad_hidden;
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			row[j] += coeff * x[j];
	}
}

// ============================================================================
// Parallel Implementation Using pthreads
// ============================================================================

/**
 * Worker thread function for forward pass (hidden layer computation)
 * Each thread computes a range of hidden neuron activations
 */
static void* tinynet_forward_hidden_worker(void* arg)
{
	TinyNetTrainingTask* task = (TinyNetTrainingTask*)arg;
	
	for (int i = task->start_hidden; i < task->end_hidden; ++i)
	{
		float sum = task->net->b1[i];
		const float* row = task->net->w1 + i * TINYNET_INPUT_SIZE;
		
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			sum += row[j] * task->x[j];
		
		// Store both pre-activation and post-activation values
		((float*)task->pre)[i] = sum;
		((float*)task->hidden)[i] = sum > 0.0f ? sum : 0.0f;  // ReLU
	}
	
	pthread_exit(NULL);
}

/**
 * Worker thread function for backward pass (gradient computation and weight updates)
 * Each thread updates a range of hidden neuron weights and biases
 */
static void* tinynet_backward_worker(void* arg)
{
	TinyNetTrainingTask* task = (TinyNetTrainingTask*)arg;
	
	for (int i = task->start_hidden; i < task->end_hidden; ++i)
	{
		float w2_old = task->net->w2[i];
		task->net->w2[i] -= task->lr * task->grad_out * task->hidden[i];

		// Gradient for hidden layer
		float grad_hidden = task->grad_out * w2_old;
		if (task->pre[i] <= 0.0f)
			grad_hidden = 0.0f;
		
		task->net->b1[i] -= task->lr * grad_hidden;

		// Update weights for hidden layer
		float* row = task->net->w1 + i * TINYNET_INPUT_SIZE;
		float coeff = -task->lr * grad_hidden;
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			row[j] += coeff * task->x[j];
	}
	
	pthread_exit(NULL);
}

/**
 * Parallel forward pass - computes hidden layer activations using multiple threads
 */
static void tinynet_forward_hidden_parallel(
	const TinyNetC* net, 
	const float* x, 
	float* hidden, 
	float* pre,
	int num_threads)
{
	pthread_t threads[TINYNET_NUM_THREADS];
	TinyNetTrainingTask tasks[TINYNET_NUM_THREADS];
	
	if (num_threads > TINYNET_NUM_THREADS)
		num_threads = TINYNET_NUM_THREADS;
	if (num_threads < 1)
		num_threads = 1;
	
	int chunk_size = TINYNET_HIDDEN_SIZE / num_threads;
	
	// Spawn threads
	for (int t = 0; t < num_threads; ++t)
	{
		tasks[t].net = (TinyNetC*)net;
		tasks[t].x = x;
		tasks[t].hidden = hidden;
		tasks[t].pre = pre;
		tasks[t].start_hidden = t * chunk_size;
		tasks[t].end_hidden = (t == num_threads - 1) ? TINYNET_HIDDEN_SIZE : (t + 1) * chunk_size;
		
		pthread_create(&threads[t], NULL, tinynet_forward_hidden_worker, &tasks[t]);
	}
	
	// Wait for all threads
	for (int t = 0; t < num_threads; ++t)
		pthread_join(threads[t], NULL);
}

/**
 * Parallel backward pass - updates weights using multiple threads
 */
static void tinynet_backward_parallel(
	TinyNetC* net,
	const float* x,
	const float* hidden,
	const float* pre,
	float grad_out,
	float lr,
	int num_threads)
{
	pthread_t threads[TINYNET_NUM_THREADS];
	TinyNetTrainingTask tasks[TINYNET_NUM_THREADS];
	
	if (num_threads > TINYNET_NUM_THREADS)
		num_threads = TINYNET_NUM_THREADS;
	if (num_threads < 1)
		num_threads = 1;
	
	int chunk_size = TINYNET_HIDDEN_SIZE / num_threads;
	
	// Update output bias (no need to parallelize - it's just one value)
	net->b2 -= lr * grad_out;
	
	// Spawn threads for backward pass
	for (int t = 0; t < num_threads; ++t)
	{
		tasks[t].net = net;
		tasks[t].x = x;
		tasks[t].hidden = hidden;
		tasks[t].pre = pre;
		tasks[t].grad_out = grad_out;
		tasks[t].lr = lr;
		tasks[t].start_hidden = t * chunk_size;
		tasks[t].end_hidden = (t == num_threads - 1) ? TINYNET_HIDDEN_SIZE : (t + 1) * chunk_size;
		
		pthread_create(&threads[t], NULL, tinynet_backward_worker, &tasks[t]);
	}
	
	// Wait for all threads
	for (int t = 0; t < num_threads; ++t)
		pthread_join(threads[t], NULL);
}

/**
 * Parallel training function using pthreads
 * num_threads: Number of worker threads (default is TINYNET_NUM_THREADS)
 */
void tinynet_train_on_sample_parallel(
	TinyNetC* net, 
	const float* x, 
	float target, 
	float lr,
	int num_threads)
{
	float hidden[TINYNET_HIDDEN_SIZE];
	float pre[TINYNET_HIDDEN_SIZE];
	
	// Forward pass using parallel threads
	tinynet_forward_hidden_parallel(net, x, hidden, pre, num_threads);
	
	// Compute output layer (single-threaded reduction)
	float out = net->b2;
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
		out += net->w2[i] * hidden[i];
	
	// Compute loss and output gradient
	float grad_out = out - target;
	
	// Backward pass using parallel threads
	tinynet_backward_parallel(net, x, hidden, pre, grad_out, lr, num_threads);
}
