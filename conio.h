#ifndef CONIO_H
#define CONIO_H

struct text_info {
    unsigned char curx, cury;
    unsigned short attribute, normattr;
    unsigned char screenwidth, screenheight;
};

struct char_info {
    char letter;
    unsigned short attr;
};

enum COLORS {
    BLACK, BLUE, GREEN, CYAN,
    RED, MAGENTA, BROWN, LIGHTGRAY,
    DARKGRAY, LIGHTBLUE, LIGHTGREEN, LIGHTCYAN,
    LIGHTRED, LIGHTMAGENTA, YELLOW, WHITE
};

#ifdef SDL_INCLUDE

#include <SDL2/SDL.h>

SDL_Window *getwindow();

SDL_Renderer *getrenderer();

SDL_GLContext *getcontext();

#endif

#ifdef DEBUG_MODE

void getbuffer(char ***buff);

#endif

#ifdef __cplusplus
extern "C"{
#endif

    void getbuffersize(unsigned int *w, unsigned int *h);

    int wasinit(void);

    int initconio(unsigned int flags);

    void quitconio(void);

    void scalewindow(unsigned scale);

    void titlewindow(const char *title);

    int loadfont(const char *filename);

    void resize(int columns, int rows);

    int refresh(void);

    void gettextinfo(struct text_info *info);

    void inittextinfo(void);

    void clreol(void);

    void clrscr(void);

    void delline(void);

    void insline(void);

    void _conio_gettext(int left, int top, int right, int bottom, struct char_info *buf);

    void puttext(int left, int top, int right, int bottom, struct char_info *buf);

    void movetext(int left, int top, int right, int bottom, int destleft, int desttop);

    void gotoxy(int x, int y);

    void cputsxy(int x, int y, char *str);

    void putchxy(int x, int y, char ch);

    void _setcursortype(int type);

    void textattr(int _attr);

    void normvideo(void);

    void textbackground(int color);

    void textcolor(int color);

    int wherex(void);

    int wherey(void);

    char *getpass(const char *prompt, char *str);

    void highvideo(void);

    void lowvideo(void);

    void delay(int ms);

    void switchbackground(int color);

    void flashbackground(int color, int ms);

    void clearkeybuf(void);

    int kbhit(void);

    int getch(void);

    int getche(void);

#ifdef __cplusplus
}
#endif

#endif //CONIO_H