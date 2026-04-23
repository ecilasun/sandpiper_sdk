"""Core data model and format conversion for Sandpiper sprite formats.

Supports:
  - 8-bit indexed (ECM_8bit_Indexed): 1 byte/pixel + 256-entry palette
  - 16-bit RGB565 (ECM_16bit_RGB): 2 bytes/pixel, r5g6b5
"""

from __future__ import annotations

import struct
from dataclasses import dataclass, field
from typing import List, Optional, Tuple

# ---------------------------------------------------------------------------
# RGB565 helpers  (matches SDK's MAKECOLORRGB16 / RGB565_CONST)
# ---------------------------------------------------------------------------

def rgb888_to_rgb565(r: int, g: int, b: int) -> int:
    """Convert 8-bit RGB to 16-bit RGB565 (uint16, little-endian on host)."""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


def rgb565_to_rgb88565(val: int) -> Tuple[int, int, int]:
    """Expand a 16-bit RGB565 value back to 8-bit RGB."""
    r = (val >> 11) & 0x1F
    g = (val >> 5) & 0x3F
    b = val & 0x1F
    # Scale up to 8-bit range
    r = (r << 3) | (r >> 2)   # 5-bit -> 8-bit
    g = (g << 2) | (g >> 4)   # 6-bit -> 8-bit
    b = (b << 3) | (b >> 2)
    return (r, g, b)


def rgb565_to_rgb888(val: int) -> Tuple[int, int, int]:
    """Convert 16-bit RGB565 to 8-bit RGB (full 0-255 range)."""
    r = ((val >> 11) & 0x1F) * 255 // 31
    g = ((val >> 5) & 0x3F) * 255 // 63
    b = (val & 0x1F) * 255 // 31
    return (r, g, b)


# ---------------------------------------------------------------------------
# Palette quantization (nearest-neighbor in RGB space)
# ---------------------------------------------------------------------------

def nearest_palette_index(palette: List[Tuple[int, int, int]], r: int, g: int, b: int) -> int:
    """Find the palette index whose color is closest to (r, g, b) using Euclidean distance."""
    best_idx = 0
    best_dist = float("inf")
    for i, (pr, pg, pb) in enumerate(palette):
        dr = r - pr
        dg = g - pg
        db = b - pb
        dist = dr * dr + dg * dg + db * db
        if dist < best_dist:
            best_dist = dist
            best_idx = i
            if dist == 0:
                break
    return best_idx


def color_to_hex(r: int, g: int, b: int) -> str:
    """Convert RGB to hex string for Tkinter."""
    return f"#{r:02x}{g:02x}{b:02x}"


def hex_to_color(hex_str: str) -> Tuple[int, int, int]:
    """Convert hex string to RGB tuple."""
    hex_str = hex_str.lstrip("#")
    return (int(hex_str[0:2], 16), int(hex_str[2:4], 16), int(hex_str[4:6], 16))


# ---------------------------------------------------------------------------
# Default Sandpiper palette
# ---------------------------------------------------------------------------

def make_default_palette() -> List[Tuple[int, int, int]]:
    """Create the default Sandpiper console palette (matches VPUSetDefaultPalette).

    Indices 0-7:  dim colors (R, G, B, C, M, Y, W, K)
    Indices 8-15: bright colors
    Indices 16-255: black (transparent)
    """
    palette: List[Tuple[int, int, int]] = []

    # Dim colors (indices 0-7)
    dim_colors = [
        (0x80, 0x80, 0x80),  # 0: dim gray
        (0x00, 0x00, 0x80),  # 1: dim blue
        (0x00, 0x80, 0x00),  # 2: dim green
        (0x00, 0x80, 0x80),  # 3: dim cyan
        (0x80, 0x00, 0x00),  # 4: dim red
        (0x80, 0x00, 0x80),  # 5: dim magenta
        (0x80, 0x80, 0x00),  # 6: dim yellow
        (0xC0, 0xC0, 0xC0),  # 7: dim white
    ]
    palette.extend(dim_colors)

    # Bright colors (indices 8-15)
    bright_colors = [
        (0x80, 0x80, 0x80),  # 8: gray
        (0x00, 0x00, 0xFF),  # 9: blue
        (0x00, 0xFF, 0x00),  # 10: green
        (0x00, 0xFF, 0xFF),  # 11: cyan
        (0xFF, 0x00, 0x00),  # 12: red
        (0xFF, 0x00, 0xFF),  # 13: magenta
        (0xFF, 0xFF, 0x00),  # 14: yellow
        (0xFF, 0xFF, 0xFF),  # 15: white
    ]
    palette.extend(bright_colors)

    # Fill remaining with black
    while len(palette) < 256:
        palette.append((0, 0, 0))

    return palette


