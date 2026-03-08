/**
 * \file copperdemo.c
 * \brief Copper Plasma Demo
 *
 * \ingroup examples
 *
 * Demonstrates per-scanline palette animation driven entirely by the VCP
 * (Video Co-Processor), analogous to the "copper list" on the Amiga.
 *
 * The CPU renders a static plasma-like texture into both framebuffers once
 * at startup using palette indices 0–15.  After that the CPU is completely
 * idle — the VCP rewrites all 16 palette entries on every scanline with
 * smoothly cycling RGB hue values derived from:
 *
 *   colour(scanline, entry) = hue( scanline*3 + frame_phase + entry*16 )
 *
 * Because the palette meaning of each index changes on every scanline the
 * static pixel pattern animates as a full-screen plasma with zero CPU cost
 * after initialisation.
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
 *   R8  – t_base = scanline*3 + frame_phase
 *   R9  – palette entry index  (0 … 15)
 *   RA  – 16           (entry count AND per-entry hue step — same value)
 *   RB  – colour being assembled (R8G8B8 packed)
 *   RC  – t_entry = t_base + entry_index*16  (advanced each inner iteration)
 *   RD  – (unused)
 *   RE  – green channel scratch
 *   RF  – red channel scratch
 *
 * Program byte-address map
 *   0x00 – 0x18  initialisation block (7 instructions, executed once)
 *   0x1C         scanline_wait  (outer loop top)
 *   0x40         palette_loop   (inner loop top, 15 instructions)
 *   0x80         frame_code     (scanline-0 handler)
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
 *   instr 10  branchim(+0x58)  addr 40  →  40+88=128  = instr 32 (frame_code)  ✓
 *   instr 30  branchim(-0x38)  addr 120 → 120-56= 64  = instr 16 (palette_loop) ✓
 *   instr 31  jumpim  (-0x60)  addr 124 → 124-96= 28  = instr  7 (scanline_wait) ✓
 *   instr 33  jumpim  (-0x68)  addr 132 → 132-104=28  = instr  7 (scanline_wait) ✓
 * --------------------------------------------------------------------------- */
