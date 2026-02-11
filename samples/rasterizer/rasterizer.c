#include "rasterizer.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(void)
{
    int screen_width = 64;
    int screen_height = 64;
    
    triangle_t triangle;
    
    /* Define a simple triangle centered in the screen
     * Vertices: (10.5, 10.5), (50.5, 10.5), (30.5, 50.5)
     */
    RPUInitPrimitive(&triangle,
                            10.5f, 10.5f,
                            50.5f, 10.5f,
                            30.5f, 50.5f);
    
    printf("Triangle initialized:\n");
    printf("  Bounding box: (%d, %d) to (%d, %d) in pixels\n",
           FIXED16_INT(triangle.min_x),
           FIXED16_INT(triangle.min_y),
           FIXED16_INT(triangle.max_x),
           FIXED16_INT(triangle.max_y));
    
    printf("  Edge functions:\n");
    for (int i = 0; i < 3; i++) {
        printf("    Edge %d: %d*x + %d*y + %d\n", i,
               triangle.edges[i].a, triangle.edges[i].b, triangle.edges[i].c);
    }
    
    /* Allocate screen-sized block buffer */
    int num_blocks_x = (screen_width + 3) / 4;
    int num_blocks_y = (screen_height + 3) / 4;
    raster_block_t *screen_blocks = (raster_block_t *)malloc(
        num_blocks_x * num_blocks_y * sizeof(raster_block_t));
    
    /* Rasterize entire triangle to screen blocks */
    printf("\nRasterizing triangle to %dx%d screen (%dx%d blocks)...\n",
           screen_width, screen_height, num_blocks_x, num_blocks_y);
    RPURasterize(&triangle, screen_blocks, screen_width, screen_height);
    
    /* Allocate output buffer */
    uint16_t *output = (uint16_t *)malloc(screen_width * screen_height * sizeof(uint16_t));
    
    /* Resolve entire screen */
    printf("Resolving screen...\n");
    RPUResolve(screen_blocks, output, screen_width, screen_height);
    
    /* Display result */
    printf("\nRasterized triangle (screen view):\n");
    for (int y = 0; y < screen_height; y++) {
        printf("  ");
        for (int x = 0; x < screen_width; x++) {
            printf("%c", output[y * screen_width + x] ? 'X' : '.');
        }
        printf("\n");
    }
    
    /* Count total coverage */
    int total_coverage = 0;
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            if (output[y * screen_width + x])
                total_coverage++;
        }
    }
    printf("\nTotal coverage: %d/%d pixels\n", total_coverage, screen_width * screen_height);
    
    /* Cleanup */
    free(screen_blocks);
    free(output);
    
    return 0;
}
