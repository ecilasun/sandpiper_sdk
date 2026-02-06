/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>
#include "core.h"
#include "platform.h"
#include "vpu.h"

/*** defines ***/

#define KILO_VERSION "0.0.1"
#define KILO_TAB_STOP 8
#define KILO_QUIT_TIMES 3

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN,
  MOUSE_EVENT,
  MOUSE_MOVE
};

/*** data ***/

typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;
} erow;

struct editorConfig {
  int cx, cy;
  int rx;
  int rowoff;
  int coloff;
  int screenrows;
  int screencols;
  int numrows;
  erow *row;
  int dirty;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  struct termios orig_termios;
  int mouse_row;
  int mouse_col;
  int local_mouse_fd;
  int local_mouse_x;
  int local_mouse_y;
  int screen_width_pixels;
  int screen_height_pixels;
  int cursor_visible;
  int cursor_stored_x;
  int cursor_stored_y;
  int remote_mouse_visible;
  int remote_mouse_x;
  int remote_mouse_y;
  uint16_t cursor_backup[16*16]; // Backup 16x16 area
  struct SPPlatform* platform;
};

struct editorConfig E;

/*** prototypes ***/

const char* getSessionName();
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();
char *editorPrompt(char *prompt, void (*callback)(char *, int));
int editorRowRxToCx(erow *row, int rx);

/*** terminal ***/

void die(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s);
  exit(1);
}

int getSessionType() {
    if (getenv("SSH_CONNECTION") || getenv("SSH_CLIENT")) return 1; // SSH
    
    char *tty = ttyname(STDIN_FILENO);
    if (!tty) return 0; // Local Console likely
    
    if (strstr(tty, "/dev/pts/") == tty) return 2; // Pseudo-Terminal (xterm, screen, etc)
    if (strstr(tty, "/dev/ttyS") == tty) return 3; // Serial
    if (strstr(tty, "/dev/ttyUSB") == tty) return 3; // USB Serial
    
    return 0; // Console
}

const char* getSessionName() {
    switch (getSessionType()) {
        case 1: return "Remote(SSH)";
        case 2: return "Terminal";
        case 3: return "Serial";
        default: return "Console";
    }
}

void disableRawMode() {
  if (getSessionType() != 0) {
      write(STDOUT_FILENO, "\x1b[?1003;1006l", 12);
  }
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = E.orig_termios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
  
  if (getSessionType() != 0) {
      write(STDOUT_FILENO, "\x1b[?1003;1006h", 12);
  }
}

static void undraw_local_cursor() {
    if (!E.cursor_visible || !E.platform || E.platform->mapped_memory == MAP_FAILED) return;

    // Restore backup
    uint8_t* fb = (uint8_t*)E.platform->mapped_memory;
    int start_x = E.cursor_stored_x;
    int start_y = E.cursor_stored_y;
    int stride = 1280; // 640 * 2

    for (int dy = 0; dy < 16; dy++) {
        int y = start_y + dy;
        if (y >= 480) break;
        for (int dx = 0; dx < 16; dx++) {
            int x = start_x + dx;
            if (x >= 640) break;
            
            uint16_t* p = (uint16_t*)(fb + y * stride + x * 2);
            *p = E.cursor_backup[dy * 16 + dx];
        }
    }
    E.cursor_visible = 0;
}

static void draw_local_cursor() {
    if (!E.platform || E.platform->mapped_memory == MAP_FAILED) return;

    if (E.cursor_visible) undraw_local_cursor();

    uint8_t* fb = (uint8_t*)E.platform->mapped_memory;
    int start_x = E.local_mouse_x;
    int start_y = E.local_mouse_y;
    int stride = 1280;

    // Save background and draw cursor (simple red cross)
    // RGB565 Red = 0xF800
    uint16_t cursor_color = 0xF800; // Red

    for (int dy = 0; dy < 16; dy++) {
        int y = start_y + dy;
        if (y >= 480) break;
        for (int dx = 0; dx < 16; dx++) {
            int x = start_x + dx;
            if (x >= 640) break;
            
            uint16_t* p = (uint16_t*)(fb + y * stride + x * 2);
            E.cursor_backup[dy * 16 + dx] = *p;

            // Draw Crosshair logic
            // Center is around 8,8
            int draw = 0;
            if (dy == 8 || dx == 8) draw = 1;
            // Border
            if (dy == 0 || dy == 15) { if(dx>=7 && dx<=9) draw=1; }
            if (dx == 0 || dx == 15) { if(dy>=7 && dy<=9) draw=1; }

            if (draw) {
                *p = cursor_color;
            }
        }
    }

    E.cursor_stored_x = start_x;
    E.cursor_stored_y = start_y;
    E.cursor_visible = 1;
}

