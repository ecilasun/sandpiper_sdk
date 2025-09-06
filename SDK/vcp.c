
#include "platform.h"
#include "vcp.h"

/*
 * Upload a program to the VCP
 * ctx: VCP context
 * program: Pointer to the program data
 * size: Size of the program data in bytes
 */
void VCPUploadProgram(SPPlatform *ctx, const uint32_t *program, size_t size)
{
	for (uint32_t i = 0; i < size / sizeof(uint32_t); i++)
		vcpwrite32(ctx, i * 4, program[i]);
}
