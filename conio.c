#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


#define I_MAP_COLOR(r, g, b) { r, g, b, 255 }

#define MAP_COLOR(r, g, b) ({ SDL_Color color = I_MAP_COLOR(r, g, b); color; })

#define DEFAULT_F_RECT() ({SDL_FRect __rect = {0.0, 0.0, 0.0, 0.0}; __rect; })

#define DEFAULT_PSIZE               16
#define DEFAULT_WINDOW_SIZE_X       640
#define DEFAULT_WINDOW_SIZE_Y       480

#define DEFAULT_COLUMNS             80
#define DEFAULT_ROWS                25

#define DEFAULT_SCALE_X             6
#define DEFAULT_SCALE_Y             12

#include "conio.h"

const SDL_Rect empty_rect = {0, 0, 0, 0};
const SDL_FRect empty_rectf = {0, 0, 0, 0};

const SDL_Color colors[] = {
        [BLACK] = I_MAP_COLOR(0, 0, 0),
        [BLUE] = I_MAP_COLOR(0, 0, 255),
        [GREEN] = I_MAP_COLOR(0, 255, 0),
        [CYAN] = I_MAP_COLOR(0, 255, 255),
        [RED] = I_MAP_COLOR(255, 0, 0),
        [MAGENTA] = I_MAP_COLOR(255, 0, 255),
        [WHITE] = I_MAP_COLOR(255, 255, 255),
};

typedef struct {
    SDL_Color foreign, background;
} ColorCharacter;

/* private attributes */
// hardware
static SDL_Window *m_window;
static SDL_Renderer *m_renderer;

// system resources
int key_id_pressed = 0;
static SDL_Texture *m_map_textures[UCHAR_MAX];
static TTF_Font *m_font;
static char m_buff_chr[32];
SDL_Color m_background;
SDL_Color m_foreign;

// attributes console
static char **m_chr_matrix;
static ColorCharacter **m_color_matrix;
SDL_Point m_shape_window = {DEFAULT_WINDOW_SIZE_X, DEFAULT_WINDOW_SIZE_Y};
SDL_Point m_shape_matrix = {DEFAULT_COLUMNS, DEFAULT_ROWS};
size_t m_scale = 5;

// attributes cursor
SDL_Point m_cursor = {0, 0};

// private functions
bool valid_cursor(int x, int y);

void initcolors(void);

void initmatrix(void);

void initmapchr(void);

void quitcolors(void);

void quitmatrix(void);

void quitmapchr(void);

void resetcolors(void);

// public functions
void getbuffer(char ***buff) {
    *buff = m_chr_matrix;
}

void getbuffersize(unsigned int *w, unsigned int *h) {
    *w = m_shape_matrix.x;
    *h = m_shape_matrix.y;
}

int wasinit(void);

int initconio(unsigned int flags) {
    if (!SDL_WasInit(0)) {
        // nothing
    }

    SDL_Init(SDL_INIT_EVERYTHING);

    if (SDL_CreateWindowAndRenderer(m_shape_window.x, m_shape_window.y, SDL_RENDERER_ACCELERATED | SDL_WINDOW_RESIZABLE,
                                    &m_window, &m_renderer) != 0) {
        return EXIT_FAILURE;
    }

    initmatrix();
    initcolors();

    return EXIT_SUCCESS;
}

void quitconio(void) {
    quitmatrix();
    quitmapchr();
    quitcolors();

    free(m_chr_matrix);

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);

    TTF_CloseFont(m_font);

    TTF_Quit();
    SDL_Quit();
}

void scalewindow(unsigned scale);

void titlewindow(const char *title) {
    SDL_SetWindowTitle(m_window, title);
}

int loadfont(const char *filename) {
    if (!TTF_WasInit()) {
        TTF_Init();
    }

    m_font = TTF_OpenFont(filename, DEFAULT_PSIZE);

    if (!m_font) {
        return 0;
    }

    initmapchr();

    return 1;
}

void resize(int columns, int rows) {
    quitmatrix();
    quitcolors();

    m_shape_matrix.x = columns;
    m_shape_matrix.y = rows;

    initmatrix();
    initcolors();
}

int refresh(void) {
    size_t y, x;
    SDL_Event event;

    SDL_RenderClear(m_renderer);

    for (y = 0; y < m_shape_matrix.y; y++) {
        for (x = 0; x < m_shape_matrix.x; x++) {
            int w, h;
            char chr;
            SDL_Color background, foreign;
            SDL_FRect rect;
            SDL_Texture *texture;

            chr = m_chr_matrix[y][x];
            texture = m_map_textures[chr];

            SDL_QueryTexture(texture, NULL, NULL, &w, &h);

            rect.w = (((float) m_shape_window.x / (float)m_shape_matrix.x) / (float)w) * (float)w;
            rect.h = (((float) m_shape_window.y / (float)m_shape_matrix.y) / (float)h) * (float)h;
            rect.x = rect.w * (float)x;
            rect.y = rect.h * (float)y;

            foreign = m_color_matrix[y][x].foreign;
            background = m_color_matrix[y][x].background;

            //if(chr != ' ')
            {
                SDL_FRect _rect = {rect.x, rect.y, 25, 25};
                SDL_SetRenderDrawColor(m_renderer, background.r, background.g, background.b, background.a);
                SDL_RenderFillRectF(m_renderer, &_rect);
            }

            SDL_SetTextureColorMod(texture, foreign.r, foreign.g, foreign.b);
            SDL_RenderCopyF(m_renderer, texture, NULL, &rect);
        }
    }

    SDL_RenderPresent(m_renderer);

    memset(m_buff_chr, 0, 32); // clear buffer characters

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 0;
        }

        if (event.type == SDL_KEYDOWN) {
            key_id_pressed = event.key.keysym.sym;

            strcat(m_buff_chr, SDL_GetKeyName(key_id_pressed));
            if (strlen(m_buff_chr) != 1) {
                if (strcmp(m_buff_chr, "Return") == 1) {
                    key_id_pressed = '\n';
                }
            } else {
                key_id_pressed = tolower(m_buff_chr[0]);
            }
        } else {
            key_id_pressed = 0;
        }

        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_shape_window.x = event.window.data1;
                m_shape_window.y = event.window.data2;

                // initmapchr();
            }
        }
    }

    resetcolors();

    return 1;
}

