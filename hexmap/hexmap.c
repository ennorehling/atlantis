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

int terrain_icon(const char *tname) {
    if (!tname) return ' ';
    if (strcmp(tname, "plain")==0) return '+' | COLOR_PAIR(COLOR_GREEN);
    if (strcmp(tname, "ocean")==0) return '.' | COLOR_PAIR(COLOR_BLUE);
    return tname[0] | COLOR_PAIR(COLOR_YELLOW);
}

void draw_map(WINDOW *win, point cursor) {
    int x, y, w, h;
    rect view;
    point origin;

    iregion.get_size(&w, &h);
    getbegyx(win, view.y, view.x);
    getmaxyx(win, view.h, view.w);

    origin.x = cursor.x;
    origin.y = cursor.y - view.h/2 - 1;
    for (y=0;y<view.h-2;++y) {
        int yp = y;
        for (x=0;x<view.w-2;x+=2) {
            int xp = x + (y&1);
            int t = ' ';
            int cx = (origin.x + (xp+yp+1)/2) % w;
            int cy = (origin.y + yp) % h;
            HREGION r = iregion.get(cx, cy);

            if (cursor.x==cx && cursor.y==cy) {
                mvwaddstr(win, yp+1, xp, "< >");
            }
            if (!IS_NULL(r)) {
                const char * tname = iregion.terrain(r);
                t = terrain_icon(tname);
            }
            mvwaddch(win, yp+1, xp+1, t);
        }
    }
}

void run(void) {
    int rows, cols;
    int mapw, maph;
    WINDOW *win;
    point cursor = {0, 0};

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
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    attrset(COLOR_PAIR(COLOR_BLACK));
    bkgd(' ' | COLOR_PAIR(COLOR_BLACK));
    bkgdset(' ' | COLOR_PAIR(COLOR_BLACK));
    nonl();         /* tell curses not to do NL->CR/NL on output */
    scrollok(stdscr, FALSE);
    wclear(stdscr);

    win = newwin(rows-1, cols, 0, 0);
    keypad(win, TRUE);
    box(win, 0, 0);
    for (;;) {
        int c;
        char buf[64];

        draw_map(win, cursor);
        wrefresh(win);
        sprintf(buf, "cursor [%d/%d] ", cursor.x, cursor.y);
        mvwaddstr(win, 1, 1, buf);
        c = wgetch(win);     /* refresh, accept single keystroke of input */
        switch (c) {
        case 'k':
        case KEY_UP:
            cursor.y = (maph+cursor.y-1) % maph;
            break;
        case 'j':
        case KEY_DOWN:
            cursor.y = (maph+cursor.y+1) % maph;
            break;
        case 'h':
        case KEY_LEFT:
            cursor.x = (mapw+cursor.x-1) % mapw;
            break;
        case 'l':
        case KEY_RIGHT:
            cursor.x = (mapw+cursor.x+1) % mapw;
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
