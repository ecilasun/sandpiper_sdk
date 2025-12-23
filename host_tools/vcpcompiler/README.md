# vcpcompiler

A small, portable VCP “C-like” compiler/assembler.

It compiles a `.vcp` source file to a **binary blob** containing:
- instruction words (32-bit)
- a data section appended after code
- zero-padding to the next allowed VCP program size (see `EVCPBufferSize`: 128/256/512/1024/2048/4096 bytes)

## Build

### Windows

From a PowerShell prompt:

```powershell
cd host_tools/vcpcompiler
./build.bat configure
./build.bat build
```

The built binary is copied to `host_tools/vcpcompiler/bin/vcpcompiler.exe`.

Toolchain selection (optional):

```powershell
./build.bat configure --toolchain=msvc
./build.bat configure --toolchain=gcc
./build.bat configure --toolchain=clang
```

### Linux/macOS

```sh
cd host_tools/vcpcompiler
python3 waf configure
python3 waf build
```

The built binary is copied to `host_tools/vcpcompiler/bin/vcpcompiler`.

## Run

```sh
bin/vcpcompiler examples/scroll_palette.vcp -o scroll_palette.bin
```

## Language (minimal)

This is a deliberately small C99-like subset that maps onto the VCP ISA.

### Types
- `u32` (32-bit unsigned)

### Declarations
- `u32 x;` (zero-initialized)
- `u32 x = 123;` (initializer must be a constant 0..0xFFFFFF)

### Statements
- Assignment: `x = expr;`
- `if (a == b) { ... } else { ... }`
- `while (cond) { ... }`
- Labels/goto: `label:`, `goto label;`

### Expressions
- Literals: decimal or hex (`0x1234`)
- Operators (supported by VCP ISA): `+ - & | ^ ~ << >>` and parentheses.

### VCP intrinsics
- `wait_scanline(expr);`
- `wait_pixel(expr);`
- `pal_write(addrExpr, valueExpr);`  (palette address in *entries*; typical use: `pal_write(0, color);`)
- `scanline_read(var);`
- `scanpixel_read(var);`
- `load(addrExpr, var);` / `store(addrExpr, expr);` (address must be 4-byte aligned)

### Stack helpers
- `stack(words);` declares an internal stack of N 32-bit words
- `push(expr);`
- `pop(var);`

## Notes / current limits
- All immediates use VCP `LOADIMM` (24-bit). Values outside `0..0xFFFFFF` are rejected.
- `load/store` addresses are byte offsets in the final program blob; they must be multiples of 4.
- No include support yet (by design).