static int map_value(int v, int max_src, int max_dst) {
    if (v < 0) return 0;
    if (v >= max_src) return max_dst - 1;
    return (v * max_dst) / max_src;
}

int editorReadKey() {
  int nread;
  char c;

  struct pollfd pfd[2];
  pfd[0].fd = STDIN_FILENO;
  pfd[0].events = POLLIN;
  pfd[1].fd = E.local_mouse_fd;
  pfd[1].events = POLLIN;

  while (1) {
      int poll_count = (E.local_mouse_fd >= 0) ? 2 : 1;
      if (poll(pfd, poll_count, -1) == -1) {
          if (errno == EINTR) continue;
          die("poll");
      }

      // Check local mouse
      if (E.local_mouse_fd >= 0 && (pfd[1].revents & (POLLIN | POLLERR | POLLHUP))) {
          struct input_event ev;
          int n = read(E.local_mouse_fd, &ev, sizeof(ev));
          if (n <= 0) {
              // Device disconnected or error, stop polling it
              close(E.local_mouse_fd);
              E.local_mouse_fd = -1;
          }
          else if (n == sizeof(ev)) {
             if (ev.type == EV_REL) {
                 if (ev.code == REL_X) E.local_mouse_x += ev.value;
                 if (ev.code == REL_Y) E.local_mouse_y += ev.value;
                 
                 // Clamp
                 if (E.local_mouse_x < 0) E.local_mouse_x = 0;
                 if (E.local_mouse_x >= E.screen_width_pixels) E.local_mouse_x = E.screen_width_pixels - 1;
                 
                 if (E.local_mouse_y < 0) E.local_mouse_y = 0;
                 if (E.local_mouse_y >= E.screen_height_pixels) E.local_mouse_y = E.screen_height_pixels - 1;
                 
                 draw_local_cursor();
             } else if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1) {
                 // Left click
                 E.mouse_col = map_value(E.local_mouse_x, E.screen_width_pixels, E.screencols);
                 E.mouse_row = map_value(E.local_mouse_y, E.screen_height_pixels, E.screenrows);
                 return MOUSE_EVENT;
             }
          }
      }

      // Check STDIN
      if (pfd[0].revents & POLLIN) {
        undraw_local_cursor();
        if (read(STDIN_FILENO, &c, 1) == 1) break;
      }
  }

  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] == '<') {
        char mbuf[32];
        int i = 0;
        int max_len = sizeof(mbuf) - 1;
        while (i < max_len) {
          if (read(STDIN_FILENO, &mbuf[i], 1) != 1) break;
          if (mbuf[i] == 'm' || mbuf[i] == 'M') {
            if (mbuf[i] == 'M') {
                int b, x, y;
                mbuf[i] = '\0';
                if (sscanf(mbuf, "%d;%d;%d", &b, &x, &y) == 3) {
                    if (b == 0) { // Click
                        E.mouse_col = x - 1;
                        E.mouse_row = y - 1;
                        return MOUSE_EVENT;
                    } else if (b == 35) { // Move
                        E.remote_mouse_x = x - 1;
                        E.remote_mouse_y = y - 1;
                        E.remote_mouse_visible = 1;
                        return MOUSE_MOVE;
                    }
                }
            }
            return '\x1b';
          }
          i++;
        }
      }
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
        if (seq[2] == ~0) {
          switch (seq[1]) {
            case '1': return HOME_KEY;
            case '3': return DEL_KEY;
            case '4': return END_KEY;
            case '5': return PAGE_UP;
            case '6': return PAGE_DOWN;
            case '7': return HOME_KEY;
            case '8': return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
          case 'A': return ARROW_UP;
          case 'B': return ARROW_DOWN;
          case 'C': return ARROW_RIGHT;
          case 'D': return ARROW_LEFT;
          case 'H': return HOME_KEY;
          case 'F': return END_KEY;
        }
      }
    } else if (seq[0] == 'O') {
      switch (seq[1]) {
        case 'H': return HOME_KEY;
        case 'F': return END_KEY;
      }
    }

    return '\x1b';
  } else {
    return c;
  }
}

