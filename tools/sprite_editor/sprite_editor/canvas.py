"""Pixel grid canvas for the Sandpiper Sprite Editor.

Provides a zoomable, pannable canvas with grid lines, keycolor overlay,
and selection rectangle rendering.
"""

from __future__ import annotations

import tkinter as tk
from typing import TYPE_CHECKING, Optional, Tuple

if TYPE_CHECKING:
    from .formats import Sprite
    from .tools import Tool


class PixelCanvas(tk.Canvas):
    """A tkinter Canvas that renders a pixel grid for sprite editing."""

    def __init__(self, parent, sprite: Sprite, **kwargs):
        super().__init__(parent, bg="#2B2B2B", highlightthickness=0, **kwargs)
        self.sprite = sprite
        self.zoom = 8  # pixels per grid cell
        self.offset_x = 20  # scroll offset in canvas pixels
        self.offset_y = 20
        self.show_grid = True
        self.show_keycolor = True
        self._selection: Optional[Tuple[int, int, int, int]] = None
        self._current_tool: Optional[Tool] = None
        self._is_panning = False
        self._pan_start = (0, 0)
        self._mouse_x = 0
        self._mouse_y = 0
        self._base_image: Optional[tk.PhotoImage] = None
        self._base_image_src: Optional[tk.PhotoImage] = None
        self._overlay_ids: list[int] = []
        self._render_job: Optional[str] = None
        self._needs_full_render = True

        self._bind_events()
        self._request_render(full=True)

    def _bind_events(self):
        self.bind("<Button-1>", self._on_left_press)
        self.bind("<B1-Motion>", self._on_left_drag)
        self.bind("<ButtonRelease-1>", self._on_left_release)
        self.bind("<Button-3>", self._on_right_press)
        self.bind("<B3-Motion>", self._on_right_drag)
        self.bind("<ButtonRelease-3>", self._on_right_release)
        self.bind("<Motion>", self._on_motion)
        self.bind("<MouseWheel>", self._on_mousewheel)
        self.bind("<Configure>", self._on_resize)

    def _on_resize(self, event):
        self._request_render(full=True)

    def _canvas_to_pixel(self, cx: int, cy: int) -> Tuple[int, int]:
        """Convert canvas coordinates to pixel coordinates."""
        px = (cx - self.offset_x) // self.zoom
        py = (cy - self.offset_y) // self.zoom
        return (px, py)

    def _pixel_to_canvas(self, px: int, py: int) -> Tuple[int, int]:
        """Convert pixel coordinates to canvas coordinates."""
        cx = px * self.zoom + self.offset_x
        cy = py * self.zoom + self.offset_y
        return (cx, cy)

    def get_nearest_palette_index(self, r: int, g: int, b: int) -> int:
        """Find nearest palette index for a color."""
        from .formats import nearest_palette_index
        return nearest_palette_index(self.sprite.palette, r, g, b)

    def set_tool(self, tool: Tool):
        """Set the current drawing tool."""
        self._current_tool = tool

    def set_selection(self, selection: Optional[Tuple[int, int, int, int]]):
        """Set the current selection rectangle."""
        self._selection = selection
        self._request_render(full=False)

    def pan_by(self, dx: int, dy: int):
        """Pan the canvas by (dx, dy) pixels."""
        self.offset_x += dx
        self.offset_y += dy
        self._request_render(full=True)

    def zoom_at(self, factor: float, cx: int, cy: int):
        """Zoom in/out centered at canvas coordinates (cx, cy)."""
        old_zoom = self.zoom
        new_zoom = int(round(old_zoom * factor))
        if new_zoom == old_zoom:
            if factor > 1.0 and old_zoom < 32:
                new_zoom = old_zoom + 1
            elif factor < 1.0 and old_zoom > 1:
                new_zoom = old_zoom - 1
        new_zoom = max(1, min(32, new_zoom))
        if new_zoom == old_zoom:
            return
        # Adjust offset to zoom toward cursor
        px, py = self._canvas_to_pixel(cx, cy)
        self.zoom = new_zoom
        self.offset_x = cx - px * self.zoom
        self.offset_y = cy - py * self.zoom
        self._request_render(full=True)

    def _on_mousewheel(self, event):
        factor = 1.1 if event.delta > 0 else 0.9
        self.zoom_at(factor, event.x, event.y)

    def _on_left_press(self, event):
        if self._current_tool:
            px, py = self._canvas_to_pixel(event.x, event.y)
            self._current_tool.on_press(self, px, py)
            self._request_render(full=True)

    def _on_left_drag(self, event):
        if self._current_tool:
            px, py = self._canvas_to_pixel(event.x, event.y)
            self._current_tool.on_drag(self, px, py)
            self._request_render(full=True)

    def _on_left_release(self, event):
        if self._current_tool:
            px, py = self._canvas_to_pixel(event.x, event.y)
            self._current_tool.on_release(self, px, py)
            self._request_render(full=True)

    def _on_right_press(self, event):
        self._is_panning = True
        self._pan_start = (event.x, event.y)
        self.config(cursor="fleur")

    def _on_right_drag(self, event):
        if self._is_panning:
            dx = event.x - self._pan_start[0]
            dy = event.y - self._pan_start[1]
            self.offset_x += dx
            self.offset_y += dy
            self._pan_start = (event.x, event.y)
            self._request_render(full=True)

    def _on_right_release(self, event):
        self._is_panning = False
        self.config(cursor=self._current_tool.cursor if self._current_tool else "arrow")

    def _on_motion(self, event):
        self._mouse_x, self._mouse_y = self._canvas_to_pixel(event.x, event.y)
        self._request_render(full=False)

    def _request_render(self, full: bool):
        """Schedule a coalesced render pass on the Tk idle queue."""
        if full:
            self._needs_full_render = True
        if self._render_job is None:
            self._render_job = self.after_idle(self._perform_render)

    def _perform_render(self):
        self._render_job = None
        if self._needs_full_render:
            self._render_base_layer()
            self._needs_full_render = False
        self._render_overlays()

    def _render_base_layer(self):
        """Render static scene elements that do not depend on mouse position."""
        self.delete(tk.ALL)

        w, h = self.sprite.width, self.sprite.height
        canvas_w = w * self.zoom + self.offset_x + 40
        canvas_h = h * self.zoom + self.offset_y + 40

        # Configure canvas size
        self.config(scrollregion=(0, 0, canvas_w, canvas_h))
        self.config(width=min(canvas_w, self.winfo_width() or 800))
        self.config(height=min(canvas_h, self.winfo_height() or 600))

        # Draw background
        self.create_rectangle(0, 0, canvas_w, canvas_h, fill="#1E1E1E", outline="")

        # Rasterize sprite into a small image and scale it up for fast redraws.
        src = tk.PhotoImage(width=w, height=h)
        for y in range(h):
            row_colors = []
            for x in range(w):
                r, g, b = self.sprite.get_pixel(x, y)
                row_colors.append(f"#{r:02x}{g:02x}{b:02x}")
            src.put("{" + " ".join(row_colors) + "}", to=(0, y))

        self._base_image_src = src
        self._base_image = src.zoom(self.zoom, self.zoom)
        self.create_image(self.offset_x, self.offset_y, image=self._base_image, anchor=tk.NW)

        # Draw checkerboard for transparent areas (keycolor pixels)
        if self.show_keycolor:
            for y in range(h):
                for x in range(w):
                    if self.sprite.is_keycolor(x, y):
                        cx, cy = self._pixel_to_canvas(x, y)
                        self.create_rectangle(
                            cx,
                            cy,
                            cx + self.zoom,
                            cy + self.zoom,
                            fill="#FF00FF",
                            outline="",
                            stipple="gray50",
                        )

        # Draw grid
        if self.show_grid and self.zoom >= 4:
            self.config(cursor="crosshair")
            for x in range(w + 1):
                cx = x * self.zoom + self.offset_x
                self.create_line(cx, self.offset_y, cx, h * self.zoom + self.offset_y, fill="#444444", width=1)
            for y in range(h + 1):
                cy = y * self.zoom + self.offset_y
                self.create_line(self.offset_x, cy, w * self.zoom + self.offset_x, cy, fill="#444444", width=1)
        else:
            self.config(cursor=self._current_tool.cursor if self._current_tool else "arrow")

    def _render_overlays(self):
        """Render dynamic overlays (selection and cursor highlight)."""
        for item_id in self._overlay_ids:
            self.delete(item_id)
        self._overlay_ids.clear()

        w, h = self.sprite.width, self.sprite.height

        # Draw selection rectangle
        if self._selection:
            x1, y1, x2, y2 = self._selection
            for y in range(y1, y2 + 1):
                for x in range(x1, x2 + 1):
                    cx, cy = self._pixel_to_canvas(x, y)
                    self._overlay_ids.append(
                        self.create_rectangle(
                            cx, cy, cx + self.zoom, cy + self.zoom,
                            outline="#00FFFF", width=1
                        )
                    )

        # Draw cursor position indicator
        px, py = self._mouse_x, self._mouse_y
        if 0 <= px < w and 0 <= py < h:
            cx, cy = self._pixel_to_canvas(px, py)
            self._overlay_ids.append(
                self.create_rectangle(
                    cx - 1, cy - 1, cx + self.zoom + 1, cy + self.zoom + 1,
                    outline="#FFFF00", width=2
                )
            )

    def _render(self):
        """Public render entry point used by the app/tooling."""
        self._request_render(full=True)

    def set_zoom(self, zoom: int):
        """Set zoom level directly."""
        self.zoom = max(1, min(32, zoom))
        self._render()

    def toggle_grid(self):
        """Toggle grid visibility."""
        self.show_grid = not self.show_grid
        self._render()

    def toggle_keycolor(self):
        """Toggle keycolor overlay visibility."""
        self.show_keycolor = not self.show_keycolor
        self._render()

    def fit_to_window(self, window_w: int, window_h: int):
        """Calculate zoom to fit the sprite in the given window dimensions."""
        w, h = self.sprite.width, self.sprite.height
        zx = (window_w - 60) // w if w > 0 else 32
        zy = (window_h - 80) // h if h > 0 else 32
        self.set_zoom(max(1, min(zx, zy)))
        # Center
        canvas_w = w * self.zoom
        canvas_h = h * self.zoom
        self.offset_x = max(20, (window_w - canvas_w) // 2)
        self.offset_y = max(20, (window_h - canvas_h) // 2)
        self._render()
