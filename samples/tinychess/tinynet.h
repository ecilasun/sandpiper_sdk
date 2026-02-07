#ifndef TINYNET_H
#define TINYNET_H

#ifdef __cplusplus
extern "C" {
#endif

#define TINYNET_INPUT_SIZE (64 * 12 + 1)
#define TINYNET_HIDDEN_SIZE 32

typedef struct {
	float w1[TINYNET_HIDDEN_SIZE * TINYNET_INPUT_SIZE];
	float b1[TINYNET_HIDDEN_SIZE];
	float w2[TINYNET_HIDDEN_SIZE];
	float b2;
} TinyNetC;

float tinynet_predict(const TinyNetC* net, const float* x);
void tinynet_train_on_sample(TinyNetC* net, const float* x, float target, float lr);

// Parallel training function using pthreads
// num_threads: Number of worker threads to use (recommended: 2-8)
void tinynet_train_on_sample_parallel(
	TinyNetC* net, 
	const float* x, 
	float target, 
	float lr,
	int num_threads);

#ifdef __cplusplus
}
#endif

#endif // TINYNET_H
