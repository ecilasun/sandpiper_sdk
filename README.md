# Welcome to the sandpiper SDK

[![Build SDK](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build-sdk.yml/badge.svg?branch=main)](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build-sdk.yml)

[![Build Samples](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build-samples.yml/badge.svg?branch=main)](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build-samples.yml)

## Installation
For normal operation it is sufficient to clone this repo and place it in any convenient location on one of your linux partitions.

## Usage
Please check out the samples/ directory to learn how to build and how certain hardware components work.

Documentation can be found in the documents/ directory for more in-detail explanation of the hardware and SDK components.

## Copying files to the emulator
First, install winscp, then use the following command line to copy binaries to current remote directory.
In this example peta is your username, and mandelbrot is the binary we're copying. Type your password when prompted:
```
winscp  peta@localhost:2222 .\mandelbrot
```

After copying binaries you will need to set them to executable mode on the emulator.
In this example we're making mandelbrot sample executable by running the following on the emulator terminal:
```
sudo chmod +x mandelbrot
```

## Related repos

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for related repositories and documentation.

## Development toolchains

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for development toolchain setup instructions.
