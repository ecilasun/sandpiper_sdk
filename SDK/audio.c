/**
 * @file audio.c
 * 
 * @brief Audio Processing Unit (APU) interface.
 *
 * This file provides functions for interacting with the Audio Processing Unit (APU).
 * It includes functions for allocating buffers, setting buffer size, starting audio DMA, and setting the sample rate.
 */

#include "audio.h"
#include "core.h"
#include <stdlib.h>

void APUSetBufferSize(struct EAPUContext* _context, uint32_t audioBufferSize)
{
	//_context->m_platform->audioio
    *APUIO = APUCMD_BUFFERSIZE;
    *APUIO = audioBufferSize-1;

	_context->m_bufferSize = audioBufferSize;
}

void APUStartDMA(struct EAPUContext* _context, uint32_t audioBufferAddress16byteAligned)
{
	//_context->m_platform->audioio
    *APUIO = APUCMD_START;
    *APUIO = audioBufferAddress16byteAligned;
}

void APUSetSampleRate(struct EAPUContext* _context, enum EAPUSampleRate sampleRate)
{
	//_context->m_platform->audioio
    *APUIO = APUCMD_SETRATE;
    *APUIO = (uint32_t)sampleRate;

	_context->m_sampleRate = sampleRate;
}

uint32_t APUFrame(struct EAPUContext* _context)
{
	//_context->m_platform->audioio
    return *APUIO;
}

int APUInit(struct EAPUContext* _context, struct SPPlatform* _platform)
{
	_context->m_platform = _platform;
	_context->m_sampleRate = ASR_44_100_Hz;
	_context->m_bufferSize = 0;

	return 0;
}

void APUShutdown(struct EAPUContext* _context)
{
	// TODO:
}