int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
    if (buf[i] == 'R') break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[') return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;

  return 0;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

/*** row operations ***/

int editorRowCxToRx(erow *row, int cx) {
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (row->chars[j] == '\t')
      rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
  }
  return rx;
}

void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t') tabs++;

  free(row->render);
  row->render = malloc(row->size + tabs * (KILO_TAB_STOP - 1) + 1);

  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      row->render[idx++] = ' ';
      while (idx % KILO_TAB_STOP != 0) row->render[idx++] = ' ';
    } else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
}

void editorInsertRow(int at, char *s, size_t len) {
  if (at < 0 || at > E.numrows) return;

  E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

  E.row[at].size = len;
  E.row[at].chars = malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize = 0;
  E.row[at].render = NULL;
  editorUpdateRow(&E.row[at]);

  E.numrows++;
  E.dirty++;
}

void editorFreeRow(erow *row) {
  free(row->chars);
  free(row->render);
}

void editorDelRow(int at) {
  if (at < 0 || at >= E.numrows) return;
  editorFreeRow(&E.row[at]);
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
  E.numrows--;
  E.dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
  if (at < 0 || at > row->size) at = row->size;
  row->chars = realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(row->chars + row->size, s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  E.dirty++;
}

void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  E.dirty++;
}

/*** editor operations ***/

void editorInsertChar(int c) {
  if (E.cy == E.numrows) {
    editorInsertRow(E.numrows, "", 0);
  }
  editorRowInsertChar(&E.row[E.cy], E.cx, c);
  E.cx++;
}

void editorInsertNewline() {
  if (E.cx == 0) {
    editorInsertRow(E.cy, "", 0);
  } else {
    erow *row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
    row = &E.row[E.cy];
    row->size = E.cx;
    row->chars[row->size] = '\0';
    editorUpdateRow(row);
  }
  E.cy++;
  E.cx = 0;
}

void editorDelChar() {
  if (E.cy == E.numrows) return;
  if (E.cx == 0 && E.cy == 0) return;

  erow *row = &E.row[E.cy];
  if (E.cx > 0) {
    editorRowDelChar(row, E.cx - 1);
    E.cx--;
  } else {
    E.cx = E.row[E.cy - 1].size;
    editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
    editorDelRow(E.cy);
    E.cy--;
  }
}

/*** file i/o ***/

char *editorRowsToString(int *buflen) {
  int totlen = 0;
  int j;
  for (j = 0; j < E.numrows; j++)
    totlen += E.row[j].size + 1;
  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (j = 0; j < E.numrows; j++) {
    memcpy(p, E.row[j].chars, E.row[j].size);
    p += E.row[j].size;
    *p = '\n';
    p++;
  }
  return buf;
}

void editorOpen(char *filename) {
  free(E.filename);
  E.filename = strdup(filename);

  FILE *fp = fopen(filename, "r");
  if (!fp) die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while (linelen > 0 && (line[linelen - 1] == '\n' ||
                           line[linelen - 1] == '\r'))
      linelen--;
    editorInsertRow(E.numrows, line, linelen);
  }
  free(line);
  fclose(fp);
  E.dirty = 0;
}

void editorSave() {
  if (E.filename == NULL) {
    E.filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
    if (E.filename == NULL) {
      editorSetStatusMessage("Save aborted");
      return;
    }
  }

  int len;
  char *buf = editorRowsToString(&len);

  int fd = open(E.filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != -1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        E.dirty = 0;
        editorSetStatusMessage("%d bytes written to disk", len);
        return;
      }
    }
    close(fd);
  }
  free(buf);
  editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

/*** find ***/

void editorFindCallback(char *query, int key) {
  static int last_match = -1;
  static int direction = 1;

  if (key == '\r' || key == '\x1b') {
    last_match = -1;
    direction = 1;
    return;
  } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
    direction = 1;
  } else if (key == ARROW_LEFT || key == ARROW_UP) {
    direction = -1;
  } else {
    last_match = -1;
    direction = 1;
  }

  if (last_match == -1) direction = 1;
  int current = last_match;
  int i;
  for (i = 0; i < E.numrows; i++) {
    current += direction;
    if (current == -1) current = E.numrows - 1;
    else if (current == E.numrows) current = 0;

    erow *row = &E.row[current];
    char *match = strstr(row->render, query);
    if (match) {
      last_match = current;
      E.cy = current;
      E.cx = editorRowRxToCx(row, match - row->render);
      E.rowoff = E.numrows;
      return;
    }
  }
}

