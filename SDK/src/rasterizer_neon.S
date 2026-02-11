/*
 * rasterizer_neon.S - ARMv7 NEON optimized rasterizer
 * 4x4 edge evaluation for triangle rasterization
 * 
 * This implementation evaluates 3 edge plane equations across a 4x4 pixel block
 * using NEON for parallel computation.
 * 
 * Register layout:
 *   q0, q1, q2 = DDX values for edges 0,1,2 duplicated [ddx[i], ddx[i], ddx[i], ddx[i]]
 *   q3, q4, q5 = DDY values for edges 0,1,2 duplicated [ddy[i], ddy[i], ddy[i], ddy[i]]
 *   q6, q7, q8 = E values computed for current row
 */

    .syntax unified
    .thumb
    .text
    .align 4

/*
 * void rasterizer_eval_4x4_neon_impl(
 *     int32_t e0, int32_t e1, int32_t e2,        r0-r2: edge values at block origin
 *     int32_t ddx0, int32_t ddx1, int32_t ddx2,  r3, sp[0], sp[4]: edge X derivatives
 *     int32_t ddy0, int32_t ddy1, int32_t ddy2,  sp[8], sp[12], sp[16]: edge Y derivatives
 *     uint16_t *mask_out                         sp[20]: output mask buffer
 * )
 * 
 * Helper function called from C code
 * Uses NEON to compute all 16 pixels in parallel
 */
    .globl rasterizer_eval_4x4_neon_impl
    .type rasterizer_eval_4x4_neon_impl, %function

rasterizer_eval_4x4_neon_impl:
    push {r4, lr}
    vpush {d8-d15}
    
    /* r0 = e0, r1 = e1, r2 = e2 */
    /* r3 = ddx0 */
    /* stack offsets (from sp after push): 
     *   0: ddx1
     *   4: ddx2
     *   8: ddy0
     *  12: ddy1
     *  16: ddy2
     *  20: mask_out pointer
     */
    
    ldr r4, [sp, #72]               /* r4 = ddx1 (offset 0 + 64 for vpush, 8 for push) */
    ldr r5, [sp, #76]               /* r5 = ddx2 */
    ldr r6, [sp, #80]               /* r6 = ddy0 */
    ldr r7, [sp, #84]               /* r7 = ddy1 */
    ldr r8, [sp, #88]               /* r8 = ddy2 */
    ldr r9, [sp, #92]               /* r9 = mask_out */
    
    /* Load and splat X step values into NEON registers */
    vdup.s32 q0, r3                 /* q0 = [ddx0, ddx0, ddx0, ddx0] */
    vdup.s32 q1, r4                 /* q1 = [ddx1, ddx1, ddx1, ddx1] */
    vdup.s32 q2, r5                 /* q2 = [ddx2, ddx2, ddx2, ddx2] */
    
    /* Load and splat Y step values */
    vdup.s32 q3, r6                 /* q3 = [ddy0, ddy0, ddy0, ddy0] */
    vdup.s32 q4, r7                 /* q4 = [ddy1, ddy1, ddy1, ddy1] */
    vdup.s32 q5, r8                 /* q5 = [ddy2, ddy2, ddy2, ddy2] */
    
    /* Create X offset sequence [0, 1, 2, 3] */
    movw r10, #0x0100
    movt r10, #0x0302               /* r10 = 0x03020100 */
    vmov.32 d12[0], r10
    vdup.s32 q7, d12[0]             /* q7 = [0, 1, 2, 3] as 32-bit, or use different approach */
    
    /* Actually simpler: use vector of offsets */
    mov r10, #0
    mov r11, #1
    mov r12, #2
    mov r4, #3
    vmov.s32 d12[0], r10
    vmov.s32 d12[1], r11
    vmov.s32 d13[0], r12
    vmov.s32 d13[1], r4              /* q7 = [0, 1, 2, 3] */
    
    /* Compute X offsets: [0, 1, 2, 3] * [ddx0, ddx0, ddx0, ddx0] = [0, ddx0, 2*ddx0, 3*ddx0] */
    vmul.s32 q8, q7, q0             /* q8 = x_offset for edge 0 */
    vmul.s32 q9, q7, q1             /* q9 = x_offset for edge 1 */
    vmul.s32 q10, q7, q2            /* q10 = x_offset for edge 2 */
    
    /* Process 4 rows */
    mov r10, #0                      /* row counter */
    
.row_loop:
    cmp r10, #4
    bge .row_done
    
    /* Compute Y offset for this row: row * ddy */
    mul r11, r10, r6                /* r11 = row * ddy0 */
    mul r12, r10, r7                /* r12 = row * ddy1 */
    mul r4, r10, r8                 /* r4 = row * ddy2 */
    
    /* Compute edge values for 4 pixels in this row */
    /* e0_row[i] = e0 + x_offset[i] + y_offset */
    vdup.s32 q11, r0                /* q11 = [e0, e0, e0, e0] */
    vadd.s32 q11, q11, q8           /* Add X offsets */
    vdup.s32 q12, r11
    vadd.s32 q11, q11, q12          /* Add Y offset */
    
    /* Same for edges 1 and 2 */
    vdup.s32 q12, r1
    vadd.s32 q12, q12, q9
    vdup.s32 q13, r12
    vadd.s32 q12, q12, q13
    
    vdup.s32 q13, r2
    vadd.s32 q13, q13, q10
    vdup.s32 q14, r4
    vadd.s32 q13, q13, q14
    
    /* Check which pixels are inside all edges: e0 >= 0 && e1 >= 0 && e2 >= 0 */
    vcge.s32 q8, q11, #0            /* q8 = (e0 >= 0) ? 0xFFFFFFFF : 0 */
    vcge.s32 q9, q12, #0            /* q9 = (e1 >= 0) ? 0xFFFFFFFF : 0 */
    vcge.s32 q10, q13, #0           /* q10 = (e2 >= 0) ? 0xFFFFFFFF : 0 */
    
    /* AND all three conditions */
    vand.u32 q8, q8, q9
    vand.u32 q8, q8, q10            /* q8 now contains final mask */
    
    /* Convert 32-bit masks to 16-bit (0xFFFFFFFF -> 0xFFFF, 0 -> 0) */
    vmovn.i32 d16, q8               /* d16 = narrow to 16-bit */
    
    /* Store 4 x uint16_t to output */
    vst1.u16 {d16}, [r9]!
    
    /* Next row */
    add r10, r10, #1
    b .row_loop
    
.row_done:
    vpop {d8-d15}
    pop {r4, pc}

    .size rasterizer_eval_4x4_neon_impl, .-rasterizer_eval_4x4_neon_impl

    .section .note.GNU-stack,"",%progbits
