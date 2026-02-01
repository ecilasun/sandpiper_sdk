# Remote Debugging HelloWorld

This project is configured for cross-compilation on Windows and remote debugging on an ARM Linux target using VS Code.

## Prerequisites

1.  **Cross-Compiler Toolchain**: Ensure `arm-none-linux-gnueabihf-g++` and `arm-none-linux-gnueabihf-gdb` are installed and available in your system `PATH`.
2.  **Remote Target**:
    -   IP Address: `192.168.1.87` (Default configured).
    -   User: `peta`.
    -   `gdbserver` must be installed on the target machine.
3.  **SSH Access**:
    -   You must have `ssh` and `scp` clients installed (e.g., Windows OpenSSH).
    -   **Recommended**: Setup SSH key-based authentication for the user `peta` to avoid password prompts during the automated deployment and debugging process.

## How to Debug

1.  Open this folder in VS Code.
2.  Open `helloworld.cpp` and place a breakpoint.
3.  Press **F5** or go to the Run and Debug view and select **"Remote Debug (gdbserver)"**.

### What happens automatically:

1.  **Build**: The project is compiled with debug symbols (`-g -O0`) using `make DEBUG=1`.
2.  **Deploy**: The resulting `helloworld` binary is copied to `/home/peta/helloworld` on the target via `scp`.
3.  **Start Debug Server**: A background task cleans up any old `gdbserver` instances, sets executable permissions, and starts `gdbserver` on port `2000`.
4.  **Connect**: VS Code's debugger (`arm-none-linux-gnueabihf-gdb`) starts locally and connects to the remote `gdbserver`.

## Configuration Files

-   **`Makefile`**: Supports a `DEBUG` flag.
    -   `make` (default): Optimised release build (`-Ofast -flto`).
    -   `make DEBUG=1`: Debug build (`-O0 -g`).
-   **`.vscode/tasks.json`**:
    -   `build`: Runs the make command.
    -   `deploy`: Copies the binary to the target.
    -   `start remote gdbserver`: Manages the remote process lifecycle.
-   **`.vscode/launch.json`**: Configures the GDB client to connect to the remote target.

## Troubleshooting

-   **Permission Denied**: The `start remote gdbserver` task automatically runs `chmod +x` on the binary.
-   **Address already in use**: The task automatically runs `killall -9 gdbserver` before starting a new session to clear hung ports.
-   **Waiting for tasks...**: If the task hangs on "deploy" or "start remote gdbserver", it is likely waiting for an SSH password. Configure SSH keys to resolve this.
