#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pull in the allocator implementation under test
#include "../platform.c"

// Stub out hardware-facing init routines referenced by platform.c when we compile it directly
void VPUInitVideo(struct EVideoContext* ctx, struct SPPlatform* platform) {(void)ctx; (void)platform;}
int APUInitAudio(struct EAudioContext* ctx, struct SPPlatform* platform) {(void)ctx; (void)platform; return 0;}

static void SetupPlatform(struct SPPlatform* platform)
{
    memset(platform, 0, sizeof(*platform));
    platform->mapped_memory = (uint8_t*)malloc(RESERVED_MEMORY_SIZE);
    assert(platform->mapped_memory);
    memset(platform->mapped_memory, 0xCD, RESERVED_MEMORY_SIZE);
    platform->alloc_cursor = E32AlignUp(0x96000, SP_ALLOC_ALIGNMENT);
    memset(platform->freeLists, 0, sizeof(platform->freeLists));
}

static void TeardownPlatform(struct SPPlatform* platform)
{
    free(platform->mapped_memory);
    platform->mapped_memory = (uint8_t*)MAP_FAILED;
}

static void TestBasicAllocation(void)
{
    struct SPPlatform platform;
    SetupPlatform(&platform);

    struct SPSizeAlloc alloc = { .cpuAddress = NULL, .dmaAddress = NULL, .size = 64 };
    assert(SPAllocateBuffer(&platform, &alloc) == 0);
    assert(alloc.cpuAddress != NULL);
    assert(alloc.dmaAddress != NULL);
    assert(alloc.size == SP_ALLOC_ALIGNMENT);
    uintptr_t offset = (uintptr_t)(alloc.cpuAddress - platform.mapped_memory);
    assert(alloc.dmaAddress == (uint8_t*)RESERVED_MEMORY_ADDRESS + offset);

    TeardownPlatform(&platform);
}

static void TestFreeReuse(void)
{
    struct SPPlatform platform;
    SetupPlatform(&platform);

    struct SPSizeAlloc alloc = { .size = 200 };
    assert(SPAllocateBuffer(&platform, &alloc) == 0);
    uint8_t* firstAddress = alloc.cpuAddress;
    SPFreeBuffer(&platform, &alloc);
    assert(alloc.cpuAddress == NULL);

    struct SPSizeAlloc alloc2 = { .size = 200 };
    assert(SPAllocateBuffer(&platform, &alloc2) == 0);
    assert(alloc2.cpuAddress == firstAddress);
    assert(alloc2.size == SPBlockSizeForOrder(SPOrderForSize(SPAlignSize(200))));

    TeardownPlatform(&platform);
}

static void TestOrderIndependence(void)
{
    struct SPPlatform platform;
    SetupPlatform(&platform);

    struct SPSizeAlloc small = { .size = 100 };
    struct SPSizeAlloc large = { .size = 4096 };
    assert(SPAllocateBuffer(&platform, &small) == 0);
    assert(SPAllocateBuffer(&platform, &large) == 0);

    uint8_t* smallAddr = small.cpuAddress;
    uint8_t* largeAddr = large.cpuAddress;

    SPFreeBuffer(&platform, &small);
    SPFreeBuffer(&platform, &large);

    struct SPSizeAlloc large2 = { .size = 4096 };
    struct SPSizeAlloc small2 = { .size = 100 };
    assert(SPAllocateBuffer(&platform, &large2) == 0);
    assert(SPAllocateBuffer(&platform, &small2) == 0);
    assert(large2.cpuAddress == largeAddr);
    assert(small2.cpuAddress == smallAddr);

    TeardownPlatform(&platform);
}

static void TestOutOfMemory(void)
{
    struct SPPlatform platform;
    SetupPlatform(&platform);

    platform.alloc_cursor = RESERVED_MEMORY_SIZE - (SP_ALLOC_ALIGNMENT / 2);
    struct SPSizeAlloc alloc = { .size = SP_ALLOC_ALIGNMENT };
    assert(SPAllocateBuffer(&platform, &alloc) == -1);
    assert(alloc.cpuAddress == NULL);
    assert(alloc.dmaAddress == NULL);

    TeardownPlatform(&platform);
}

int main(void)
{
    TestBasicAllocation();
    TestFreeReuse();
    TestOrderIndependence();
    TestOutOfMemory();
    puts("sp_allocator_test: all tests passed");
    return 0;
}
