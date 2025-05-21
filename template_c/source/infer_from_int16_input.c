#include "model_weights.h"
#include <stdint.h>



int infer_from_int16_input(const int16_t* input_image) {
    int16_t layer1[128];
    int16_t layer2[64];
    int16_t output[10];

    dense(layer1, input_image, fc1_weight, fc1_bias, 784, 128);
    for (int i = 0; i < 128; i++) layer1[i] = relu(layer1[i]);

    dense(layer2, layer1, fc2_weight, fc2_bias, 128, 64);
    for (int i = 0; i < 64; i++) layer2[i] = relu(layer2[i]);

    dense(output, layer2, fc3_weight, fc3_bias, 64, 10);

    int max_index = 0;
    for (int i = 1; i < 10; i++) {
        if (output[i] > output[max_index]) max_index = i;
    }

    return max_index;
}
