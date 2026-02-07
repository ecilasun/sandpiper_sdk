// Example program demonstrating parallel neural network training
// Compile: gcc -o train_example train_example.c tinynet.c -lpthread -lm

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tinynet.h"

// Initialize network weights randomly
void tinynet_init_random(TinyNetC* net) {
    srand(time(NULL));
    
    // Initialize weights with small random values
    for (int i = 0; i < TINYNET_HIDDEN_SIZE * TINYNET_INPUT_SIZE; ++i)
        net->w1[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
    
    for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
        net->b1[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
    
    for (int i = 0; i < TINYNET_HIDDEN_SIZE; ++i)
        net->w2[i] = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
    
    net->b2 = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
}

// Benchmark: compare serial vs parallel training
void benchmark_training(int num_samples, int num_trials) {
    printf("=== Parallel Training Benchmark ===\n");
    printf("Network: Input=%d, Hidden=%d, Output=1\n", 
           TINYNET_INPUT_SIZE, TINYNET_HIDDEN_SIZE);
    printf("Samples: %d, Trials: %d\n\n", num_samples, num_trials);
    
    // Allocate test data
    float** inputs = (float**)malloc(num_samples * sizeof(float*));
    float* targets = (float*)malloc(num_samples * sizeof(float));
    
    for (int i = 0; i < num_samples; ++i) {
        inputs[i] = (float*)malloc(TINYNET_INPUT_SIZE * sizeof(float));
        // Fill with random data
        for (int j = 0; j < TINYNET_INPUT_SIZE; ++j)
            inputs[i][j] = (float)rand() / RAND_MAX;
        targets[i] = (float)rand() / RAND_MAX;
    }
    
    float learning_rate = 0.01f;
    
    // Benchmark serial version
    printf("Testing SERIAL version...\n");
    TinyNetC net_serial;
    tinynet_init_random(&net_serial);
    
    clock_t start = clock();
    for (int trial = 0; trial < num_trials; ++trial) {
        for (int i = 0; i < num_samples; ++i) {
            tinynet_train_on_sample(&net_serial, inputs[i], targets[i], learning_rate);
        }
    }
    clock_t end = clock();
    double serial_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("  Time: %.3f seconds\n\n", serial_time);
    
    // Benchmark parallel version with 2 threads
    printf("Testing PARALLEL version (2 threads)...\n");
    TinyNetC net_parallel_2;
    tinynet_init_random(&net_parallel_2);
    
    start = clock();
    for (int trial = 0; trial < num_trials; ++trial) {
        for (int i = 0; i < num_samples; ++i) {
            tinynet_train_on_sample_parallel(&net_parallel_2, inputs[i], targets[i], 
                                            learning_rate, 2);
        }
    }
    end = clock();
    double parallel_2_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("  Time: %.3f seconds\n", parallel_2_time);
    printf("  Speedup: %.2fx\n\n", serial_time / parallel_2_time);
    
    // Benchmark parallel version with 4 threads
    printf("Testing PARALLEL version (4 threads)...\n");
    TinyNetC net_parallel_4;
    tinynet_init_random(&net_parallel_4);
    
    start = clock();
    for (int trial = 0; trial < num_trials; ++trial) {
        for (int i = 0; i < num_samples; ++i) {
            tinynet_train_on_sample_parallel(&net_parallel_4, inputs[i], targets[i], 
                                            learning_rate, 4);
        }
    }
    end = clock();
    double parallel_4_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("  Time: %.3f seconds\n", parallel_4_time);
    printf("  Speedup: %.2fx\n\n", serial_time / parallel_4_time);
    
    // Cleanup
    for (int i = 0; i < num_samples; ++i)
        free(inputs[i]);
    free(inputs);
    free(targets);
}

// Example: Simple training
void simple_training_example() {
    printf("=== Simple Training Example ===\n\n");
    
    // Initialize network
    TinyNetC net;
    tinynet_init_random(&net);
    
    // Create a simple 2-sample training set
    float input1[TINYNET_INPUT_SIZE];
    float input2[TINYNET_INPUT_SIZE];
    memset(input1, 0, sizeof(input1));
    memset(input2, 0, sizeof(input2));
    
    // Set a few features to non-zero values
    input1[0] = 1.0f;
    input1[10] = 0.5f;
    input2[5] = 0.8f;
    input2[15] = 0.3f;
    
    float target1 = 0.8f;  // High output
    float target2 = 0.2f;  // Low output
    
    printf("Training network for 10000 epochs...\n");
    
    for (int epoch = 0; epoch < 10000; ++epoch) {
        // Train on sample 1
        tinynet_train_on_sample_parallel(&net, input1, target1, 0.01f, 2);
        
        // Train on sample 2
        tinynet_train_on_sample_parallel(&net, input2, target2, 0.01f, 2);
        
        if ((epoch + 1) % 1000 == 0) {
            float pred1 = tinynet_predict(&net, input1);
            float pred2 = tinynet_predict(&net, input2);
            float loss1 = (pred1 - target1) * (pred1 - target1);
            float loss2 = (pred2 - target2) * (pred2 - target2);
            printf("  Epoch %d: Sample1 pred=%.4f (target=%.1f), "
                   "Sample2 pred=%.4f (target=%.1f), MSE=%.6f\n",
                   epoch + 1, pred1, target1, pred2, target2, 
                   (loss1 + loss2) / 2.0f);
        }
    }
    
    printf("\nFinal predictions:\n");
    float pred1 = tinynet_predict(&net, input1);
    float pred2 = tinynet_predict(&net, input2);
    printf("  Sample 1: predicted=%.4f, target=%.1f\n", pred1, target1);
    printf("  Sample 2: predicted=%.4f, target=%.1f\n", pred2, target2);
}

int main(int argc, char* argv[]) {
    printf("TinyNet Parallel Training Example\n");
    printf("==================================\n\n");
    
    // Run simple example
    simple_training_example();
    printf("\n");
    
    // Run benchmark
    benchmark_training(100, 50);
    
    return 0;
}
