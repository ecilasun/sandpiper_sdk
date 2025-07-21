#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize)
{
	_context->m_bufferSize = 128 << (uint32_t)_bufferSize;

	audiowrite32(_context->m_platform, APUCMD_BUFFERSIZE);
	audiowrite32(_context->m_platform, _context->m_bufferSize);
}

void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned)
{
	audiowrite32(_context->m_platform, APUCMD_START);
	audiowrite32(_context->m_platform, _audioBufferAddress16byteAligned);
}

void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate)
{
	_context->m_sampleRate = _sampleRate;

	audiowrite32(_context->m_platform, APUCMD_SETRATE);
	audiowrite32(_context->m_platform, (uint32_t)_sampleRate);
}

void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap)
{
	audiowrite32(_context->m_platform, APUCMD_SWAPCHANNELS);
	audiowrite32(_context->m_platform, _swap);
}

void APUSync(struct EAudioContext* _context)
{
	// Dummy command
	audiowrite32(_context->m_platform, APUCMD_NOOP);
}

uint32_t APUFrame(struct EAudioContext* _context)
{
	uint32_t status = audioread32(_context->m_platform);
	return status & 1;
}

uint32_t APUGetWordCount(struct EAudioContext* _context)
{
	uint32_t status = audioread32(_context->m_platform);
	return (status>>1)&0x3FF;
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
	if (_context->m_sampleRate != ASR_Halt)
	{
		// In case the user forgot to stop audio
		APUSetSampleRate(_context, ASR_Halt);
	}
}
