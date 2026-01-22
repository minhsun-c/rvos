#include "nyancat-frame.h"
#include "defs.h"
#include "types.h"
#include "vga.h"

void delay(uint32_t count)
{
    for (volatile uint32_t i = 0; i < count; i++)
        __asm__("nop");
}

void nyan_task(void *param)
{
    volatile uint8_t *fb = (volatile uint8_t *) VGA_FB_BASE;
    const int screen_width = 320;

    // Scaling and Centering Settings
    const int scale = 2;      // 2x size
    const int offset_x = 96;  // (320 - (64*2)) / 2
    const int offset_y = 36;  // (200 - (64*2)) / 2

    while (1) {
        for (int frame = 0; frame < 12; frame++) {
            for (int y = 0; y < 64; y++) {
                for (int x_word = 0; x_word < 8; x_word++) {
                    uint32_t word = nyancat_frames[frame][y * 8 + x_word];

                    for (int p = 0; p < 8; p++) {
                        uint8_t color_idx = (word >> (p * 4)) & 0x0F;
                        int sprite_x = (x_word * 8) + p;

                        // Scale the pixel by drawing a block (scale x scale)
                        for (int sy = 0; sy < scale; sy++) {
                            for (int sx = 0; sx < scale; sx++) {
                                int out_x = offset_x + (sprite_x * scale) + sx;
                                int out_y = offset_y + (y * scale) + sy;

                                // Boundary check to prevent kernel panic/memory
                                // corruption
                                if (out_x < 320 && out_y < 200) {
                                    fb[out_y * screen_width + out_x] =
                                        color_idx;
                                }
                            }
                        }
                    }
                }
            }

            __asm__ volatile("fence w, o" : : : "memory");

            // Slow down the animation
            delay(20000000);
        }
    }
}

void vga_test(void)
{
    task_t *task0;
    task0 = task_init("task0", nyan_task, NULL, 4096, 10);
    task_startup(task0);
}