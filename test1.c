#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void rawmode(){
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &- ~(ECHO);
    tcsetattr(STDIN_FILENO,TCSAFLUSH, &raw);
}
int main(){
    rawmode();
    char c;
    while(read(STDIN_FILENO,&c,1)==1 && c!='q');
    printf("Public static\n");
}
