/*************************** sh.c ************************
            cat   [filename]
            more  [filename]         // more as in Unix
            grep  pattern [filename] // print lines containing pattern
            l2u   [f1 f2]            // convert lower case to upper case
            cp    src dest           // src may be a file or DIR
**********************************************************/
#include "ucode.c"


void menu(){
    printf("\n\r***************** SHELL MENU *******************\n\r");
    printf("   | cd | ls | l2u | more | cat | pipe |\n\r");
    printf("************************************************\n\r");
}

/************ simple pipe example *************************
  pipe(pd);
  if (fork()){
    printf("parent************************\n\r");
    close(pd[0]);
    while(i++ < 200){
      //printf("parent pipe");
      write(pd[1],"a",1);
      printf("%d",i);
    }
    printf("parent exiting...\n\r");
    exit(0);
  }
  else{
    printf("child***************\n\r");
    close(pd[1]);
    while(1){
      //printf("child reading..\n\r");
      if ((n = read(pd[0],line,256)) == 0){
        printf("CHILD EXIT..");
        exit(0);

      }
      printf("read a line %s%d",line,strlen(line));
    }
  }
**********************************************************/

void printHead(char **tokens,int start,int stop){

  while (start <= stop){
    printf("%s ",tokens[start++]);
  }
  printf("\n\r");
}


// returns the right most pipe index in tokens between start and stop (inclusive of both)
// otherwise returns -1
int hasPipe(char ** cmdtokens,int start, int stop){
  int i = 0;
  for (i = stop; i >= start; i--){
    if (strcmp("|",cmdtokens[i]) == 0){
      return i;
    }
  }
  return -1;
}


int isIORedirect(char * in){
  if (strcmp(in,"<") == 0 || strcmp(in,">") == 0 || strcmp(in, ">>") == 0 || strcmp(in, "<<") == 0)
    return 1;
  return 0;
}
/*
cat   [filename]         // cat filename or stdin to stdout
more  [filename]         // more as in Unix
grep  pattern [filename] // print lines containing pattern
l2u   [f1 f2]            // convert lower case to upper case
cp    src dest           // src may be a file or DIR
*/
int IORedirection(char **cmdtokens, int start, int stop){
  int i = 0;
  int in_fd = 0;
  int out_fd = 0;
  int patternIndex = 0;
  int cmdlen = stop - start;

  if (cmdlen > 0){
    if ((strcmp(cmdtokens[start],"cat") == 0 || strcmp(cmdtokens[start],"more") == 0) && !isIORedirect(cmdtokens[start+1])){
      in_fd = open(cmdtokens[start+1],O_RDONLY);
      dup2(in_fd,0);
      close(in_fd);
    }
    else if (strcmp(cmdtokens[start],"l2u") == 0){ // at least one file input
      if (!isIORedirect(cmdtokens[start+1])){ // if the first input isnt ioredirect then it is our input file
        in_fd = open(cmdtokens[start+1],O_RDONLY);
        dup2(in_fd,0);
        close(in_fd);
        if (cmdlen > 1 && !isIORedirect(cmdtokens[start+2])){ // the second argument is our output file
          out_fd = open(cmdtokens[start+2],O_WRONLY|O_CREAT);
          dup2(out_fd,1);
          close(out_fd);
        }
      }
    }
    else if (strcmp(cmdtokens[start],"grep") == 0){
      if (cmdlen > 1 && !isIORedirect(cmdtokens[start+2])) { // then second arg is a file
        in_fd = open(cmdtokens[start+2],O_RDONLY);
        dup2(in_fd,0);
        close(in_fd);
      }
      patternIndex = start+1;
    }
    else if (strcmp(cmdtokens[start],"cp") == 0){ // requires two args
      //printf("COPYING redirect\n\r");
      in_fd = open(cmdtokens[start+1],O_RDONLY);
      dup2(in_fd,0);
      close(in_fd);
      out_fd = open(cmdtokens[start+2],O_WRONLY|O_CREAT);
      dup2(out_fd,1);
      close(out_fd);
    }
  }
  else{
    //printf("didnt redirect yet...\n\r");
  }

  for (i = start; i < stop; i++){
    if(strcmp(cmdtokens[i],"<") == 0){
      in_fd = open(cmdtokens[i+1], O_RDONLY);
      dup2(in_fd, 0);
      close(in_fd);
    }
    if(strcmp(cmdtokens[i],"<<") == 0){ // NO SUPPORT: this is a here document

    }
    if(strcmp(cmdtokens[i],">") == 0){
      //out_fd = open(cmdtokens[i+1],O_CREAT);
      out_fd = open(cmdtokens[i+1],O_WRONLY|O_CREAT);
      //printf("redirecting to a file...");
      //printf("******** new file fd: %d\n\r",out_fd);
      /*if (out_fd == -1){
        creat(cmdtokens[i+1]);
        out_fd = open(cmdtokens[i+1],O_WRONLY);
        //printf("filename: %s\n\r",cmdtokens[i+1]);
        ///out_fd = open(cmdtokens[i+1],O_RDWR);
        //printf("******** new file fd: %d\n\r",out_fd);
      }*/
      dup2(out_fd,1);
      close(out_fd);

    }
    if(strcmp(cmdtokens[i],">>") == 0){
      out_fd = open(cmdtokens[i+1], O_WRONLY | O_APPEND);
      dup2(out_fd,1);
      close(out_fd);
    }
  }
  return patternIndex;
}

