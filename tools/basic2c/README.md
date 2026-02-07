# BASIC to C converter (tiny subset)

This folder contains a small BASIC-to-C translator tailored for the sandpiper cross toolchain (`arm-amd-linux-gnueabi-gcc`). It is standalone and does not depend on external runtime code; everything is emitted into the generated C file.

## Supported BASIC subset
- Line-numbered programs
- Numeric variables use `double`
- 1-D arrays via `DIM A(10)`; elements referenced as `A(1)`
- Statements: `PRINT` (multiple items; trailing `;` or `,` suppresses newline; optional `#n,` to write to an open file), `INPUT` (multiple targets, including array elements; optional `#n,` to read from an open file), `LET` (or implicit assignment), `IF ... THEN <line>`, `GOTO`, `GOSUB` / `RETURN`, `FOR ... TO ... [STEP ...]` / `NEXT`, `END`, `REM`, `DIM`, `OPEN`, `CLOSE`
- Expressions: +, -, *, /, parentheses, comparisons (<, >, <=, >=, =, <>), logical `AND` / `OR` / `NOT`

## Limitations
- Floating point math via `double`; `PRINT` ends with a newline unless the last separator is `;` or `,`
- `IF` only supports `THEN <line-number>` targets
- No strings inside expressions (only literal strings in `PRINT` items)
- Arrays are 1-D; `DIM` sizes must be positive numeric literals; array indices are cast to `int` (no bounds checks)
- File I/O: text mode only; `OPEN "file" FOR INPUT|OUTPUT|APPEND AS #n`; channels 0-15; runtime errors if channels are invalid or unopened
- `GOSUB` uses a fixed stack of 1024 entries (per-program constant)

## Examples
- Multi-item PRINT with suppressed newline: `PRINT "X=", X; " Y=", Y`
- Multiple INPUT targets including arrays: `INPUT A, B(1), C( I )`
- File I/O: `OPEN "out.txt" FOR OUTPUT AS #1` then `PRINT #1, "hello"` and `CLOSE #1`

## Usage
On the sandpiper device (where `arm-amd-linux-gnueabi-gcc` is present):

```bash
python3 basic2c.py example.bas --output-c example.c --compile --out-elf example.elf --cflags "-Os"
```

- Omit `--compile` if you only want the generated C file.
- Change `--cc` if your cross compiler lives at a different path.

The script will write `example.c` and, when `--compile` is set, build `example.elf` using the cross compiler.

## Quick demo
The included [example.bas](example.bas) computes the sum of 1..N:

```
10 PRINT "Enter a number"
20 INPUT N
30 LET S = 0
40 FOR I = 1 TO N
50 LET S = S + I
60 NEXT I
70 PRINT "Sum is"
80 PRINT S
90 END
```

Running the command above will produce a C file and an ELF for sandpiper.
