#include <stdint.h>
#include <stdbool.h>

#include "gba_ui.h"
#include "digits.h"
#include "img_ops.h"
#include <stdint.h>

#define GBA_SCREEN_W            240
#define GBA_SCREEN_H            160

#define REG_DISPCNT             *((volatile uint16_t *)0x04000000)

#define DISPCNT_BG_MODE_MASK    (0x7)
#define DISPCNT_BG_MODE(n)      ((n) & DISPCNT_BG_MODE_MASK) // 0 to 5

#define DISPCNT_BG2_ENABLE      (1 << 10)

#define MEM_VRAM_MODE3_FB       ((uint16_t *)0x06000000)

#define DMA3COPY(src, dst, count) \
    DMA3_SRC = (uint32_t)(src); \
    DMA3_DST = (uint32_t)(dst); \
    DMA3_CNT = (count) | (1 << 31)

#define DMA3_SRC  (*(volatile uint32_t*)0x40000D4)
#define DMA3_DST  (*(volatile uint32_t*)0x40000D8)
#define DMA3_CNT  (*(volatile uint32_t*)0x40000DC)
#define DMA16     (1<<10)

#define REG_KEY_INPUT *((volatile uint16_t *)0x04000130)

#define KEY_A        (1 << 0)
#define KEY_B        (1 << 1)
#define KEY_SELECT   (1 << 2)
#define KEY_START    (1 << 3)
#define KEY_RIGHT    (1 << 4)
#define KEY_LEFT     (1 << 5)
#define KEY_UP       (1 << 6)
#define KEY_DOWN     (1 << 7)
#define KEY_R        (1 << 8)
#define KEY_L        (1 << 9)

#define INPUT_SIZE 784
#define SCALE 1000

struct Position
{
    uint16_t x;
    uint16_t y;
};


struct Color
{
    uint16_t r;
    uint16_t g;
    uint16_t b;
};


static inline uint16_t RGB15(uint16_t r, uint16_t g, uint16_t b)
{
    return (r & 0x1F) | ((g & 0x1F) << 5) | ((b & 0x1F) << 10);
}


void draw_canvas_cursor(struct Position cursor_position, struct Color color)
{
    uint16_t *pixel = MEM_VRAM_MODE3_FB + cursor_position.x + cursor_position.y * GBA_SCREEN_W;
    // Draw first row
    *pixel = RGB15(color.r, color.g, color.b);
    *(pixel + 1) = RGB15(color.r, color.g, color.b);
    *(pixel + 5) = RGB15(color.r, color.g, color.b);
    *(pixel + 6) = RGB15(color.r, color.g, color.b);
    // second row
    *(pixel + GBA_SCREEN_W) = RGB15(color.r, color.g, color.b);
    *(pixel + GBA_SCREEN_W + 6) = RGB15(color.r, color.g, color.b);
    // sixth row
    *(pixel + GBA_SCREEN_W * 5) = RGB15(color.r, color.g, color.b);
    *(pixel + GBA_SCREEN_W * 5 + 6) = RGB15(color.r, color.g, color.b);
    // seventh row
    *(pixel + GBA_SCREEN_W * 6) = RGB15(color.r, color.g, color.b);
    *(pixel + GBA_SCREEN_W * 6 + 1) = RGB15(color.r, color.g, color.b);
    *(pixel + GBA_SCREEN_W * 6 + 5) = RGB15(color.r, color.g, color.b);
    *(pixel + GBA_SCREEN_W * 6 + 6) = RGB15(color.r, color.g, color.b);
}


void draw_rectangle(struct Position top_left, int length, struct Color color)
{
    for (int i = 0; i < length; i++) {  //columns
        for (int j = 0; j < length; j++) { // rows
            MEM_VRAM_MODE3_FB[(top_left.x + i) + (top_left.y + j) * GBA_SCREEN_W] = RGB15(color.r, color.g, color.b);
        }
    }
}

// Prototipo de la funcion de inferencia
int infer_from_int16_input(const int16_t* input);

/**
 * @brief Makes a prediction based on a 28x28 grayscale image.
 *
 * @details
 * This function is a placeholder and should be completed by the student
 * with the implementation of their neural network.
 *
 * The input parameter `image` is a 2D array of unsigned 8-bit integers
 * representing a 28×28 grayscale image (e.g., a digit from the MNIST dataset).
 *
 * In C, arrays cannot be passed by value, so the parameter `image[28][28]`
 * is actually passed as a pointer to the first element: `uint8_t (*)[28]`.
 * This allows the function to access the image data using normal indexing,
 * e.g., `image[y][x]`.
 *
 * @param image A 28x28 array of grayscale pixel values (0–255).
 * @return The predicted digit (0–9).
 */
