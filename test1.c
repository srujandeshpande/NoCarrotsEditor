#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>

#define CTRL_KEY(k) ((k)&0x1F)

/* data*/
struct editorConfig {
	int screenrows;
	int screencols;
	struct termios orig_termios;
};

struct editorConfig E;

/*for terminal*/
void die(const char*s){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}
void noRawMode(){
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios)==-1)
		die("tcsetattr");
}

void rawMode(){
    if(tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
		die("tcgetattr");
    atexit(noRawMode);
    
    struct termios raw = E.orig_termios;
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

int getCursorPosition(int *rows, int *cols) {
	if(write(STDOUT_FILENO, "\x1b[6n",4) != 4) 
		return -1;
		
	printf("\r\n");
	char c;
	while (read(STDIN_FILENO, &c, 1) == 1) {
		if (iscntrl(c)) {
			printf("%d\r\n",c);
		}
		else {
			printf("%d ('%c')\r\n", c, c);
		}
	}
	
	readKey();
	return -1;
}

int getWindowSize(int *rows, int *cols) {
	struct winsize ws;
	
	if(1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col ==0) {
		if(write(STDOUT_FILENO,"\x1b[999C\x1b[999B", 12) != 12) return -1;
		readKey();
		return getCursorPosition(rows, cols);;
	}
	else {
		*cols = ws.ws_col;
		*rows = ws.ws_row;
		return 0;
	}
}


/*for output*/
void drawRows(){
	int y;
	for (y=0;y<E.screenrows;y++) {
		write(STDOUT_FILENO, "~\r\n", 3);
	}
}

void refreshScreen() {
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	
	drawRows();
	write(STDOUT_FILENO,  "\x1b[H",3);
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
void initEditor() {
	if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(){
    rawMode();
    initEditor();
	while(1) {
		refreshScreen();
		processKeys();
	}
	return 0;
}
