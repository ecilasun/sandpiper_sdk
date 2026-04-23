#!/usr/bin/env python3
"""Sandpiper Sprite Editor — entry point.

Usage:
    python sprite_editor.py
    python sprite_editor.py image.png   # open an image on startup
"""

import sys
import os

# Ensure the package directory is on the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from sprite_editor.app import SpriteEditorApp
from sprite_editor.file_io import import_png_file


def main():
    app = SpriteEditorApp()

    # If an image was passed as argument, open it
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
        if os.path.isfile(filepath):
            try:
                app.sprite = import_png_file(filepath)
                app._current_file = filepath
                app._unsaved = False
                app._history = [app.sprite.clone()]
                app._history_idx = 0
                app._init_canvas()
                app._update_title()
                app._update_statusbar()
            except Exception as e:
                print(f"Error opening {filepath}: {e}", file=sys.stderr)

    app.run()


if __name__ == "__main__":
    main()
