#include <math.h>
#include "tinynet.h"

static inline uint32_t tinynet_lcg(uint32_t* state)
{
	*state = (*state * 1664525u) + 1013904223u;
	return *state;
}

static inline float tinynet_rand_uniform(uint32_t* state)
{
	// Returns value in [-0.05, 0.05)
	const float scale = 2.3283064365386963e-10f; // 1 / 2^32
	float u = tinynet_lcg(state) * scale;        // [0, 1)
	return (u - 0.5f) * 0.1f;
}

void tinynet_init(TinyNetC* net, uint32_t seed)
{
	uint32_t state = seed ? seed : 1u;
	for (int i = 0; i < TINYNET_HIDDEN_SIZE * TINYNET_INPUT_SIZE; ++i)
		net->w1[i] = tinynet_rand_uniform(&state);
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
		net->b1[i] = tinynet_rand_uniform(&state);
	for (int i = 0; i < TINYNET_HIDDEN2_SIZE * TINYNET_HIDDEN_SIZE; ++i)
		net->w2[i] = tinynet_rand_uniform(&state);
	for (int i = 0; i < TINYNET_HIDDEN2_SIZE; ++i)
		net->b2[i] = tinynet_rand_uniform(&state);
	for (int i = 0; i < TINYNET_HIDDEN2_SIZE; ++i)
		net->w3[i] = tinynet_rand_uniform(&state);
	net->b3 = tinynet_rand_uniform(&state);
}

float tinynet_predict(const TinyNetC* net, const float* x)
{
	float hidden1[TINYNET_HIDDEN_SIZE];
	float hidden2[TINYNET_HIDDEN2_SIZE];

	// Compute hidden layer activations
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float sum = net->b1[i];
		const float* restrict row = net->w1 + i * TINYNET_INPUT_SIZE;
		const float* restrict xi = x;
		const float* restrict row_end = row + TINYNET_INPUT_SIZE;

		for (; row < row_end; ++row, ++xi)
			sum += (*row) * (*xi);

		hidden1[i] = fmaxf(sum, 0.0f);  // ReLU
	}

	// Second hidden layer
	for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j)
	{
		float sum = net->b2[j];
		const float* restrict row = net->w2 + j * TINYNET_HIDDEN_SIZE;
		const float* restrict h1 = hidden1;
		const float* restrict row_end = row + TINYNET_HIDDEN_SIZE;

		for (; row < row_end; ++row, ++h1)
			sum += (*row) * (*h1);

		hidden2[j] = fmaxf(sum, 0.0f);
	}

	// Compute output layer
	float out = net->b3;
	const float* restrict w3 = net->w3;
	const float* restrict w3_end = w3 + TINYNET_HIDDEN2_SIZE;
	const float* restrict h2 = hidden2;
	for (; w3 < w3_end; ++w3, ++h2)
		out += (*w3) * (*h2);
	
	return out;
}

void tinynet_train_on_sample(TinyNetC* net, const float* x, float target, float lr)
{
	float hidden1[TINYNET_HIDDEN_SIZE];
	float pre1[TINYNET_HIDDEN_SIZE];
	float hidden2[TINYNET_HIDDEN2_SIZE];
	float pre2[TINYNET_HIDDEN2_SIZE];
	float grad_h2[TINYNET_HIDDEN2_SIZE];
	float grad_h1[TINYNET_HIDDEN_SIZE];
	
	// Forward pass for hidden layer
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float sum = net->b1[i];
		const float* restrict row = net->w1 + i * TINYNET_INPUT_SIZE;
		const float* restrict xi = x;
		const float* restrict row_end = row + TINYNET_INPUT_SIZE;

		for (; row < row_end; ++row, ++xi)
			sum += (*row) * (*xi);

		pre1[i] = sum;
		hidden1[i] = fmaxf(sum, 0.0f);
	}

	// Forward pass for second hidden layer
	for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j)
	{
		float sum = net->b2[j];
		const float* restrict row = net->w2 + j * TINYNET_HIDDEN_SIZE;
		const float* restrict h1 = hidden1;
		const float* restrict row_end = row + TINYNET_HIDDEN_SIZE;

		for (; row < row_end; ++row, ++h1)
			sum += (*row) * (*h1);

		pre2[j] = sum;
		hidden2[j] = fmaxf(sum, 0.0f);
	}

	// Output layer
	float out = net->b3;
	const float* restrict w3 = net->w3;
	const float* restrict w3_end = w3 + TINYNET_HIDDEN2_SIZE;
	const float* restrict h2 = hidden2;
	for (; w3 < w3_end; ++w3, ++h2)
		out += (*w3) * (*h2);

	// Compute loss and output gradient
	float loss = out - target;
	float grad_out = loss;

	// Backward pass
	net->b3 -= lr * grad_out;
	for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j)
		grad_h2[j] = (pre2[j] > 0.0f) ? grad_out * net->w3[j] : 0.0f;

	// Gradients for first hidden layer (use current w2 values before update)
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		float sum = 0.0f;
		const float* restrict row = net->w2 + i; // column i across rows
		for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j, row += TINYNET_HIDDEN_SIZE)
			sum += grad_h2[j] * (*row);
		grad_h1[i] = (pre1[i] > 0.0f) ? sum : 0.0f;
	}

	// Update output layer weights
	float* restrict w3w = net->w3;
	const float* restrict h2w = hidden2;
	for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j)
		w3w[j] -= lr * grad_out * h2w[j];

	// Update second hidden layer weights and biases
	for (int j = 0; j < TINYNET_HIDDEN2_SIZE; ++j)
	{
		net->b2[j] -= lr * grad_h2[j];
		float* restrict row = net->w2 + j * TINYNET_HIDDEN_SIZE;
		float coeff = -lr * grad_h2[j];
		const float* restrict h1 = hidden1;
		for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
			row[i] += coeff * h1[i];
	}

	// Update first hidden layer weights and biases
	for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
	{
		net->b1[i] -= lr * grad_h1[i];
		float* restrict row = net->w1 + i * TINYNET_INPUT_SIZE;
		float coeff = -lr * grad_h1[i];
		for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
			row[j] += coeff * x[j];
	}
}
