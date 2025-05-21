#pragma once
#include <stdint.h>
#include <stdbool.h>


void duplicate_array_size(const bool src[14][14], bool dst[28][28]);
void boolean_to_grayscale(const bool src[28][28], uint8_t dst[28][28]);
void gaussian_blur_3x3(const uint8_t src[28][28], uint8_t dst[28][28]);