// start and stop are the indices in cmdtokens containing the beginning and end of
// the command inclusive
char * combine = 0;
int executeCommand(char **cmdtokens, int start, int stop){
    // at this point both ends of the pipe are configured properly to stdin and stdout
    // as replacements for FD0 and FD1 we will only modify them if there are IO redirects
    // which takes precedence over the pipes
    // cat more
    char *cp = 0;
    char *pp = 0;
    int patternIndex = 0;
    //printHead(cmdtokens,start,stop);
    patternIndex = IORedirection(cmdtokens,start,stop); // finds and completes ioredirection
    if (patternIndex != 0){
      cp = cmdtokens[start];
      pp = cmdtokens[patternIndex];
      while(*cp){
        cp++;
      }
      *cp++ = ' ';
      while(*pp){
        *cp++ = *pp++;
      }
      *cp = '\0';
    }

    //printf("exec arg: %s",cmdtokens[start]);
    exec(cmdtokens[start]);
    // head cmdline before redirections
    // exec(head  int pid, pd[2], status;)
    // exec("cat filename");
    // exec("cat");
}


int pid, status;

// > file   redirects stdout to this file
// >> file  redirects stdout to this file and appends
void doPipe(char ** cmdtokens,int tokenStart,int tokenStop, int *pd){
  int pipeIndex;
  int head = tokenStart, tail = -2;
  int lpd[2];
  //printf("START: %d STOP: %d ",tokenStart,tokenStop);
  //printHead(cmdtokens,tokenStart,tokenStop);
  if (pd){ // gonna wanna write this to the write end of the pipe the parent created
    //printf("current proc: %d lpd[0] %d lpd[1] %d\n\r",getpid(),pd[0],pd[1]);
    //printf("setting %d up as writer..",getpid());
    close(pd[0]);   // close the read side of the pipe, we are the child and we only need
                    // to write to this pipe
    dup2(pd[1],1);  // copy stdout out with the write side of the pipe, now all
                    // our commands that are intended to write to stdout will actually
                    // be writing to the pipe
    // close(pd[1]); // no need to keep two connections to the pipe open
  }

  tail = hasPipe(cmdtokens,tokenStart,tokenStop);
  // printf("pipe index %d\n\r",tail);
  // printf("dopipe pid: %d tail: %d\n\r",getpid(),tail);

  if(tail != -1){ // FOUND A PIPE

    pipe(lpd); // creating a new pipe
    pid = fork();
    if (pid){  // make the parent the reader of the pipe
      //printf("current proc: %d lpd[0] %d lpd[1] %d\n\r",getpid(),lpd[0],lpd[1]);
      //printf("child proc: %d\n\r",pid);
      close(lpd[1]);         // immediately proceed down dup stdin in to be the pipe
      dup2(lpd[0],0);        // so that when we go to execute executeCommand it is going to freeze
      //close(lpd[0]);       // because it tries to read from an empty pipe, so the child will begin executing to get some stuff in that
      executeCommand(cmdtokens,tail+1,tokenStop); // pipe for the parent to read
    }
    else{
      //printf("current proc: %d lpd[0] %d lpd[1] %d\n\r",getpid(),lpd[0],lpd[1]);
      //printf("child proc: %d\n\r",pid);
      doPipe(cmdtokens,tokenStart,tail-1,lpd);
    }
  }
  else{ // no pipe handle this normally
    executeCommand(cmdtokens,tokenStart,tokenStop);
  }
}


