# Remote Debugging HelloWorld

This project is configured for cross-compilation on Windows and remote debugging on an ARM Linux target using VS Code.

Please see the README.md file at the root of this repo for changes required to connect to the emulator using GDB.

## Prerequisites

1.  **Cross-Compiler Toolchain**: Ensure `arm-none-linux-gnueabihf-g++` and `arm-none-linux-gnueabihf-gdb` are installed and available in your system `PATH`.
2.  **Remote Target**:
    -   IP Address: `192.168.1.87` (Default configured).
    -   User: `peta`.
    -   `gdbserver` must be installed on the target machine.
    -   **Configuration**: The connection settings are stored in `.vscode/settings.json`. You can easily change the IP address, username, or paths there.
3.  **SSH Access**:
    -   You must have `ssh` and `scp` clients installed (e.g., Windows OpenSSH).
    -   **Recommended**: Setup SSH key-based authentication for the user `peta` to avoid password prompts during the automated deployment and debugging process.

## How to Debug

1.  Open this folder in VS Code.
2.  Open `helloworld.cpp` and place a breakpoint.
3.  Press **F5** or go to the Run and Debug view and select **"Remote Debug (gdbserver)"**.

P.S.
There may not be sufficient space on the root file system for VS Code to install its remote debugging tools. In that case you may want to do a first install attempt, check the generated vs code tools folder name, and convert it into a symbolic link to a larger partition.

This will be addressed in future OS image releases.