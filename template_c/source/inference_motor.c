#include <stdint.h>
#define SCALE 1000

int16_t relu(int16_t x) { return x > 0 ? x : 0; }

void dense(int16_t* out, const int16_t* in, const int16_t* weight, const int16_t* bias, int in_size, int out_size) {
    for (int i = 0; i < out_size; i++) {
        int32_t acc = bias[i];
        for (int j = 0; j < in_size; j++) {
            acc += ((int32_t)in[j] * weight[i * in_size + j]) / SCALE;
        }
        out[i] = (int16_t)acc;
    }
}
