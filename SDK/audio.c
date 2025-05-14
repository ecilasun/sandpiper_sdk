#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAudioContext* _context, uint32_t audioBufferSize)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_BUFFERSIZE);
	metal_io_write32(_context->m_platform->audioio, 0, audioBufferSize-1);

	_context->m_bufferSize = audioBufferSize;
}

void APUStartDMA(struct EAudioContext* _context, uint32_t audioBufferAddress16byteAligned)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_START);
	metal_io_write32(_context->m_platform->audioio, 0, audioBufferAddress16byteAligned);
}

void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate sampleRate)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_SETRATE);
	metal_io_write32(_context->m_platform->audioio, 0, (uint32_t)sampleRate);

	_context->m_sampleRate = sampleRate;
}

uint32_t APUFrame(struct EAudioContext* _context)
{
	return metal_io_read32(_context->m_platform->audioio, 0);
}

int APUInitAudio(struct EAudioContext* _context, struct SPPlatform* _platform)
{
	_context->m_platform = _platform;
	_context->m_sampleRate = ASR_44_100_Hz;
	_context->m_bufferSize = 0;

	return 0;
}

void APUShutdownAudio(struct EAudioContext* _context)
{
	// TODO:
}
