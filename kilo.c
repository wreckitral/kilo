#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios; // saves terminal original's attribute'
void enableRawMode();
void disableRawMode();

int main()
{
    enableRawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') 
    {
        if (iscntrl(c))
        {
            printf("%d\r\n", c);
        }
        else 
        {
            printf("%d ('%c')\r\n", c, c);
        }
    }
    return 0;
} 

void enableRawMode() 
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode); // called when the program exits
    
    struct termios raw = orig_termios; // assigned orig_termios to raw to make a copy of it
    raw.c_iflag &= ~(ICRNL | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
    // ECHO is a bitflag, therefore this bit operation flipped the bits to be 00000000000000000000000000000000
    // ICANON is not an input flag, its a "local" flag in the c_lflag field, 
    // so now the program will quit when 'q' was pressed.
    // ISIG is also not an input flag, now the ctrl-c and ctrl-z is disabled

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
