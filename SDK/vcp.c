
#include "platform.h"
#include "vcp.h"

/*
 * Upload a program to the VCP
 * ctx: VCP context
 * program: Pointer to the program data
 * size: One of the EVCPBufferSize enum values indicating the size of the program
 */
void VCPUploadProgram(SPPlatform *ctx, const uint32_t* _program, enum EVCPBufferSize size)
{
	uint32_t bufferSize = 128 << (uint32_t)size;

	// Set aside some space for program uploads
	struct SPSizeAlloc programUploadBuffer;
	programUploadBuffer.size = bufferSize;
	SPAllocateBuffer(ctx, &programUploadBuffer);

	// Copy the program into the upload buffer
	for (uint32_t i = 0; i < (bufferSize / 4); i++)
		((uint32_t*)programUploadBuffer.cpuAddress)[i] = _program[i];

	// Set upload size
	vcpwrite32(ctx, 0, VCPSETBUFFERSIZE);
	vcpwrite32(ctx, 0, bufferSize);

	// Kick the DMA from the upload buffer to the VCP
	vcpwrite32(ctx, 0, VCPSTARTDMA);
	vcpwrite32(ctx, 0, programUploadBuffer.dmaAddress);

	// TODO: We shuould wait for DMA completion here
	// TODO: We should hand back the upload buffer to the platform
}

void VCPExecProgram(SPPlatform *ctx, const uint8_t _enableExecution)
{
	// Start or stop execution
	vcpwrite32(ctx, 0, VCPEXEC | ((_enableExecution&1) << 4));
}
