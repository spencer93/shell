/********************** login.c ************************/
// login.c : upon entry, argv[0]=login, argv[1]=/dev/ttyX
#include "ucode.c"
int in, out, err;
char lname[128], lpassword[128];

// argv[0]=login argv[1]=/dev/ttyX
main(int argc, char *argv[]){

  int pwdfd, endFile, i = 0;
  char *tokens[20];
  char temp[1024];
  int linelen = 0, loggedIn = 0, result;
  int uid, gid;
  int duped = 0;

  close(0);
  close(1);
  in = open(argv[1], O_RDONLY); // file descriptor 0
  out = open(argv[1], O_WRONLY); // for display to console fd 1
  printf("IN: %d\n\r",in);
  printf("OUT: %d\n\r",out);
  /*
  err = open(argv[1], O_RDONLY); // fd 2
  printf("ERR: %d\n\r",err);
  */

  fixtty(argv[1]); // set tty name string in PROC.tty

  pwdfd = open("/etc/passwd", O_RDONLY);

  while(1){
    printf("login:"); gets(lname);
    printf("password:"); gets(lpassword);
    do{
      linelen = fileGetLine(pwdfd,temp,&endFile);
      //printf("temp - %s pwd %d linelen %d",temp,pwdfd,linelen);
      //getc();
      if (linelen != -1 && linelen != 0){
        parseLine(temp,tokens,':');
        //printf("parsing");
        //printf("%s %s",tokens[0],tokens[1]);
        //getc();
        if (strcmp(tokens[0],lname) == 0 && strcmp(tokens[1],lpassword) == 0){
          printf("matched\n\r");
          getc();
          //loggedIn == 1;
          uid = atoi(tokens[2]);
          gid = atoi(tokens[3]);
          // uid = (int)tokens[2][0] - 48;
          // gid = (int)tokens[3][0] - 48;
          printf("uid %d gid %d tokens[5] %s --\n\r",uid,gid,tokens[5]);
          getc();
          chuid(uid,gid);
          printf("No----\n\r");
          getc();
          chdir(tokens[5]);
          printf("case 3\n\r");
          getc();
          close(pwdfd);
          printf("case 4\n\r");
          getc();
          //printf("userprogram:%s",tokens[6]);
          //getc();
          exec("sh");
        //  exec(tokens[6]); // exec to program in user account
        }
      }
    }
    while(endFile != 0);
    printf("exec continued");

    /*if (loggedIn == 1){
          printf("returning from closed shell");
          printf("login failed, try again");
          close(pwd);
          loggedIn = 0;
    }*/
    printf("-- %d --",pwdfd);
    close(pwdfd);
    pwdfd = open("/etc/passwd", O_RDONLY);

  }
}
