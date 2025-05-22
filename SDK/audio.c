#include "core.h"
#include "audio.h"

void APUSetBufferSize(struct EAudioContext* _context, enum EAPUBufferSize _bufferSize)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_BUFFERSIZE);
	metal_io_write32(_context->m_platform->audioio, 0, (uint32_t)_bufferSize);

	_context->m_bufferSize = 128 << (uint32_t)_bufferSize;
}

void APUStartDMA(struct EAudioContext* _context, uint32_t _audioBufferAddress16byteAligned)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_START);
	metal_io_write32(_context->m_platform->audioio, 0, _audioBufferAddress16byteAligned);
}

void APUSetSampleRate(struct EAudioContext* _context, enum EAPUSampleRate _sampleRate)
{
	_context->m_sampleRate = sampleRate;
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_SETRATE);
	metal_io_write32(_context->m_platform->audioio, 0, (uint32_t)_sampleRate);
}

void APUSwapChannels(struct EAudioContext* _context, uint32_t _swap)
{
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_SWAPCHANNELS);
	metal_io_write32(_context->m_platform->audioio, 0, (uint32_t)_swap);
}

void APUSync(struct EAudioContext* _context)
{
	// Dummy command
	metal_io_write32(_context->m_platform->audioio, 0, APUCMD_NOOP);
}

uint32_t APUFrame(struct EAudioContext* _context)
{
	//rx_addr_offset = SHM_DESC_OFFSET_RX + SHM_DESC_ADDR_ARRAY_OFFSET;
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
	if (_context->m_sampleRate != ASR_Halt)
	{
		// In case the user forgot to stop audio
		APUSetSampleRate(_context, ASR_Halt);
	}
}