int make_prediction(const uint8_t image[28][28])
{
    int16_t input_scaled[INPUT_SIZE];
    
    // Aplanar y escalar imagen de 28x28 a 784x1
    for (int y = 0; y < 28; y++) {
        for (int x = 0; x < 28; x++) {
            input_scaled[y * 28 + x] = (image[y][x] * SCALE) / 255;
        }
    }

    return infer_from_int16_input(input_scaled);
}



void draw_prediction(int prediction, struct Position position)
{
    const uint16_t (*digit)[7];
    switch (prediction)
    {
        case 0: digit = digit_0; break;
        case 1: digit = digit_1; break;
        case 2: digit = digit_2; break;
        case 3: digit = digit_3; break;
        case 4: digit = digit_4; break;
        case 5: digit = digit_5; break;
        case 6: digit = digit_6; break;
        case 7: digit = digit_7; break;
        case 8: digit = digit_8; break;
        case 9: digit = digit_9; break;
        default: return;  // invalid input, do nothing
    }
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 7; x++) {
            uint16_t color = digit[y][x];
            MEM_VRAM_MODE3_FB[(position.y + y) * GBA_SCREEN_W + position.x + x] = color;
        }
    }
}


int main(int argc, char *argv[])
{
    REG_DISPCNT = DISPCNT_BG_MODE(3) | DISPCNT_BG2_ENABLE;

    // Draw the background
    DMA3COPY(ui_bitmap, MEM_VRAM_MODE3_FB, DMA16 | (240 * 160));

    // Draw cursor initial position
    struct Position cursor_position = {78, 34};
    struct Color cursor_color = {31, 30, 19};
    draw_canvas_cursor(cursor_position, cursor_color);

    // Dynamically draw the canvas cursor
    bool is_canvas_cell_active[14][14] = {false};
    int x_grid_position = 0;  // 0 to 13, x position on the 14x14 grid
    int y_grid_position = 0;  // 0 to 13, y position on the 14x14 grid
    struct Color canvas_border_color = {22, 20, 27};
    struct Color cell_active_color = {0, 0, 0};
    struct Color cell_inactive_color = {29, 29, 29};
    struct Position cursor_position_new = {78, 34};
    struct Position digit_position = {121, 150};
    while(1) {
        if (!(REG_KEY_INPUT & KEY_RIGHT) && x_grid_position < 13) {
            x_grid_position++;
            cursor_position_new.x += 6;
            draw_canvas_cursor(cursor_position, canvas_border_color);
            draw_canvas_cursor(cursor_position_new, cursor_color);
            cursor_position = cursor_position_new;
            while (!(REG_KEY_INPUT & KEY_RIGHT));
        }
        if (!(REG_KEY_INPUT & KEY_LEFT) && x_grid_position > 0) {
            x_grid_position--;
            cursor_position_new.x -= 6;
            draw_canvas_cursor(cursor_position, canvas_border_color);
            draw_canvas_cursor(cursor_position_new, cursor_color);
            cursor_position = cursor_position_new;
            while (!(REG_KEY_INPUT & KEY_LEFT));
        }
        if (!(REG_KEY_INPUT & KEY_UP) && y_grid_position > 0) {
            y_grid_position--;
            cursor_position_new.y -= 6;
            draw_canvas_cursor(cursor_position, canvas_border_color);
            draw_canvas_cursor(cursor_position_new, cursor_color);
            cursor_position = cursor_position_new;
            while (!(REG_KEY_INPUT & KEY_UP));
        }
        if (!(REG_KEY_INPUT & KEY_DOWN) && y_grid_position < 13) {
            y_grid_position++;
            cursor_position_new.y += 6;
            draw_canvas_cursor(cursor_position, canvas_border_color);
            draw_canvas_cursor(cursor_position_new, cursor_color);
            cursor_position = cursor_position_new;
            while (!(REG_KEY_INPUT & KEY_DOWN));
        }
        if (!(REG_KEY_INPUT & KEY_A)) {
            struct Position top_left = {cursor_position.x + 1, cursor_position.y + 1};
            if (is_canvas_cell_active[y_grid_position][x_grid_position]) {
                is_canvas_cell_active[y_grid_position][x_grid_position] = false;
                draw_rectangle(top_left, 5, cell_inactive_color);
            } else {
                is_canvas_cell_active[y_grid_position][x_grid_position] = true;
                draw_rectangle(top_left, 5, cell_active_color);
            }
            while (!(REG_KEY_INPUT & KEY_A));
        }
        if (!(REG_KEY_INPUT & KEY_START)) {
            // Convert the 14x14 grid to a 28x28 grid
            bool upsampled[28][28];
            duplicate_array_size(is_canvas_cell_active, upsampled);
            // Convert the boolean array to grayscale
            uint8_t grayscale[28][28];
            boolean_to_grayscale(upsampled, grayscale);
            // Apply Gaussian blur
            uint8_t blurred[28][28];
            gaussian_blur_3x3(grayscale, blurred);
            draw_prediction(make_prediction(blurred), digit_position);
            while (!(REG_KEY_INPUT & KEY_START));
        }
    }
    return 0;
}
