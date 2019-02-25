#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>

#define CTRL_KEY(k) ((k)&0x1F)

/* data*/
struct termios orig_termios;

/*for terminal*/
void die(const char*s){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}
void noRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)==-1)
		die("tcsetattr");
}

void rawMode(){
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1)
		die("tcgetattr");
    atexit(noRawMode);
    
    struct termios raw = orig_termios;
	raw.c_iflag &= ~(ICRNL | IXON | BRKINT | INPCK | ISTRIP);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN]=0;
	raw.c_cc[VTIME]=1;
    
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) 
		die("tcsetattr");
}

char readKey() {
	int n;
	char c;
	while (n = read(STDIN_FILENO, &c, 1)!=1) {
		if(n ==-1 && errno != EAGAIN)
			die("read");
	}
	return c;
}


/*for output*/
void refreshScreen() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
}
	

/*for input*/
void processKeys() {
	char c = readKey();
	
	switch (c) {
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}

/*init*/
int main(){
    rawMode();
	while(1) {
		refreshScreen();
		processKeys();
	}
	return 0;
}
