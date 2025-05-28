#include "layers.h"

#define RELU(x) ((x) > 0 ? (x) : 0)
#define CLAMP_INT16(x) ((x) < -32768 ? -32768 : ((x) > 32767 ? 32767 : (x)))

void conv2d(const int16_t* input, int in_h, int in_w,
            const int16_t* weight, const int16_t* bias,
            int16_t* output, int out_channels, int kernel_size, int in_channels) {
    int out_h = in_h;
    int out_w = in_w;
    int pad = 1;

    for (int oc = 0; oc < out_channels; ++oc) {
        for (int y = 0; y < out_h; ++y) {
            for (int x = 0; x < out_w; ++x) {
                int32_t acc = bias[oc];
                for (int ic = 0; ic < in_channels; ++ic) {
                    for (int ky = 0; ky < kernel_size; ++ky) {
                        for (int kx = 0; kx < kernel_size; ++kx) {
                            int iy = y + ky - pad;
                            int ix = x + kx - pad;
                            if (iy >= 0 && iy < in_h && ix >= 0 && ix < in_w) {
                                int input_idx = ic * in_h * in_w + iy * in_w + ix;
                                int weight_idx = oc * in_channels * kernel_size * kernel_size +
                                                 ic * kernel_size * kernel_size +
                                                 ky * kernel_size + kx;
                                acc += (int32_t)input[input_idx] * weight[weight_idx];
                            }
                        }
                    }
                }
                int out_idx = oc * out_h * out_w + y * out_w + x;
                output[out_idx] = CLAMP_INT16(RELU(acc >> 8));
            }
        }
    }
}

void maxpool2d(const int16_t* input, int in_h, int in_w, int channels, int16_t* output) {
    int out_h = in_h / 2;
    int out_w = in_w / 2;

    for (int c = 0; c < channels; ++c) {
        for (int y = 0; y < out_h; ++y) {
            for (int x = 0; x < out_w; ++x) {
                int16_t max_val = -32768;
                for (int dy = 0; dy < 2; ++dy) {
                    for (int dx = 0; dx < 2; ++dx) {
                        int iy = y * 2 + dy;
                        int ix = x * 2 + dx;
                        int idx = c * in_h * in_w + iy * in_w + ix;
                        if (input[idx] > max_val) max_val = input[idx];
                    }
                }
                output[c * out_h * out_w + y * out_w + x] = max_val;
            }
        }
    }
}

void linear(const int16_t* input, const int16_t* weights, const int16_t* bias,
            int input_size, int output_size, int16_t* output) {
    for (int i = 0; i < output_size; ++i) {
        int32_t acc = bias[i];
        for (int j = 0; j < input_size; ++j) {
            acc += (int32_t)input[j] * weights[i * input_size + j];
        }
        output[i] = CLAMP_INT16(RELU(acc >> 8));
    }
}