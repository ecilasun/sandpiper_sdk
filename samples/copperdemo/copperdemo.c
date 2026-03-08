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
 * 0-15. After that the CPU stops touching the image data.
 *
 * The VCP then updates those 16 palette entries on every scanline. The
 * palette phase advances every frame, and each scanline gets a small extra
 * phase offset, so the colour bands drift smoothly across the scalar field.
 * The visible motion comes from palette animation, not from re-rendering the
 * plasma on the CPU.
 *
 * VCP register assignments
 *   R0  (VREG_ZERO) – hardware zero (always 0)
 *   R1  – frame_phase  (incremented at scanline 0 each frame)
 *   R2  – 0xFF         (byte mask)
 *   R3  – 8            (green channel bit-shift into bits 8–15)
 *   R4  – 16           (red channel bit-shift into bits 16–23, palette step, entry count)
 *   R5  – 85           (1/3 of 256 — hue offset between colour channels)
 *   R6  – 640          (end-of-scanline pixel position)
 *   R7  – current scanline (refreshed each iteration)
 *   R8  – line_phase = frame_phase + (scanline >> 2)
 *   R9  – palette entry index  (0 … 15)
 *   RA  – 16           (entry count AND per-entry hue step — same value)
 *   RB  – colour being assembled (R8G8B8 packed)
 *   RC  – t_entry = line_phase + entry_index*16  (advanced each inner iteration)
 *   RD  – 2            (scanline attenuation shift)
 *   RE  – green channel scratch
 *   RF  – red channel scratch
 *
 * Program byte-address map
 *   0x00 – 0x20  initialisation block (8 instructions, executed once)
 *   0x20         scanline_wait  (outer loop top)
 *   0x44         palette_loop   (inner loop top, 15 instructions)
 *   0x88         frame_code     (scanline-0 handler)
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
 *   instr 11  branchim(+0x5C)  addr 44  →  44+92=136  = instr 34 (frame_code)  ✓
 *   instr 31  branchim(-0x38)  addr 124 → 124-56= 68  = instr 17 (palette_loop) ✓
 *   instr 32  jumpim  (-0x60)  addr 128 → 128-96= 32  = instr  8 (scanline_wait) ✓
 *   instr 37  jumpim  (-0x74)  addr 148 → 148-116=32  = instr  8 (scanline_wait) ✓
 * --------------------------------------------------------------------------- */
