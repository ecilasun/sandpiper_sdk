# Welcome to the sandpiper SDK

## Installation
For normal operation it is sufficient to clone this repo and place it in any convenient location on one of your linux partitions.

## Usage
Please check out the samples/ directory to learn how to build and how certain hardware components work.

Documentation can be found in the documents/ directory for more in-detail explanation of the hardware and SDK components.

# Related repos

Sandpiper is an interesting machine. It is a linux based small computer based around a Zynq 7020 SoC, with custom video and audio circuitry programmed into the FPGA fabric. A specialzed device driver allows access to a shared memory region and some control registers to control these video and audio devices.

An SDK is provided alongside the PCB for the keyboard module and enclosure files for 3D printing, as well as the build files for Linux kernel and drivers in the following repositories:

https://github.com/ecilasun/sandpiper_hw/

https://github.com/ecilasun/sandpiper_petalinux/

https://github.com/ecilasun/sandpiper_pcb/

https://github.com/ecilasun/sandpiper_sdk/
