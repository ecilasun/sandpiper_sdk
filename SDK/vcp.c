
#include "platform.h"
#include "vcp.h"

/*
 * Upload a program to the VCP
 * ctx: VCP context
 * program: Pointer to the program data
 * count: Number of instructions in the program
 */
void VCPUploadProgram(SPPlatform *ctx, const uint32_t *program, size_t count)
{
	for (uint32_t i = 0; i < count; i++)
		vcpwrite32(ctx, i * 4, program[i]);
}