int editorRowRxToCx(erow *row, int rx) {
  int cur_rx = 0;
  int cx;
  for (cx = 0; cx < row->size; cx++) {
    if (row->chars[cx] == '\t')
      cur_rx += (KILO_TAB_STOP - 1) - (cur_rx % KILO_TAB_STOP);
    cur_rx++;

    if (cur_rx > rx) return cx;
  }
  return cx;
}

void editorFind() {
  int saved_cx = E.cx;
  int saved_cy = E.cy;
  int saved_coloff = E.coloff;
  int saved_rowoff = E.rowoff;

  char *query = editorPrompt("Search: %s (Use ESC/Arrows/Enter)", editorFindCallback);

  if (query) {
    free(query);
  } else {
    E.cx = saved_cx;
    E.cy = saved_cy;
    E.coloff = saved_coloff;
    E.rowoff = saved_rowoff;
  }
}

/*** append buffer ***/

struct abuf {
  char *b;
  int len;
};

#define ABUF_INIT {NULL, 0}

void abAppend(struct abuf *ab, const char *s, int len) {
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL) return;
  memcpy(&new[ab->len], s, len);
  ab->b = new;
  ab->len += len;
}

void abFree(struct abuf *ab) {
  free(ab->b);
}

/*** output ***/

void editorScroll() {
  E.rx = 0;
  if (E.cy < E.numrows) {
    E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
  }

  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
}

void editorDrawRows(struct abuf *ab) {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
          "Tiny Text Editor -- version %s", KILO_VERSION);
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--) abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      int len = E.row[filerow].rsize - E.coloff;
      if (len < 0) len = 0;
      if (len > E.screencols) len = E.screencols;
      abAppend(ab, &E.row[filerow].render[E.coloff], len);
    }

    abAppend(ab, "\x1b[K", 3);
    abAppend(ab, "\r\n", 2);
  }
}

void editorDrawStatusBar(struct abuf *ab) {
  abAppend(ab, "\x1b[7m", 4);
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
    E.filename ? E.filename : "[No Name]", E.numrows,
    E.dirty ? "(modified)" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",
    E.cy + 1, E.numrows);
  if (len > E.screencols) len = E.screencols;
  abAppend(ab, status, len);
  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      abAppend(ab, rstatus, rlen);
      break;
    } else {
      abAppend(ab, " ", 1);
      len++;
    }
  }
  abAppend(ab, "\x1b[m", 3);
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
  abAppend(ab, "\x1b[K", 3);
  int msglen = strlen(E.statusmsg);
  if (msglen > E.screencols) msglen = E.screencols;
  if (msglen && time(NULL) - E.statusmsg_time < 5)
    abAppend(ab, E.statusmsg, msglen);
}

void editorRefreshScreen() {
  editorScroll();

  struct abuf ab = ABUF_INIT;

  abAppend(&ab, "\x1b[?25l", 6);
  abAppend(&ab, "\x1b[H", 3);

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  if (getSessionType() != 0 && E.remote_mouse_visible) {
      // Basic bound check. Remember screen is 1-based in ANSI
      if (E.remote_mouse_x >= 0 && E.remote_mouse_x < E.screencols &&
          E.remote_mouse_y >= 0 && E.remote_mouse_y <= E.screenrows + 1) { // +1 for status bars
          
           char cursor[32];
           snprintf(cursor, sizeof(cursor), "\x1b[%d;%dH\x1b[31m+\x1b[m", 
               E.remote_mouse_y + 1, E.remote_mouse_x + 1);
           abAppend(&ab, cursor, strlen(cursor));
      }
  }

  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                            (E.rx - E.coloff) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6);

  write(STDOUT_FILENO, ab.b, ab.len);
  abFree(&ab);
}

void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

/*** input ***/

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  size_t buflen = 0;
  buf[0] = '\0';

  while (1) {
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();

    int c = editorReadKey();
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      if (buflen != 0) buf[--buflen] = '\0';
    } else if (c == '\x1b') {
      editorSetStatusMessage("");
      if (callback) callback(buf, c);
      free(buf);
      return NULL;
    } else if (c == '\r') {
      if (buflen != 0) {
        editorSetStatusMessage("");
        if (callback) callback(buf, c);
        return buf;
      }
    } else if (!iscntrl(c) && c < 128) {
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }

    if (callback) callback(buf, c);
  }
}

