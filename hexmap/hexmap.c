#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <curses.h>

typedef struct rect {
    int x, y, w, h;
} rect;

typedef struct point {
    int x, y;
} point;
/*
typedef struct window {
    struct window *child;
    struct window *next;
    WINDOW *win;
    int w, h;
    unsigned int flags;
} window;

#define MAXWINDOWS 16
window * windows[MAXWINDOWS];

window * create_window(window *parent, rect size, unsigned int flags) {
    int i;
    for (i=0;i!=MAXWINDOWS;++i) {
        if (!windows[i]) {
            window *w = (window *)malloc(sizeof(window));
            windows[i] = w;
            if (parent) {
                w->next = parent->child;
                parent->child = w;
                w->win = derwin(parent->win, size.h, size.w, size.y, size.x);
            } else {
                w->win = newwin(size.h, size.w, size.y, size.x);
            }
            return w;
        }
    }
    return 0;
}
*/
static int json_getint(cJSON *json, const char *key, int def) {
    json = cJSON_GetObjectItem(json, key);
    return json ? json->valueint : def;
}

static const char * json_getstr(cJSON *json, const char *key, const char *def) {
    json = cJSON_GetObjectItem(json, key);
    return json ? json->valuestring : def;
}

void finish(int sig) {
    erase();
    endwin();
    exit(sig);
}

void draw_map(WINDOW *win, point origin, cJSON *json, int def) {
    int x, y;
    rect view;

    getbegyx(win, view.y, view.x);
    getmaxyx(win, view.h, view.w);
    for (y=0;y<view.h;++y) {
        int yp = y;
        for (x=0;x<view.w;x+=2) {
            int xp = x + (y&1);
            mvwaddch(win, yp, xp, def);
        }
    }
}

void run(cJSON *json) {
    int def = 'a';
    WINDOW *win, *sub;
    point origin = {0, 0};

    initscr();
    signal(SIGINT, finish);      /* arrange interrupts to terminate */
    start_color();
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    attrset(COLOR_PAIR(COLOR_BLACK));
    bkgd(' ' | COLOR_PAIR(COLOR_BLACK));
    bkgdset(' ' | COLOR_PAIR(COLOR_BLACK));
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    nonl();         /* tell curses not to do NL->CR/NL on output */
    cbreak();       /* take input chars one at a time, no wait for \n */
    noecho();       /* don't echo input */
    scrollok(stdscr, FALSE);
    wclear(stdscr);

    win = newwin(20, 72, 2, 4);
    box(win, 0, 0);
    sub = subwin(win, 12, 40, 4, 16);
    draw_map(win, origin, json, def | COLOR_PAIR(COLOR_WHITE));
    wrefresh(win);
    for (;;) {
        int c;
        // wrefresh(sub);
        c = getch();     /* refresh, accept single keystroke of input */
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
        draw_map(sub, origin, json, (def+1) | COLOR_PAIR(COLOR_YELLOW));
        wrefresh(sub);
    }
}
/*
void print_map(cJSON *json, FILE *F) {
    int w = 1, h = 1;
    int fno = 0;
    cJSON *region, *regions = cJSON_GetObjectItem(json, "regions");
    cJSON *faction, *factions = cJSON_GetObjectItem(json, "factions");
    char * mapdata;

    faction = cJSON_GetObjectItem(json, "faction");
    if (faction) {
        fno = cJSON_GetObjectItem(faction, "id")->valueint;
    }
    for (region=regions->child; region; region=region->next) {
        int x = json_getint(region, "x", 0);
        int y = json_getint(region, "y", 0);
        if (x>=w) w=x+1;
        if (y>=h) h=y+1;
    }
    mapdata = (char *)malloc(w*h*sizeof(char));
    memset(mapdata, ' ', w*h*sizeof(char));
    for (region=regions->child; region; region=region->next) {
        int x = (json_getint(region, "x", 0) + xof) % w;
        int y = (json_getint(region, "y", 0) + yof) % h;
        const char * terrain = json_getstr(region, "terrain", 0);
        mapdata[x+y*h] = terrain[0];
    }
}
*/
int main (int argc, char **argv) {
    FILE *in = stdin, *out = stdout;
    long len;
    char *data;
    cJSON *json;
    if (argc>1) {
        in = fopen(argv[1], "r");
        if (!in || errno) {
            perror("could not open input file");
            return errno;
        }
    }
    fseek(in,0,SEEK_END);
    len=ftell(in);
    fseek(in,0,SEEK_SET);
    data = (char *)malloc(len+1);
    if (data) {
        fread(data,1,len,in);
    }
    if (in!=stdin) fclose(in);

    json = cJSON_Parse(data);
    run(json);
    free(data);
    cJSON_Delete(json);
    if (out!=stdout) fclose(out);

    return 0;
}
