#include "layers.h"
#include "quantized_weights_int16.h"

#define RELU(x) ((x) > 0 ? (x) : 0)
#define CLAMP_INT16(x) ((x) < -32768 ? -32768 : ((x) > 32767 ? 32767 : (x)))

// ===== Convolución capa 1 (shift = 16)
void conv1(const int16_t* input, int16_t* output) {
    int in_h = 28, in_w = 28;
    int out_channels = 8, in_channels = 1, kernel = 3, pad = 1;
    int out_h = in_h;

    for (int oc = 0; oc < out_channels; ++oc) {
        for (int y = 0; y < out_h; ++y) {
            for (int x = 0; x < in_w; ++x) {
                int32_t acc = _Conv1_bias[oc];
                for (int ic = 0; ic < in_channels; ++ic) {
                    for (int ky = 0; ky < kernel; ++ky) {
                        for (int kx = 0; kx < kernel; ++kx) {
                            int iy = y + ky - pad;
                            int ix = x + kx - pad;
                            if (iy >= 0 && iy < in_h && ix >= 0 && ix < in_w) {
                                int idx_input = ic * in_h * in_w + iy * in_w + ix;
                                int idx_weight = oc * in_channels * kernel * kernel +
                                                 ic * kernel * kernel +
                                                 ky * kernel + kx;
                                acc += input[idx_input] * _Conv1_weight[idx_weight];
                            }
                        }
                    }
                }
                output[oc * in_h * in_w + y * in_w + x] = CLAMP_INT16(RELU(acc >> 16));
            }
        }
    }
}

// ===== Convolución capa 2 (shift = 16)
void conv2(const int16_t* input, int16_t* output) {
    int in_h = 14, in_w = 14;
    int out_channels = 16, in_channels = 8, kernel = 3, pad = 1;
    int out_h = in_h;

    for (int oc = 0; oc < out_channels; ++oc) {
        for (int y = 0; y < out_h; ++y) {
            for (int x = 0; x < in_w; ++x) {
                int32_t acc = _Conv2_bias[oc];
                for (int ic = 0; ic < in_channels; ++ic) {
                    for (int ky = 0; ky < kernel; ++ky) {
                        for (int kx = 0; kx < kernel; ++kx) {
                            int iy = y + ky - pad;
                            int ix = x + kx - pad;
                            if (iy >= 0 && iy < in_h && ix >= 0 && ix < in_w) {
                                int idx_input = ic * in_h * in_w + iy * in_w + ix;
                                int idx_weight = oc * in_channels * kernel * kernel +
                                                 ic * kernel * kernel +
                                                 ky * kernel + kx;
                                acc += input[idx_input] * _Conv2_weight[idx_weight];
                            }
                        }
                    }
                }
                output[oc * in_h * in_w + y * in_w + x] = CLAMP_INT16(RELU(acc >> 16));
            }
        }
    }
}

// ===== Max Pooling 2x2 (igual para ambas)
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

// ===== FC1 (shift = 16) con ReLU
void fc1(const int16_t* input, int16_t* output) {
    for (int i = 0; i < 32; ++i) {
        int32_t acc = _Linear1_bias[i];
        for (int j = 0; j < 16 * 7 * 7; ++j) {
            acc += input[j] * _Linear1_weight[i * 16 * 7 * 7 + j];
        }
        output[i] = CLAMP_INT16(RELU(acc >> 16));
    }
}

// ===== FC2 (shift = 17) sin ReLU
void fc2(const int16_t* input, int16_t* output) {
    for (int i = 0; i < 10; ++i) {
        int32_t acc = _Linear2_bias[i];
        for (int j = 0; j < 32; ++j) {
            acc += input[j] * _Linear2_weight[i * 32 + j];
        }
        output[i] = CLAMP_INT16(acc >> 17);
    }
}