"""Drawing tools for the Sandpiper Sprite Editor.

Tools: Brush, Eraser, Fill (flood-fill), Eyedropper, Pan, Select.
"""

from __future__ import annotations

import collections
from typing import TYPE_CHECKING, List, Optional, Tuple

if TYPE_CHECKING:
    from .formats import Sprite


class Tool:
    """Base class for all drawing tools."""

    name: str = "Tool"
    cursor = "arrow"

    def on_press(self, canvas, x: int, y: int):
        pass

    def on_drag(self, canvas, x: int, y: int):
        pass

    def on_release(self, canvas, x: int, y: int):
        pass


class BrushTool(Tool):
    """Paint pixels with the current color."""

    name = "Brush"
    cursor = "crosshair"

    def __init__(self, sprite: Sprite, color: Tuple[int, int, int], brush_size: int = 1):
        self.sprite = sprite
        self.color = color
        self.brush_size = max(1, min(16, brush_size))

    def on_press(self, canvas, x: int, y: int):
        self._paint(canvas, x, y)

    def on_drag(self, canvas, x: int, y: int):
        self._paint(canvas, x, y)

    def _paint(self, canvas, x: int, y: int):
        r, g, b = self.color
        half = (self.brush_size - 1) // 2
        for dy in range(self.brush_size):
            for dx in range(self.brush_size):
                px, py = x + dx - half, y + dy - half
                if 0 <= px < self.sprite.width and 0 <= py < self.sprite.height:
                    if self.sprite.mode == "8bit":
                        self.sprite.set_pixel_indexed(px, py, canvas.get_nearest_palette_index(r, g, b))
                    else:
                        self.sprite.set_pixel(px, py, r, g, b)


class EraserTool(Tool):
    """Paint pixels with the keycolor (transparent)."""

    name = "Eraser"
    cursor = "crosshair"

    def __init__(self, sprite: Sprite, brush_size: int = 1):
        self.sprite = sprite
        self.brush_size = max(1, min(16, brush_size))

    def on_press(self, canvas, x: int, y: int):
        self._erase(canvas, x, y)

    def on_drag(self, canvas, x: int, y: int):
        self._erase(canvas, x, y)

    def _erase(self, canvas, x: int, y: int):
        half = (self.brush_size - 1) // 2
        for dy in range(self.brush_size):
            for dx in range(self.brush_size):
                px, py = x + dx - half, y + dy - half
                if 0 <= px < self.sprite.width and 0 <= py < self.sprite.height:
                    if self.sprite.mode == "8bit":
                        self.sprite.set_pixel_indexed(px, py, self.sprite.keycolor)
                    else:
                        self.sprite.set_pixel_rgb565(px, py, self.sprite.keycolor)


class FillTool(Tool):
    """Flood-fill a region with the current color (4-way)."""

    name = "Fill"
    cursor = "plus"

    def __init__(self, sprite: Sprite, color: Tuple[int, int, int]):
        self.sprite = sprite
        self.color = color

    def on_press(self, canvas, x: int, y: int):
        if not (0 <= x < self.sprite.width and 0 <= y < self.sprite.height):
            return
        target_r, target_g, target_b = self.sprite.get_pixel(x, y)
        fill_r, fill_g, fill_b = self.color
        if target_r == fill_r and target_g == fill_g and target_b == fill_b:
            return
        # BFS flood fill (4-way)
        target_rgb = (target_r, target_g, target_b)
        fill_rgb = (fill_r, fill_g, fill_b)
        w, h = self.sprite.width, self.sprite.height
        queue = collections.deque([(x, y)])
        visited = set()
        visited.add((x, y))
        while queue:
            cx, cy = queue.popleft()
            # Check if this pixel matches target
            cr, cg, cb = self.sprite.get_pixel(cx, cy)
            if (cr, cg, cb) != target_rgb:
                continue
            # Fill it
            if self.sprite.mode == "8bit":
                idx = self.sprite.palette.index(fill_rgb) if fill_rgb in self.sprite.palette else 0
                self.sprite.set_pixel_indexed(cx, cy, idx)
            else:
                from .formats import rgb888_to_rgb565
                self.sprite.set_pixel_rgb565(cx, cy, rgb888_to_rgb565(*fill_rgb))
            # Queue neighbors
            for nx, ny in [(cx + 1, cy), (cx - 1, cy), (cx, cy + 1), (cx, cy - 1)]:
                if 0 <= nx < w and 0 <= ny < h and (nx, ny) not in visited:
                    visited.add((nx, ny))
                    queue.append((nx, ny))


class EyedropperTool(Tool):
    """Sample a color from the canvas."""

    name = "Eyedropper"
    cursor = "target"

    def __init__(self, sprite: Sprite, on_color_pick):
        """
        Args:
            sprite: The current sprite.
            on_color_pick: Callback function(color_rgb) called when a pixel is sampled.
        """
        self.sprite = sprite
        self.on_color_pick = on_color_pick

    def on_press(self, canvas, x: int, y: int):
        if 0 <= x < self.sprite.width and 0 <= y < self.sprite.height:
            color = self.sprite.get_pixel(x, y)
            self.on_color_pick(color)


