/********** init.c file *************
* set up new processes for terminal login
*
*
************************************/

#include "ucode.c"
int console = 0;

void child(){
    printf("executing CHILD umode image...");
}
void parent(){
    int pid, status;
    printf("executing PARENT umode image...");

    while(1){
      printf("INIT: wait for ZOMBIE child");
      pid = wait(&status);
      printf("awakening from wait");
      if (pid == console){
        printf("INIT: forks a new console");
        console = fork();
        if (console){
          printf("RIGHT AFTER FORK");
          continue;
        }
        else{
          printf("LATER AFTER FORK\n\r");
          exec("login /dev/tty0");
        }
      }
      printf("INIT: I just buried an orphan child proc %d",pid);
    }

}

int main(int argc, char *argv[ ])
{
  int i = 0;
  int in, out, err; // file descriptors for terminal I/O
  int pid, status;

  in = open("/dev/tty0", O_RDONLY); // file descriptor 0
  out = open("/dev/tty0",O_WRONLY); // for display to console
  err = open("/dev/tty0", O_RDONLY); // fd 2

  //printf("in %d",in);
  //printf("out %d",out);

  //printf("INIT: fork a  login process on the console");

  console = fork(); // new process

  if(console) // 1 = parent proc, nonzero means we've got the child PID
  {
    printf("PARENT process... %d",console);
    parent();
    //printf("before infinite loop");
    // wait(&status);
    //while(1);
    //printf("after infinite loop");
  }
  else if(console == 0) // 0 = child proc
  {
    printf("CHILD process... %d",console);
    //child();
  }

}