int main(int argc, char*argv[]){
  int numTokens;
  int in = -1,out = -2, err = -3;
  int i = 0, index = 0;
  int cci = 0; // current command index
  int console;
  int foundPipe = 0, pipeIndex = 0,n;

  int realLen = 0;
  int length;
  char* valids[40], line[256];
  char command[1024];
  char *tokens[40];
  char tty[256];

  close(0);
  close(1);
  gettty(tty);
  in = open(tty, O_RDONLY); // file descriptor 0
  out = open(tty, O_WRONLY); // for display to console
  err = open(tty, O_RDONLY);

  if (out == -1){
    return -1;
  }
  printf("tty: %s\n\r",tty);
  printf("argv[0] %s\n\r",argv[0]);
  printf("argc:%d\n\r",argc);
  printf("SHELL pid: %d\n\r",getpid());

  while(1){
    close(0);
    close(1);
    close(2);
    in = open("/dev/tty0", O_RDONLY); // file descriptor 0
    out = open("/dev/tty0",O_WRONLY); // for display to console 1
    err = open("/dev/tty0", O_RDONLY); // file descriptor 2
    menu();
    gettty(tty);
    printf("---tty: %s -----\n\r",tty);
    index = 0;
    printf("sh # "); gets(command);
    numTokens = parseLine(command,tokens,' ');
    printf("%d",numTokens);
    /********** removes all empty strings ******************/
    for (i = 0; i < numTokens; i++){
      if (strcmp(tokens[i],"") == 0)
        continue;
      else
        valids[index++] = tokens[i];
    }
    /*******************************************************/
    // now we've got the real list of commands no empty strings
    realLen = index;
    printf("realLen %d\n\r",realLen);
    printf("recomp\n\r");
    for (i = 0; i < realLen; i++){
      printf("%s \n\r",valids[i]);
    }
    if(realLen <= 0) // user input no commands
      continue;
    if (realLen >= 2 && strcmp("cd",tokens[0]) == 0){ // handles simple change dir
      chdir(tokens[1]);
      continue;
    }
    if (strcmp("exit",tokens[0]) == 0){ // handles exit shell
      printf("\n\rexiting....\n\r");
      exit(0);
    }
    if (strcmp("pwd",tokens[0]) == 0){
      pwd();
      continue;
    }


    // if this point is reached the command in not trivial so fork a proc to handle it
    pid = fork(); // fork a child proc returns the child proc pid

    if (pid){ // this is the parent proc
        // if & do in new shell????
      printf("current proc: %d\n\r",getpid());
      printf("child proc: %d\n\r",pid);
      pid = wait(&status);
      printf("SHELL COMMAND FINISHED");
    }
    else{
      doPipe(valids,0,realLen-1,0);
    }

  } // main shell while loop

} // end main prog
