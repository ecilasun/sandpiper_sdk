# Welcome to the sandpiper SDK

[![Build SDK and Samples](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/ecilasun/sandpiper_sdk/actions/workflows/build.yml)

## Installation
For normal operation it is sufficient to clone this repo and place it in any convenient location on one of your linux partitions.

## Usage
Please check out the samples/ directory to learn how to build and how certain hardware components work.
Notable sample: samples/vpu_layers_demo shows layer B scanout, mix modes, and keycolor transparency.

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

## Documentation

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for links to all related repositories and SDK documentation.

## Development toolchains

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for development toolchain setup instructions.
