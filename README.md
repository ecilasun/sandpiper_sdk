# Welcome to the sandpiper SDK

## Installation
For normal operation it is sufficient to clone this repo and place it in any convenient location.

The linux setup on sandpiper already contains the gcc compilers, git, and nano editor therefore getting started should be as easy as compiling one of the samples provided and running it.

## Memory and the CPU
The current hardware utilizes a Zynq 7020 system on module and 512Mbytes of DDR memory.

The Zynq CPU is clocked at 666MHz and contains two 32bit ARM cores (arm7a), which are accompanied by a few custom hardware on the programmable logic (PL) side.

Some of the custom hardware includes video and audio modules, which can access a 32Mbyte reserved memory region out of the 512Mbyte DDR chip. This region is uncached and Linux can't directly use it for allocations, therefore it is for the sole use of graphics and audio subsystems.

This SDK provides allocation functions which will automatically manage the CPU/PL address pairs and the sub-allocation from the 32Mbyte pool.

The rest of the memory (~480Mbytes) is shared between Linux and user applications. Caching is enabled for this memory region, and all allocations can go through regular memory allocation libraries or OS calls as expected under Linux.

## Using shared memory between devices

To access the 32Mbyte shared memory region, we need to initialize the platform library first:

```
struct SPPlatform platform;
SPInitPlatform(&platform);
```

This ensures the 32Mbyte region is mapped and ready for uncached access for use by the CPU and PL devices.

Now we can call our allocation function:
```
struct SPSizeAlloc mybuffer;
mybuffer.size = 16384;
SPAllocateBuffer(&platform, &mybuffer);
```

With the above call, the buffer size is rounded upwards to the nearest multiple of 128 to make sure it's aligned properly for device DMA, and sets the CPU and PL addresses in the structure.

After this we can write to the buffer from the CPU and read from the PL:

```
uint8_t *mybytes = (uint8_t*)mybuffer.cpuAddress;
// TODO: Write some values to mybytes[]
// At this point hardware can use mybuffer.dmaAddress
// to read these values, for example to play audio.
```

When we're done with the buffer, we need to yield memory back to the system and shut down the platform:

```
SPFreeBuffer(&platform, &mybuffer);
SPShutdownPlatform(&platform);
```

## Installing atexit/SIGINT handlers for graceful recovery
