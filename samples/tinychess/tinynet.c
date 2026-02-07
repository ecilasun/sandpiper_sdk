#include <string.h>
#include <math.h>

#define kInputSize (64 * 12 + 1)
#define kHiddenSize 32

typedef struct {
	float w1[kHiddenSize * kInputSize];
	float b1[kHiddenSize];
	float w2[kHiddenSize];
	float b2;
} TinyNetC;

float tinynet_predict(const TinyNetC* net, const float* x)
{
	float hidden[kHiddenSize];

    // Compute hidden layer activations
	for (int i = 0; i < kHiddenSize; ++i)
	{
		float sum = net->b1[i];
		const float* row = net->w1 + i * kInputSize;
		
		for (int j = 0; j < kInputSize; ++j)
			sum += row[j] * x[j];
		
		hidden[i] = sum > 0.0f ? sum : 0.0f;  // ReLU
	}

    // Compute output layer
	float out = net->b2;
	for (int i = 0; i < kHiddenSize; ++i)
		out += net->w2[i] * hidden[i];
	
	return out;
}

void tinynet_train_on_sample(TinyNetC* net, const float* x, float target, float lr)
{
	float hidden[kHiddenSize];
	float pre[kHiddenSize];
	
	// Forward pass for hidden layer
	for (int i = 0; i < kHiddenSize; ++i)
	{
		float sum = net->b1[i];
		const float* row = net->w1 + i * kInputSize;
		
		for (int j = 0; j < kInputSize; ++j)
			sum += row[j] * x[j];
		
		pre[i] = sum;
		hidden[i] = sum > 0.0f ? sum : 0.0f;
	}

	// Output layer
	float out = net->b2;
	for (int i = 0; i < kHiddenSize; ++i)
		out += net->w2[i] * hidden[i];

    // Compute loss and output gradient
	float loss = out - target;
	float grad_out = loss;

	// Backward pass
	net->b2 -= lr * grad_out;
	for (int i = 0; i < kHiddenSize; ++i)
	{
		float w2_old = net->w2[i];
		net->w2[i] -= lr * grad_out * hidden[i];

        // Gradient for hidden layer
		float grad_hidden = grad_out * w2_old;
		if (pre[i] <= 0.0f)
			grad_hidden = 0.0f;
		net->b1[i] -= lr * grad_hidden;

        // Update weights for hidden layer
		float* row = net->w1 + i * kInputSize;
		float coeff = -lr * grad_hidden;
		for (int j = 0; j < kInputSize; ++j)
			row[j] += coeff * x[j];
	}
}