static const uint32_t s_vcpprogram[64] = {
    /* --- initialisation (runs once at program start) -------------------- */
    /* 00 */ vcp_ldim(VREG_2, 0xFF),                    /* R2 = 0xFF  (byte mask)              */
    /* 01 */ vcp_ldim(VREG_3, 8),                       /* R3 = 8     (green shift)            */
    /* 02 */ vcp_ldim(VREG_4, 16),                      /* R4 = 16    (red shift / hue step)   */
    /* 03 */ vcp_ldim(VREG_5, 85),                      /* R5 = 85    (120 degree hue offset)  */
    /* 04 */ vcp_ldim(VREG_6, 640),                     /* R6 = 640   (end-of-line pixel)      */
    /* 05 */ vcp_ldim(VREG_A, 16),                      /* RA = 16    (entry count & step)     */
    /* 06 */ vcp_ldim(VREG_D, 2),                       /* RD = 2     (scanline >> 2)          */
    /* 07 */ vcp_ldim(VREG_1, 0),                       /* R1 = 0     (initial phase)          */

    /* --- scanline_wait: outer loop top (addr 0x20 = 32) ---------------- */
    /* 08 */ vcp_wpix(VREG_6),                          /* wait for pixel 640 (end of line)    */
    /* 09 */ vcp_scanline_read(VREG_7),                 /* R7 = current scanline               */
    /* 10 */ vcp_cmp(COND_EQ, VREG_7, VREG_ZERO),      /* scanline == 0 ?                     */
    /* 11 */ vcp_branchim(0x5C),                        /* branch.EQ → frame_code  (+0x5C)     */

    /* --- scanline_setup: line_phase = frame + (scanline >> 2) ---------- */
    /* 12 */ vcp_rshr(VREG_8, VREG_7, VREG_D),         /* R8 = scanline >> 2                  */
    /* 13 */ vcp_radd(VREG_8, VREG_8, VREG_1),         /* R8 = phase + (scanline >> 2)        */
    /* 14 */ vcp_clr(VREG_9),                           /* R9 = 0 (palette entry index)        */
    /* 15 */ vcp_mv(VREG_C, VREG_8),                    /* RC = t_entry = line_phase           */
    /* 16 */ vcp_noop(),

    /* --- palette_loop: write entries 0..15 (addr 0x44 = 68) ----------- */
    /* 17 */ vcp_rand(VREG_B, VREG_C, VREG_2),         /* RB  = t_entry & 0xFF  (blue)        */
    /* 18 */ vcp_radd(VREG_E, VREG_C, VREG_5),         /* RE  = t_entry + 85                  */
    /* 19 */ vcp_rand(VREG_E, VREG_E, VREG_2),         /* RE  = (t_entry+85) & 0xFF           */
    /* 20 */ vcp_rshl(VREG_E, VREG_E, VREG_3),         /* RE <<= 8                            */
    /* 21 */ vcp_ror(VREG_B, VREG_B, VREG_E),          /* RB |= green                         */
    /* 22 */ vcp_radd(VREG_F, VREG_C, VREG_5),         /* RF  = t_entry + 85                  */
    /* 23 */ vcp_radd(VREG_F, VREG_F, VREG_5),         /* RF  = t_entry + 170                 */
    /* 24 */ vcp_rand(VREG_F, VREG_F, VREG_2),         /* RF  = (t_entry+170) & 0xFF          */
    /* 25 */ vcp_rshl(VREG_F, VREG_F, VREG_4),         /* RF <<= 16                           */
    /* 26 */ vcp_ror(VREG_B, VREG_B, VREG_F),          /* RB |= red                           */
    /* 27 */ vcp_pwrt(VREG_9, VREG_B),                 /* palette[entry_index] = R8G8B8       */
    /* 28 */ vcp_radd(VREG_C, VREG_C, VREG_A),         /* t_entry += 16                       */
    /* 29 */ vcp_rinc(VREG_9, VREG_9),                 /* entry_index++                       */
    /* 30 */ vcp_cmp(COND_NE, VREG_9, VREG_A),         /* entry_index != 16 ?                 */
    /* 31 */ vcp_branchim(-0x38),                       /* branch.NE → palette_loop  (-0x38)   */
    /* 32 */ vcp_jumpim(-0x60),                         /* jmp → scanline_wait       (-0x60)   */

    /* --- frame_code: scanline-0 handler (addr 0x88 = 136) -------------- */
    /* 33 */ vcp_noop(),
    /* 34 */ vcp_rinc(VREG_1, VREG_1),                 /* phase += 3                          */
    /* 35 */ vcp_rinc(VREG_1, VREG_1),
    /* 36 */ vcp_rinc(VREG_1, VREG_1),
    /* 37 */ vcp_jumpim(-0x74),                         /* jmp → scanline_wait       (-0x74)   */

    /* --- padding to fill PRG_256Bytes (64 words) ----------------------- */
    /* 38 */ vcp_noop(), /* 39 */ vcp_noop(), /* 40 */ vcp_noop(), /* 41 */ vcp_noop(),
    /* 42 */ vcp_noop(), /* 43 */ vcp_noop(), /* 44 */ vcp_noop(), /* 45 */ vcp_noop(),
    /* 46 */ vcp_noop(), /* 47 */ vcp_noop(), /* 48 */ vcp_noop(), /* 49 */ vcp_noop(),
    /* 50 */ vcp_noop(), /* 51 */ vcp_noop(), /* 52 */ vcp_noop(), /* 53 */ vcp_noop(),
    /* 54 */ vcp_noop(), /* 55 */ vcp_noop(), /* 56 */ vcp_noop(), /* 57 */ vcp_noop(),
    /* 58 */ vcp_noop(), /* 59 */ vcp_noop(), /* 60 */ vcp_noop(), /* 61 */ vcp_noop(),
    /* 62 */ vcp_noop(), /* 63 */ vcp_noop(),
};

/* ---------------------------------------------------------------------------
 * buildPlasma
 *
 * Fills the framebuffer with a static scalar field using palette indices
 * 0-15. The field is a proper plasma: multiple orthogonal and radial waves
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

            uint32_t idx = (uint32_t)((value + 5.0f) * 1.6f);
            if (idx > 15)
                idx = 15;

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
