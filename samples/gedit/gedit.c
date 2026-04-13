/**
 * \file gedit.c
 * \brief Simple graphics text editor sample
 *
 * This example demonstrates a simple text editor using the graphics capabilities
 * of the Sandpiper platform in 640x480x16bpp mode.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

#include "core.h"
#include "platform.h"
#include "vpu.h"

// Editor configuration
#define EDITOR_WIDTH      640
#define EDITOR_HEIGHT     480
#define EDITOR_MODE       EVM_640_480
#define EDITOR_COLOR      ECM_16bit_RGB
#define EDITOR_FG_COLOR   MAKECOLORRGB16(0, 0, 0)     // Black
#define EDITOR_BG_COLOR   MAKECOLORRGB16(31, 63, 31)  // White
#define EDITOR_CURSOR_COLOR MAKECOLORRGB16(31, 32, 0)   // Orange
#define EDITOR_SELECT_COLOR MAKECOLORRGB16(0, 0, 31)   // Blue

// Character dimensions (built-in RGB565 font is 8x8)
#define CHAR_WIDTH  8
#define CHAR_HEIGHT 8
#define CHARS_PER_LINE  (EDITOR_WIDTH / CHAR_WIDTH)   // 80 chars
#define LINES_PER_SCREEN (EDITOR_HEIGHT / CHAR_HEIGHT) // 60 lines
#define TEXT_LINES_PER_SCREEN (LINES_PER_SCREEN - 1)

#define STATUS_FG_COLOR MAKECOLORRGB16(31, 63, 31)
#define STATUS_BG_COLOR MAKECOLORRGB16(0, 0, 0)

#define DIALOG_MAX_ENTRIES 256
#define DIALOG_MAX_NAME_LEN 128
#define DIALOG_MAX_PATH_LEN 256

#define COLOR_DIALOG_BG MAKECOLORRGB16(26, 52, 26)
#define COLOR_DIALOG_BORDER MAKECOLORRGB16(8, 16, 8)
#define COLOR_DIALOG_TITLE_BG MAKECOLORRGB16(0, 0, 20)
#define COLOR_DIALOG_TITLE_FG MAKECOLORRGB16(31, 63, 31)
#define COLOR_DIALOG_LIST_BG MAKECOLORRGB16(31, 63, 31)
#define COLOR_DIALOG_LIST_FG MAKECOLORRGB16(0, 0, 0)
#define COLOR_DIALOG_SEL_BG MAKECOLORRGB16(0, 0, 22)
#define COLOR_DIALOG_SEL_FG MAKECOLORRGB16(31, 63, 31)

// Maximum text buffer size
#define MAX_BUFFER_SIZE  (CHARS_PER_LINE * LINES_PER_SCREEN * 2) // Double buffer for safety

struct SPPlatform* g_platform = NULL;
struct SPSizeAlloc g_framebuffer_a;
struct SPSizeAlloc g_framebuffer_b;

// Simple text editor state
typedef struct {
    char name[DIALOG_MAX_NAME_LEN];
    bool is_dir;
} FileEntry;

typedef enum {
    DIALOG_NONE = 0,
    DIALOG_OPEN,
    DIALOG_SAVE,
    DIALOG_HELP
} DialogMode;

typedef enum {
    DIALOG_FOCUS_LIST = 0,
    DIALOG_FOCUS_FILENAME,
    DIALOG_FOCUS_OK,
    DIALOG_FOCUS_CANCEL
} DialogFocus;

typedef struct {
    char* buffer;
    size_t buffer_size;
    size_t cursor_pos;
    size_t scroll_offset;
    bool insert_mode;
    char file_name[128];
    char status_msg[128];

    DialogMode dialog_mode;
    char dialog_path[DIALOG_MAX_PATH_LEN];
    char dialog_filename[DIALOG_MAX_NAME_LEN];
    FileEntry dialog_entries[DIALOG_MAX_ENTRIES];
    int dialog_entry_count;
    int dialog_selected;
    int dialog_scroll;
    DialogFocus dialog_focus;
    uint32_t cursor_blink_ticks;
} EditorState;

EditorState editor;
static struct termios g_orig_termios;
static bool g_raw_mode_enabled = false;
static void editor_set_file_name(const char* path);
static bool dialog_activate_selected(void);
static bool dialog_commit_save(void);

enum EditorKey {
    KEY_NONE = -1,
    KEY_BACKSPACE = 127,
    KEY_ARROW_LEFT = 1000,
    KEY_ARROW_RIGHT,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_F1
};

static void disable_raw_mode(void) {
    if (g_raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_orig_termios);
        g_raw_mode_enabled = false;
    }
}

static bool path_is_directory(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
}

static int dialog_entry_compare(const void* a, const void* b) {
    const FileEntry* ea = (const FileEntry*)a;
    const FileEntry* eb = (const FileEntry*)b;

    if (ea->is_dir != eb->is_dir) {
        return ea->is_dir ? -1 : 1;
    }
    return strcasecmp(ea->name, eb->name);
}

static void editor_set_status_message(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(editor.status_msg, sizeof(editor.status_msg), fmt, args);
    va_end(args);
}

static void build_path(char* out, size_t out_size, const char* dir, const char* name) {
    if (!dir || dir[0] == '\0') {
        snprintf(out, out_size, "%s", name ? name : "");
        return;
    }

    size_t len = strlen(dir);
    if (len > 0 && dir[len - 1] == '/') {
        snprintf(out, out_size, "%s%s", dir, name ? name : "");
    } else {
        snprintf(out, out_size, "%s/%s", dir, name ? name : "");
    }
}

static void dialog_refresh_entries(void) {
    editor.dialog_entry_count = 0;
    editor.dialog_selected = 0;
    editor.dialog_scroll = 0;

    DIR* dir = opendir(editor.dialog_path);
    if (!dir) {
        editor_set_status_message("Cannot open directory: %s", strerror(errno));
        return;
    }

    if (strcmp(editor.dialog_path, "/") != 0 && strcmp(editor.dialog_path, ".") != 0) {
        FileEntry* up = &editor.dialog_entries[editor.dialog_entry_count++];
        strncpy(up->name, "..", sizeof(up->name) - 1);
        up->name[sizeof(up->name) - 1] = '\0';
        up->is_dir = true;
    }

    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0) {
            continue;
        }
        if (editor.dialog_entry_count >= DIALOG_MAX_ENTRIES) {
            break;
        }

        FileEntry* entry = &editor.dialog_entries[editor.dialog_entry_count];
        strncpy(entry->name, ent->d_name, sizeof(entry->name) - 1);
        entry->name[sizeof(entry->name) - 1] = '\0';

        char full_path[DIALOG_MAX_PATH_LEN];
        build_path(full_path, sizeof(full_path), editor.dialog_path, entry->name);
        entry->is_dir = path_is_directory(full_path);
        editor.dialog_entry_count++;
    }

    closedir(dir);

    qsort(editor.dialog_entries, (size_t)editor.dialog_entry_count, sizeof(FileEntry), dialog_entry_compare);
}

static void editor_clear_buffer(void) {
    editor.buffer_size = 0;
    editor.cursor_pos = 0;
    if (editor.buffer) {
        editor.buffer[0] = '\0';
    }
}

static bool editor_load_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        editor_set_status_message("Open failed: %s", strerror(errno));
        return false;
    }

    editor_clear_buffer();

    size_t write_pos = 0;
    int c;
    while ((c = fgetc(f)) != EOF && write_pos < MAX_BUFFER_SIZE - 1) {
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            size_t next_line = ((write_pos / CHARS_PER_LINE) + 1) * CHARS_PER_LINE;
            write_pos = next_line;
            continue;
        }
        if (isprint((unsigned char)c) || c == '\t') {
            editor.buffer[write_pos++] = (char)c;
        }
    }

    fclose(f);

    editor.buffer_size = write_pos;
    editor.cursor_pos = (editor.buffer_size > 0) ? (editor.buffer_size - 1) : 0;
    editor.buffer[editor.buffer_size] = '\0';
    editor_set_file_name(path);
    editor_set_status_message("Opened %s", editor.file_name);
    return true;
}

static bool editor_save_file(const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        editor_set_status_message("Save failed: %s", strerror(errno));
        return false;
    }

    size_t total_lines = (editor.buffer_size + CHARS_PER_LINE - 1) / CHARS_PER_LINE;
    if (total_lines == 0) {
        total_lines = 1;
    }

    for (size_t line = 0; line < total_lines; line++) {
        size_t start = line * CHARS_PER_LINE;
        size_t line_len = 0;

        if (start < editor.buffer_size) {
            line_len = editor.buffer_size - start;
            if (line_len > CHARS_PER_LINE) {
                line_len = CHARS_PER_LINE;
            }
            while (line_len > 0 && editor.buffer[start + line_len - 1] == ' ') {
                line_len--;
            }
        }

        if (line_len > 0) {
            fwrite(&editor.buffer[start], 1, line_len, f);
        }
        fputc('\n', f);
    }

    fclose(f);
    editor_set_file_name(path);
    editor_set_status_message("Saved %s", editor.file_name);
    return true;
}

static void dialog_open(DialogMode mode) {
    editor.dialog_mode = mode;
    editor.dialog_focus = DIALOG_FOCUS_LIST;
    if (getcwd(editor.dialog_path, sizeof(editor.dialog_path)) == NULL) {
        strncpy(editor.dialog_path, ".", sizeof(editor.dialog_path) - 1);
        editor.dialog_path[sizeof(editor.dialog_path) - 1] = '\0';
    }

    if (mode == DIALOG_SAVE) {
        strncpy(editor.dialog_filename, editor.file_name, sizeof(editor.dialog_filename) - 1);
        editor.dialog_filename[sizeof(editor.dialog_filename) - 1] = '\0';
    } else {
        editor.dialog_filename[0] = '\0';
    }

    dialog_refresh_entries();
}

static void dialog_close(void) {
    editor.dialog_mode = DIALOG_NONE;
}

static void dialog_open_help(void) {
    editor.dialog_mode = DIALOG_HELP;
}

static void dialog_move_selection(int delta) {
    if (editor.dialog_entry_count <= 0) {
        return;
    }

    editor.dialog_selected += delta;
    if (editor.dialog_selected < 0) {
        editor.dialog_selected = 0;
    }
    if (editor.dialog_selected >= editor.dialog_entry_count) {
        editor.dialog_selected = editor.dialog_entry_count - 1;
    }
}

static void dialog_focus_next(void) {
    if (editor.dialog_mode == DIALOG_HELP) {
        return;
    }

    editor.dialog_focus = (DialogFocus)(((int)editor.dialog_focus + 1) % 4);
}

static bool dialog_open_from_filename(void) {
    if (editor.dialog_filename[0] == '\0') {
        return false;
    }

    char full_path[DIALOG_MAX_PATH_LEN];
    build_path(full_path, sizeof(full_path), editor.dialog_path, editor.dialog_filename);

    if (path_is_directory(full_path)) {
        strncpy(editor.dialog_path, full_path, sizeof(editor.dialog_path) - 1);
        editor.dialog_path[sizeof(editor.dialog_path) - 1] = '\0';
        editor.dialog_filename[0] = '\0';
        dialog_refresh_entries();
        return false;
    }

    if (editor_load_file(full_path)) {
        dialog_close();
        return true;
    }

    return false;
}

static void dialog_select_file_name_from_entry(void) {
    if (editor.dialog_selected < 0 || editor.dialog_selected >= editor.dialog_entry_count) {
        return;
    }

    FileEntry* sel = &editor.dialog_entries[editor.dialog_selected];
    if (!sel->is_dir) {
        strncpy(editor.dialog_filename, sel->name, sizeof(editor.dialog_filename) - 1);
        editor.dialog_filename[sizeof(editor.dialog_filename) - 1] = '\0';
    }
}

static bool dialog_activate_ok(void) {
    if (editor.dialog_mode == DIALOG_OPEN) {
        if (editor.dialog_filename[0] != '\0') {
            return dialog_open_from_filename();
        }
        return dialog_activate_selected();
    }

    if (editor.dialog_mode == DIALOG_SAVE) {
        if (editor.dialog_filename[0] == '\0') {
            dialog_select_file_name_from_entry();
        }
        return dialog_commit_save();
    }

    return false;
}

static bool dialog_activate_selected(void) {
    if (editor.dialog_selected < 0 || editor.dialog_selected >= editor.dialog_entry_count) {
        return false;
    }

    FileEntry* sel = &editor.dialog_entries[editor.dialog_selected];
    char full_path[DIALOG_MAX_PATH_LEN];
    build_path(full_path, sizeof(full_path), editor.dialog_path, sel->name);

    if (sel->is_dir) {
        if (strcmp(sel->name, "..") == 0) {
            char* slash = strrchr(editor.dialog_path, '/');
            if (slash && slash != editor.dialog_path) {
                *slash = '\0';
            } else {
                strncpy(editor.dialog_path, ".", sizeof(editor.dialog_path) - 1);
                editor.dialog_path[sizeof(editor.dialog_path) - 1] = '\0';
            }
        } else {
            strncpy(editor.dialog_path, full_path, sizeof(editor.dialog_path) - 1);
            editor.dialog_path[sizeof(editor.dialog_path) - 1] = '\0';
        }
        dialog_refresh_entries();
        return false;
    }

    if (editor.dialog_mode == DIALOG_OPEN) {
        if (editor_load_file(full_path)) {
            dialog_close();
            return true;
        }
    } else if (editor.dialog_mode == DIALOG_SAVE) {
        strncpy(editor.dialog_filename, sel->name, sizeof(editor.dialog_filename) - 1);
        editor.dialog_filename[sizeof(editor.dialog_filename) - 1] = '\0';
    }

    return false;
}

static bool dialog_commit_save(void) {
    if (editor.dialog_filename[0] == '\0') {
        editor_set_status_message("Save cancelled: empty filename");
        return false;
    }

    char full_path[DIALOG_MAX_PATH_LEN];
    build_path(full_path, sizeof(full_path), editor.dialog_path, editor.dialog_filename);
    if (editor_save_file(full_path)) {
        dialog_close();
        return true;
    }
    return false;
}

static bool enable_raw_mode(void) {
    if (tcgetattr(STDIN_FILENO, &g_orig_termios) == -1) {
        return false;
    }

    struct termios raw = g_orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        return false;
    }

    g_raw_mode_enabled = true;
    atexit(disable_raw_mode);
    return true;
}

static int editor_read_key(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) {
        return KEY_NONE;
    }

    if (c == '\x1b') {
        char seq[4];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return KEY_HOME;
                        case '2': return KEY_INSERT;
                        case '3': return KEY_DELETE;
                        case '4': return KEY_END;
                        case '7': return KEY_HOME;
                        case '8': return KEY_END;
                    }
                }
                if (seq[1] == '1' && seq[2] == '1') {
                    if (read(STDIN_FILENO, &seq[3], 1) == 1 && seq[3] == '~') {
                        return KEY_F1;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return KEY_ARROW_UP;
                    case 'B': return KEY_ARROW_DOWN;
                    case 'C': return KEY_ARROW_RIGHT;
                    case 'D': return KEY_ARROW_LEFT;
                    case 'H': return KEY_HOME;
                    case 'F': return KEY_END;
                }
            }
        } else if (seq[0] == 'O') {
            if (seq[1] == 'P') {
                return KEY_F1;
            }
        }

        return '\x1b';
    }

    return c;
}

static size_t editor_line_length(size_t line) {
    size_t line_start = line * CHARS_PER_LINE;
    if (line_start >= editor.buffer_size) {
        return 0;
    }

    size_t remaining = editor.buffer_size - line_start;
    return remaining > CHARS_PER_LINE ? CHARS_PER_LINE : remaining;
}

static void editor_set_file_name(const char* path) {
    if (!path || path[0] == '\0') {
        strncpy(editor.file_name, "untitled", sizeof(editor.file_name) - 1);
        editor.file_name[sizeof(editor.file_name) - 1] = '\0';
        return;
    }

    const char* slash = strrchr(path, '/');
    const char* backslash = strrchr(path, '\\');
    const char* base = path;

    if (slash && backslash) {
        base = (slash > backslash) ? (slash + 1) : (backslash + 1);
    } else if (slash) {
        base = slash + 1;
    } else if (backslash) {
        base = backslash + 1;
    }

    if (base[0] == '\0') {
        base = path;
    }

    strncpy(editor.file_name, base, sizeof(editor.file_name) - 1);
    editor.file_name[sizeof(editor.file_name) - 1] = '\0';
}

// Initialize editor state
void editor_init() {
    editor.buffer = (char*)malloc(MAX_BUFFER_SIZE);
    editor.buffer_size = 0;
    editor.cursor_pos = 0;
    editor.scroll_offset = 0;
    editor.insert_mode = true;
    editor.status_msg[0] = '\0';
    editor.dialog_mode = DIALOG_NONE;
    editor.dialog_filename[0] = '\0';
    editor.dialog_path[0] = '\0';
    editor.dialog_entry_count = 0;
    editor.dialog_selected = 0;
    editor.dialog_scroll = 0;
    editor.dialog_focus = DIALOG_FOCUS_LIST;
    editor.cursor_blink_ticks = 0;
    editor_set_file_name(NULL);
    
    // Initialize with empty buffer
    if (editor.buffer) {
        editor.buffer[0] = '\0';
    }
}

// Free editor resources
void editor_cleanup() {
    if (editor.buffer) {
        free(editor.buffer);
        editor.buffer = NULL;
    }
}

// Insert character at cursor position
void editor_insert_char(char c) {
    if (!editor.buffer || editor.buffer_size >= MAX_BUFFER_SIZE - 1) {
        return;
    }
    
    // Handle newline
    if (c == '\n') {
        // Move to next line
        size_t line_start = (editor.cursor_pos / CHARS_PER_LINE) * CHARS_PER_LINE;
        editor.cursor_pos = line_start + CHARS_PER_LINE;
        if (editor.cursor_pos > editor.buffer_size) {
            editor.cursor_pos = editor.buffer_size;
        }
        return;
    }
    
    // Overwrite existing character when in overwrite mode.
    if (!editor.insert_mode && editor.cursor_pos < editor.buffer_size) {
        editor.buffer[editor.cursor_pos] = c;
        editor.cursor_pos++;
        return;
    }

    // Shift characters to make space
    for (size_t i = editor.buffer_size; i > editor.cursor_pos; i--) {
        editor.buffer[i] = editor.buffer[i-1];
    }
    
    // Insert character
    editor.buffer[editor.cursor_pos] = c;
    editor.buffer_size++;
    editor.cursor_pos++;
    editor.buffer[editor.buffer_size] = '\0';
}

// Delete character before cursor
void editor_delete_char() {
    if (!editor.buffer || editor.cursor_pos == 0) {
        return;
    }
    
    // Shift characters left
    for (size_t i = editor.cursor_pos; i < editor.buffer_size; i++) {
        editor.buffer[i-1] = editor.buffer[i];
    }
    
    editor.buffer_size--;
    editor.cursor_pos--;
    editor.buffer[editor.buffer_size] = '\0';
}

// Move cursor left
void editor_cursor_left() {
    if (editor.cursor_pos > 0) {
        editor.cursor_pos--;
    }
}

// Move cursor right
void editor_cursor_right() {
    if (editor.cursor_pos < editor.buffer_size) {
        editor.cursor_pos++;
    }
}

// Move cursor up
void editor_cursor_up() {
    size_t line = editor.cursor_pos / CHARS_PER_LINE;
    size_t col = editor.cursor_pos % CHARS_PER_LINE;

    if (line == 0) {
        editor.cursor_pos = 0;
        return;
    }

    size_t prev_line = line - 1;
    size_t prev_line_len = editor_line_length(prev_line);
    if (col > prev_line_len) {
        col = prev_line_len;
    }

    editor.cursor_pos = prev_line * CHARS_PER_LINE + col;
}

// Move cursor down
void editor_cursor_down() {
    size_t line = editor.cursor_pos / CHARS_PER_LINE;
    size_t col = editor.cursor_pos % CHARS_PER_LINE;
    size_t next_line = line + 1;
    size_t next_line_start = next_line * CHARS_PER_LINE;

    if (next_line_start >= editor.buffer_size) {
        editor.cursor_pos = editor.buffer_size;
        return;
    }

    size_t next_line_len = editor_line_length(next_line);
    if (col > next_line_len) {
        col = next_line_len;
    }

    editor.cursor_pos = next_line_start + col;
}

void editor_cursor_home() {
    size_t line = editor.cursor_pos / CHARS_PER_LINE;
    editor.cursor_pos = line * CHARS_PER_LINE;
}

void editor_cursor_end() {
    size_t line = editor.cursor_pos / CHARS_PER_LINE;
    size_t line_start = line * CHARS_PER_LINE;
    size_t line_len = editor_line_length(line);
    editor.cursor_pos = line_start + line_len;
}

void editor_delete_at_cursor() {
    if (!editor.buffer || editor.cursor_pos >= editor.buffer_size) {
        return;
    }

    for (size_t i = editor.cursor_pos + 1; i <= editor.buffer_size; i++) {
        editor.buffer[i - 1] = editor.buffer[i];
    }

    editor.buffer_size--;
}

static bool editor_process_key(int key) {
    if (editor.dialog_mode != DIALOG_NONE) {
        switch (key) {
            case '\x1b':
            case KEY_F1:
            case '~':
                dialog_close();
                return true;
            case '\t':
                dialog_focus_next();
                return true;
            case KEY_ARROW_UP:
                if (editor.dialog_mode == DIALOG_HELP) {
                    return true;
                }
                if (editor.dialog_focus == DIALOG_FOCUS_LIST) {
                    dialog_move_selection(-1);
                }
                return true;
            case KEY_ARROW_DOWN:
                if (editor.dialog_mode == DIALOG_HELP) {
                    return true;
                }
                if (editor.dialog_focus == DIALOG_FOCUS_LIST) {
                    dialog_move_selection(1);
                }
                return true;
            case '\r':
            case '\n':
                if (editor.dialog_mode == DIALOG_HELP) {
                    dialog_close();
                    return true;
                }
                if (editor.dialog_focus == DIALOG_FOCUS_CANCEL) {
                    dialog_close();
                } else if (editor.dialog_focus == DIALOG_FOCUS_OK) {
                    dialog_activate_ok();
                } else if (editor.dialog_focus == DIALOG_FOCUS_LIST) {
                    dialog_activate_selected();
                } else {
                    dialog_activate_ok();
                }
                return true;
            case KEY_BACKSPACE:
            case 8:
                if (editor.dialog_focus == DIALOG_FOCUS_FILENAME) {
                    size_t n = strlen(editor.dialog_filename);
                    if (n > 0) {
                        editor.dialog_filename[n - 1] = '\0';
                    }
                }
                return true;
            default:
                if (editor.dialog_focus == DIALOG_FOCUS_FILENAME && key >= 32 && key <= 126) {
                    size_t n = strlen(editor.dialog_filename);
                    if (n + 1 < sizeof(editor.dialog_filename)) {
                        editor.dialog_filename[n] = (char)key;
                        editor.dialog_filename[n + 1] = '\0';
                    }
                }
                return true;
        }
    }

    switch (key) {
        case '\x1b':
            return false;
        case KEY_F1:
        case '~':
            dialog_open_help();
            break;
        case 15:
            dialog_open(DIALOG_OPEN);
            break;
        case 19:
            dialog_open(DIALOG_SAVE);
            break;
        case '\r':
        case '\n':
            editor_insert_char('\n');
            break;
        case KEY_BACKSPACE:
        case 8:
            editor_delete_char();
            break;
        case KEY_DELETE:
            editor_delete_at_cursor();
            break;
        case KEY_INSERT:
            editor.insert_mode = !editor.insert_mode;
            break;
        case KEY_ARROW_LEFT:
            editor_cursor_left();
            break;
        case KEY_ARROW_RIGHT:
            editor_cursor_right();
            break;
        case KEY_ARROW_UP:
            editor_cursor_up();
            break;
        case KEY_ARROW_DOWN:
            editor_cursor_down();
            break;
        case KEY_HOME:
            editor_cursor_home();
            break;
        case KEY_END:
            editor_cursor_end();
            break;
        default:
            if (key >= 32 && key <= 126) {
                editor_insert_char((char)key);
            }
            break;
    }

    return true;
}

static void editor_draw_status_line(uint16_t* fb, uint32_t stride, size_t line, size_t column) {
    char status[CHARS_PER_LINE + 1];
    const char* mode_name = editor.insert_mode ? "INS" : "OVR";

    int len = snprintf(
        status,
        sizeof(status),
        " %s | %s | Ln %zu Col %zu | %s | Fn+ESC for help | %s ",
        "gedit",
        editor.file_name,
        line + 1,
        column + 1,
        mode_name,
        editor.status_msg
    );

    if (len < 0) {
        return;
    }

    if (len > CHARS_PER_LINE) {
        len = CHARS_PER_LINE;
    }

    for (int i = len; i < CHARS_PER_LINE; i++) {
        status[i] = ' ';
    }
    status[CHARS_PER_LINE] = '\0';

    VPUPrintStringRGB565(
        (uint8_t*)fb,
        stride,
        EDITOR_WIDTH,
        EDITOR_HEIGHT,
        0,
        (uint16_t)(TEXT_LINES_PER_SCREEN * CHAR_HEIGHT),
        status,
        STATUS_FG_COLOR,
        STATUS_BG_COLOR,
        CHARS_PER_LINE
    );
}

static void fill_rect16(uint16_t* fb, uint32_t stride, int x, int y, int w, int h, uint16_t color) {
    if (w <= 0 || h <= 0) {
        return;
    }

    int max_w = EDITOR_WIDTH;
    int max_h = EDITOR_HEIGHT;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (x + w > max_w) {
        w = max_w - x;
    }
    if (y + h > max_h) {
        h = max_h - y;
    }
    if (w <= 0 || h <= 0) {
        return;
    }

    int stride_px = (int)(stride / sizeof(uint16_t));
    for (int yy = 0; yy < h; yy++) {
        uint16_t* row = fb + (y + yy) * stride_px + x;
        for (int xx = 0; xx < w; xx++) {
            row[xx] = color;
        }
    }
}

static void draw_text_band(uint16_t* fb, uint32_t stride, int x, int y, int char_count, uint16_t fg, uint16_t bg) {
    if (char_count <= 0) {
        return;
    }

    char blanks[CHARS_PER_LINE + 1];
    if (char_count > CHARS_PER_LINE) {
        char_count = CHARS_PER_LINE;
    }

    for (int i = 0; i < char_count; i++) {
        blanks[i] = ' ';
    }
    blanks[char_count] = '\0';

    VPUPrintStringRGB565(
        (uint8_t*)fb,
        stride,
        EDITOR_WIDTH,
        EDITOR_HEIGHT,
        (uint16_t)x,
        (uint16_t)y,
        blanks,
        fg,
        bg,
        char_count
    );
}

static void editor_draw_dialog(uint16_t* fb, uint32_t stride, bool cursor_visible) {
    if (editor.dialog_mode == DIALOG_NONE) {
        return;
    }

    const int dlg_x = 72;
    const int dlg_y = 40;
    const int dlg_w = 496;
    const int dlg_h = 392;
    const int title_h = 16;
    const int list_x = dlg_x + 8;
    const int list_y = dlg_y + 34;
    const int list_w = dlg_w - 16;
    const int list_h = dlg_h - 98;
    const int list_rows = list_h / CHAR_HEIGHT;
    const int dlg_chars = dlg_w / CHAR_WIDTH;
    const int list_chars = list_w / CHAR_WIDTH;
    const int dlg_rows = dlg_h / CHAR_HEIGHT;
    const bool list_focused = (editor.dialog_focus == DIALOG_FOCUS_LIST);
    const bool filename_focused = (editor.dialog_focus == DIALOG_FOCUS_FILENAME);
    const bool ok_focused = (editor.dialog_focus == DIALOG_FOCUS_OK);
    const bool cancel_focused = (editor.dialog_focus == DIALOG_FOCUS_CANCEL);

    for (int row = 0; row < dlg_rows; row++) {
        draw_text_band(fb, stride, dlg_x, dlg_y + row * CHAR_HEIGHT, dlg_chars,
            COLOR_DIALOG_BG, COLOR_DIALOG_BG);
    }
    for (int row = 0; row < 2; row++) {
        draw_text_band(fb, stride, dlg_x, dlg_y + row * CHAR_HEIGHT, dlg_chars,
            COLOR_DIALOG_TITLE_BG, COLOR_DIALOG_TITLE_BG);
    }
    if (editor.dialog_mode != DIALOG_HELP) {
        for (int row = 0; row < list_rows; row++) {
            draw_text_band(fb, stride, list_x, list_y + row * CHAR_HEIGHT, list_chars,
                COLOR_DIALOG_LIST_BG, COLOR_DIALOG_LIST_BG);
        }
    }

    const char* title = (editor.dialog_mode == DIALOG_OPEN) ? "Open File" :
        (editor.dialog_mode == DIALOG_SAVE) ? "Save File" : "Help";
    VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
        (uint16_t)(dlg_x + 8), (uint16_t)(dlg_y + 4), title,
        COLOR_DIALOG_TITLE_FG, COLOR_DIALOG_TITLE_BG, (int)strlen(title));

    if (editor.dialog_mode == DIALOG_HELP) {
        const char* help_lines[] = {
            "gedit Help",
            "",
            "Editing",
            "  Type text to insert or overwrite characters.",
            "  Enter creates a new line.",
            "  Backspace deletes before the cursor.",
            "  Delete removes the character at the cursor.",
            "",
            "Navigation",
            "  Arrow keys move the cursor.",
            "  Home/End jump to start/end of the line.",
            "  Insert toggles INS/OVR mode.",
            "",
            "Files",
            "  Ctrl+O opens the file dialog.",
            "  Ctrl+S opens the save dialog.",
            "  Fn+ESC opens this help dialog.",
            "",
            "Dialogs",
            "  Tab switches focus between controls.",
            "  Enter activates the focused control.",
            "  Type when the File name field is focused.",
            "  Esc closes the current dialog.",
            "  Fn+ESC also closes this help dialog.",
            "",
            "Press Enter or Esc to close."
        };
        int help_y = dlg_y + 24;
        int help_count = (int)(sizeof(help_lines) / sizeof(help_lines[0]));
        for (int i = 0; i < help_count; i++) {
            VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
                (uint16_t)(dlg_x + 12), (uint16_t)help_y, help_lines[i],
                COLOR_DIALOG_LIST_FG, COLOR_DIALOG_BG, (int)strlen(help_lines[i]));
            help_y += CHAR_HEIGHT + 2;
        }

        const char* close_text = "[ Close ]";
        VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
            (uint16_t)(dlg_x + dlg_w - 96), (uint16_t)(dlg_y + dlg_h - 24), close_text,
            COLOR_DIALOG_LIST_FG, COLOR_DIALOG_BG, (int)strlen(close_text));
        return;
    }

    char path_line[96];
    snprintf(path_line, sizeof(path_line), "Look in: %s", editor.dialog_path);
    VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
        (uint16_t)(dlg_x + 8), (uint16_t)(dlg_y + 20), path_line,
        COLOR_DIALOG_LIST_FG, COLOR_DIALOG_BG, (int)strlen(path_line));

    if (editor.dialog_selected < editor.dialog_scroll) {
        editor.dialog_scroll = editor.dialog_selected;
    }
    if (editor.dialog_selected >= editor.dialog_scroll + list_rows) {
        editor.dialog_scroll = editor.dialog_selected - list_rows + 1;
    }
    if (editor.dialog_scroll < 0) {
        editor.dialog_scroll = 0;
    }

    for (int i = 0; i < list_rows; i++) {
        int idx = editor.dialog_scroll + i;
        if (idx >= editor.dialog_entry_count) {
            break;
        }

        FileEntry* e = &editor.dialog_entries[idx];
        bool selected = (idx == editor.dialog_selected);
        uint16_t row_bg = COLOR_DIALOG_LIST_BG;
        uint16_t row_fg = COLOR_DIALOG_LIST_FG;
        if (selected) {
            row_bg = list_focused ? COLOR_DIALOG_SEL_BG : COLOR_DIALOG_BG;
            row_fg = list_focused ? COLOR_DIALOG_SEL_FG : COLOR_DIALOG_LIST_FG;
        }
        int row_y = list_y + i * CHAR_HEIGHT;

        draw_text_band(fb, stride, list_x, row_y, list_chars, row_bg, row_bg);

        char row[72];
        snprintf(row, sizeof(row), "%c %s", e->is_dir ? '>' : ' ', e->name);
        VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
            (uint16_t)(list_x + 4), (uint16_t)row_y, row, row_fg, row_bg, (int)strlen(row));
    }

    const int file_y = dlg_y + dlg_h - 52;
    const int file_chars = (dlg_w - 16) / CHAR_WIDTH;
    uint16_t file_fg = filename_focused ? COLOR_DIALOG_SEL_FG : COLOR_DIALOG_LIST_FG;
    uint16_t file_bg = filename_focused ? COLOR_DIALOG_SEL_BG : COLOR_DIALOG_BG;
    draw_text_band(fb, stride, dlg_x + 8, file_y, file_chars, file_bg, file_bg);

    char file_line[96];
    snprintf(file_line, sizeof(file_line), "File name: %s", editor.dialog_filename);
    VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
        (uint16_t)(dlg_x + 8), (uint16_t)file_y, file_line,
        file_fg, file_bg, (int)strlen(file_line));

    if (filename_focused && cursor_visible) {
        const int file_prefix_chars = (int)strlen("File name: ");
        int name_chars = (int)strlen(editor.dialog_filename);
        int max_name_chars = file_chars - file_prefix_chars - 1;
        if (max_name_chars < 0) {
            max_name_chars = 0;
        }
        if (name_chars > max_name_chars) {
            name_chars = max_name_chars;
        }

        int caret_char = file_prefix_chars + name_chars;
        if (caret_char < 0) {
            caret_char = 0;
        }
        if (caret_char >= file_chars) {
            caret_char = file_chars - 1;
        }

        int caret_x = dlg_x + 8 + caret_char * CHAR_WIDTH;
        fill_rect16(fb, stride, caret_x, file_y, 2, CHAR_HEIGHT, file_fg);
    }

    const char* ok_text = (editor.dialog_mode == DIALOG_OPEN) ? "[ Open ]" : "[ Save ]";
    const char* cancel_text = "[ Cancel ]";
    int ok_x = dlg_x + dlg_w - 176;
    int cancel_x = dlg_x + dlg_w - 88;
    uint16_t ok_fg = ok_focused ? COLOR_DIALOG_SEL_FG : COLOR_DIALOG_LIST_FG;
    uint16_t ok_bg = ok_focused ? COLOR_DIALOG_SEL_BG : COLOR_DIALOG_BG;
    uint16_t cancel_fg = cancel_focused ? COLOR_DIALOG_SEL_FG : COLOR_DIALOG_LIST_FG;
    uint16_t cancel_bg = cancel_focused ? COLOR_DIALOG_SEL_BG : COLOR_DIALOG_BG;

    draw_text_band(fb, stride, ok_x, dlg_y + dlg_h - 24, (int)strlen(ok_text), ok_bg, ok_bg);
    draw_text_band(fb, stride, cancel_x, dlg_y + dlg_h - 24, (int)strlen(cancel_text), cancel_bg, cancel_bg);

    VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
        (uint16_t)ok_x, (uint16_t)(dlg_y + dlg_h - 24), ok_text,
        ok_fg, ok_bg, (int)strlen(ok_text));
    VPUPrintStringRGB565((uint8_t*)fb, stride, EDITOR_WIDTH, EDITOR_HEIGHT,
        (uint16_t)cancel_x, (uint16_t)(dlg_y + dlg_h - 24), cancel_text,
        cancel_fg, cancel_bg, (int)strlen(cancel_text));
}

// Get line and column from cursor position
void editor_get_cursor_pos(size_t* line, size_t* column) {
    *line = editor.cursor_pos / CHARS_PER_LINE;
    *column = editor.cursor_pos % CHARS_PER_LINE;
}

// Render the editor screen
void editor_render(uint8_t* draw_page) {
    if (!editor.buffer || !draw_page) {
        return;
    }
    
    // Clear screen with background color
    uint16_t* fb = (uint16_t*)draw_page;
    uint32_t stride = VPUGetStride(EDITOR_MODE, EDITOR_COLOR);
    uint32_t pixels = stride * EDITOR_HEIGHT / sizeof(uint16_t);
    
    for (uint32_t i = 0; i < pixels; i++) {
        fb[i] = EDITOR_BG_COLOR;
    }
    
    // Render text
    size_t line, column;
    editor_get_cursor_pos(&line, &column);
    bool cursor_visible = ((editor.cursor_blink_ticks / 30) % 2) == 0;
    editor.cursor_blink_ticks++;
    
    // Calculate visible lines based on scroll, avoiding unsigned underflow.
    size_t total_lines = (editor.buffer_size + CHARS_PER_LINE - 1) / CHARS_PER_LINE;
    if (total_lines == 0) {
        total_lines = 1;
    }

    size_t half_screen = TEXT_LINES_PER_SCREEN / 2;
    size_t start_line = (line > half_screen) ? (line - half_screen) : 0;
    if (start_line + TEXT_LINES_PER_SCREEN > total_lines) {
        start_line = (total_lines > TEXT_LINES_PER_SCREEN) ? (total_lines - TEXT_LINES_PER_SCREEN) : 0;
    }
    
    // Render each visible line
    for (size_t screen_line = 0; screen_line < TEXT_LINES_PER_SCREEN; screen_line++) {
        size_t buffer_line = start_line + screen_line;
        size_t char_pos = buffer_line * CHARS_PER_LINE;
        
        // Don't render beyond buffer
        if (char_pos >= editor.buffer_size) {
            break;
        }
        
        // Calculate screen position
        int16_t x = 0;
        int16_t y = screen_line * CHAR_HEIGHT;
        
        // Render characters in this line
        size_t chars_to_render = CHARS_PER_LINE;
        if (char_pos + chars_to_render > editor.buffer_size) {
            chars_to_render = editor.buffer_size - char_pos;
        }
        
        // Create temporary string for this line
        char line_buffer[CHARS_PER_LINE + 1];
        memcpy(line_buffer, &editor.buffer[char_pos], chars_to_render);
        line_buffer[chars_to_render] = '\0';
        
        // Render the line
        VPUPrintStringRGB565(
            (uint8_t*)fb, 
            stride, 
            EDITOR_WIDTH, 
            EDITOR_HEIGHT, 
            x, 
            y, 
            line_buffer, 
            EDITOR_FG_COLOR, 
            EDITOR_BG_COLOR, 
            (int)chars_to_render
        );
    }
    
    // Render cursor
    editor_get_cursor_pos(&line, &column);
    if (editor.dialog_mode == DIALOG_NONE && cursor_visible &&
        line >= start_line && line < start_line + TEXT_LINES_PER_SCREEN) {
        size_t cursor_screen_line = line - start_line;
        int16_t cursor_x = column * CHAR_WIDTH;
        int16_t cursor_y = cursor_screen_line * CHAR_HEIGHT;
        
        // Draw cursor as a 2-pixel vertical bar
        for (int y = 0; y < CHAR_HEIGHT; y++) {
            if (cursor_x < EDITOR_WIDTH && cursor_y + y < EDITOR_HEIGHT) {
                fb[(cursor_y + y) * (stride / sizeof(uint16_t)) + cursor_x] = EDITOR_CURSOR_COLOR;
                if (cursor_x + 1 < EDITOR_WIDTH) {
                    fb[(cursor_y + y) * (stride / sizeof(uint16_t)) + cursor_x + 1] = EDITOR_CURSOR_COLOR;
                }
            }
        }
    }

    editor_draw_status_line(fb, stride, line, column);
    editor_draw_dialog(fb, stride, cursor_visible);
}

int main(int argc, char** argv) {
    g_platform = SPInitPlatform();
    if (!g_platform) {
        printf("Failed to initialize platform\n");
        return 1;
    }
    
    // Calculate framebuffer size and allocate
    uint32_t stride = VPUGetStride(EDITOR_MODE, EDITOR_COLOR);
    g_framebuffer_a.size = stride * EDITOR_HEIGHT;
    g_framebuffer_b.size = stride * EDITOR_HEIGHT;

    if (SPAllocateBuffer(g_platform, &g_framebuffer_a) != 0 ||
        SPAllocateBuffer(g_platform, &g_framebuffer_b) != 0) {
        printf("Failed to allocate framebuffers\n");
        if (g_framebuffer_a.cpuAddress) {
            SPFreeBuffer(g_platform, &g_framebuffer_a);
        }
        if (g_framebuffer_b.cpuAddress) {
            SPFreeBuffer(g_platform, &g_framebuffer_b);
        }
        SPShutdownPlatform(g_platform);
        return 1;
    }
    
    // Initialize editor
    editor_init();
    if (!editor.buffer) {
        SPFreeBuffer(g_platform, &g_framebuffer_a);
        SPFreeBuffer(g_platform, &g_framebuffer_b);
        SPShutdownPlatform(g_platform);
        return 1;
    }

    if (argc >= 2) {
        if (!editor_load_file(argv[1])) {
            editor_set_file_name(argv[1]);
        }
    }

    enable_raw_mode();
    
    // Set up video mode
    VPUSetVideoMode(g_platform->vx, EDITOR_MODE, EDITOR_COLOR, EVS_Enable);
    
    // Set up double buffering like other graphics samples.
    g_platform->sc->cycle = 0;
    g_platform->sc->framebufferA = &g_framebuffer_a;
    g_platform->sc->framebufferB = &g_framebuffer_b;

    // Clear both pages once before entering the main loop.
    VPUSwapPages(g_platform->vx, g_platform->sc);
    editor_render(g_platform->sc->writepage);
    VPUSwapPages(g_platform->vx, g_platform->sc);
    editor_render(g_platform->sc->writepage);

    // Prepare for synced swaps between A and B.
    VPUSetScanoutAddress(g_platform->vx, (uint32_t)(uintptr_t)g_framebuffer_a.dmaAddress);
    VPUSetScanoutAddress2(g_platform->vx, (uint32_t)(uintptr_t)g_framebuffer_b.dmaAddress);
   
    // Main loop
    bool running = true;
    while (running) {
        // Wait until the previously queued swap/noop has completed.
        while (VPUGetFIFONotEmpty(g_platform->vx)) { }

        // Update write page pointer and render into the back buffer.
        VPUSwapPages(g_platform->vx, g_platform->sc);
        editor_render(g_platform->sc->writepage);

        // Queue synced front/back buffer swap and a barrier noop.
        VPUSyncSwap(g_platform->vx, 0);
        VPUNoop(g_platform->vx);

        int key = editor_read_key();
        if (key != KEY_NONE) {
            running = editor_process_key(key);
        }
    }
    
    // Cleanup
    editor_cleanup();
    SPFreeBuffer(g_platform, &g_framebuffer_a);
    SPFreeBuffer(g_platform, &g_framebuffer_b);
    SPShutdownPlatform(g_platform);
    
    return 0;
}