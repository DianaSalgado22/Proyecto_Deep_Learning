#ifndef LAYERS_H
#define LAYERS_H

#include <stdint.h>

void conv2d(const int16_t* input, int in_h, int in_w,
            const int16_t* weight, const int16_t* bias,
            int16_t* output, int out_channels, int kernel_size, int in_channels);

void maxpool2d(const int16_t* input, int in_h, int in_w,
               int channels, int16_t* output);

void linear(const int16_t* input, const int16_t* weights, const int16_t* bias,
            int input_size, int output_size, int16_t* output);

#endif // LAYERS_H