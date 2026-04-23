"""Color picker dialog for the Sandpiper Sprite Editor."""

from __future__ import annotations

import tkinter as tk
from tkinter import ttk
from typing import Callable, Optional, Tuple

from .formats import rgb888_to_rgb565, rgb565_to_rgb888, color_to_hex, hex_to_color


class ColorPickerDialog(tk.Toplevel):
    """A dialog for picking an RGB color with HSV slider and hex input."""

    def __init__(
        self,
        parent: tk.Widget,
        initial_color: Tuple[int, int, int] = (255, 255, 255),
        title: str = "Color Picker",
    ):
        super().__init__(parent)
        self.title(title)
        self.resizable(False, False)
        self._result: Optional[Tuple[int, int, int]] = None
        self.grab_set()
        self.transient(parent)
        self.focus_set()

        r, g, b = initial_color
        self._hsv = self._rgb_to_hsv(r, g, b)
        self._current_hex = color_to_hex(r, g, b)
        self._result = (r, g, b)

        self._build_ui()
        self._update_preview()
        self.protocol("WM_DELETE_WINDOW", self._on_cancel)

        # Center on parent
        self.update_idletasks()
        pw = self.winfo_width()
        ph = self.winfo_height()
        x = self.winfo_x() + (self.winfo_width() - pw) // 2
        y = self.winfo_y() + (self.winfo_height() - ph) // 2
        # Use geometry to center
        px = parent.winfo_rootx() + (parent.winfo_width() - pw) // 2
        py = parent.winfo_rooty() + (parent.winfo_height() - ph) // 2
        self.geometry(f"+{px}+{py}")

    def _build_ui(self):
        main = ttk.Frame(self, padding=16)
        main.pack(fill=tk.BOTH, expand=True)

        # HSV Slider (hue)
        ttk.Label(main, text="Hue:").grid(row=0, column=0, sticky=tk.W, pady=2)
        self.hue_scale = ttk.Scale(
            main, from_=0, to=360, orient=tk.HORIZONTAL,
            command=self._on_hue_change, length=200,
        )
        self.hue_scale.grid(row=0, column=1, sticky=tk.EW, pady=2)
        self.hue_label = ttk.Label(main, text="0")
        self.hue_label.grid(row=0, column=2, sticky=tk.W, padx=(8, 0))

        # Saturation
        ttk.Label(main, text="Sat:").grid(row=1, column=0, sticky=tk.W, pady=2)
        self.sat_scale = ttk.Scale(
            main, from_=0, to=100, orient=tk.HORIZONTAL,
            command=self._on_sat_change, length=200,
        )
        self.sat_scale.grid(row=1, column=1, sticky=tk.EW, pady=2)
        self.sat_label = ttk.Label(main, text="100")
        self.sat_label.grid(row=1, column=2, sticky=tk.W, padx=(8, 0))

        # Value
        ttk.Label(main, text="Val:").grid(row=2, column=0, sticky=tk.W, pady=2)
        self.val_scale = ttk.Scale(
            main, from_=0, to=100, orient=tk.HORIZONTAL,
            command=self._on_val_change, length=200,
        )
        self.val_scale.grid(row=2, column=1, sticky=tk.EW, pady=2)
        self.val_label = ttk.Label(main, text="100")
        self.val_label.grid(row=2, column=2, sticky=tk.W, padx=(8, 0))

        # RGB hex input
        ttk.Label(main, text="Hex:").grid(row=3, column=0, sticky=tk.W, pady=2)
        self.hex_var = tk.StringVar(value=self._current_hex)
        self.hex_entry = ttk.Entry(main, textvariable=self.hex_var, width=10)
        self.hex_entry.grid(row=3, column=1, sticky=tk.W, pady=2)
        self.hex_entry.bind("<Return>", self._on_hex_enter)
        ttk.Label(main, text="(RRGGBB)").grid(row=3, column=2, sticky=tk.W, padx=(8, 0))

        # RGB display
        ttk.Label(main, text="RGB:").grid(row=4, column=0, sticky=tk.W, pady=2)
        self.rgb_label = ttk.Label(main, text="255, 255, 255")
        self.rgb_label.grid(row=4, column=1, columnspan=2, sticky=tk.W, pady=2)

        # Preview
        ttk.Label(main, text="Preview:").grid(row=5, column=0, sticky=tk.W, pady=(8, 2))
        self.preview_canvas = tk.Canvas(main, width=60, height=30, highlightthickness=1, highlightbackground="#888")
        self.preview_canvas.grid(row=5, column=1, sticky=tk.W, pady=(8, 2))
        # Checkerboard background for transparency preview
        self._draw_preview()

        # RGB565 display
        ttk.Label(main, text="RGB565:").grid(row=6, column=0, sticky=tk.W, pady=2)
        self.rgb565_label = ttk.Label(main, text="0xFFFF")
        self.rgb565_label.grid(row=6, column=1, columnspan=2, sticky=tk.W, pady=2)

        # Buttons
        btn_frame = ttk.Frame(main)
        btn_frame.grid(row=7, column=0, columnspan=3, pady=(12, 0))
        ttk.Button(btn_frame, text="OK", command=self._on_ok, width=10).pack(side=tk.LEFT, padx=4)
        ttk.Button(btn_frame, text="Cancel", command=self._on_cancel, width=10).pack(side=tk.LEFT, padx=4)

        main.columnconfigure(1, weight=1)

        # Set initial slider values
        self.hue_scale.set(self._hsv[0])
        self.sat_scale.set(self._hsv[1])
        self.val_scale.set(self._hsv[2])

    def _draw_preview(self):
        """Draw preview with checkerboard pattern for transparency."""
        self.preview_canvas.delete(tk.ALL)
        self.preview_canvas.configure(width=60, height=30)
        # Draw checkerboard
        sq = 6
        for y in range(0, 30, sq):
            for x in range(0, 60, sq):
                color = "#CCCCCC" if ((x // sq + y // sq) % 2 == 0) else "#FFFFFF"
                self.preview_canvas.create_rectangle(x, y, x + sq, y + sq, fill=color, outline="")
        # Draw color on top
        self.preview_canvas.create_rectangle(0, 0, 60, 30, fill=self._current_hex, outline="")

    def _update_preview(self):
        """Update the preview and labels."""
        h, s, v = self._hsv
        r, g, b = self._hsv_to_rgb(h, s, v)
        self._current_hex = color_to_hex(r, g, b)
        self.rgb_label.configure(text=f"{r}, {g}, {b}")
        rgb565_val = rgb888_to_rgb565(r, g, b)
        self.rgb565_label.configure(text=f"0x{rgb565_val:04X}")
        self.hex_var.set(self._current_hex)
        self._draw_preview()

    def _on_hue_change(self, event):
        self._hsv = (float(self.hue_scale.get()), self._hsv[1], self._hsv[2])
        self._update_preview()

    def _on_sat_change(self, event):
        self._hsv = (self._hsv[0], float(self.sat_scale.get()), self._hsv[2])
        self._update_preview()

    def _on_val_change(self, event):
        self._hsv = (self._hsv[0], self._hsv[1], float(self.val_scale.get()))
        self._update_preview()

    def _on_hex_enter(self, event):
        try:
            r, g, b = hex_to_color(self.hex_var.get())
            self._hsv = self._rgb_to_hsv(r, g, b)
            self.hue_scale.set(self._hsv[0])
            self.sat_scale.set(self._hsv[1])
            self.val_scale.set(self._hsv[2])
            self._update_preview()
        except ValueError:
            pass

    def _on_ok(self):
        h, s, v = self._hsv
        self._result = self._hsv_to_rgb(h, s, v)
        self.destroy()

    def _on_cancel(self):
        self._result = None
        self.destroy()

    def show(self) -> Optional[Tuple[int, int, int]]:
        """Show dialog and return (r, g, b) or None if cancelled."""
        self.wait_window()
        return self._result

    @staticmethod
    def _rgb_to_hsv(r: int, g: int, b: int) -> Tuple[float, float, float]:
        """Convert RGB (0-255) to HSV (H: 0-360, S: 0-100, V: 0-100)."""
        r, g, b = r / 255.0, g / 255.0, b / 255.0
        mx = max(r, g, b)
        mn = min(r, g, b)
        d = mx - mn
        if d == 0:
            h = 0
        elif mx == r:
            h = 60 * (((g - b) / d) % 6)
        elif mx == g:
            h = 60 * (((b - r) / d) + 2)
        else:
            h = 60 * (((r - g) / d) + 4)
        if h < 0:
            h += 360
        s = 0 if mx == 0 else (d / mx * 100)
        v = mx * 100
        return (h, s, v)

    @staticmethod
    def _hsv_to_rgb(h: float, s: float, v: float) -> Tuple[int, int, int]:
        """Convert HSV (H: 0-360, S: 0-100, V: 0-100) to RGB (0-255)."""
        s /= 100.0
        v /= 100.0
        c = v * s
        x = c * (1 - abs((h / 60) % 2 - 1))
        m = v - c
        if h < 60:
            r1, g1, b1 = c, x, 0
        elif h < 120:
            r1, g1, b1 = x, c, 0
        elif h < 180:
            r1, g1, b1 = 0, c, x
        elif h < 240:
            r1, g1, b1 = 0, x, c
        elif h < 300:
            r1, g1, b1 = x, 0, c
        else:
            r1, g1, b1 = c, 0, x
        r = int((r1 + m) * 255 + 0.5)
        g = int((g1 + m) * 255 + 0.5)
        b = int((b1 + m) * 255 + 0.5)
        return (max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b)))


