/*** include ***/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios; // saves terminal original's attribute'

/*** funtion declaration ***/

void enableRawMode();
void disableRawMode();
void die(const char *s);
char editorReadKey();
void editorProcessKeypresses();
void editorRefreshScreen();

/*** init ***/

int main()
{
    enableRawMode();

    while (1) 
    {
        editorRefreshScreen();
        editorProcessKeypresses();
    }
    return 0;
} 

/*** terminal ***/

void enableRawMode() 
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) die("tcsetattr");
    atexit(disableRawMode); // called when the program exits
    
    struct termios raw = orig_termios; // assigned orig_termios to raw to make a copy of it
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
    // ECHO is a bitflag, therefore this bit operation flipped the bits to be 00000000000000000000000000000000
    // ICANON is not an input flag, its a "local" flag in the c_lflag field, 
    // so now the program will quit when 'q' was pressed.
    // ISIG is also not an input flag, now the ctrl-c and ctrl-z is disabled
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode() 
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    }
}

void die(const char *s) 
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

char editorReadKey() 
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
    {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}

/*** input ***/

void editorProcessKeypresses()
{
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

/*** output ***/

void editorRefreshScreen()
{
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}
