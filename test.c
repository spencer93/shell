/********** test.c file *************
* set up new processes for terminal login
*
*
************************************/

#include "ucode.c"

int main(int argc, char *argv[ ])
{
  int i = 0;
  int in, out; // file descriptors for terminal I/O

  in = open("/dev/tty0", O_RDONLY); // file descriptor 0
  out = open("/dev/tty0",O_WRONLY); // for display to console

  int pid = getpid(); // process running init is P1
  printf("proc %d running test program\n", pid);

  printf("argc = %d\n", argc);
  for (i = 0; i < argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("updated");

  while(1);
}
