#include "rasterizer.h"
#include <stdio.h>
#include <stdint.h>

/*
 * Example usage of the software rasterizer
 */

int main(void)
{
    /* Initialize a triangle with vertices in floating-point */
    triangle_t triangle;
    
    /* Define a simple triangle centered in the screen
     * Vertices: (10.5, 10.5), (50.5, 10.5), (30.5, 50.5)
     */
    rasterizer_init_triangle(&triangle,
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
    
    /* Example: Rasterize a 4x4 block at position (10, 10) */
    fixed16_t block_x = FLOAT_TO_FIXED16(10.0f);
    fixed16_t block_y = FLOAT_TO_FIXED16(10.0f);
    
    raster_block_t block_result;
    rasterizer_eval_4x4_neon(&triangle, block_x, block_y, &block_result);
    
    printf("\n4x4 block at (10, 10):\n");
    printf("  Linear pixel masks (in order):\n  ");
    for (int i = 0; i < 16; i++) {
        printf("%c ", block_result.mask[i] ? 'X' : '.');
        if ((i + 1) % 4 == 0) printf("\n  ");
    }
    
    /* Unpack to tile format */
    tile_t tile;
    rasterizer_unpack_tile(&block_result, &tile);
    
    printf("\n4x4 tile representation:\n");
    for (int row = 0; row < 4; row++) {
        printf("  ");
        for (int col = 0; col < 4; col++) {
            printf("%c ", tile.tile[row][col] ? 'X' : '.');
        }
        printf("\n");
    }
    printf("  Coverage: %d/16 pixels\n", tile.coverage);
    
    return 0;
}
