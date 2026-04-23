"""Main application window for the Sandpiper Sprite Editor.

Ties together the canvas, palette editor, tools, and file I/O into a
complete desktop application using Tkinter.
"""

from __future__ import annotations

import os
import tkinter as tk
from tkinter import filedialog, messagebox, ttk
from typing import Optional, Tuple

from .formats import (
    Sprite,
    rgb888_to_rgb565,
    rgb565_to_rgb888,
    color_to_hex,
    make_default_palette,
    nearest_palette_index,
)
from .canvas import PixelCanvas
from .palette import PaletteEditor
from .tools import BrushTool, EraserTool, FillTool, EyedropperTool, PanTool, SelectTool
from .color_picker import ColorPickerDialog, PaletteColorPickerDialog
from .file_io import import_png_file, export_c_header, export_c_source, export_binary, export_png


class _ToolTip:
    """Small hover tooltip for toolbar controls."""

    def __init__(self, widget, text: str):
        self.widget = widget
        self.text = text
        self.tipwindow = None
        widget.bind("<Enter>", self._show)
        widget.bind("<Leave>", self._hide)
        widget.bind("<ButtonPress>", self._hide)

    def _show(self, _event=None):
        if self.tipwindow is not None:
            return
        x = self.widget.winfo_rootx() + 10
        y = self.widget.winfo_rooty() + self.widget.winfo_height() + 8
        self.tipwindow = tw = tk.Toplevel(self.widget)
        tw.wm_overrideredirect(True)
        tw.wm_geometry(f"+{x}+{y}")
        label = tk.Label(
            tw,
            text=self.text,
            justify=tk.LEFT,
            background="#1f1f1f",
            foreground="#f2f2f2",
            relief=tk.SOLID,
            borderwidth=1,
            padx=6,
            pady=3,
            font=("Segoe UI", 9),
        )
        label.pack()

    def _hide(self, _event=None):
        if self.tipwindow is not None:
            self.tipwindow.destroy()
            self.tipwindow = None