class PanTool(Tool):
    """Pan the canvas by dragging."""

    name = "Pan"
    cursor = "fleur"

    def __init__(self, canvas):
        self.canvas = canvas
        self._start_x = 0
        self._start_y = 0

    def on_press(self, canvas, x: int, y: int):
        self._start_x = x
        self._start_y = y

    def on_drag(self, canvas, x: int, y: int):
        dx = x - self._start_x
        dy = y - self._start_y
        self.canvas.pan_by(dx, dy)
        self._start_x = x
        self._start_y = y


class SelectTool(Tool):
    """Rectangular selection tool."""

    name = "Select"
    cursor = "dot"

    def __init__(self, sprite: Sprite):
        self.sprite = sprite
        self._selection: Optional[Tuple[int, int, int, int]] = None  # (x1, y1, x2, y2)
        self._start_x = 0
        self._start_y = 0
        self._dragging = False

    @property
    def selection(self):
        return self._selection

    def on_press(self, canvas, x: int, y: int):
        self._start_x = x
        self._start_y = y
        self._dragging = True

    def on_drag(self, canvas, x: int, y: int):
        if self._dragging:
            x1 = min(self._start_x, x)
            y1 = min(self._start_y, y)
            x2 = max(self._start_x, x)
            y2 = max(self._start_y, y)
            self._selection = (x1, y1, x2, y2)

    def on_release(self, canvas, x: int, y: int):
        self._dragging = False
        if self._selection:
            x1, y1, x2, y2 = self._selection
            if x2 - x1 < 2 and y2 - y1 < 2:
                # Single click — select entire sprite
                self._selection = (0, 0, self.sprite.width - 1, self.sprite.height - 1)
            else:
                self._selection = (x1, y1, x2, y2)
        else:
            self._selection = (0, 0, self.sprite.width - 1, self.sprite.height - 1)

    def cut(self) -> Optional[Tuple[Sprite, Tuple[int, int, int, int]]]:
        """Cut the current selection, returning a new sprite and the selection rect."""
        if not self._selection:
            return None
        x1, y1, x2, y2 = self._selection
        w = x2 - x1 + 1
        h = y2 - y1 + 1
        new_sprite = Sprite(width=w, height=h, mode=self.sprite.mode)
        for dy in range(h):
            for dx in range(w):
                sx, sy = x1 + dx, y1 + dy
                if self.sprite.mode == "8bit":
                    new_sprite.set_pixel_indexed(dx, dy, self.sprite.pixels_8bit[sy * self.sprite.width + sx])
                else:
                    new_sprite.set_pixel_rgb565(dx, dy, self.sprite.get_pixel_rgb565(sx, sy))
                # Clear original
                if self.sprite.mode == "8bit":
                    self.sprite.set_pixel_indexed(sx, sy, 0)
                else:
                    self.sprite.set_pixel_rgb565(sx, sy, self.sprite.keycolor)
        return (new_sprite, (x1, y1, x2, y2))

    def paste(self, source: Sprite, offset_x: int, offset_y: int):
        """Paste a sprite at the given offset."""
        for dy in range(source.height):
            for dx in range(source.width):
                sx, sy = offset_x + dx, offset_y + dy
                if 0 <= sx < self.sprite.width and 0 <= sy < self.sprite.height:
                    if self.sprite.mode == "8bit":
                        self.sprite.set_pixel_indexed(sx, sy, source.pixels_8bit[dy * source.width + dx])
                    else:
                        self.sprite.set_pixel_rgb565(sx, sy, source.get_pixel_rgb565(dx, dy))

    def clear_selection(self):
        """Clear the selected region with keycolor."""
        if not self._selection:
            return
        x1, y1, x2, y2 = self._selection
        for y in range(y1, y2 + 1):
            for x in range(x1, x2 + 1):
                if 0 <= x < self.sprite.width and 0 <= y < self.sprite.height:
                    if self.sprite.mode == "8bit":
                        self.sprite.set_pixel_indexed(x, y, self.sprite.keycolor)
                    else:
                        self.sprite.set_pixel_rgb565(x, y, self.sprite.keycolor)

    def copy(self) -> Optional[Sprite]:
        """Copy the current selection to a new sprite."""
        if not self._selection:
            return None
        x1, y1, x2, y2 = self._selection
        w = x2 - x1 + 1
        h = y2 - y1 + 1
        new_sprite = Sprite(width=w, height=h, mode=self.sprite.mode)
        for dy in range(h):
            for dx in range(w):
                sx, sy = x1 + dx, y1 + dy
                if self.sprite.mode == "8bit":
                    new_sprite.set_pixel_indexed(dx, dy, self.sprite.pixels_8bit[sy * self.sprite.width + sx])
                else:
                    new_sprite.set_pixel_rgb565(dx, dy, self.sprite.get_pixel_rgb565(sx, sy))
        return new_sprite

    def deselect(self):
        self._selection = None
