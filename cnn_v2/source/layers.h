#ifndef LAYERS_H
#define LAYERS_H

#include <stdint.h>

// Convolución
void conv1(const int16_t* input, int16_t* output);
void conv2(const int16_t* input, int16_t* output);

// Pooling
void maxpool2d(const int16_t* input, int in_h, int in_w, int channels, int16_t* output);

// Fully Connected
void fc1(const int16_t* input, int16_t* output);
void fc2(const int16_t* input, int16_t* output);  // sin ReLU

#endif // LAYERS_H