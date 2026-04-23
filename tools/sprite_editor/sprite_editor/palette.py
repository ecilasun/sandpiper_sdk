"""Palette editor widget for the Sandpiper Sprite Editor.

Displays a 16×16 grid of color swatches for editing the 256-entry palette.
"""

from __future__ import annotations

import tkinter as tk
from tkinter import ttk
from typing import Callable, Optional, Tuple

from .formats import color_to_hex


class PaletteEditor(ttk.Frame):
    """A 16×16 grid of palette swatches for editing the 256-color palette."""

    SWATCH_SIZE = 20
    GAP = 1

    def __init__(
        self,
        parent,
        palette: list,
        current_color_idx: int = 0,
        keycolor_idx: int = 0,
        on_color_select: Optional[Callable[[int, Tuple[int, int, int]], None]] = None,
        on_color_change: Optional[Callable[[int, Tuple[int, int, int]], None]] = None,
        on_keycolor_select: Optional[Callable[[int], None]] = None,
    ):
        super().__init__(parent, padding=4)
        self._palette = palette
        self._current_color_idx = current_color_idx
        self._keycolor_idx = keycolor_idx
        self._on_color_select = on_color_select
        self._on_color_change = on_color_change
        self._on_keycolor_select = on_keycolor_select

        self._build_ui()
        self._render()

    def _build_ui(self):
        # Scrollable canvas for the palette grid
        scroll_frame = ttk.Frame(self)
        scroll_frame.pack(fill=tk.BOTH, expand=True)

        self.canvas = tk.Canvas(scroll_frame, bg="#1E1E1E", highlightthickness=0,
                                width=16 * (self.SWATCH_SIZE + self.GAP) + 4,
                                height=16 * (self.SWATCH_SIZE + self.GAP) + 24)
        scrollbar = ttk.Scrollbar(scroll_frame, orient=tk.VERTICAL, command=self.canvas.yview)
        self.canvas.configure(yscrollcommand=scrollbar.set)
        self.canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Info label
        self.info_label = ttk.Label(self, text="Current: #FFFFFF  |  Key: #000000")
        self.info_label.pack(fill=tk.X, pady=(4, 0))

        # Bind mouse events on canvas
        self.canvas.bind("<Button-1>", self._on_click)
        self.canvas.bind("<Button-3>", self._on_right_click)
        self.canvas.bind("<Double-Button-1>", self._on_double_click)

    def _render(self):
        """Render the palette grid."""
        self.canvas.delete(tk.ALL)

        # Clear background
        self.canvas.create_rectangle(0, 0, 400, 400, fill="#1E1E1E", outline="")

        for idx in range(256):
            row = idx // 16
            col = idx % 16
            x = col * (self.SWATCH_SIZE + self.GAP) + 2
            y = row * (self.SWATCH_SIZE + self.GAP) + 2

            r, g, b = self._palette[idx]
            hex_color = color_to_hex(r, g, b)

            # Draw swatch
            outline = "#FFFFFF"
            width = 1
            if idx == self._current_color_idx and idx == self._keycolor_idx:
                outline = "#00FFFF"
                width = 3
            elif idx == self._current_color_idx:
                outline = "#FFFF00"
                width = 3
            elif idx == self._keycolor_idx:
                outline = "#FF0000"
                width = 2

            self.canvas.create_rectangle(
                x, y, x + self.SWATCH_SIZE, y + self.SWATCH_SIZE,
                fill=hex_color, outline=outline, width=width
            )

        # Update info label
        cur_r, cur_g, cur_b = self._palette[self._current_color_idx]
        key_r, key_g, key_b = self._palette[self._keycolor_idx]
        self.info_label.configure(
            text=(
                f"Current[{self._current_color_idx}]: #{cur_r:02x}{cur_g:02x}{cur_b:02x}  |  "
                f"Key[{self._keycolor_idx}]: #{key_r:02x}{key_g:02x}{key_b:02x}"
            )
        )

    def _pixel_to_index(self, cx: int, cy: int) -> Optional[int]:
        """Convert canvas coordinates to palette index."""
        col = (cx - 2) // (self.SWATCH_SIZE + self.GAP)
        row = (cy - 2) // (self.SWATCH_SIZE + self.GAP)
        if 0 <= col < 16 and 0 <= row < 16:
            return row * 16 + col
        return None

    def _on_click(self, event):
        idx = self._pixel_to_index(int(self.canvas.canvasx(event.x)), int(self.canvas.canvasy(event.y)))
        if idx is not None and self._on_color_select:
            r, g, b = self._palette[idx]
            self._current_color_idx = idx
            self._on_color_select(idx, (r, g, b))
            self._render()

    def _on_right_click(self, event):
        idx = self._pixel_to_index(int(self.canvas.canvasx(event.x)), int(self.canvas.canvasy(event.y)))
        if idx is not None and self._on_keycolor_select:
            self._keycolor_idx = idx
            self._on_keycolor_select(idx)
            self._render()

    def _on_double_click(self, event):
        idx = self._pixel_to_index(int(self.canvas.canvasx(event.x)), int(self.canvas.canvasy(event.y)))
        if idx is not None and self._on_color_change:
            r, g, b = self._palette[idx]
            self._on_color_change(idx, (r, g, b))

    def update_palette(self, palette: list):
        """Update the palette and re-render."""
        self._palette = palette
        self._render()

    def set_current_color_idx(self, idx: int):
        """Set the current color index."""
        self._current_color_idx = idx
        self._render()

    def set_keycolor_idx(self, idx: int):
        """Set the keycolor index."""
        self._keycolor_idx = idx
        self._render()
