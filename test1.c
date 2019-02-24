#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

struct termios orig_termios;

void norawmode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void rawmode(){
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(norawmode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
int main(){
    rawmode();
    char c;
    while(read(STDIN_FILENO,&c,1)==1 && c!='q'){
        if (iscntrl(c)) {
            printf("%d\n",c);
        }
        else {
            printf("%d ('%c')\n",c,c);
        }
    }
}