class SpriteEditorApp:
    """Main application class for the Sandpiper Sprite Editor."""

    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Sandpiper Sprite Editor")
        self.root.minsize(800, 600)

        self._ui = {
            "bg": "#1e1e1e",
            "surface": "#252526",
            "muted": "#9da5b4",
            "text": "#d4d4d4",
            "border": "#3c3c3c",
            "accent": "#0e639c",
            "accent_hover": "#1177bb",
            "accent_pressed": "#094771",
        }
        self._configure_styles()

        # Current sprite (None until new/open)
        self.sprite: Optional[Sprite] = None
        self._current_color: Tuple[int, int, int] = (255, 255, 255)
        self._current_color_idx: int = 15  # white
        self._brush_size: int = 1
        self._tool_name: str = "Brush"
        self._history: list = []  # undo stack
        self._history_idx: int = -1
        self._max_history: int = 50
        self._clipboard: Optional[Sprite] = None
        self._unsaved: bool = False
        self._current_file: Optional[str] = None

        # Size presets
        self.size_presets = [(8, 8), (16, 16), (24, 24), (32, 32), (48, 48), (64, 64)]

        self._build_ui()
        self._new_sprite(32, 32)

    # ------------------------------------------------------------------
    # UI Construction
    # ------------------------------------------------------------------

    def _build_ui(self):
        self._create_menu()
        self._create_toolbar()
        self._create_main_pane()
        self._create_statusbar()

    def _configure_styles(self):
        style = ttk.Style(self.root)
        theme_names = style.theme_names()
        # Use clam for predictable flat styling.
        if "clam" in theme_names:
            style.theme_use("clam")

        self.root.configure(bg=self._ui["bg"])

        style.configure("TFrame", background=self._ui["bg"])
        style.configure("App.TFrame", background=self._ui["bg"])
        style.configure("Toolbar.TFrame", background=self._ui["surface"])
        style.configure("Status.TFrame", background=self._ui["surface"])

        style.configure("TLabel", background=self._ui["bg"], foreground=self._ui["text"], font=("Segoe UI", 10))
        style.configure("Toolbar.TLabel", background=self._ui["surface"], foreground=self._ui["muted"], font=("Segoe UI", 10))
        style.configure("Status.TLabel", background=self._ui["surface"], foreground=self._ui["muted"], font=("Segoe UI", 9))

        style.configure(
            "Group.TLabelframe",
            background=self._ui["surface"],
            bordercolor=self._ui["border"],
            relief="solid",
            borderwidth=1,
        )
        style.configure(
            "Group.TLabelframe.Label",
            background=self._ui["surface"],
            foreground=self._ui["muted"],
            font=("Segoe UI Semibold", 10),
        )

        style.configure(
            "Flat.TButton",
            font=("Segoe UI", 9),
            padding=(10, 4),
            relief="flat",
            borderwidth=1,
            foreground=self._ui["text"],
            background=self._ui["surface"],
        )
        style.map(
            "Flat.TButton",
            foreground=[("!disabled", self._ui["text"])],
            background=[("active", "#1d1d1d"), ("pressed", "#242424")],
            bordercolor=[("active", self._ui["border"])],
        )

        style.configure(
            "ToolActive.TButton",
            font=("Segoe UI Semibold", 9),
            padding=(10, 4),
            foreground="#ffffff",
            background=self._ui["accent"],
            borderwidth=1,
            relief="flat",
        )
        style.map(
            "ToolActive.TButton",
            foreground=[("!disabled", "#ffffff")],
            background=[("pressed", self._ui["accent_pressed"]), ("active", self._ui["accent_hover"])],
        )

        style.configure("TEntry", font=("Segoe UI", 10), padding=4)
        style.configure("TCombobox", font=("Segoe UI", 10), padding=3)
        style.configure("TSpinbox", font=("Segoe UI", 10), padding=3)
        style.configure("TPanedwindow", background=self._ui["bg"])
        style.configure("Sash", sashthickness=6)

    def _menu(self, parent):
        return tk.Menu(parent, tearoff=0)

    def _create_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        # File menu
        file_menu = self._menu(menubar)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="New...", command=self._cmd_new)
        file_menu.add_command(label="Open PNG...", command=self._cmd_open)
        file_menu.add_separator()
        file_menu.add_command(label="Save", command=self._cmd_save, accelerator="Ctrl+S")
        file_menu.add_separator()
        file_menu.add_command(label="Export C Header...", command=self._cmd_export_header)
        file_menu.add_command(label="Export C Source...", command=self._cmd_export_source)
        file_menu.add_command(label="Export Binary...", command=self._cmd_export_binary)
        file_menu.add_command(label="Export PNG...", command=self._cmd_export_png)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self._cmd_exit)

        # Edit menu
        edit_menu = self._menu(menubar)
        menubar.add_cascade(label="Edit", menu=edit_menu)
        edit_menu.add_command(label="Undo", command=self._cmd_undo, accelerator="Ctrl+Z")
        edit_menu.add_command(label="Redo", command=self._cmd_redo, accelerator="Ctrl+Shift+Z")
        edit_menu.add_separator()
        edit_menu.add_command(label="Cut", command=self._cmd_cut, accelerator="Ctrl+X")
        edit_menu.add_command(label="Copy", command=self._cmd_copy, accelerator="Ctrl+C")
        edit_menu.add_command(label="Paste", command=self._cmd_paste, accelerator="Ctrl+V")
        edit_menu.add_command(label="Clear Selection", command=self._cmd_clear, accelerator="Delete")
        edit_menu.add_command(label="Select All", command=self._cmd_select_all, accelerator="Ctrl+A")
        edit_menu.add_separator()
        edit_menu.add_command(label="Fill with Keycolor", command=self._cmd_fill_keycolor)

        # View menu
        view_menu = self._menu(menubar)
        menubar.add_cascade(label="View", menu=view_menu)
        view_menu.add_command(label="Zoom In", command=self._cmd_zoom_in, accelerator="Ctrl++")
        view_menu.add_command(label="Zoom Out", command=self._cmd_zoom_out, accelerator="Ctrl+-")
        view_menu.add_command(label="Fit to Window", command=self._cmd_fit)
        view_menu.add_separator()
        view_menu.add_checkbutton(label="Show Grid", variable=tk.BooleanVar(value=True),
                                   command=self._cmd_toggle_grid)
        view_menu.add_checkbutton(label="Show Keycolor Overlay", variable=tk.BooleanVar(value=True),
                                   command=self._cmd_toggle_keycolor)

        # Sprite menu
        sprite_menu = self._menu(menubar)
        menubar.add_cascade(label="Sprite", menu=sprite_menu)
        sprite_menu.add_cascade(label="Resize...", command=self._cmd_resize)
        sprite_menu.add_separator()
        sprite_menu.add_command(label="Flip Horizontal", command=self._cmd_flip_h)
        sprite_menu.add_command(label="Flip Vertical", command=self._cmd_flip_v)
        sprite_menu.add_command(label="Rotate 90° CW", command=self._cmd_rotate_cw)
        sprite_menu.add_command(label="Rotate 90° CCW", command=self._cmd_rotate_ccw)
        sprite_menu.add_separator()
        sprite_menu.add_command(label="Convert to 8-bit", command=self._cmd_convert_8bit)
        sprite_menu.add_command(label="Convert to 16-bit", command=self._cmd_convert_16bit)

        # Help menu
        help_menu = self._menu(menubar)
        menubar.add_cascade(label="Help", menu=help_menu)
        help_menu.add_command(label="Keyboard Shortcuts", command=self._cmd_shortcuts)
        help_menu.add_command(label="About", command=self._cmd_about)

        # Keyboard bindings
        self.root.bind("<Control-z>", lambda e: self._cmd_undo())
        self.root.bind("<Control-Z>", lambda e: self._cmd_undo())
        self.root.bind("<Control-Shift-z>", lambda e: self._cmd_redo())
        self.root.bind("<Control-Shift-Z>", lambda e: self._cmd_redo())
        self.root.bind("<Control-s>", lambda e: self._cmd_save())
        self.root.bind("<Control-S>", lambda e: self._cmd_save())
        self.root.bind("<Control-o>", lambda e: self._cmd_open())
        self.root.bind("<Control-O>", lambda e: self._cmd_open())
        self.root.bind("<Control-n>", lambda e: self._cmd_new())
        self.root.bind("<Control-N>", lambda e: self._cmd_new())
        self.root.bind("<Control-e>", lambda e: self._cmd_export_header())
        self.root.bind("<Control-E>", lambda e: self._cmd_export_header())
        self.root.bind("<Control-a>", lambda e: self._cmd_select_all())
        self.root.bind("<Control-A>", lambda e: self._cmd_select_all())
        self.root.bind("<Control-x>", lambda e: self._cmd_cut())
        self.root.bind("<Control-X>", lambda e: self._cmd_cut())
        self.root.bind("<Control-c>", lambda e: self._cmd_copy())
        self.root.bind("<Control-C>", lambda e: self._cmd_copy())
        self.root.bind("<Control-v>", lambda e: self._cmd_paste())
        self.root.bind("<Control-V>", lambda e: self._cmd_paste())
        # Zoom via mouse wheel only (Ctrl++/Ctrl+- unreliable on Windows Tkinter)
        self.root.bind("<Delete>", lambda e: self._cmd_clear())
        self.root.bind("<Key-b>", lambda e: self._set_tool("Brush"))
        self.root.bind("<Key-B>", lambda e: self._set_tool("Brush"))
        self.root.bind("<Key-e>", lambda e: self._set_tool("Eraser"))
        self.root.bind("<Key-E>", lambda e: self._set_tool("Eraser"))
        self.root.bind("<Key-g>", lambda e: self._set_tool("Fill"))
        self.root.bind("<Key-G>", lambda e: self._set_tool("Fill"))
        self.root.bind("<Key-i>", lambda e: self._set_tool("Eyedropper"))
        self.root.bind("<Key-I>", lambda e: self._set_tool("Eyedropper"))
        self.root.bind("<Key-p>", lambda e: self._set_tool("Pan"))
        self.root.bind("<Key-P>", lambda e: self._set_tool("Pan"))
        self.root.bind("<Key-m>", lambda e: self._set_tool("Select"))
        self.root.bind("<Key-M>", lambda e: self._set_tool("Select"))

    def _create_toolbar(self):
        toolbar = ttk.Frame(self.root, style="Toolbar.TFrame")
        toolbar.pack(fill=tk.X, padx=8, pady=(2, 4))

        # Tool buttons
        self._tool_btn_frame = ttk.Frame(toolbar)
        self._tool_btn_frame.pack(side=tk.LEFT)

        self._tool_buttons = {}
        self._tool_tips = []
        tool_icons = {
            "Brush": "✎",
            "Eraser": "⌫",
            "Fill": "▧",
            "Eyedropper": "◉",
            "Pan": "✥",
            "Select": "▭",
        }
        for name in ["Brush", "Eraser", "Fill", "Eyedropper", "Pan", "Select"]:
            btn = ttk.Button(
                self._tool_btn_frame,
                text=tool_icons.get(name, "•"),
                command=lambda n=name: self._set_tool(n),
                width=4,
                style="Flat.TButton",
            )
            btn.pack(side=tk.LEFT, padx=1)
            self._tool_buttons[name] = btn
            self._tool_tips.append(_ToolTip(btn, name))

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=6)

        # Color picker
        ttk.Label(toolbar, text="Color:", style="Toolbar.TLabel").pack(side=tk.LEFT, padx=(4, 0))
        self._color_var = tk.StringVar(value=color_to_hex(*self._current_color))
        self._color_entry = ttk.Entry(toolbar, textvariable=self._color_var, width=10)
        self._color_entry.pack(side=tk.LEFT)
        self._color_entry.bind("<Return>", self._on_color_enter)
        self._color_btn = ttk.Button(toolbar, text="Pick", command=self._cmd_pick_color, width=6, style="Flat.TButton")
        self._color_btn.pack(side=tk.LEFT)

        # Keycolor picker
        ttk.Label(toolbar, text="Key:", style="Toolbar.TLabel").pack(side=tk.LEFT, padx=(8, 0))
        self._keycolor_var = tk.StringVar(value=color_to_hex(255, 0, 255))
        self._keycolor_entry = ttk.Entry(toolbar, textvariable=self._keycolor_var, width=10)
        self._keycolor_entry.pack(side=tk.LEFT)
        self._keycolor_entry.bind("<Return>", self._on_keycolor_enter)
        self._keycolor_btn = ttk.Button(toolbar, text="Pick", command=self._cmd_pick_keycolor, width=6, style="Flat.TButton")
        self._keycolor_btn.pack(side=tk.LEFT)

        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=6)

        # Brush size
        ttk.Label(toolbar, text="Size:", style="Toolbar.TLabel").pack(side=tk.LEFT, padx=(4, 0))
        self._brush_var = tk.StringVar(value=str(self._brush_size))
        brush_spin = ttk.Spinbox(toolbar, from_=1, to=16, textvariable=self._brush_var, width=3,
                                 command=self._on_brush_change)
        brush_spin.pack(side=tk.LEFT)

        # Mode toggle
        ttk.Separator(toolbar, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=6)
        self._mode_var = tk.StringVar(value="8-bit")
        mode_combo = ttk.Combobox(toolbar, textvariable=self._mode_var, values=["8-bit", "16-bit"],
                                  state="readonly", width=6)
        mode_combo.pack(side=tk.LEFT)
        mode_combo.bind("<<ComboboxSelected>>", self._on_mode_change)

    def _create_main_pane(self):
        self.paned = ttk.PanedWindow(self.root, orient=tk.HORIZONTAL)
        self.paned.pack(fill=tk.BOTH, expand=True, padx=8, pady=(0, 8))

        # Left panel: palette editor
        self.palette_frame = ttk.LabelFrame(self.paned, text="Palette", style="Group.TLabelframe")
        self.paned.add(self.palette_frame, weight=1)

        # Right panel: canvas
        self.canvas_frame = ttk.LabelFrame(self.paned, text="Canvas", style="Group.TLabelframe")
        self.paned.add(self.canvas_frame, weight=4)

        self.canvas: Optional[PixelCanvas] = None

    def _create_statusbar(self):
        self.statusbar = ttk.Frame(self.root, style="Status.TFrame")
        self.statusbar.pack(fill=tk.X, padx=8, pady=(0, 8))

        self._status_tool = ttk.Label(self.statusbar, text="Tool: Brush", style="Status.TLabel")
        self._status_tool.pack(side=tk.LEFT, padx=4)

        self._status_cursor = ttk.Label(self.statusbar, text="Cursor: 0, 0", style="Status.TLabel")
        self._status_cursor.pack(side=tk.LEFT, padx=4)

        self._status_size = ttk.Label(self.statusbar, text="Size: 32×32", style="Status.TLabel")
        self._status_size.pack(side=tk.LEFT, padx=4)

        self._status_zoom = ttk.Label(self.statusbar, text="Zoom: 8x", style="Status.TLabel")
        self._status_zoom.pack(side=tk.LEFT, padx=4)

        self._status_file = ttk.Label(self.statusbar, text="untitled", style="Status.TLabel")
        self._status_file.pack(side=tk.RIGHT, padx=4)

    # ------------------------------------------------------------------
    # Sprite management
    # ------------------------------------------------------------------

    @staticmethod
    def _mode_label_to_internal(mode_label: str) -> str:
        return "8bit" if mode_label == "8-bit" else "16bit"

    def _new_sprite(self, width: int = 32, height: int = 32):
        """Create a new empty sprite."""
        self.sprite = Sprite(width=width, height=height, mode=self._mode_label_to_internal(self._mode_var.get()))
        self._current_file = None
        self._unsaved = True
        self._history = []
        self._history_idx = -1
        self._push_history()
        self._init_canvas()
        self._update_title()
        self._update_statusbar()

    def _init_canvas(self):
        """Create/recreate the canvas widget for the current sprite."""
        if self.canvas:
            self.canvas.destroy()
        self.canvas = PixelCanvas(self.canvas_frame, self.sprite)
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self._set_tool(self._tool_name)
        fit_w, fit_h = self._get_fit_target_size()
        self.canvas.fit_to_window(fit_w, fit_h)
        # Run a second fit after geometry settles to avoid startup 1:1 zoom.
        self.root.after_idle(self._cmd_fit)

    def _get_fit_target_size(self) -> Tuple[int, int]:
        """Get reliable dimensions for fit-to-window even before full layout."""
        frame_w = self.canvas_frame.winfo_width()
        frame_h = self.canvas_frame.winfo_height()
        canvas_w = self.canvas.winfo_width() if self.canvas else 0
        canvas_h = self.canvas.winfo_height() if self.canvas else 0

        w = max(frame_w, canvas_w)
        h = max(frame_h, canvas_h)

        # Tk can report 1x1 during initial layout; use sensible defaults then.
        if w < 120:
            w = 600
        if h < 120:
            h = 400
        return (w, h)

    # ------------------------------------------------------------------
    # History (undo/redo)
    # ------------------------------------------------------------------

    def _push_history(self):
        """Save current sprite state to the undo stack."""
        if self._history_idx < len(self._history) - 1:
            self._history = self._history[:self._history_idx + 1]
        self._history.append(self.sprite.clone())
        if len(self._history) > self._max_history:
            self._history.pop(0)
        else:
            self._history_idx += 1
        self._unsaved = True

    def _cmd_undo(self):
        if self._history_idx > 0:
            self._history_idx -= 1
            self.sprite = self._history[self._history_idx]
            self._init_canvas()
            self._update_statusbar()

    def _cmd_redo(self):
        if self._history_idx < len(self._history) - 1:
            self._history_idx += 1
            self.sprite = self._history[self._history_idx]
            self._init_canvas()
            self._update_statusbar()

    # ------------------------------------------------------------------
    # Tool management
    # ------------------------------------------------------------------

    def _set_tool(self, name: str):
        self._tool_name = name
        if not self.sprite or not self.canvas:
            return

        if name == "Brush":
            tool = BrushTool(self.sprite, self._current_color, self._brush_size)
        elif name == "Eraser":
            tool = EraserTool(self.sprite, self._brush_size)
        elif name == "Fill":
            tool = FillTool(self.sprite, self._current_color)
        elif name == "Eyedropper":
            tool = EyedropperTool(self.sprite, self._on_eyedropper)
        elif name == "Pan":
            tool = PanTool(self.canvas)
        elif name == "Select":
            tool = SelectTool(self.sprite)
        else:
            return

        self.canvas.set_tool(tool)
        self._status_tool.configure(text=f"Tool: {name}")

        # Highlight active tool button
        for n, btn in self._tool_buttons.items():
            if n == name:
                btn.config(style="ToolActive.TButton")
            else:
                btn.config(style="Flat.TButton")

    def _on_eyedropper(self, color: Tuple[int, int, int]):
        self._current_color = color
        if self.sprite:
            if color in self.sprite.palette:
                self._current_color_idx = self.sprite.palette.index(color)
            else:
                self._current_color_idx = nearest_palette_index(self.sprite.palette, *color)
        self._on_active_color_changed()

    # ------------------------------------------------------------------
    # Color management
    # ------------------------------------------------------------------

    def _on_color_enter(self, event):
        try:
            self._current_color = self._hex_to_rgb(self._color_var.get())
            if self.sprite:
                self._current_color_idx = nearest_palette_index(self.sprite.palette, *self._current_color)
            self._on_active_color_changed()
        except ValueError:
            pass

    def _on_keycolor_enter(self, event):
        try:
            r, g, b = self._hex_to_rgb(self._keycolor_var.get())
            if self.sprite.mode == "8bit":
                self.sprite.keycolor = self.sprite.palette.index((r, g, b)) if (r, g, b) in self.sprite.palette else 0
            else:
                self.sprite.keycolor = rgb888_to_rgb565(r, g, b)
            self._update_keycolor_display()
        except ValueError:
            pass

    def _cmd_pick_color(self):
        dialog = ColorPickerDialog(self.root, self._current_color)
        color = dialog.show()
        if color:
            self._current_color = color
            if self.sprite:
                self._current_color_idx = nearest_palette_index(self.sprite.palette, *self._current_color)
            self._on_active_color_changed()

    def _cmd_pick_keycolor(self):
        dialog = ColorPickerDialog(self.root, (255, 0, 255), "Keycolor Picker")
        color = dialog.show()
        if color:
            r, g, b = color
            if self.sprite.mode == "8bit":
                self.sprite.keycolor = self.sprite.palette.index((r, g, b)) if (r, g, b) in self.sprite.palette else 0
            else:
                self.sprite.keycolor = rgb888_to_rgb565(r, g, b)
            self._update_keycolor_display()

    @staticmethod
    def _hex_to_rgb(hex_str: str) -> Tuple[int, int, int]:
        hex_str = hex_str.lstrip("#")
        return (int(hex_str[0:2], 16), int(hex_str[2:4], 16), int(hex_str[4:6], 16))

    def _update_color_display(self):
        self._color_var.set(color_to_hex(*self._current_color))

    def _update_keycolor_display(self):
        if self.sprite.mode == "8bit":
            r, g, b = self.sprite.palette[self.sprite.keycolor]
        else:
            r, g, b = rgb565_to_rgb888(self.sprite.keycolor)
        self._keycolor_var.set(color_to_hex(r, g, b))

    def _update_palette_display(self):
        """Update the palette editor widget."""
        if not hasattr(self, '_palette_editor'):
            return
        self._palette_editor.update_palette(self.sprite.palette)
        self._palette_editor.set_current_color_idx(self._current_color_idx)
        self._palette_editor.set_keycolor_idx(self.sprite.keycolor if self.sprite.mode == "8bit" else 0)

    def _on_active_color_changed(self):
        """Apply current color changes across UI and active paint tools."""
        self._update_color_display()
        if hasattr(self, '_palette_editor'):
            self._palette_editor.set_current_color_idx(self._current_color_idx)
        if self._tool_name in ("Brush", "Fill"):
            self._set_tool(self._tool_name)

    # ------------------------------------------------------------------
    # File operations
    # ------------------------------------------------------------------

    def _cmd_new(self):
        if self._unsaved and self.sprite:
            if not messagebox.askyesno("New Sprite", "Save changes to current sprite?"):
                pass
        dialog = tk.Toplevel(self.root)
        dialog.title("New Sprite")
        dialog.transient(self.root)
        dialog.grab_set()

        ttk.Label(dialog, text="Width:").grid(row=0, column=0, padx=4, pady=4)
        ttk.Label(dialog, text="Height:").grid(row=0, column=1, padx=4, pady=4)

        preset_var = tk.StringVar(value=f"{self.size_presets[0][0]}x{self.size_presets[0][1]}")
        for i, (w, h) in enumerate(self.size_presets):
            ttk.Radiobutton(
                dialog,
                text=f"{w}×{h}",
                variable=preset_var,
                value=f"{w}x{h}",
            ).grid(row=i + 1, column=0, columnspan=2, padx=4, pady=2, sticky=tk.W)

        # Custom size
        custom_row = len(self.size_presets) + 1
        ttk.Radiobutton(
            dialog,
            text="Custom",
            variable=preset_var,
            value="custom",
        ).grid(row=custom_row, column=0, padx=4, pady=4, sticky=tk.W)
        custom_w = tk.StringVar(value="128")
        custom_h = tk.StringVar(value="128")
        ttk.Entry(dialog, textvariable=custom_w, width=6).grid(row=custom_row, column=1, padx=4, sticky=tk.W)
        ttk.Entry(dialog, textvariable=custom_h, width=6).grid(row=custom_row, column=1, padx=(54, 4), sticky=tk.W)

        def on_ok():
            selected = preset_var.get()
            if selected == "custom":
                w, h = int(custom_w.get()), int(custom_h.get())
            else:
                w_str, h_str = selected.split("x", 1)
                w, h = int(w_str), int(h_str)
            dialog.destroy()
            self._new_sprite(w, h)

        ttk.Button(dialog, text="OK", command=on_ok).grid(row=len(self.size_presets) + 2, column=0, columnspan=2, pady=8)
        ttk.Button(dialog, text="Cancel", command=dialog.destroy).grid(row=len(self.size_presets) + 3, column=0, columnspan=2)

    def _cmd_open(self):
        filepath = filedialog.askopenfilename(
            title="Open PNG",
            filetypes=[("PNG files", "*.png"), ("All files", "*.*")],
        )
        if filepath:
            try:
                self.sprite = import_png_file(filepath)
                self._current_file = filepath
                self._unsaved = False
                self._history = [self.sprite.clone()]
                self._history_idx = 0
                self._init_canvas()
                self._update_title()
                self._update_statusbar()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to open file:\n{e}")

    def _cmd_save(self):
        if self._current_file:
            self._export_png(self._current_file)
            self._unsaved = False
            self._update_title()
        else:
            self._cmd_export_png()

    def _export_png(self, filepath: str):
        export_png(self.sprite, filepath)

    def _cmd_export_png(self):
        filepath = filedialog.asksaveasfilename(
            title="Export PNG",
            defaultextension=".png",
            filetypes=[("PNG files", "*.png"), ("All files", "*.*")],
        )
        if filepath:
            try:
                export_png(self.sprite, filepath)
                self._current_file = filepath
                self._unsaved = False
                self._update_title()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export:\n{e}")

    def _cmd_export_header(self):
        filepath = filedialog.asksaveasfilename(
            title="Export C Header",
            defaultextension=".h",
            filetypes=[("C Header", "*.h"), ("All files", "*.*")],
        )
        if filepath:
            try:
                content = export_c_header(self.sprite)
                with open(filepath, "w") as f:
                    f.write(content)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export:\n{e}")

    def _cmd_export_source(self):
        filepath = filedialog.asksaveasfilename(
            title="Export C Source",
            defaultextension=".c",
            filetypes=[("C Source", "*.c"), ("All files", "*.*")],
        )
        if filepath:
            try:
                content = export_c_source(self.sprite)
                with open(filepath, "w") as f:
                    f.write(content)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export:\n{e}")

    def _cmd_export_binary(self):
        filepath = filedialog.asksaveasfilename(
            title="Export Binary",
            defaultextension=".bin",
            filetypes=[("Binary", "*.bin"), ("All files", "*.*")],
        )
        if filepath:
            try:
                export_binary(self.sprite, filepath)
            except Exception as e:
                messagebox.showerror("Error", f"Failed to export:\n{e}")

    # ------------------------------------------------------------------
    # Edit operations
    # ------------------------------------------------------------------

    def _cmd_cut(self):
        if not self.canvas:
            return
        sel_tool = self.canvas._current_tool
        if isinstance(sel_tool, SelectTool):
            result = sel_tool.cut()
            if result:
                self._clipboard, _ = result
                self._push_history()
                self._unsaved = True

    def _cmd_copy(self):
        if not self.canvas:
            return
        sel_tool = self.canvas._current_tool
        if isinstance(sel_tool, SelectTool):
            self._clipboard = sel_tool.copy()

    def _cmd_paste(self):
        if self._clipboard and self.canvas:
            sel_tool = self.canvas._current_tool
            if isinstance(sel_tool, SelectTool):
                # Paste at center
                ox = (self.sprite.width - self._clipboard.width) // 2
                oy = (self.sprite.height - self._clipboard.height) // 2
                sel_tool.paste(self._clipboard, max(0, ox), max(0, oy))
                self._push_history()
                self._unsaved = True

    def _cmd_clear(self):
        if not self.canvas:
            return
        sel_tool = self.canvas._current_tool
        if isinstance(sel_tool, SelectTool):
            sel_tool.clear_selection()
            self._push_history()
            self._unsaved = True
            self.canvas._render()

    def _cmd_select_all(self):
        if not self.canvas:
            return
        sel_tool = self.canvas._current_tool
        if isinstance(sel_tool, SelectTool):
            sel_tool._selection = (0, 0, self.sprite.width - 1, self.sprite.height - 1)
            self.canvas.set_selection(sel_tool._selection)

    def _cmd_fill_keycolor(self):
        if not self.sprite:
            return
        for y in range(self.sprite.height):
            for x in range(self.sprite.width):
                if self.sprite.mode == "8bit":
                    self.sprite.set_pixel_indexed(x, y, self.sprite.keycolor)
                else:
                    self.sprite.set_pixel_rgb565(x, y, self.sprite.keycolor)
        self._push_history()
        if self.canvas:
            self.canvas._render()

    # ------------------------------------------------------------------
    # View operations
    # ------------------------------------------------------------------

    def _cmd_zoom_in(self):
        if self.canvas:
            self.canvas.zoom_at(1.3,
                                self.canvas.winfo_width() // 2,
                                self.canvas.winfo_height() // 2)
            self._status_zoom.configure(text=f"Zoom: {self.canvas.zoom}x")

    def _cmd_zoom_out(self):
        if self.canvas:
            self.canvas.zoom_at(0.7,
                                self.canvas.winfo_width() // 2,
                                self.canvas.winfo_height() // 2)
            self._status_zoom.configure(text=f"Zoom: {self.canvas.zoom}x")

    def _cmd_fit(self):
        if self.canvas:
            fit_w, fit_h = self._get_fit_target_size()
            self.canvas.fit_to_window(
                fit_w,
                fit_h,
            )
            self._status_zoom.configure(text=f"Zoom: {self.canvas.zoom}x")

    def _cmd_toggle_grid(self):
        if self.canvas:
            self.canvas.toggle_grid()

    def _cmd_toggle_keycolor(self):
        if self.canvas:
            self.canvas.toggle_keycolor()

    # ------------------------------------------------------------------
    # Sprite operations
    # ------------------------------------------------------------------

    def _cmd_resize(self):
        dialog = tk.Toplevel(self.root)
        dialog.title("Resize Sprite")
        dialog.transient(self.root)
        dialog.grab_set()

        ttk.Label(dialog, text="Width:").grid(row=0, column=0, padx=4, pady=4)
        ttk.Label(dialog, text="Height:").grid(row=0, column=1, padx=4, pady=4)

        w_var = tk.StringVar(value=str(self.sprite.width))
        h_var = tk.StringVar(value=str(self.sprite.height))
        ttk.Entry(dialog, textvariable=w_var, width=8).grid(row=0, column=0, padx=4)
        ttk.Entry(dialog, textvariable=h_var, width=8).grid(row=0, column=1, padx=4)

        def on_ok():
            try:
                new_w = max(1, min(256, int(w_var.get())))
                new_h = max(1, min(256, int(h_var.get())))
                self.sprite.resize(new_w, new_h)
                self._init_canvas()
                self._update_statusbar()
                self._push_history()
                self._unsaved = True
            except ValueError:
                pass
            dialog.destroy()

        ttk.Button(dialog, text="OK", command=on_ok).grid(row=1, column=0, columnspan=2, pady=8)
        ttk.Button(dialog, text="Cancel", command=dialog.destroy).grid(row=2, column=0, columnspan=2)

    def _cmd_flip_h(self):
        if self.sprite:
            self.sprite.flip_horizontal()
            self._push_history()
            if self.canvas:
                self.canvas._render()

    def _cmd_flip_v(self):
        if self.sprite:
            self.sprite.flip_vertical()
            self._push_history()
            if self.canvas:
                self.canvas._render()

    def _cmd_rotate_cw(self):
        if self.sprite:
            self.sprite.rotate_90_cw()
            self._push_history()
            self._init_canvas()
            self._update_statusbar()

    def _cmd_rotate_ccw(self):
        if self.sprite:
            self.sprite.rotate_90_ccw()
            self._push_history()
            self._init_canvas()
            self._update_statusbar()

    def _cmd_convert_8bit(self):
        if self.sprite and self.sprite.mode != "8bit":
            self.sprite.mode = "8bit"
            self._mode_var.set("8-bit")
            self._init_canvas()
            self._update_keycolor_display()
            self._update_palette_display()
            self._push_history()

    def _cmd_convert_16bit(self):
        if self.sprite and self.sprite.mode != "16bit":
            self.sprite.mode = "16bit"
            self._mode_var.set("16-bit")
            self._init_canvas()
            self._update_keycolor_display()
            self._push_history()

    def _on_brush_change(self):
        try:
            self._brush_size = max(1, min(16, int(self._brush_var.get())))
            if self.canvas and self.canvas._current_tool:
                self._set_tool(self._tool_name)
        except ValueError:
            pass

    def _on_mode_change(self, event=None):
        if self.sprite:
            new_mode = self._mode_label_to_internal(self._mode_var.get())
            if new_mode != self.sprite.mode:
                if messagebox.askyesno("Convert Mode",
                                       f"Convert to {new_mode}? This will re-quantize all pixels."):
                    self.sprite.mode = new_mode
                    self._init_canvas()
                    self._update_keycolor_display()
                    self._push_history()

    # ------------------------------------------------------------------
    # Help
    # ------------------------------------------------------------------

    def _cmd_shortcuts(self):
        shortcuts = (
            "Keyboard Shortcuts\n"
            "==================\n\n"
            "Tools:    B=Brush, E=Eraser, G=Fill, I=Eyedropper, P=Pan, M=Select\n"
            "File:     Ctrl+N=New, Ctrl+O=Open, Ctrl+S=Save, Ctrl+E=Export Header\n"
            "Edit:     Ctrl+Z=Undo, Ctrl+Shift+Z=Redo\n"
            "          Ctrl+X=Cut, Ctrl+C=Copy, Ctrl+V=Paste\n"
            "          Ctrl+A=Select All, Delete=Clear\n"
            "View:     Ctrl++=Zoom In, Ctrl+-=Zoom Out\n"
        )
        messagebox.showinfo("Keyboard Shortcuts", shortcuts)

    def _cmd_about(self):
        messagebox.showinfo("About",
                           "Sandpiper Sprite Editor v1.0\n"
                           "Create and edit sprites for the Sandpiper VPU.\n"
                           "Supports 8-bit palette and 16-bit RGB565 formats.")

    def _cmd_exit(self):
        if messagebox.askyesno("Exit", "Save changes before exiting?"):
            self._cmd_save()
        self.root.destroy()

    # ------------------------------------------------------------------
    # UI updates
    # ------------------------------------------------------------------

    def _update_title(self):
        name = os.path.basename(self._current_file) if self._current_file else "untitled"
        unsaved = " *" if self._unsaved else ""
        mode = self._mode_var.get()
        self.root.title(f"Sandpiper Sprite Editor — {name} [{mode}]{unsaved}")

    def _update_statusbar(self):
        if self.sprite:
            self._status_size.configure(text=f"Size: {self.sprite.width}×{self.sprite.height}")
            self._status_file.configure(text=os.path.basename(self._current_file) if self._current_file else "untitled")
        if self.canvas:
            self._status_zoom.configure(text=f"Zoom: {self.canvas.zoom}x")

    # ------------------------------------------------------------------
    # Run
    # ------------------------------------------------------------------

    def run(self):
        """Start the application main loop."""
        # Initialize palette editor after canvas
        self.root.after(100, self._init_palette_editor)
        self.root.mainloop()

    def _init_palette_editor(self):
        """Create the palette editor widget after the main window is laid out."""
        if hasattr(self, '_palette_editor'):
            return
        self._palette_editor = PaletteEditor(
            self.palette_frame,
            palette=self.sprite.palette,
            current_color_idx=self._current_color_idx,
            keycolor_idx=self.sprite.keycolor if self.sprite.mode == "8bit" else 0,
            on_color_select=self._on_palette_color_select,
            on_color_change=self._on_palette_color_change,
            on_keycolor_select=self._on_palette_keycolor_select,
        )
        self._palette_editor.pack(fill=tk.BOTH, expand=True)

    def _on_palette_color_select(self, idx: int, color: Tuple[int, int, int]):
        self._current_color_idx = idx
        self._current_color = color
        self._on_active_color_changed()

    def _on_palette_color_change(self, idx: int, color: Tuple[int, int, int]):
        from .color_picker import PaletteColorPickerDialog

        def on_apply(pal_idx, new_color):
            self.sprite.palette[pal_idx] = new_color
            self._update_palette_display()
            if self.canvas:
                self.canvas._render()
            self._unsaved = True
            self._push_history()

        dialog = PaletteColorPickerDialog(self.root, idx, color, on_apply)
        dialog.show()

    def _on_palette_keycolor_select(self, idx: int):
        if self.sprite.mode == "8bit":
            self.sprite.keycolor = idx
