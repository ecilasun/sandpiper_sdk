"""File import/export for the Sandpiper Sprite Editor.

Supports:
  - Import: PNG images (via Pillow)
  - Export: C header (.h), C source (.c), raw binary (.bin)
"""

from __future__ import annotations

import io
from typing import Optional, Tuple

from PIL import Image

from .formats import (
    Sprite,
    rgb888_to_rgb565,
    make_default_palette,
    nearest_palette_index,
)


# ---------------------------------------------------------------------------
# Import
# ---------------------------------------------------------------------------

def import_png(
    data: bytes,
    width: int = 32,
    height: int = 32,
    mode: str = "16bit",
) -> Sprite:
    """Import a PNG image into a Sprite.

    Args:
        data: Raw PNG file bytes.
        width: Desired sprite width (used if PNG has no dimensions).
        height: Desired sprite height.
        mode: '8bit' or '16bit'.

    Returns:
        A new Sprite populated with the image data.
    """
    img = Image.open(io.BytesIO(data))

    # Convert to RGBA for uniform handling
    if img.mode != "RGBA":
        img = img.convert("RGBA")

    # Resize if needed
    if img.size[0] != width or img.size[1] != height:
        img = img.resize((width, height), Image.LANCZOS)

    sprite = Sprite(width=width, height=height, mode=mode)

    pixels = img.load()
    for y in range(height):
        for x in range(width):
            r, g, b, a = pixels[x, y]
            if a == 0:
                # Fully transparent → keycolor
                if mode == "8bit":
                    sprite.set_pixel_indexed(x, y, sprite.keycolor)
                else:
                    sprite.set_pixel_rgb565(x, y, sprite.keycolor)
            elif a < 255:
                # Partially transparent — alpha blend with transparent bg
                r = int(r * a / 255)
                g = int(g * a / 255)
                b = int(b * a / 255)
                if mode == "8bit":
                    sprite.set_pixel_indexed(x, y, nearest_palette_index(sprite.palette, r, g, b))
                else:
                    sprite.set_pixel(x, y, r, g, b)
            else:
                if mode == "8bit":
                    sprite.set_pixel_indexed(x, y, nearest_palette_index(sprite.palette, r, g, b))
                else:
                    sprite.set_pixel(x, y, r, g, b)

    return sprite


def import_png_file(
    filepath: str,
    width: int = 32,
    height: int = 32,
    mode: str = "16bit",
) -> Sprite:
    """Import a PNG file into a Sprite."""
    with open(filepath, "rb") as f:
        data = f.read()
    return import_png(data, width, height, mode)


# ---------------------------------------------------------------------------
# Export
# ---------------------------------------------------------------------------

def export_c_header(sprite: Sprite) -> str:
    """Export a sprite as a C header file string.

    For 8-bit mode, includes palette array + pixel array.
    For 16-bit mode, includes pixel array only.
    """
    lines: list[str] = []
    name = sprite.name.replace(" ", "_")

    lines.append(f"#pragma once")
    lines.append("")
    lines.append(f"#include <stdint.h>")
    lines.append(f"#include \"vcp.h\"")
    lines.append("")
    lines.append(f"#define {name.upper()}_W        {sprite.width}")
    lines.append(f"#define {name.upper()}_H        {sprite.height}")

    if sprite.mode == "16bit":
        rgb565_val = sprite.keycolor
        lines.append(f"#define {name.upper()}_KEY_COLOR  0x{rgb565_val:04X}")
    else:
        lines.append(f"#define {name.upper()}_KEY_COLOR  {sprite.keycolor}")
    lines.append("")

    if sprite.mode == "8bit":
        # Palette
        lines.append(f"static const uint32_t {name}_palette[256] = {{")
        lines.append(sprite.to_c_palette())
        lines.append("};")
        lines.append("")

        # Pixel data
        lines.append(f"static const uint8_t {name}[{name.upper()}_W * {name.upper()}_H] = {{")
        lines.append(sprite.to_c_array_8bit())
        lines.append("};")
    else:
        # 16-bit pixel data
        lines.append(f"static const uint16_t {name}[{name.upper()}_W * {name.upper()}_H] = {{")
        lines.append(sprite.to_c_array_16bit())
        lines.append("};")

    lines.append("")
    return "\n".join(lines)


def export_c_source(sprite: Sprite) -> str:
    """Export a sprite as a C source file string (with includes)."""
    return export_c_header(sprite)


def export_binary(sprite: Sprite, filepath: str):
    """Export a sprite as raw binary pixel data."""
    with open(filepath, "wb") as f:
        f.write(sprite.to_binary())


def export_png(sprite: Sprite, filepath: str):
    """Export a sprite as a PNG image."""
    if sprite.mode == "8bit":
        # Create palettized image
        img = Image.new("P", (sprite.width, sprite.height))
        # Build palette (768 bytes: 256 × 3)
        palette_bytes = []
        for r, g, b in sprite.palette:
            palette_bytes.extend([r, g, b])
        # Pad to 768
        while len(palette_bytes) < 768:
            palette_bytes.extend([0, 0, 0])
        img.putpalette(palette_bytes)

        pixels = img.load()
        for y in range(sprite.height):
            for x in range(sprite.width):
                pixels[x, y] = sprite.pixels_8bit[y * sprite.width + x]
    else:
        # RGB image
        img = Image.new("RGB", (sprite.width, sprite.height))
        pixels = img.load()
        for y in range(sprite.height):
            for x in range(sprite.width):
                pixels[x, y] = sprite.get_pixel(x, y)

    img.save(filepath)


def export_binary_string(sprite: Sprite) -> bytes:
    """Export sprite as raw binary bytes."""
    return sprite.to_binary()
