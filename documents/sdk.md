# Sandpiper SDK documentation

## Platform overview

Sandpiper is a custom SoC sytem based around the Zynq7020 FPGA.

Video, audio and other devices and interconnect fabric has been implemented using the programmable logic fabric. On the physical layer (non-programmable) side of the chip, there are two 32bit ARMv7 cores with hard floating point and AXI bus controllers.

The two ARM cores each run at 667MHz and boot up to a command line environment. The current operating system is Petalinux (from Xilinx). Ubuntu 20.04 also runs on the device but was dropped due to complications with future versions not supporting 32 bit processors out of the box.

Sandpiper's hardware can be updated by simply changing the bitstream file on the SD card and does not require complicated flashing procedures or hardware programmers.

There is a special device driver (/dev/sandpiper) which exposes the video and audio hardware via several command FIFOs. The driver will take care of relinquishing control back to the console once the application terminates (either via SIGKILL or other means) It is possible to SSH to the device to stop any process that might not want to terminate gracefully due to any reason.

This SDK contains several samples and the files required to build binaries to take advantage of the custom hardware.

Please see [Platform](platform.md) for more information about the platform API.

## Audio Processing Unit

Please see [Audio](audio.md) for more information about the audio API.

## Video Processing Unit

Please see [Video](video.md) for more information about the video output API.

## Video Coprocessor

Please see [Copper](copper.md) for more information about the video coprocessor API.
