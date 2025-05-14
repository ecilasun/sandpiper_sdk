#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAPUContext* _context, uint32_t audioBufferSize)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_BUFFERSIZE);
	metal_io_write32(_context->m_platform->audioio, 0, audioBufferSize-1);

	_context->m_bufferSize = audioBufferSize;
}

void APUStartDMA(struct EAPUContext* _context, uint32_t audioBufferAddress16byteAligned)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_START);
	metal_io_write32(_context->m_platform->audioio, 0, audioBufferAddress16byteAligned);
}

void APUSetSampleRate(struct EAPUContext* _context, enum EAPUSampleRate sampleRate)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_SETRATE);
	metal_io_write32(_context->m_platform->audioio, 0, (uint32_t)sampleRate);

	_context->m_sampleRate = sampleRate;
}

uint32_t APUFrame(struct EAPUContext* _context)
{
	return metal_io_read32(_context->m_platform->audioio, 0);
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
