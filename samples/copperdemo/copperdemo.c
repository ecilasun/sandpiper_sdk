/**
 * \file copperdemo.c
 * \brief Copper Plasma Demo
 *
 * \ingroup examples
 *
 * Demonstrates an indexed-colour plasma where the framebuffer holds a static
 * scalar field and the VCP (Video Co-Processor) supplies the motion by
 * rewriting the palette continuously.
 *
 * The CPU builds a proper plasma field once at startup from several blended
 * sine and radial terms. Each pixel stores only a palette index in the range
 * 0-31. After that the CPU stops touching the image data.
 *
 * The VCP then updates the full plasma palette once per frame during the
 * frame boundary. The palette phase advances every frame, so the colour bands
 * drift smoothly across the scalar field while the underlying index field
 * stays fixed. The visible motion comes from palette animation, not from
 * re-rendering the plasma on the CPU.
 *
 * VCP register assignments
 *   R0  (VREG_ZERO) – hardware zero (always 0)
 *   R1  – frame_phase  (incremented at the frame boundary)
 *   R2  – 0xFF         (byte mask)
 *   R3  – 8            (green channel bit-shift into bits 8–15)
 *   R4  – 16           (red channel bit-shift into bits 16–23)
 *   R5  – 85           (1/3 of 256 — hue offset between colour channels)
 *   R6  – 640          (end-of-scanline pixel position)
 *   R7  – current scanline (refreshed each iteration)
 *   R8  – t_entry = frame_phase + entry_index*8
 *   R9  – palette entry index  (0 … 31)
 *   RA  – 32           (palette entry count)
 *   RB  – last visible scanline (239)
 *   RC  – frame phase increment (5)
 *   RD  – palette hue step (8)
 *   RE  – green channel scratch
 *   RF  – red channel scratch
 *
 * Program byte-address map
 *   0x00 – 0x24  initialisation block (10 instructions, executed once)
 *   0x28         wait_loop      (outer loop top)
 *   0x3C         frame_code     (frame-boundary handler)
 *   0x48         palette_loop   (inner loop top)
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"
#include "vcp.h"

#define VIDEO_MODE      EVM_320_240
#define VIDEO_COLOR     ECM_8bit_Indexed
#define VIDEO_WIDTH     320
#define VIDEO_HEIGHT    240

static struct SPPlatform *s_platform = NULL;
static struct SPSizeAlloc s_frameBufferA;
static struct SPSizeAlloc s_frameBufferB;

/* ---------------------------------------------------------------------------
 * VCP copper plasma program  (PRG_256Bytes = 64 words)
 *
 * Branch / jump offset verification (offsets are PC-relative from the
 * address of the branch/jump instruction itself):
 *
 *   instr 13  branchim(+0x08)  addr 52  →  52+8 =60  = instr 15 (frame_code)    ✓
 *   instr 32  branchim(-0x38)  addr 128 → 128-56=72  = instr 18 (palette_loop)  ✓
 *   instr 33  jumpim  (-0x5C)  addr 132 → 132-92=40  = instr 10 (wait_loop)     ✓
 * --------------------------------------------------------------------------- */
