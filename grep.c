/********************** cat.c **************************
this program reads from file descriptor 0 and write to
file descriptor 1. Upon entry these have been set to the
correct files by the shell, no modification of the input and
output source takes place in this file.
NOTE: FD0 and FD1 could refer to actual stdin or stdout, keyboard input
      and lcd output, or they could refer to files, OR they could refer to the
      read and write ends of a pipe
********************************************************/
#include "ucode.c"


int containsPattern(char *s,char *pattern, int len){
  s[len] = '\0';
  int index = strlen(s)-strlen(pattern);

  while(index >= 0) {
      if (strncmp(s + index, pattern, strlen(pattern)) == 0) {
          return 1;
      }
      index--;
  }
  return 0;
}
int main(int argc, char *argv[]){
  int bytesRead = 0;
  int i = 0;
  int lineLength = 128;
  int characterCount = 0;
  char line[lineLength];
  char buf[1];
  char carriageReturn = '\r';
  char z = 'Z';
  int len = 0;
  //printf("arg0 %s\n\r",argv[0]);
  //printf("arg1 %s\n\r",argv[1]);
  // at this point fd will be either 0 (stdin) or N (file) where N > 0
  if(!inputRedirected()){ // reading from stdin in
    while(1){
      len = gets(line);
      if (containsPattern(line,argv[1],len)){
        printf("%s",line);
      }
      if (!outputRedirected()){ // if printing to stdout print a newline
        printf("\n\r");
      }
    }
  }
  else{ // reading from file or pipe
    while (bytesRead = read(0,buf,1)){
      line[characterCount++] = buf[0];
      // if we've filled up a line OR we see a newline character (this must be dealed with immediately)
      if(characterCount >= lineLength || buf[0] == '\n'){
        if (containsPattern(line,argv[1],characterCount)){
          while(i < characterCount){
            write(1,&line[i++],1);
          }
          if (!outputRedirected()){ // WRITING TO STDOUT, so this is needed for a newline
            write(1,&carriageReturn,1);
          }
        }
        characterCount = 0;
        i = 0;
      }
    }
  }

}