# ---------------------------------------------------------------------------
# Sprite data model
# ---------------------------------------------------------------------------

@dataclass
class Sprite:
    """In-memory representation of a Sandpiper sprite.

    Attributes:
        width:  Sprite width in pixels (1-256)
        height: Sprite height in pixels (1-256)
        mode:   '8bit' for indexed palette, '16bit' for RGB565
        pixels_8bit:  1-byte-per-pixel palette indices (length width*height)
        pixels_16bit: 2-byte-per-pixel RGB565 values (length width*height)
        palette: 256 entries of (r, g, b) — always maintained for display
        keycolor: Transparency key. 8bit = palette index, 16bit = RGB565 value
        name:   Sprite name (used in C export)
    """
    width: int = 32
    height: int = 32
    mode: str = "16bit"  # '8bit' or '16bit'
    pixels_8bit: bytearray = field(default_factory=lambda: bytearray(32 * 32))
    pixels_16bit: bytearray = field(default_factory=lambda: bytearray(32 * 32 * 2))
    palette: List[Tuple[int, int, int]] = field(default_factory=make_default_palette)
    keycolor: int = 0  # palette index (8bit) or RGB565 value (16bit)
    name: str = "sprite"

    def __post_init__(self):
        self._ensure_size()

    def _ensure_size(self):
        """Resize pixel buffers to match width × height."""
        total = self.width * self.height
        # 8-bit buffer
        cur = len(self.pixels_8bit)
        if total > cur:
            self.pixels_8bit.extend(b"\x00" * (total - cur))
        elif total < cur:
            del self.pixels_8bit[total:]
        # 16-bit buffer
        total16 = total * 2
        cur16 = len(self.pixels_16bit)
        if total16 > cur16:
            self.pixels_16bit.extend(b"\x00" * (total16 - cur16))
        elif total16 < cur16:
            del self.pixels_16bit[total16:]

    def get_pixel(self, x: int, y: int) -> Tuple[int, int, int]:
        """Get pixel at (x, y) as RGB888 tuple."""
        if not (0 <= x < self.width and 0 <= y < self.height):
            return (0, 0, 0)
        idx = y * self.width + x
        if self.mode == "8bit":
            pal_idx = self.pixels_8bit[idx]
            return self.palette[pal_idx]
        else:
            off = idx * 2
            val = self.pixels_16bit[off] | (self.pixels_16bit[off + 1] << 8)
            return rgb565_to_rgb888(val)

    def set_pixel(self, x: int, y: int, r: int, g: int, b: int):
        """Set pixel at (x, y) to RGB888 color."""
        if not (0 <= x < self.width and 0 <= y < self.height):
            return
        idx = y * self.width + x
        rgb565_val = rgb888_to_rgb565(r, g, b)
        # Store in 16-bit buffer
        off = idx * 2
        self.pixels_16bit[off] = rgb565_val & 0xFF
        self.pixels_16bit[off + 1] = (rgb565_val >> 8) & 0xFF
        # Also store in 8-bit buffer
        pal_idx = nearest_palette_index(self.palette, r, g, b)
        self.pixels_8bit[idx] = pal_idx

    def set_pixel_indexed(self, x: int, y: int, pal_idx: int):
        """Set pixel by palette index (8-bit mode)."""
        if not (0 <= x < self.width and 0 <= y < self.height):
            return
        idx = y * self.width + x
        self.pixels_8bit[idx] = pal_idx & 0xFF
        # Update 16-bit buffer too
        r, g, b = self.palette[pal_idx & 0xFF]
        rgb565_val = rgb888_to_rgb565(r, g, b)
        off = idx * 2
        self.pixels_16bit[off] = rgb565_val & 0xFF
        self.pixels_16bit[off + 1] = (rgb565_val >> 8) & 0xFF

    def set_pixel_rgb565(self, x: int, y: int, val: int):
        """Set pixel by RGB565 value (16-bit mode)."""
        if not (0 <= x < self.width and 0 <= y < self.height):
            return
        idx = y * self.width + x
        off = idx * 2
        self.pixels_16bit[off] = val & 0xFF
        self.pixels_16bit[off + 1] = (val >> 8) & 0xFF
        # Update 8-bit buffer
        r, g, b = rgb565_to_rgb888(val)
        pal_idx = nearest_palette_index(self.palette, r, g, b)
        self.pixels_8bit[idx] = pal_idx

    def get_pixel_rgb565(self, x: int, y: int) -> int:
        """Get pixel as RGB565 value."""
        if not (0 <= x < self.width and 0 <= y < self.height):
            return 0
        idx = y * self.width + x
        off = idx * 2
        return self.pixels_16bit[off] | (self.pixels_16bit[off + 1] << 8)

    def is_keycolor(self, x: int, y: int) -> bool:
        """Check if pixel is the keycolor (transparent)."""
        if self.mode == "8bit":
            idx = y * self.width + x
            return self.pixels_8bit[idx] == self.keycolor
        else:
            return self.get_pixel_rgb565(x, y) == self.keycolor

    def clone(self) -> "Sprite":
        """Deep copy this sprite."""
        other = Sprite(
            width=self.width,
            height=self.height,
            mode=self.mode,
            name=self.name,
        )
        other.pixels_8bit = bytearray(self.pixels_8bit)
        other.pixels_16bit = bytearray(self.pixels_16bit)
        other.palette = list(self.palette)
        other.keycolor = self.keycolor
        return other

    def resize(self, new_width: int, new_height: int):
        """Resize sprite, preserving existing pixels."""
        new_width = max(1, min(256, new_width))
        new_height = max(1, min(256, new_height))
        old_pixels_8 = bytearray(self.pixels_8bit)
        old_pixels_16 = bytearray(self.pixels_16bit)
        old_w, old_h = self.width, self.height

        self.width = new_width
        self.height = new_height
        self._ensure_size()

        # Copy old pixels
        for y in range(min(old_h, new_height)):
            for x in range(min(old_w, new_width)):
                old_idx = y * old_w + x
                new_idx = y * new_width + x
                self.pixels_8bit[new_idx] = old_pixels_8[old_idx]
                self.pixels_16bit[new_idx * 2] = old_pixels_16[old_idx * 2]
                self.pixels_16bit[new_idx * 2 + 1] = old_pixels_16[old_idx * 2 + 1]

    def flip_horizontal(self):
        """Flip sprite horizontally."""
        for y in range(self.height):
            row_start = y * self.width
            row_8 = self.pixels_8bit[row_start:row_start + self.width]
            self.pixels_8bit[row_start:row_start + self.width] = row_8[::-1]
            row_16 = self.pixels_16bit[row_start * 2:(row_start + self.width) * 2]
            # Reverse pairs
            pairs = [row_16[i:i + 2] for i in range(0, len(row_16), 2)]
            new_pairs = pairs[::-1]
            self.pixels_16bit[row_start * 2:(row_start + self.width) * 2] = b"".join(new_pairs)

    def flip_vertical(self):
        """Flip sprite vertically."""
        # Swap rows
        for y in range(self.height // 2):
            y2 = self.height - 1 - y
            row1_8 = y * self.width
            row2_8 = y2 * self.width
            # Swap 8-bit rows
            self.pixels_8bit[row1_8:row1_8 + self.width], \
                self.pixels_8bit[row2_8:row2_8 + self.width] = \
                self.pixels_8bit[row2_8:row2_8 + self.width], \
                self.pixels_8bit[row1_8:row1_8 + self.width]
            # Swap 16-bit rows
            row1_16 = row1_8 * 2
            row2_16 = row2_8 * 2
            row_len = self.width * 2
            self.pixels_16bit[row1_16:row1_16 + row_len], \
                self.pixels_16bit[row2_16:row2_16 + row_len] = \
                self.pixels_16bit[row2_16:row2_16 + row_len], \
                self.pixels_16bit[row1_16:row1_16 + row_len]

    def rotate_90_cw(self):
        """Rotate sprite 90 degrees clockwise."""
        old_w, old_h = self.width, self.height
        old_8 = bytearray(self.pixels_8bit)
        old_16 = bytearray(self.pixels_16bit)

        self.width, self.height = old_h, old_w
        self._ensure_size()

        for y in range(old_h):
            for x in range(old_w):
                new_x = old_h - 1 - y
                new_y = x
                new_idx = new_y * self.width + new_x
                old_idx = y * old_w + x
                self.pixels_8bit[new_idx] = old_8[old_idx]
                self.pixels_16bit[new_idx * 2] = old_16[old_idx * 2]
                self.pixels_16bit[new_idx * 2 + 1] = old_16[old_idx * 2 + 1]

    def rotate_90_ccw(self):
        """Rotate sprite 90 degrees counter-clockwise."""
        old_w, old_h = self.width, self.height
        old_8 = bytearray(self.pixels_8bit)
        old_16 = bytearray(self.pixels_16bit)

        self.width, self.height = old_h, old_w
        self._ensure_size()

        for y in range(old_h):
            for x in range(old_w):
                new_x = y
                new_y = old_w - 1 - x
                new_idx = new_y * self.width + new_x
                old_idx = y * old_w + x
                self.pixels_8bit[new_idx] = old_8[old_idx]
                self.pixels_16bit[new_idx * 2] = old_16[old_idx * 2]
                self.pixels_16bit[new_idx * 2 + 1] = old_16[old_idx * 2 + 1]

    def to_binary(self) -> bytes:
        """Export sprite as raw binary pixel data.

        8-bit mode: 1 byte per pixel
        16-bit mode: 2 bytes per pixel (little-endian RGB565)
        """
        if self.mode == "8bit":
            return bytes(self.pixels_8bit[:self.width * self.height])
        else:
            return bytes(self.pixels_16bit[:self.width * self.height * 2])

    def to_c_array_16bit(self) -> str:
        """Export as C array of uint16_t RGB565 values."""
        lines: List[str] = []
        total = self.width * self.height
        for i in range(0, total, 16):
            chunk = []
            for j in range(i, min(i + 16, total)):
                val = self.pixels_16bit[j * 2] | (self.pixels_16bit[j * 2 + 1] << 8)
                chunk.append(f"0x{val:04X}")
            lines.append("    " + ", ".join(chunk) + ",")
        return "\n".join(lines)

    def to_c_array_8bit(self) -> str:
        """Export as C array of uint8_t palette indices."""
        lines: List[str] = []
        total = self.width * self.height
        for i in range(0, total, 16):
            chunk = []
            for j in range(i, min(i + 16, total)):
                chunk.append(f"0x{self.pixels_8bit[j]:02X}")
            lines.append("    " + ", ".join(chunk) + ",")
        return "\n".join(lines)

    def to_c_palette(self) -> str:
        """Export palette as C array of MAKECOLORRGB24 macros."""
        lines: List[str] = []
        for r, g, b in self.palette:
            lines.append(f"    MAKECOLORRGB24(0x{r:02X}, 0x{g:02X}, 0x{b:02X}),")
        return "\n".join(lines)