static const uint32_t s_vcpprogram[64] = {
    /* --- initialisation (runs once at program start) -------------------- */
    /* 00 */ vcp_ldim(VREG_2, 0xFF),                    /* R2 = 0xFF  (byte mask)            */
    /* 01 */ vcp_ldim(VREG_3, 8),                       /* R3 = 8     (green shift)          */
    /* 02 */ vcp_ldim(VREG_4, 16),                      /* R4 = 16    (red shift)            */
    /* 03 */ vcp_ldim(VREG_5, 85),                      /* R5 = 85    (120 degree hue split) */
    /* 04 */ vcp_ldim(VREG_6, 640),                     /* R6 = 640   (end-of-line pixel)    */
    /* 05 */ vcp_ldim(VREG_A, 32),                      /* RA = 32    (palette entry count)  */
    /* 06 */ vcp_ldim(VREG_B, 239),                     /* RB = 239   (last visible line)    */
    /* 07 */ vcp_ldim(VREG_C, 5),                       /* RC = 5     (frame phase step)     */
    /* 08 */ vcp_ldim(VREG_D, 8),                       /* RD = 8     (palette hue step)     */
    /* 09 */ vcp_ldim(VREG_1, 0),                       /* R1 = 0     (initial phase)        */

    /* --- wait_loop: spin until the final visible line ------------------ */
    /* 10 */ vcp_wpix(VREG_6),                          /* wait for end-of-line              */
    /* 11 */ vcp_scanline_read(VREG_7),                 /* R7 = current scanline             */
    /* 12 */ vcp_cmp(COND_EQ, VREG_7, VREG_B),         /* scanline == 239 ?                 */
    /* 13 */ vcp_branchim(0x08),                        /* branch.EQ → frame_code  (+0x08)   */
    /* 14 */ vcp_jumpim(-0x10),                         /* jmp → wait_loop         (-0x10)   */

    /* --- frame_code: advance phase and rebuild palette ----------------- */
    /* 15 */ vcp_radd(VREG_1, VREG_1, VREG_C),         /* phase += frame_step               */
    /* 16 */ vcp_clr(VREG_9),                           /* entry_index = 0                   */
    /* 17 */ vcp_mv(VREG_8, VREG_1),                    /* t_entry = phase                   */

    /* --- palette_loop: write entries 0..31 ----------------------------- */
    /* 18 */ vcp_rand(VREG_7, VREG_8, VREG_2),         /* R7  = t_entry & 0xFF  (blue)      */
    /* 19 */ vcp_radd(VREG_E, VREG_8, VREG_5),         /* RE  = t_entry + 85                */
    /* 20 */ vcp_rand(VREG_E, VREG_E, VREG_2),         /* RE  = (t_entry+85) & 0xFF         */
    /* 21 */ vcp_rshl(VREG_E, VREG_E, VREG_3),         /* RE <<= 8                          */
    /* 22 */ vcp_ror(VREG_7, VREG_7, VREG_E),          /* R7 |= green                       */
    /* 23 */ vcp_radd(VREG_F, VREG_8, VREG_5),         /* RF  = t_entry + 85                */
    /* 24 */ vcp_radd(VREG_F, VREG_F, VREG_5),         /* RF  = t_entry + 170               */
    /* 25 */ vcp_rand(VREG_F, VREG_F, VREG_2),         /* RF  = (t_entry+170) & 0xFF        */
    /* 26 */ vcp_rshl(VREG_F, VREG_F, VREG_4),         /* RF <<= 16                         */
    /* 27 */ vcp_ror(VREG_7, VREG_7, VREG_F),          /* R7 |= red                         */
    /* 28 */ vcp_pwrt(VREG_9, VREG_7),                 /* palette[index] = R8G8B8           */
    /* 29 */ vcp_radd(VREG_8, VREG_8, VREG_D),         /* t_entry += palette_step           */
    /* 30 */ vcp_rinc(VREG_9, VREG_9),                 /* entry_index++                     */
    /* 31 */ vcp_cmp(COND_NE, VREG_9, VREG_A),         /* entry_index != 32 ?               */
    /* 32 */ vcp_branchim(-0x38),                       /* branch.NE → palette_loop (-0x38)  */
    /* 33 */ vcp_jumpim(-0x5C),                         /* jmp → wait_loop         (-0x5C)   */

    /* --- padding to fill PRG_256Bytes (64 words) ----------------------- */
    /* 34 */ vcp_noop(), /* 35 */ vcp_noop(), /* 36 */ vcp_noop(), /* 37 */ vcp_noop(),
    /* 38 */ vcp_noop(), /* 39 */ vcp_noop(), /* 40 */ vcp_noop(), /* 41 */ vcp_noop(),
    /* 42 */ vcp_noop(), /* 43 */ vcp_noop(), /* 44 */ vcp_noop(), /* 45 */ vcp_noop(),
    /* 46 */ vcp_noop(), /* 47 */ vcp_noop(), /* 48 */ vcp_noop(), /* 49 */ vcp_noop(),
    /* 50 */ vcp_noop(), /* 51 */ vcp_noop(), /* 52 */ vcp_noop(), /* 53 */ vcp_noop(),
    /* 54 */ vcp_noop(), /* 55 */ vcp_noop(), /* 56 */ vcp_noop(), /* 57 */ vcp_noop(),
    /* 58 */ vcp_noop(), /* 59 */ vcp_noop(), /* 60 */ vcp_noop(), /* 61 */ vcp_noop(),
    /* 62 */ vcp_noop(), /* 63 */ vcp_noop(),
};

