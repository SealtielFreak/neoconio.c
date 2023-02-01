#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "conio.h"

#define RES_FONT         "../assets/ModernDOS8x16.ttf"

int main() {
    int x , y;

    initconio(0);

    if(auto status = !loadfont(RES_FONT); status) {
        return EXIT_FAILURE;
    }

    titlewindow("Conio!");

    while(refresh()) {
        clrscr();

        if(kbhit()) {
            char chr = getch();
            printf("%i %i\n", x, y);
            printf("%c %u\n", chr, chr);

            switch (chr) {
                case 'w': {
                    y -= 1;
                    break;
                }
                case 's': {
                    y += 1;
                    break;
                }
                case 'a': {
                    x -= 1;
                    break;
                }
                case 'd': {
                    x += 1;
                    break;
                }
            }

            continue;
        }


        putchxy(x, y, '0');

        textcolor(BLUE);
        putchxy(0, 0, '*');
        textcolor(RED);
        putchxy(79, 0, '*');
        textcolor(GREEN);
        putchxy(0, 24, '*');
        textcolor(MAGENTA);
        putchxy(79, 24, '*');

        textcolor(CYAN);
        textbackground(WHITE);
        cputsxy(0, 1, "Hello World");
        cputsxy(0, 2, "From conio.h");
    }

    quitconio();

    return 0;
}