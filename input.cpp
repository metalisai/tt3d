#include "input.h"

#include <stdio.h>

int charToKeycode(char ch)
{
    switch(ch)
    {
        case 'q':
            return KEYCODE_Q;
        case 'w':
            return KEYCODE_W;
        case 'e':
            return KEYCODE_E;
        case 'r':
            return KEYCODE_R;
        case 't':
            return KEYCODE_T;
        case 'y':
            return KEYCODE_Y;
        case 'u':
            return KEYCODE_U;
        case 'i':
            return KEYCODE_I;
        case 'o':
            return KEYCODE_O;
        case 'p':
            return KEYCODE_P;
        case 'a':
            return KEYCODE_A;
        case 's':
            return KEYCODE_S;
        case 'd':
            return KEYCODE_D;
        case 'f':
            return KEYCODE_F;
        case 'g':
            return KEYCODE_G;
        case 'h':
            return KEYCODE_H;
        case 'j':
            return KEYCODE_J;
        case 'k':
            return KEYCODE_K;
        case 'l':
            return KEYCODE_L;
        case 'z':
            return KEYCODE_Z;
        case 'x':
            return KEYCODE_X;
        case 'c':
            return KEYCODE_C;
        case 'v':
            return KEYCODE_V;
        case 'b':
            return KEYCODE_B;
        case 'n':
            return KEYCODE_N;
        case 'm':
            return KEYCODE_M;
        default:
            printf("invalid char! %c\n",ch);
            return -1;
    }
}
