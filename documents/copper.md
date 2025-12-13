[Back](sdk.md)
---
# Video Coprocessor (VCP)

## VCP overview

## API Documentation

---
<span style="color:#00F0D0;">void VCPUploadProgram(struct SPPlatform *ctx, const uint32_t* _program, enum EVCPBufferSize size);</span><br>
Uploads VCP program at address _program to the VCP. Programs have to be in exact multiples of bytes defined by the ECVPBufferSize constants, therefore paddings of NOOP instructions might be needed where necessary. The program is first copied to a shared memory location so it can be DMAd to the VCP device's internal program memory, then a DMA operation is queued up. If you need to wait for this operation to finish (it is in fact incredibly fast), you can check the status bits using the following VCPStatus() instruction.

---
<span style="color:#00F0D0;">void VCPExecProgram(struct SPPlatform *ctx, const uint8_t _execFlags);</span><br>
Toggles the execute bit for VCP on and off. Currently there's only one VCP unit, so only the lowest bit is in effect. You can set this to zero any time during program execution, which will stop the VCP after the current instruction has fully executed.

---
<span style="color:#00F0D0;">uint32_t VCPStatus(struct SPPlatform *ctx);</span><br>
Reads the status register of the VPC. The bit pattern returned contains the following information:<br>
```
0000 OOOO 0CFP PPPP PPPP PPPP RRRR EEEE
E = execstate (execution state machine state, 4 bits)
R = runstate (high for running, low for stopped, 4 bits)
P = address of current instruction (PC, 13 bits)
F = high when command FIFO is not empty (to wait for DMA; add a spare 'noop' command after a DMA command, and wait for empty fifo, 1 bit)
C = program DMA in flight (1 bit)
O = opcode at program address (4 bits)
```

## VCP instruction list

### Wait instructions

<span style="color:#00F0D0;">wscn(src)</span><br>
Waits for scanline that matches the contents of register src. If the value is outside the range 0..524, this wait will cause an infinite loop and the VCP will not proceed to execute other instructions.

<span style="color:#00F0D0;">wpix(src)</span><br>
Waits for pixel that matches the contents of register src. If the value is outside the range 0..799, this wait will cause an infinite loop and the VCP will not proceed to execute other instructions.

### Color palette access

<span style="color:#00F0D0;">pwrt(addrs, src)</span><br>
Writes value of register src to palette entry at addrs where valid addrs range is from 0 to 255, in increments of 1 (this is unlike program memory addressing where a 'byte' address is in fact a palette index instead)

### Arithmetic instructions

<span style="color:#00F0D0;">radd(dest, src1, src2)</span><br>
Adds contents of register src2 to contents of register src1 and writes the result into register dest. No overflow flag is set, overflow bits are simply discarded.

<span style="color:#00F0D0;">rsub(dest, src1, src2)</span><br>
Subtracts contents of register src2 from contents of register src1 and writes the result into register dest

<span style="color:#00F0D0;">rmul(dest, src1, src2)</span><br>
INSTRUCTION NOT IMPLEMENTED

<span style="color:#00F0D0;">rdiv(dest, src1, src2)</span><br>
INSTRUCTION NOT IMPLEMENTED

### Branch instructions

<span style="color:#00F0D0;">jump(addrs)</span><br>
Direct branch to program memory address pointed by contents of register adrs. Program memory addresses have to be 4-byte aligned, otherwise the behavior is undefined.

<span style="color:#00F0D0;">branch(addrs, src)</span><br>
Takes a branch to program memory address pointed by contents of register addrs, if the lowest bit of the value in register src is nonzero. Program memory addresses have to be 4-byte aligned, otherwise the behavior is undefined.

### Program memory access

<span style="color:#00F0D0;">store(addrs, src)</span><br>
Store contents of register src at program memory address pointed by contents of register addrs. Memoy addresses have to be 4-byte aligned, otherwise the behavior is undefined.

<span style="color:#00F0D0;">load(addrs, dest)</span><br>
Load contents of program memory address pointed by contents of register addrs and write it into register dest. Memory addresses have to be 4-byte aligned, otherwise the behavior is undefined.

### Internal register access

<span style="color:#00F0D0;">scanline_read(dest)</span><br>
Read value of current scanline the instruction is currently at, and write result into register dest. The current scanline can be off-screen, and values range from 0 to 524

<span style="color:#00F0D0;">scanpixel_read(dest)</span><br>
Read value of current pixel (X coordinate) the instruction is currently at, and write result into register dest. The current pixel can be off-screen, and values range from 0 to 799

### Logic instructions

<span style="color:#00F0D0;">cmp(cmpflags, dest, src1, src2)</span><br>
Compares register src1 with register src2 given compare flags and writes result into register dest.<br>
Valid cmpflags values are a combination of the followinv values:<br>
LE (1) - Less or equal<br>
LT (2) - Less<br>
EQ (4) -  Equal<br>
NOT (8) - Negates compare, OR with above to generate GT (greater), GE (greater or equal) and NE (not equal)<br>
<I>Please note that only one of the LE/LT/EQ bits are considered, in the listed order, therefore try to use only one.</I><br>

<span style="color:#00F0D0;">rand(dest, src1, src2)</span><br>
ANDs register src1 with register src2 and writes result into register dest

<span style="color:#00F0D0;">ror(dest, src1, src2)</span><br>
ORs register src1 with register src2 and writes result into register dest

<span style="color:#00F0D0;">rxor(dest, src1, src2)</span><br>
XORs register src1 with register src2 and writes result into register dest

<span style="color:#00F0D0;">rasr(dest, src1, src2)</span><br>
Arithmetic shifts (sign extends) register src1 right by value of register src2 (only lowest 5 bits are used) and writes result into register dest

<span style="color:#00F0D0;">rshr(dest, src1, src2)</span><br>
Shifts register src1 right by value of register src2 (only lowest 5 bits are used) and writes result into register dest

<span style="color:#00F0D0;">rshl(dest, src1, src2)</span><br>
Shifts register src1 left by value of register src2 (only lowest 5 bits are used) and writes result into register dest

<span style="color:#00F0D0;">rneg(dest, src)</span><br>
Negates bits of register and writes result into register dest (same as dest = src ^ 0xFFFFFF)

### Other instuctions

<span style="color:#00F0D0;">ldim(dest, immed)</span><br>
Load 24 bit immediate to register dest

<span style="color:#00F0D0;">lctl(dest)</span><br>
Load VPU control register into lower 8 bits of destination register. This allows for CPU writes to VPU control register to affect program flow on the VCP while a program is active.

<span style="color:#00F0D0;">noop()</span><br>
No operation

<span style="color:#00F0D0;">mv(dest, src)</span><br>
Copies contents of register src into register dest (same as r2 = r1 + 0)

<span style="color:#00F0D0;">mvi(dest, imm)</span><br>
Loads an immediate to register dest (same as ldim)

<span style="color:#00F0D0;">clr(dest)</span><br>
Assigns zero to register dest
