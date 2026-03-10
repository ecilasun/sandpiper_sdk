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
 * This sample also demonstrates memory read / write from VCP to implement a simple mailbox
 *
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

#define HUD_ROW (VIDEO_HEIGHT - 1u)
#define HUD_COLOR_INDEX 255u

static struct SPPlatform *s_platform = NULL;
static struct SPSizeAlloc s_frameBufferA;
static struct SPSizeAlloc s_frameBufferB;
static struct SPSizeAlloc s_mailbox;

/* ---------------------------------------------------------------------------
 * VCP copper plasma program  (PRG_256Bytes = 64 words)
 *
 * Branch / jump offset verification (offsets are PC-relative from the
 * address of the branch/jump instruction itself):
 *
 *   instr 29  branchim(-0x38)  PC 116 -> 116-56 =60  = instr 15 (palette_loop)
 *   instr 33  jumpim  (-0x5C)  PC 132 -> 132-92 =40  = instr 10 (wait_loop)
 * --------------------------------------------------------------------------- */
static uint32_t s_vcpprogram[64] = {
    /* --- mailbox address setup------------------------------------------- */
    /* 00 */ vcp_ldim(VREG_B, 0x0),             // High 8 bits of mailbox - patched in by main() with the actual address
    /* 01 */ vcp_ldim(VREG_C, 0x0),             // Low 24 bits of mailbox - patched in by main() with the actual address
    /* 02 */ vcp_ldim(VREG_3, 8),               // Generic shift amount
    /* 03 */ vcp_rshl(VREG_B, VREG_B, VREG_3),  // Shift high bits into position
    /* 04 */ vcp_ror(VREG_C, VREG_B, VREG_C),   // Combine high and low bits into full 32-bit address

    /* --- initialisation (runs once at program start) -------------------- */
    /* 05 */ vcp_ldim(VREG_2, 0xFF),
    /* 06 */ vcp_ldim(VREG_4, 16),
    /* 07 */ vcp_ldim(VREG_5, 85),
    /* 08 */ vcp_ldim(VREG_A, PLASMA_PALETTE_SIZE),
    /* 09 */ vcp_ldim(VREG_1, 0),

    /* --- wait_loop: wait for start-of-frame scanline ------------------- */
    /* 10 */ vcp_wscn(VREG_ZERO),

    /* --- frame_code: advance phase and rebuild palette ----------------- */
    /* 11 */ vcp_ldim(VREG_6, PLASMA_PHASE_STEP),
    /* 12 */ vcp_radd(VREG_1, VREG_1, VREG_6),
    /* 13 */ vcp_clr(VREG_9),
    /* 14 */ vcp_mv(VREG_8, VREG_1),

    /* --- palette_loop: write entries 0..31 ----------------------------- */
    /* 15 */ vcp_rand(VREG_7, VREG_8, VREG_2),
    /* 16 */ vcp_radd(VREG_6, VREG_8, VREG_5),
    /* 17 */ vcp_rand(VREG_6, VREG_6, VREG_2),
    /* 18 */ vcp_rshl(VREG_6, VREG_6, VREG_3),
    /* 19 */ vcp_ror(VREG_7, VREG_7, VREG_6),
    /* 20 */ vcp_radd(VREG_6, VREG_8, VREG_5),
    /* 21 */ vcp_radd(VREG_6, VREG_6, VREG_5),
    /* 22 */ vcp_rand(VREG_6, VREG_6, VREG_2),
    /* 23 */ vcp_rshl(VREG_6, VREG_6, VREG_4),
    /* 24 */ vcp_ror(VREG_7, VREG_7, VREG_6),
    /* 25 */ vcp_pwrt(VREG_9, VREG_7),
    /* 26 */ vcp_radd(VREG_8, VREG_8, VREG_3),
    /* 27 */ vcp_rinc(VREG_9, VREG_9),
    /* 28 */ vcp_cmp(COND_NE, VREG_9, VREG_A),
    /* 29 */ vcp_branchim(-0x38),

    /* --- mailbox writeback: publish current phase for CPU HUD ---------- */
    /* 30 */ vcp_sysmemwrite(VREG_C, VREG_1),

    /* --- rearm_wait: leave scanline 0 before re-arming ----------------- */
    /* 31 */ vcp_ldim(VREG_6, VIDEO_REARM_SCANLINE),
    /* 32 */ vcp_wscn(VREG_6),
    /* 33 */ vcp_jumpim(-0x5C),

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

static void drawHudPixel(uint8_t *framebuffer, uint32_t strideBytes, uint32_t x)
{
    uint8_t *row = framebuffer + (HUD_ROW * strideBytes);

    memset(row, 0, VIDEO_WIDTH);
    row[x % 255u] = HUD_COLOR_INDEX;
}

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

    // Allocate mailbox
    s_mailbox.size = 4; // We only need to send one 32-bit data, so 4 bytes is sufficient
    SPAllocateBuffer(s_platform, &s_mailbox);
    *(uint32_t *)s_mailbox.cpuAddress = 0;

    // Patch program with mailbox address
    s_vcpprogram[0] = vcp_ldim(VREG_B, (((uint32_t)s_mailbox.dmaAddress) >> 8) & 0x00FF0000u);
    s_vcpprogram[1] = vcp_ldim(VREG_C, ((uint32_t)s_mailbox.dmaAddress) & 0x00FFFFFFu);

    printf("Building wrapped plasma phase field...\n");
    buildPlasma(s_frameBufferA.cpuAddress, stride, VIDEO_WIDTH, VIDEO_HEIGHT);
    memcpy(s_frameBufferB.cpuAddress, s_frameBufferA.cpuAddress, stride * VIDEO_HEIGHT);

    seedPalette(s_platform->vx, 0);
    VPUSetPal(s_platform->vx, HUD_COLOR_INDEX, 255u, 255u, 255u);

    VPUWriteControlRegister(s_platform->vx, 0x0F, 0x00);

    printf("Launching copper plasma VCP program...\n");
    VCPUploadProgram(s_platform, s_vcpprogram, PRG_256Bytes);
    VCPExecProgram(s_platform, 0x1);

    printf("Running - VCP drives palette cycling over a static phase field.\n");

    while (1)
    {
        while (VPUGetFIFONotEmpty(s_platform->vx)) { }
        VPUSwapPages(s_platform->vx, s_platform->sc);

        // Scroll a single pixel at the position written by the VCP
        uint32_t hudPhase = *(volatile uint32_t *)s_mailbox.cpuAddress;
        drawHudPixel(s_platform->sc->writepage, stride, hudPhase);

        VPUSyncSwap(s_platform->vx, 0);
        VPUNoop(s_platform->vx);
    }

    return 0;
}
