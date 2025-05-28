#include "preprocessing.h"

#define CLAMP_INT16(x) ((x) < -32768 ? -32768 : ((x) > 32767 ? 32767 : (x)))

void preprocess_input(const uint8_t input[28][28], int16_t output[28][28]) {
    for (int i = 0; i < 28; ++i) {
        for (int j = 0; j < 28; ++j) {
            int centered = ((int)input[i][j] - 128) * 32767 / 128;
            output[i][j] = CLAMP_INT16(centered);
        }
    }
}