static const uint32_t s_vcpprogram[64] = {
    /* --- initialisation (runs once at program start) -------------------- */
    /* 00 */ vcp_ldim(VREG_2, 0xFF),                    /* R2 = 0xFF  (byte mask)          */
    /* 01 */ vcp_ldim(VREG_3, 8),                       /* R3 = 8     (green shift)        */
    /* 02 */ vcp_ldim(VREG_4, 16),                      /* R4 = 16    (red shift / step)   */
    /* 03 */ vcp_ldim(VREG_5, 85),                      /* R5 = 85    (hue offset 1/3×256) */
    /* 04 */ vcp_ldim(VREG_6, 640),                     /* R6 = 640   (end-of-line pixel)  */
    /* 05 */ vcp_ldim(VREG_A, 16),                      /* RA = 16    (entry count & step) */
    /* 06 */ vcp_ldim(VREG_1, 0),                       /* R1 = 0     (initial phase)      */

    /* --- scanline_wait: outer loop top (addr 0x1C = 28) ---------------- */
    /* 07 */ vcp_wpix(VREG_6),                          /* wait for pixel 640 (end of line)   */
    /* 08 */ vcp_scanline_read(VREG_7),                 /* R7 = current scanline              */
    /* 09 */ vcp_cmp(COND_EQ, VREG_7, VREG_ZERO),      /* scanline == 0 ?                    */
    /* 10 */ vcp_branchim(0x58),                        /* branch.EQ → frame_code  (+0x58)    */

    /* --- scanline_setup: compute t_base = scanline*3 + phase ----------- */
    /* 11 */ vcp_radd(VREG_8, VREG_7, VREG_7),         /* R8 = scanline * 2                  */
    /* 12 */ vcp_radd(VREG_8, VREG_8, VREG_7),         /* R8 = scanline * 3                  */
    /* 13 */ vcp_radd(VREG_8, VREG_8, VREG_1),         /* R8 = scanline*3 + phase            */
    /* 14 */ vcp_clr(VREG_9),                           /* R9 = 0  (palette entry index)      */
    /* 15 */ vcp_mv(VREG_C, VREG_8),                    /* RC = t_entry = t_base              */

    /* --- palette_loop: write entries 0..15 (addr 0x40 = 64) ----------- */
    /* 16 */ vcp_rand(VREG_B, VREG_C, VREG_2),         /* RB  = t_entry & 0xFF  (blue ch.)   */
    /* 17 */ vcp_radd(VREG_E, VREG_C, VREG_5),         /* RE  = t_entry + 85                 */
    /* 18 */ vcp_rand(VREG_E, VREG_E, VREG_2),         /* RE  = (t_entry+85) & 0xFF          */
    /* 19 */ vcp_rshl(VREG_E, VREG_E, VREG_3),         /* RE <<= 8  (place in bits 8–15)     */
    /* 20 */ vcp_ror(VREG_B, VREG_B, VREG_E),          /* RB |= green                        */
    /* 21 */ vcp_radd(VREG_F, VREG_C, VREG_5),         /* RF  = t_entry + 85                 */
    /* 22 */ vcp_radd(VREG_F, VREG_F, VREG_5),         /* RF  = t_entry + 170                */
    /* 23 */ vcp_rand(VREG_F, VREG_F, VREG_2),         /* RF  = (t_entry+170) & 0xFF         */
    /* 24 */ vcp_rshl(VREG_F, VREG_F, VREG_4),         /* RF <<= 16 (place in bits 16–23)    */
    /* 25 */ vcp_ror(VREG_B, VREG_B, VREG_F),          /* RB |= red                          */
    /* 26 */ vcp_pwrt(VREG_9, VREG_B),                 /* palette[entry_index] = R8G8B8      */
    /* 27 */ vcp_radd(VREG_C, VREG_C, VREG_A),         /* t_entry += 16  (next hue step)     */
    /* 28 */ vcp_rinc(VREG_9, VREG_9),                 /* entry_index++                      */
    /* 29 */ vcp_cmp(COND_NE, VREG_9, VREG_A),         /* entry_index != 16 ?                */
    /* 30 */ vcp_branchim(-0x38),                       /* branch.NE → palette_loop  (-0x38)  */
    /* 31 */ vcp_jumpim(-0x60),                         /* jmp → scanline_wait       (-0x60)  */

    /* --- frame_code: scanline-0 handler (addr 0x80 = 128) -------------- */
    /* 32 */ vcp_rinc(VREG_1, VREG_1),                 /* phase++                            */
    /* 33 */ vcp_jumpim(-0x68),                         /* jmp → scanline_wait       (-0x68)  */

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

/* ---------------------------------------------------------------------------
 * buildPlasma
 *
 * Fills both framebuffers with a static indexed-colour plasma texture using
 * palette indices 0–15.  Four pixels are packed into each 32-bit word so the
 * write pattern matches the 8-bit indexed mode stride layout.
 *
 * The formula combines four sine-wave terms in X, Y, X+Y, and radial
 * distance to produce a classic plasma pattern.  This runs once at startup;
 * all subsequent animation is handled entirely by the VCP.
 * --------------------------------------------------------------------------- */
static void buildPlasma(uint32_t *dst, uint32_t strideWords, uint32_t width, uint32_t height)
{
    float cx = width  * 0.5f;
    float cy = height * 0.5f;

    for (uint32_t y = 0; y < height; ++y)
    {
        float fy = (float)y;
        for (uint32_t wx = 0; wx < strideWords; ++wx)
        {
            uint32_t word = 0;
            for (uint32_t b = 0; b < 4; ++b)
            {
                float fx = (float)(wx * 4 - b);
                float dx = fx - cx;
                float dy = fy - cy;

                float v  = sinf(fx * 0.05f);
                v       += sinf(fy * 0.07f);
                v       += sinf((fx + fy) * 0.04f);
                v       += sinf(sqrtf(dx*dx + dy*dy) * 0.10f);

                /* v is in [-4, 4]; map to palette index 0–15 */
                uint32_t idx = (uint32_t)((v + 4.0f) * 2.0f) & 0x0F;
                word |= idx << (b * 8);
            }
            dst[y * strideWords + wx] = word;
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
    uint32_t strideWords = s_platform->vx->m_strideInWords;
    buildPlasma((uint32_t *)s_frameBufferA.cpuAddress, strideWords, VIDEO_WIDTH, VIDEO_HEIGHT);
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
