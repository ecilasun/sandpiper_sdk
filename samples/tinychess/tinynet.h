#ifndef TINYNET_H
#define TINYNET_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TINYNET_INPUT_SIZE (64 * 12 + 1)	// 64 (8x8) squares * 12 piece types + 1 for side to move
#define TINYNET_HIDDEN_SIZE 32				// First hidden layer width
#define TINYNET_HIDDEN2_SIZE 32				// Second hidden layer width

typedef struct {
	float w1[TINYNET_HIDDEN_SIZE * TINYNET_INPUT_SIZE];
	float b1[TINYNET_HIDDEN_SIZE];
	float w2[TINYNET_HIDDEN2_SIZE * TINYNET_HIDDEN_SIZE];
	float b2[TINYNET_HIDDEN2_SIZE];
	float w3[TINYNET_HIDDEN2_SIZE];
	float b3;
} TinyNetC;

void tinynet_init(TinyNetC* net, uint32_t seed);
float tinynet_predict(const TinyNetC* net, const float* x);
void tinynet_train_on_sample(TinyNetC* net, const float* x, float target, float lr);

#ifdef __cplusplus
}
#endif

#endif // TINYNET_H
