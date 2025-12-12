
#include <sys/ioctl.h> // For ioctl
#include <stdio.h> // For perror

#include "core.h"
#include "platform.h"
#include "vcp.h"

#define SP_IOCTL_VCP_READ			_IOR('k', 7, void*)
#define SP_IOCTL_VCP_WRITE			_IOW('k', 8, void*)

// Internal function to read a 32-bit value from a VCP register
uint32_t vcpread32(struct SPPlatform* _platform, uint32_t offset)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = offset;
	ioctlstruct.value = 0;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_VCP_READ, &ioctlstruct) < 0)
	{
		perror("can't read from VCP");
		return 0xCDCDCDCD;
	}
	return ioctlstruct.value;
}

// Internal function to write a 32-bit value to a VCP register
void vcpwrite32(struct SPPlatform* _platform, uint32_t offset, uint32_t value)
{
	struct SPIoctl ioctlstruct;
	ioctlstruct.offset = offset;
	ioctlstruct.value = value;
	if (ioctl(_platform->sandpiperfd, SP_IOCTL_VCP_WRITE, &ioctlstruct) < 0)
		perror("can't write to VCP");
}

/*
 * Upload a program to the VCP
 * ctx: Platform context
 * program: Pointer to the program data
 * size: One of the EVCPBufferSize enum values indicating the size of the program
 */
void VCPUploadProgram(struct SPPlatform *ctx, const uint32_t* _program, enum EVCPBufferSize size)
{
	uint32_t bufferSize = 128 << (uint32_t)size;

	// Set aside some space for program uploads
	struct SPSizeAlloc programUploadBuffer;
	programUploadBuffer.size = bufferSize;
	SPAllocateBuffer(ctx, &programUploadBuffer);

	// Copy the program into the upload buffer
	uint32_t* uploadPtr = (uint32_t*)programUploadBuffer.cpuAddress;
	for (uint32_t i = 0; i < (bufferSize / 4); i++)
		uploadPtr[i] = _program[i];

	/*printf("\n");
	for (uint32_t i = 0; i < (bufferSize / 4); i++)
		printf("VCPPROG: [%02X]: %08X\n", i, uploadPtr[i]);*/

	// Set upload size
	vcpwrite32(ctx, 0, VCPSETBUFFERSIZE);
	vcpwrite32(ctx, 0, bufferSize);

	// Kick the DMA from the upload buffer to the VCP
	vcpwrite32(ctx, 0, VCPSTARTDMA);
	vcpwrite32(ctx, 0, (uint32_t)programUploadBuffer.dmaAddress);

	// TODO: We can wait for DMA completion by polling VCP status if needed
}

/*
 * Start or stop VCP program execution
 * ctx: Platform context
 * execFlags: Execution flags where only the lowest bit is used (0 = stop, 1 = start)
 */
void VCPExecProgram(struct SPPlatform *ctx, const uint8_t _execFlags)
{
	// Start or stop execution
	vcpwrite32(ctx, 0, VCPEXEC | (_execFlags << 4));
}

/*
 * Retrieve the current VCP status
 * ctx: Platform context
 * returns: VCP status register value
 */
uint32_t VCPStatus(struct SPPlatform *ctx)
{
	return vcpread32(ctx, 0);
}
