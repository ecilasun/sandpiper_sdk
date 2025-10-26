
#include "platform.h"
#include "vcp.h"

/*
 * Upload a program to the VCP
 * ctx: VCP context
 * program: Pointer to the program data
 * size: One of the EVCPBufferSize enum values indicating the size of the program
 */
void VCPUploadProgram(SPPlatform *ctx, const uint32_t _programAddress16byteAligned, enum EVCPBufferSize size)
{
	// Set upload size
	uint32_t bufferSize = 128 << (uint32_t)size;
	vcpwrite32(_context->m_platform, 0, VCPSETBUFFERSIZE);
	vcpwrite32(_context->m_platform, 0, bufferSize);

	// Kick the DMA
	vcpwrite32(_context->m_platform, 0, VCPSTARTDMA);
	vcpwrite32(_context->m_platform, 0, _programAddress16byteAligned);
}

void VCPExecProgram(SPPlatform *ctx, const uint8_t _enableExecution)
{
	// Start or stop execution
	vcpwrite32(_context->m_platform, 0, VCPEXEC | ((_enableExecution&1) << 4));
}
