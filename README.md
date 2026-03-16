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

## Debugging with GDB

To make this work in Windows, try the following:

Install a TAP driver (One from OpenVPN is known to work https://swupdate.openvpn.net/community/releases/tap-windows-9.21.2.exe)
Bridge the TAP network device with your host PC network device.
Renamed it to EthernetTAP so it is clear to make it easy to find.

Change the boot_emulator.bat to use the tap device instead of the default slirp and port forwards for network. Replace the -net arguments in there with this (replace EthernetTAP with whatever you named your TAP device above):
```
-net nic,netdev=mynet0 -netdev tap,id=mynet0,ifname=EthernetTAP
```

Find the IP address of the sandpiper instance in qemu using  ifconfig command from the emulator.
Put the IP address in the settings.json in the helloworld sample
A bonus is that using the tap device is much faster than using slirp!

(Thanks to Sam Izzo for the instructions)

## Documentation

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for links to all related repositories and SDK documentation.

## Development toolchains

See the [Sandpiper project page](https://ecilasun.github.io/sandpiper/) for development toolchain setup instructions.
