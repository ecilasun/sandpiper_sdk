/**
 * \file copperdemo.c
 * \brief Copper Plasma Demo
 *
 * \ingroup examples
 *
 * Demonstrates an indexed-colour plasma where the framebuffer holds a static
 * wrapped phase field and the VCP (Video Co-Processor) supplies the motion by
 * rebuilding the plasma palette every frame.
 *
 * The CPU computes a scalar field once at startup and stores palette indices
 * 0-31 in VRAM. Those indices are not a one-shot brightness ramp; they are a
 * wrapped contour field designed for palette cycling. After startup the CPU
 * stops touching the image data.
 *
 * The VCP updates the full plasma palette once per frame during the frame
 * boundary. The phase advances every frame, so the contour bands appear to
 * flow through the field while the underlying index map remains fixed.
 *
 * Tuning choices:
 *   N = 32 palette entries
 *   hue step = 8 across the 0..255 colour wheel
 *   frame phase step = 4 for smooth sub-slot palette motion
 *   contour density k = 6.0 for visible travelling bands without aliasing
 *
 * VCP register assignments
 *   R0  (VREG_ZERO) - hardware zero (always 0)
 *   R1  - frame_phase
 *   R2  - 0xFF byte mask
 *   R3  - 8, green shift into bits 8-15
 *   R4  - 16, red shift into bits 16-23
 *   R5  - 85, 120 degree hue offset between channels
 *   R6  - rearm scanline (1)
 *   R7  - packed colour scratch
 *   R8  - t_entry = frame_phase + entry_index * hue_step
 *   R9  - palette entry index
 *   RA  - palette entry count
 *   RB  - frame sync scanline (0)
 *   RC  - frame phase increment
 *   RD  - palette hue step
 *   RE  - green channel scratch
 *   RF  - red channel scratch
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

#define VIDEO_MODE EVM_320_240
#define VIDEO_COLOR ECM_8bit_Indexed
#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 240
#define VIDEO_FRAME_SYNC_SCANLINE 0u
#define VIDEO_REARM_SCANLINE 1u

#define PLASMA_PALETTE_SIZE 32u
#define PLASMA_HUE_STEP 8u
#define PLASMA_PHASE_STEP 4u
#define PLASMA_CONTOUR_DENSITY 6.0f

static struct SPPlatform *s_platform = NULL;
static struct SPSizeAlloc s_frameBufferA;
static struct SPSizeAlloc s_frameBufferB;

/* ---------------------------------------------------------------------------
 * VCP copper plasma program  (PRG_256Bytes = 64 words)
 *
 * Branch / jump offset verification (offsets are PC-relative from the
 * address of the branch/jump instruction itself):
 *
 *   instr 28  branchim(-0x38)  PC 112 -> 112-56 =56  = instr 14 (palette_loop)
 *   instr 30  jumpim  (-0x50)  PC 120 -> 120-80 =40  = instr 10 (wait_loop)
 * --------------------------------------------------------------------------- */
static const uint32_t s_vcpprogram[64] = {
    /* --- initialisation (runs once at program start) -------------------- */
    /* 00 */ vcp_ldim(VREG_2, 0xFF),
    /* 01 */ vcp_ldim(VREG_3, 8),
    /* 02 */ vcp_ldim(VREG_4, 16),
    /* 03 */ vcp_ldim(VREG_5, 85),
    /* 04 */ vcp_ldim(VREG_6, VIDEO_REARM_SCANLINE),
    /* 05 */ vcp_ldim(VREG_A, PLASMA_PALETTE_SIZE),
    /* 06 */ vcp_ldim(VREG_B, VIDEO_FRAME_SYNC_SCANLINE),
    /* 07 */ vcp_ldim(VREG_C, PLASMA_PHASE_STEP),
    /* 08 */ vcp_ldim(VREG_D, PLASMA_HUE_STEP),
    /* 09 */ vcp_ldim(VREG_1, 0),

    /* --- wait_loop: wait for start-of-frame scanline ------------------- */
    /* 10 */ vcp_wscn(VREG_B),

    /* --- frame_code: advance phase and rebuild palette ----------------- */
    /* 11 */ vcp_radd(VREG_1, VREG_1, VREG_C),
    /* 12 */ vcp_clr(VREG_9),
    /* 13 */ vcp_mv(VREG_8, VREG_1),

    /* --- palette_loop: write entries 0..31 ----------------------------- */
    /* 14 */ vcp_rand(VREG_7, VREG_8, VREG_2),
    /* 15 */ vcp_radd(VREG_E, VREG_8, VREG_5),
    /* 16 */ vcp_rand(VREG_E, VREG_E, VREG_2),
    /* 17 */ vcp_rshl(VREG_E, VREG_E, VREG_3),
    /* 18 */ vcp_ror(VREG_7, VREG_7, VREG_E),
    /* 19 */ vcp_radd(VREG_F, VREG_8, VREG_5),
    /* 20 */ vcp_radd(VREG_F, VREG_F, VREG_5),
    /* 21 */ vcp_rand(VREG_F, VREG_F, VREG_2),
    /* 22 */ vcp_rshl(VREG_F, VREG_F, VREG_4),
    /* 23 */ vcp_ror(VREG_7, VREG_7, VREG_F),
    /* 24 */ vcp_pwrt(VREG_9, VREG_7),
    /* 25 */ vcp_radd(VREG_8, VREG_8, VREG_D),
    /* 26 */ vcp_rinc(VREG_9, VREG_9),
    /* 27 */ vcp_cmp(COND_NE, VREG_9, VREG_A),
    /* 28 */ vcp_branchim(-0x38),               // -14 instructions (palette_loop)

    /* --- rearm_wait: leave scanline 0 before re-arming ----------------- */
    /* 29 */ vcp_wscn(VREG_6),
    /* 30 */ vcp_jumpim(-0x50),                 // -20 instructions (wait_loop)

    /* --- padding to fill PRG_256Bytes (64 words) ----------------------- */
    /* 31 */ vcp_noop(), /* 32 */ vcp_noop(), /* 33 */ vcp_noop(), /* 34 */ vcp_noop(),
    /* 35 */ vcp_noop(), /* 36 */ vcp_noop(), /* 37 */ vcp_noop(), /* 38 */ vcp_noop(),
    /* 39 */ vcp_noop(), /* 40 */ vcp_noop(), /* 41 */ vcp_noop(), /* 42 */ vcp_noop(),
    /* 43 */ vcp_noop(), /* 44 */ vcp_noop(), /* 45 */ vcp_noop(), /* 46 */ vcp_noop(),
    /* 47 */ vcp_noop(), /* 48 */ vcp_noop(), /* 49 */ vcp_noop(), /* 50 */ vcp_noop(),
    /* 51 */ vcp_noop(), /* 52 */ vcp_noop(), /* 53 */ vcp_noop(), /* 54 */ vcp_noop(),
    /* 55 */ vcp_noop(), /* 56 */ vcp_noop(), /* 57 */ vcp_noop(), /* 58 */ vcp_noop(),
    /* 59 */ vcp_noop(), /* 60 */ vcp_noop(), /* 61 */ vcp_noop(), /* 62 */ vcp_noop(),
    /* 63 */ vcp_noop(),
};

