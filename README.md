# Welcome to the sandpiper SDK

## Installation
For normal operation it is sufficient to clone this repo and place it in any convenient location on one of your linux partitions.

## Usage
Please check out the samples/ directory to learn how to build and how certain hardware components work.

Documentation can be found in the documents/ directory for more in-detail explanation of the hardware and SDK components.

## Using the cross-compiler tool from Petalinux SDK
If you've generated the sdk using sandpiper_petalinux repo (see below for link), you can run the following to source the compiler toolchain:
```
source /opt/petalinux/2025.1/environment-setup-cortexa9t2hf-neon-amd-linux-gnueabi
```
Just make sure you have the correct prefix for the compiler tools when you use them:
```
arm-amd-linux-gnueabi-*

# for example, gcc 13.3.0 would be:
arm-amd-linux-gnueabi-gcc
```

# Related repos

Sandpiper is an interesting machine. It is a linux based small computer based around a Zynq 7020 SoC, with custom video and audio circuitry programmed into the FPGA fabric. A specialzed device driver allows access to a shared memory region and some control registers to control these video and audio devices.

An SDK is provided alongside the PCB for the keyboard module and enclosure files for 3D printing, as well as the build files for Linux kernel and drivers in the following repositories:

https://github.com/ecilasun/sandpiper_hw/

https://github.com/ecilasun/sandpiper_petalinux/

https://github.com/ecilasun/sandpiper_pcb/

https://github.com/ecilasun/sandpiper_sdk/

# Development toolchains

The sandpiper disk image comes prebuilt with gcc13.3.0

For Linux, just run the following two commands (tested on Ubuntu 24.04)
sudo apt-get install gcc-arm-linux-gnueabihf
sudo apt-get install g++-arm-linux-gnueabihf

For Windows, you can use "13.3.Rel1 AArch32 GNU/Linux target with hard float" version:
https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads
Make for Windows can be installed from this link:
https://gnuwin32.sourceforge.net/packages/make.htm

P.S.:
For Linux, the cross compilers can be built with from the sandpiper_petalinux project as well, but this is a lot harder than the above method.
Please check the repo for more information on building the tools locally, in case the options above do not work for you.
