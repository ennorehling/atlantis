#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <curses.h>

#include "service.h"

typedef struct rect {
    int x, y, w, h;
} rect;

typedef struct point {
    int x, y;
} point;

void finish(int sig) {
    erase();
    endwin();
    exit(sig);
}

void draw_map(WINDOW *win, point origin, int def) {
    int x, y, w, h;
    rect view;
    
    iregion.get_size(&w, &h);
    getbegyx(win, view.y, view.x);
    getmaxyx(win, view.h, view.w);
    for (y=0;y<view.h-2;++y) {
        int yp = y;
        for (x=0;x<view.w-2;x+=2) {
            int xp = x + (y&1);
            int t = def;
            int cx = ((xp+yp+1)/2) % w, cy = yp % h;
            HREGION r = iregion.get(cx, cy);
            if (!IS_NULL(r)) {
                const char * tname = iregion.terrain(r);
                t = tname[0] | COLOR_PAIR(COLOR_WHITE);
            }
            mvwaddch(win, yp+1, xp+1, t);
        }
    }
}

void run(void) {
    int rows, cols;
    int def = 'a';
    WINDOW *win;
    point origin = {0, 0};

    initscr();
    signal(SIGINT, finish);      /* arrange interrupts to terminate */
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, rows, cols);
    start_color();
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    attrset(COLOR_PAIR(COLOR_BLACK));
    bkgd(' ' | COLOR_PAIR(COLOR_BLACK));
    bkgdset(' ' | COLOR_PAIR(COLOR_BLACK));
    nonl();         /* tell curses not to do NL->CR/NL on output */
    scrollok(stdscr, FALSE);
    wclear(stdscr);

    win = newwin(rows, cols, 0, 0);
    box(win, 0, 0);
    draw_map(win, origin, def | COLOR_PAIR(COLOR_WHITE));
    wrefresh(win);
    for (;;) {
        int c;
        c = wgetch(win);     /* refresh, accept single keystroke of input */
        switch (c) {
        case 'Q':
            finish(0);
            break;
        case '+':
            ++def;
            break;
        case '-':
            --def;
            break;
        }
        draw_map(win, origin, (def+1) | COLOR_PAIR(COLOR_YELLOW));
        wrefresh(win);
    }
}

void fake_map(int w, int h) {
    int x, y;
    iregion.set_size(w, h);
    for (y=0;y!=h;++y) {
        for (x=0;x!=w;++x) {
            const char *tname = (x==0 || y==0) ? "plain" : "ocean";
            iregion.create(x, y, tname);
        }
    }
}

int main (int argc, char **argv) {
    fake_map(16, 16);
    run();
    return 0;
}
