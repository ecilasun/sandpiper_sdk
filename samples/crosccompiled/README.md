# Cross compiler sample

This sample is provided as a starting point for those who wish to use their Linux or Windows+WSL setups to build executables, instead of running gcc 13.3.0 binaries on the device itself.

Unfortunately at this time there's no native Windows build for these tools, as AMD's SDK generator only works on Linux systems.

# Prerequisites

First, you need to ensure you've first ran the buildsdk.sh and then install it with the sdk.sh script on your Linux machine, or on Windows with WSL.

Then you have to use the usecrosscompiler.sh script provided at the root of samples folder.
This will source the compiler binaries and allow you to build executables on your machine, instead of the sandpiper.
