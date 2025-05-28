#include "img_ops.h"


void duplicate_array_size(const bool src[14][14], bool dst[28][28])
{
    for (int y = 0; y < 14; y++) {
        for (int x = 0; x < 14; x++) {
            dst[y * 2][x * 2] = src[y][x];
            dst[y * 2 + 1][x * 2] = src[y][x];
            dst[y * 2][x * 2 + 1] = src[y][x];
            dst[y * 2 + 1][x * 2 + 1] = src[y][x];
        }
    }
}


void boolean_to_grayscale(const bool src[28][28], uint8_t dst[28][28])
{
    for (int y = 0; y < 28; y++)
        for (int x = 0; x < 28; x++)
            dst[y][x] = src[y][x] ? 255 : 0;
}


void gaussian_blur_3x3(const uint8_t src[28][28], uint8_t dst[28][28])
{
    const uint8_t kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1}
    };
    const uint8_t kernel_sum = 16;
    // uint8_t acc_all[28][28] = {0};
    for (int y = 0; y < 28; y++) {
        for (int x = 0; x < 28; x++) {
            int acc = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int iy = y + ky;
                    int ix = x + kx;
                    if (iy < 0) iy = 0;
                    if (iy > 27) iy = 27;
                    if (ix < 0) ix = 0;
                    if (ix > 27) ix = 27;
                    acc += src[iy][ix] * kernel[ky + 1][kx + 1];
                    // acc_all[y][x] = acc;
                }
            }
            int value = (acc + (kernel_sum / 2)) / kernel_sum;
            dst[y][x] = (value > 255) ? 255 : value;
        }
    }
}