void gettextinfo(struct text_info *info);

void inittextinfo(void);

void clreol(void) {
    resetcolors();
}

void clrscr(void) {
    size_t i;

    for (i = 0; i < m_shape_matrix.y; i++) {
        memset(m_chr_matrix[i], ' ', m_shape_matrix.x);
    }

    clreol();
}

void delline(void);

void insline(void);

void _conio_gettext(int left, int top, int right, int bottom, struct char_info *buf);

void puttext(int left, int top, int right, int bottom, struct char_info *buf);

void movetext(int left, int top, int right, int bottom, int destleft, int desttop);

void gotoxy(int x, int y) {
    m_cursor.x = x;
    m_cursor.y = y;

    m_color_matrix[y][x].foreign = m_foreign;
    m_color_matrix[y][x].background = m_background;
}

void cputsxy(int x, int y, char *str) {
    const size_t length = strlen(str);
    const size_t from = x;
    size_t i;

    if(!valid_cursor(x, y)) {
        return;
    }

    gotoxy(x, y);
    char *row = m_chr_matrix[wherey()];

    for (i = 0; i < length; i++) {
        gotoxy(x + i, y);
        row[from + i] = str[i];
    }
}

void putchxy(int x, int y, char ch) {
    if(!valid_cursor(x, y)) {
        return;
    }

    gotoxy(x, y);
    char *row = m_chr_matrix[wherey()];

    row[wherex()] = ch;
}

void _setcursortype(int type);

void textattr(int _attr);

void normvideo(void);

void textbackground(int color) {
    m_background = colors[color];
}

void textcolor(int color) {
    m_foreign = colors[color];
}

int wherex(void) {
    return m_cursor.x;
}

int wherey(void) {
    return m_cursor.y;
}

char *getpass(const char *prompt, char *str);

void highvideo(void);

void lowvideo(void);

void delay(int ms) {
    SDL_Delay(ms);
}

unsigned long int dt(void) {
    return SDL_GetPerformanceFrequency();
}

void switchbackground(int color);

void flashbackground(int color, int ms);

void clearkeybuf(void);

int kbhit(void) {
    // return key_id_pressed != 0;
    return m_buff_chr[0] != '\0';
    // return strlen(m_buff_chr) == 0;
}

int getch(void) {
    return tolower(m_buff_chr[0]);
}

void initmatrix(void) {
    size_t i;

    m_chr_matrix = calloc(m_shape_matrix.y, sizeof(char *));

    for (i = 0; i < m_shape_matrix.y; i++) {
        m_chr_matrix[i] = calloc(m_shape_matrix.x, sizeof(char));
        memset(m_chr_matrix[i], ' ', m_shape_matrix.x);
    }
}

bool valid_cursor(int x, int y) {
    bool valid = !((x < 0 || x > m_shape_matrix.x) || (y < 0 || y > m_shape_matrix.y));

    if(!valid) {
        SDL_SetError("Invalid cursor position [%i, %i]", x, y);
    }

    return valid;
}

void initcolors(void) {
    size_t x, y;

    m_color_matrix = calloc(m_shape_matrix.y, sizeof(ColorCharacter *));

    for (y = 0; y < m_shape_matrix.y; y++) {
        m_color_matrix[y] = calloc(m_shape_matrix.x, sizeof(ColorCharacter));
        for (x = 0; x < m_shape_matrix.x; x++) {
            m_color_matrix[y][x].background = colors[BLACK];
            m_color_matrix[y][x].foreign = colors[WHITE];
        }
    }
}

void initmapchr(void) {
    size_t i;

    for (i = 0; i < UCHAR_MAX; i++) {
        SDL_Surface *surface = TTF_RenderGlyph_Shaded(m_font, i, colors[WHITE], colors[BLACK]);
        m_map_textures[i] = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_FreeSurface(surface);
    }
}

void quitcolors(void) {
    size_t i;

    for (i = 0; i < UCHAR_MAX; i++) {

    }
}

void quitmatrix(void) {
    size_t i;

    for (i = 0; i < m_shape_matrix.y; i++) {
        free(m_chr_matrix[i]);
    }
}

void quitmapchr(void) {
    size_t i;

    for (i = 0; i < UCHAR_MAX; i++) {
        SDL_DestroyTexture(m_map_textures[i]);
    }

}

void resetcolors(void) {
    size_t y, x;

    for (y = 0; y < m_cursor.y; y++) {
        for (x = 0; x < m_cursor.x; x++) {
            m_color_matrix[y][x].foreign = colors[WHITE];
            m_color_matrix[y][x].background = colors[BLACK];
        }
    }
}
