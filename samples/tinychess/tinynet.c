#include <string.h>
#include <math.h>
#include "tinynet.h"

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