class PaletteColorPickerDialog(ColorPickerDialog):
    """Color picker that also allows setting a specific palette index."""

    def __init__(
        self,
        parent: tk.Widget,
        palette_index: int,
        color: Tuple[int, int, int],
        on_apply: Callable[[int, Tuple[int, int, int]], None],
    ):
        super().__init__(parent, initial_color=color, title=f"Palette Index {palette_index}")
        self._palette_index = palette_index
        self._on_apply = on_apply
        # Replace OK button to apply to palette
        self._build_palette_buttons()

    def _build_palette_buttons(self):
        # Find and replace the OK button frame
        for child in self.winfo_children():
            if isinstance(child, ttk.Frame):
                for sub in child.winfo_children():
                    if isinstance(sub, ttk.Frame):
                        # Remove existing buttons
                        for btn in sub.winfo_children():
                            btn.destroy()
                        # Add palette-specific buttons
                        ttk.Button(sub, text="Apply", command=self._on_apply_color, width=10).pack(side=tk.LEFT, padx=4)
                        ttk.Button(sub, text="Cancel", command=self._on_cancel, width=10).pack(side=tk.LEFT, padx=4)
                        return

    def _on_apply_color(self):
        h, s, v = self._hsv
        color = self._hsv_to_rgb(h, s, v)
        self._on_apply(self._palette_index, color)
        self.destroy()
