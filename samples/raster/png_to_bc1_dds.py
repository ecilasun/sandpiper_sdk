#!/usr/bin/env python3
"""
Convert a PNG image to BC1/DXT1 DDS.

Usage:
  python png_to_bc1_dds.py input.png output.dds

Notes:
- Output is always 64x64 by default to match the raster sample test flow.
- If input size differs, it is resized with Lanczos.
- Compression is a simple endpoint+palette fit per 4x4 block (fast, deterministic).
"""

from __future__ import annotations

import struct
import sys
from pathlib import Path

from PIL import Image


def pack565(r: int, g: int, b: int) -> int:
    r5 = (r * 31 + 127) // 255
    g6 = (g * 63 + 127) // 255
    b5 = (b * 31 + 127) // 255
    return (r5 << 11) | (g6 << 5) | b5


def unpack565(c: int) -> tuple[int, int, int]:
    return (c >> 11) & 31, (c >> 5) & 63, c & 31


def rgb565_to_888(c: int) -> tuple[int, int, int]:
    r5, g6, b5 = unpack565(c)
    r = (r5 * 255 + 15) // 31
    g = (g6 * 255 + 31) // 63
    b = (b5 * 255 + 15) // 31
    return r, g, b


def color_dist2(a: tuple[int, int, int], b: tuple[int, int, int]) -> int:
    dr = a[0] - b[0]
    dg = a[1] - b[1]
    db = a[2] - b[2]
    return dr * dr + dg * dg + db * db


def encode_bc1_block(block: list[tuple[int, int, int]]) -> bytes:
    # Pick endpoints by luminance extrema (quick heuristic)
    lum = [p[0] * 54 + p[1] * 183 + p[2] * 19 for p in block]
    c_min = block[min(range(16), key=lambda i: lum[i])]
    c_max = block[max(range(16), key=lambda i: lum[i])]

    c0 = pack565(*c_max)
    c1 = pack565(*c_min)
    if c0 <= c1:
        c0, c1 = c1, c0

    p0 = rgb565_to_888(c0)
    p1 = rgb565_to_888(c1)
    p2 = ((2 * p0[0] + p1[0]) // 3, (2 * p0[1] + p1[1]) // 3, (2 * p0[2] + p1[2]) // 3)
    p3 = ((p0[0] + 2 * p1[0]) // 3, (p0[1] + 2 * p1[1]) // 3, (p0[2] + 2 * p1[2]) // 3)
    palette = [p0, p1, p2, p3]

    bits = 0
    for i, p in enumerate(block):
        idx = min(range(4), key=lambda k: color_dist2(p, palette[k]))
        bits |= (idx & 0x3) << (2 * i)

    return struct.pack("<HHI", c0, c1, bits)


def write_dds_bc1(out_path: Path, w: int, h: int, blocks: bytes) -> None:
    header = bytearray()
    header += struct.pack("<I", 0x20534444)   # DDS magic
    header += struct.pack("<I", 124)          # header size
    header += struct.pack("<I", 0x00081007)   # CAPS|HEIGHT|WIDTH|PIXELFORMAT|LINEARSIZE
    header += struct.pack("<I", h)
    header += struct.pack("<I", w)
    header += struct.pack("<I", len(blocks))  # linear size
    header += struct.pack("<I", 0)            # depth
    header += struct.pack("<I", 0)            # mipmaps
    header += b"\x00" * (11 * 4)

    # DDS_PIXELFORMAT
    header += struct.pack("<I", 32)           # pf size
    header += struct.pack("<I", 0x00000004)   # DDPF_FOURCC
    header += struct.pack("<I", 0x31545844)   # "DXT1"
    header += struct.pack("<I", 0) * 5

    # caps
    header += struct.pack("<I", 0x00001000)   # DDSCAPS_TEXTURE
    header += struct.pack("<I", 0) * 4

    out_path.write_bytes(header + blocks)


def main() -> int:
    if len(sys.argv) not in (3, 4):
        print("Usage: python png_to_bc1_dds.py input.png output.dds [size]")
        print("  size: optional square output size (default: 64)")
        return 2

    in_path = Path(sys.argv[1])
    out_path = Path(sys.argv[2])
    size = int(sys.argv[3]) if len(sys.argv) == 4 else 64

    img = Image.open(in_path).convert("RGB")
    if img.size != (size, size):
        img = img.resize((size, size), Image.Resampling.LANCZOS)

    w, h = img.size
    if (w % 4) or (h % 4):
        raise ValueError("BC1 requires width/height multiples of 4")

    px = img.load()
    blocks_x = w // 4
    blocks_y = h // 4

    bc1_blocks = bytearray()
    for by in range(blocks_y):
        for bx in range(blocks_x):
            block = []
            for y in range(4):
                for x in range(4):
                    block.append(px[bx * 4 + x, by * 4 + y])
            bc1_blocks += encode_bc1_block(block)

    write_dds_bc1(out_path, w, h, bytes(bc1_blocks))
    print(f"Wrote {out_path} ({out_path.stat().st_size} bytes) [{w}x{h} BC1/DXT1]")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
