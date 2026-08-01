#include "../include/term.h"

void EnableRawMode(struct termios *original) {
    struct termios term = *original;

    term.c_lflag &= ~(ECHO | ICANON | ISIG);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void DisableRawMode(struct termios *original) {
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}

int TermWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_col;
}

int TermHeight() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_row;
}

void InitTerm() {
    write(STDOUT_FILENO, "\x1B[H", 3);
    write(STDOUT_FILENO, "\x1B[2J", 4);
    write(STDOUT_FILENO, "\x1B[?25l", 6);
}

void ShutdownTerm(struct termios *original) {
    DisableRawMode(original);

    write(STDOUT_FILENO, "\x1B[2J", 4);
    write(STDOUT_FILENO, "\x1B[?25h", 6);
    write(STDOUT_FILENO, "\x1B[H", 3);
}

void AddToBuffer(TextBuffer *buf, int x, int y, char ch) {
    buf->data[x + (y * buf->width)] = ch;
}

void ClearBuffer(TextBuffer *buf) {
    for (int i = 0; i < (buf->width * buf->height); i++) buf->data[i] = ' ';
}

void PresentBuffer(TextBuffer *buf) {
    write(STDOUT_FILENO, "\x1B[H", 3);
    write(STDOUT_FILENO, buf->data, (buf->width * buf->height));
}

int PeekInput(char *out) {
    if(read(STDIN_FILENO, out, sizeof(char) * 3)) {
        return 0;
    }

    return 1;
}