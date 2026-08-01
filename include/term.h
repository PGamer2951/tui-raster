#ifndef TERM_H
#define TERM_H

#include "../include/custom-types.h"

#define OK  0
#define ERR 1

typedef struct {
    char *data;
    int width;
    int height;
} TextBuffer;

int TermWidth();

int TermHeight();

void EnableRawMode(struct termios *original);

void DisableRawMode(struct termios *original);

void InitTerm();

void ShutdownTerm(struct termios *original);

void AddToBuffer(TextBuffer *buf, int x, int y, char ch);

void ClearBuffer(TextBuffer *buf);

void PresentBuffer(TextBuffer *buf);

int PeekInput(char *out);

#endif