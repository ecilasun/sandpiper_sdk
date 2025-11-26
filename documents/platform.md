[Back](sdk.md)
---
# Platform Utilities

## API Documentation

### Starting / stopping the sandpiper device support

---
<span style="color:#00F0D0;">struct SPPlatform* SPInitPlatform();</span>

Opens hardware devices and set up necessary internals so that we can start using the custom video and audio circuitry.

---
<span style="color:#00F0D0;">void SPShutdownPlatform(struct SPPlatform* _platform);</span>

Relinquishes control back to the caller by releasing memory. However it will not close the device connections, which the platfomr driver at /dev/sandpiper handles. This is done so that there is always a visible terminal window and silent audio when an application crashes or exists due to unknown reasons.

### Shared memory allocation / deallocation

---
<span style="color:#00F0D0;">int SPAllocateBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);</span>

Allocates a portion of physical memory with proper DMA alignment, and populates the SPSizeAlloc structure with matching phsical / CPU address pairs for use with devices and the ARM cores.

---
<span style="color:#00F0D0;">void SPFreeBuffer(struct SPPlatform* _platform, struct SPSizeAlloc *_sizealloc);</span>

Hands back the previously allocated buffer.