void editorMoveCursor(int key) {
  erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];

  switch (key) {
    case ARROW_LEFT:
      if (E.cx != 0) {
        E.cx--;
      } else if (E.cy > 0) {
        E.cy--;
        E.cx = E.row[E.cy].size;
      }
      break;
    case ARROW_RIGHT:
      if (row && E.cx < row->size) {
        E.cx++;
      } else if (row && E.cx == row->size) {
        E.cy++;
        E.cx = 0;
      }
      break;
    case ARROW_UP:
      if (E.cy != 0) {
        E.cy--;
      }
      break;
    case ARROW_DOWN:
      if (E.cy < E.numrows) {
        E.cy++;
      }
      break;
  }

  row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
  int rowlen = row ? row->size : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

void editorProcessKeypress() {
  static int quit_times = KILO_QUIT_TIMES;

  int c = editorReadKey();

  switch (c) {
    case '\r':
      E.remote_mouse_visible = 0;
      editorInsertNewline();
      break;

    case CTRL_KEY('q'):
      if (E.dirty && quit_times > 0) {
        editorSetStatusMessage("WARNING!!! File has unsaved changes. "
          "Press Ctrl-Q %d more times to quit.", quit_times);
        quit_times--;
        return;
      }
      write(STDOUT_FILENO, "\x1b[2J", 4);
      write(STDOUT_FILENO, "\x1b[H", 3);
      exit(0);
      break;

    case CTRL_KEY('s'):
      editorSave();
      break;

    case MOUSE_EVENT:
      {
        int y = E.mouse_row + E.rowoff;
        if (y > E.numrows) y = E.numrows;
        E.cy = y;
        
        int rx = E.mouse_col + E.coloff;

        if (E.cy < E.numrows)
          E.cx = editorRowRxToCx(&E.row[E.cy], rx);
        else
          E.cx = 0;
      }
      break;

    case MOUSE_MOVE:
        // Just refresh, cursor updated
        break;

    case HOME_KEY:
      E.cx = 0;
      break;

    case END_KEY:
      if (E.cy < E.numrows)
        E.cx = E.row[E.cy].size;
      break;

    case CTRL_KEY('f'):
      editorFind();
      break;

    case BACKSPACE:
    case CTRL_KEY('h'):
    case DEL_KEY:
      E.remote_mouse_visible = 0;
      if (c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
      editorDelChar();
      break;

    case PAGE_UP:
    case PAGE_DOWN:
      {
        if (c == PAGE_UP) {
          E.cy = E.rowoff;
        } else {
          E.cy = E.rowoff + E.screenrows - 1;
          if (E.cy > E.numrows) E.cy = E.numrows;
        }

        int times = E.screenrows;
        while (times--)
          editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
      }
      break;

    case ARROW_UP:
    case ARROW_DOWN:
    case ARROW_LEFT:
    case ARROW_RIGHT:
      editorMoveCursor(c);
      break;

    case CTRL_KEY('l'):
    case '\x1b':
      break;

    default:
      E.remote_mouse_visible = 0;
      editorInsertChar(c);
      break;
  }

  quit_times = KILO_QUIT_TIMES;
}

/*** init ***/

void initEditor() {
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.numrows = 0;
  E.row = NULL;
  E.dirty = 0;
  E.filename = NULL;
  E.statusmsg[0] = '\0';
  E.statusmsg_time = 0;

  E.platform = SPInitPlatform();
  E.local_mouse_fd = -1;
  if (E.platform) {
      E.local_mouse_fd = SPFindMouseDevice();
  }
  
  E.cursor_visible = 0;
  E.remote_mouse_visible = 0;
  E.local_mouse_x = 0;
  E.local_mouse_y = 0;
  // Default assumptions if we can't get pixel size
  E.screen_width_pixels = 640;
  E.screen_height_pixels = 480;

  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
  
  // Try to get pixel size from terminal
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
      if (ws.ws_xpixel > 0) E.screen_width_pixels = ws.ws_xpixel;
      else E.screen_width_pixels = E.screencols * 8; // Guess 8x16 font

      if (ws.ws_ypixel > 0) E.screen_height_pixels = ws.ws_ypixel;
      else E.screen_height_pixels = E.screenrows * 16;
  }
  
  E.screenrows -= 2;
}

int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Mode: %s", 
    getSessionName());

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
