#if (defined __WIN32) | (defined __WIN64)
#include <conio.h>
#else
//from : https://blog.csdn.net/gaopu12345/article/details/30467099
#include <termio.h>
#include <stdio.h>
int getch(void)
{
    struct termios tm, tm_old;
    int fd = 0, ch;
    if (tcgetattr(fd, &tm) < 0)return -1;
    tm_old = tm;
    cfmakeraw(&tm);
    if (tcsetattr(fd, TCSANOW, &tm) < 0)return -1;
    ch = getchar();
    if (tcsetattr(fd, TCSANOW, &tm_old) < 0)return -1;
    return ch;
}
#endif