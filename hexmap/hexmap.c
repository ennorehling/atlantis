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

void draw_map(WINDOW *win, point origin) {
    int x, y, w, h;
    rect view;

    iregion.get_size(&w, &h);
    getbegyx(win, view.y, view.x);
    getmaxyx(win, view.h, view.w);
    for (y=0;y<view.h-2;++y) {
        int yp = y;
        for (x=0;x<view.w-2;x+=2) {
            int xp = x + (y&1);
            int t = ' ';
            int cx = (origin.x + (xp+yp+1)/2) % w;
            int cy = (origin.y + yp) % h;
            HREGION r = iregion.get(cx, cy);
            if (!IS_NULL(r)) {
                const char * tname = iregion.terrain(r);
                t = tname[0] | COLOR_PAIR(COLOR_YELLOW);
            }
            mvwaddch(win, yp+1, xp+1, t);
        }
    }
}

void run(void) {
    int rows, cols;
    int mapw, maph;
    WINDOW *win;
    point origin = {0, 0};

    iregion.get_size(&mapw, &maph);
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
    keypad(win, TRUE);
    box(win, 0, 0);
    for (;;) {
        int c;
        char buf[64];

        draw_map(win, origin);
        wrefresh(win);
        sprintf(buf, "origin [%d/%d] ", origin.x, origin.y);
        mvwaddstr(win, 1, 1, buf);
        c = wgetch(win);     /* refresh, accept single keystroke of input */
        switch (c) {
        case 'k':
        case KEY_UP:
            origin.y = (maph+origin.y-1) % maph;
            break;
        case 'j':
        case KEY_DOWN:
            origin.y = (maph+origin.y+1) % maph;
            break;
        case 'h':
        case KEY_LEFT:
            origin.x = (mapw+origin.x-1) % mapw;
            break;
        case 'l':
        case KEY_RIGHT:
            origin.x = (mapw+origin.x+1) % mapw;
            break;
        case 'Q':
            finish(0);
            break;
        }
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