static void seedPalette(struct EVideoContext *context, uint8_t phase)
{
    for (uint32_t index = 0; index < PLASMA_PALETTE_SIZE; ++index)
    {
        uint32_t t = phase + (index * PLASMA_HUE_STEP);
        uint32_t blue = t & 0xFFu;
        uint32_t green = (t + 85u) & 0xFFu;
        uint32_t red = (t + 170u) & 0xFFu;

        VPUSetPal(context, (uint8_t)index, red, green, blue);
    }
}

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

            float value = sinf(fx * 0.061f);
            value += sinf(fy * 0.053f);
            value += sinf((fx + fy) * 0.032f);
            value += cosf((fx - fy) * 0.026f);
            value += sinf(sqrtf(d0x * d0x + d0y * d0y) * 0.095f);
            value += cosf(sqrtf(d1x * d1x + d1y * d1y) * 0.071f);
            value += sinf(sqrtf(d2x * d2x + d2y * d2y) * 0.057f);

            int phase = (int)floorf(value * PLASMA_CONTOUR_DENSITY + (fx * 0.045f) + (fy * 0.018f));
            phase %= (int)PLASMA_PALETTE_SIZE;
            if (phase < 0)
                phase += (int)PLASMA_PALETTE_SIZE;

            row[x] = (uint8_t)phase;
        }
    }
}

int main(int argc, char **argv)
{
    s_platform = SPInitPlatform();

    VPUSetVideoMode(s_platform->vx, VIDEO_MODE, VIDEO_COLOR, EVS_Enable);

    uint32_t stride = VPUGetStride(VIDEO_MODE, VIDEO_COLOR);
    s_frameBufferA.size = s_frameBufferB.size = stride * VIDEO_HEIGHT;
    SPAllocateBuffer(s_platform, &s_frameBufferA);
    SPAllocateBuffer(s_platform, &s_frameBufferB);

    VPUSetScanoutAddress(s_platform->vx, (uint32_t)s_frameBufferA.dmaAddress);
    VPUSetScanoutAddress2(s_platform->vx, (uint32_t)s_frameBufferB.dmaAddress);
    s_platform->sc->cycle = 0;
    s_platform->sc->framebufferA = &s_frameBufferA;
    s_platform->sc->framebufferB = &s_frameBufferB;

    printf("Building wrapped plasma phase field...\n");
    buildPlasma(s_frameBufferA.cpuAddress, stride, VIDEO_WIDTH, VIDEO_HEIGHT);
    memcpy(s_frameBufferB.cpuAddress, s_frameBufferA.cpuAddress, stride * VIDEO_HEIGHT);

    seedPalette(s_platform->vx, 0);

    VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);

    printf("Launching copper plasma VCP program...\n");
    VCPUploadProgram(s_platform, s_vcpprogram, PRG_256Bytes);
    VCPExecProgram(s_platform, 0x1);

    printf("Running - VCP drives palette cycling over a static phase field.\n");

    while (1)
    {
        while (VPUGetFIFONotEmpty(s_platform->vx)) { }
        VPUSwapPages(s_platform->vx, s_platform->sc);
        VPUSyncSwap(s_platform->vx, 0);
        VPUNoop(s_platform->vx);
    }

    return 0;
}