static void seedPalette(struct EVideoContext *context, uint8_t phase)
{
    for (uint32_t index = 0; index < 32; ++index)
    {
        uint32_t t = phase + (index * 8u);
        uint32_t blue = t & 0xFFu;
        uint32_t green = (t + 85u) & 0xFFu;
        uint32_t red = (t + 170u) & 0xFFu;

        VPUSetPal(context, (uint8_t)index, red, green, blue);
    }
}

/* ---------------------------------------------------------------------------
 * buildPlasma
 *
 * Fills the framebuffer with a static scalar field using palette indices
 * 0-31. The field is a proper plasma: multiple orthogonal and radial waves
 * are blended together and then quantised to palette entries.
 *
 * This runs once at startup. The image data never changes after that; motion
 * comes from the VCP animating the palette over the index field.
 * --------------------------------------------------------------------------- */
static void buildPlasma(uint8_t *dst, uint32_t strideBytes, uint32_t width, uint32_t height)
{
    float cx0 = width * 0.50f;
    float cy0 = height * 0.50f;
    float cx1 = width * 0.28f;
    float cy1 = height * 0.34f;
    float cx2 = width * 0.72f;
    float cy2 = height * 0.66f;

    for (uint32_t y = 0; y < height; ++y)
    {
        float fy = (float)y;
        uint8_t *row = dst + (y * strideBytes);
        for (uint32_t x = 0; x < width; ++x)
        {
            float fx = (float)x;
            float d0x = fx - cx0;
            float d0y = fy - cy0;
            float d1x = fx - cx1;
            float d1y = fy - cy1;
            float d2x = fx - cx2;
            float d2y = fy - cy2;

            float value  = sinf(fx * 0.047f);
            value       += sinf((fx + fy) * 0.028f);
            value       += sinf(sqrtf(d0x * d0x + d0y * d0y) * 0.090f);
            value       += cosf(sqrtf(d1x * d1x + d1y * d1y) * 0.075f);
            value       += sinf(sqrtf(d2x * d2x + d2y * d2y) * 0.061f);

            if (value < -5.0f)
                value = -5.0f;
            else if (value > 5.0f)
                value = 5.0f;

            uint32_t idx = (uint32_t)((value + 5.0f) * 3.2f);
            if (idx > 31)
                idx = 31;

            row[x] = (uint8_t)idx;
        }
    }
}

int main(int argc, char **argv)
{
    s_platform = SPInitPlatform();

    /* Set up 320×240 8-bit indexed video mode */
    VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

    /* Allocate two framebuffers for double-buffered vsync */
    uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
    s_frameBufferA.size = s_frameBufferB.size = stride * VIDEO_HEIGHT;
    SPAllocateBuffer(s_platform, &s_frameBufferA);
    SPAllocateBuffer(s_platform, &s_frameBufferB);

    /* Set up hardware-assisted page-flip addresses */
    VPUSetScanoutAddress(s_platform->vx,  (uint32_t)s_frameBufferA.dmaAddress);
    VPUSetScanoutAddress2(s_platform->vx, (uint32_t)s_frameBufferB.dmaAddress);
    s_platform->sc->cycle       = 0;
    s_platform->sc->framebufferA = &s_frameBufferA;
    s_platform->sc->framebufferB = &s_frameBufferB;

    /* Build the static plasma texture into both framebuffers */
    printf("Building plasma texture...\n");
    buildPlasma(s_frameBufferA.cpuAddress, stride, VIDEO_WIDTH, VIDEO_HEIGHT);
    memcpy(s_frameBufferB.cpuAddress, s_frameBufferA.cpuAddress, stride * VIDEO_HEIGHT);

    /* Seed the plasma palette before the VCP takes over so the first frame is valid */
    seedPalette(s_platform->vx, 0);

    /* Stop any previously running VCP programs */
    VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);

    /* Upload and launch the copper plasma program */
    printf("Launching copper plasma VCP program...\n");
    VCPUploadProgram(s_platform, s_vcpprogram, PRG_256Bytes);
    VCPExecProgram(s_platform, 0x1);

    printf("Running — VCP drives all animation, CPU is idle.\n");

    /*
     * Main loop: the CPU has nothing to do each frame.
     * VPUSwapPages + VPUSyncSwap maintain correct vsync timing; since both
     * framebuffers hold identical plasma data the visual swap is transparent.
     */
    while (1)
    {
        while (VPUGetFIFONotEmpty(s_platform->vx)) { }
        VPUSwapPages(s_platform->vx, s_platform->sc);
        VPUSyncSwap(s_platform->vx, 0);
        VPUNoop(s_platform->vx);
    }

    return 0;
}
