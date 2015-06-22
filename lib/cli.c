#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>

#define NB_ENABLE (0)
#define NB_DISABLE (1)

int keypress () {
   fd_set buffer;
   struct timeval time;
   // poll
   time.tv_sec = 0;
   time.tv_usec = 0;
   FD_ZERO(&buffer);
   FD_SET(STDIN_FILENO, &buffer);
   select(STDIN_FILENO + 1, &buffer, NULL, NULL, &time);
   return FD_ISSET(STDIN_FILENO, &buffer);
}

void raw_tty(int state) {
   struct termios ttystate;
   // get state
   tcgetattr(0, &ttystate);
   if (state == NB_ENABLE) {
       ttystate.c_lflag &= ~ICANON;
       ttystate.c_lflag &= ~ECHO;
       ttystate.c_cc[VMIN] = 1;
   } 
   else if (state == NB_DISABLE) {
       ttystate.c_lflag |= ICANON;
       ttystate.c_lflag |= ECHO;
   }
   tcsetattr(0, TCSANOW, &ttystate);
}
