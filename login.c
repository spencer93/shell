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

  fixtty(argv[1]); // set tty name string in PROC.tty

  pwdfd = open("/etc/passwd", O_RDONLY);

  while(1){
    printf("login:"); gets(lname);
    printf("password:"); gets(lpassword);
    do{
      linelen = fileGetLine(pwdfd,temp,&endFile);
      if (linelen != -1 && linelen != 0){
        parseLine(temp,tokens,':');
        if (strcmp(tokens[0],lname) == 0 && strcmp(tokens[1],lpassword) == 0){
          close(pwdfd);
          gid = atoi(tokens[2]);
          uid = atoi(tokens[3]);
          chuid(uid,gid);
          chdir(tokens[5]);
          exec(tokens[6]); // exec to program in user account
        }
      }
    }
    while(endFile != 0);
    close(pwdfd);
    pwdfd = open("/etc/passwd", O_RDONLY);
  }
}
