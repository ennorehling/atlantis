#include <curses.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void finish(int sig) {
    erase();
    endwin();
    if (sig) {
        exit(sig);
    }
}

int main(int argc, char **argv)
{
    int x, y, ch;
    WINDOW *win;

    initscr();
    signal(SIGINT, finish);      /* arrange interrupts to terminate */
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    getmaxyx(stdscr, y, x);
/*
    start_color();
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    attrset(COLOR_PAIR(COLOR_BLACK));
    bkgd(' ' | COLOR_PAIR(COLOR_BLACK));
    bkgdset(' ' | COLOR_PAIR(COLOR_BLACK));
*/
    win = newwin(y, x, 0, 0);
    werase(win);
    wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove(win, 1, 1);
    wrefresh(win);
    do {
        ch = wgetch(win);
        waddch(win, '.');
        wrefresh(win);
    } while (ch!='Q');
    finish(0);
    printf("maxxy: %d,%d\n", y, x);
    return 0;
}
