#include "console.h"

void pause() {   ///Freeze console until keyboard is pressed
    DWORD out;
    INPUT_RECORD r;
    while(1){
        HANDLE _stdin = GetStdHandle(STD_INPUT_HANDLE);
        if(!_stdin) break;
        ReadConsoleInput(_stdin, &r, 1, &out);
        if(r.EventType == KEY_EVENT) break;
        Sleep(10);
    }
}
