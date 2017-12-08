// > file   redirects stdout to this file
// >> file  redirects stdout to this file and appends
void executeCommands(char **tokens,int length){
  int i = 0, index = 0;
  int cci = 0; // current command index
  int console;
  int foundPipe = 0, pipeIndex = 0;
  int pid, status;
  int realLen = 0;
  char* valids[40];
  /********** removes all empty strings ******************/
  for (i = 0; i < length; i++){
    if (strcmp(tokens[i],"") == 0)
      continue;
    else
      valids[index++] = tokens[i];
  }
  /*******************************************************/

  // now we've got the real list of commands no empty strings
  realLen = index;
  printf("realLen %d\n\r",realLen);
  for (i = 0; i < realLen; i++){
    printf("%s \n\r",valids[i]);
  }
  if(realLen <= 0){ return; } // user input no commands
  if (realLen >= 2 && strcmp("cd",tokens[0]) == 0){ // handles simple change dir
    chdir(tokens[1]);
    return;
  }
  if (strcmp("exit",tokens[0]) == 0){ // handles exit shell
    printf("\n\rexiting....\n\r");
    quit = 1;
    return;
  }
  if (strcmp("pwd",tokens[0]) == 0){
    pwd();
    return;
  }
  // if this point is reached the command in not trivial so fork a proc to handle it

  //                              (pipe assigns previous out as in)
  // cmd ... (establishes in and out) | cmd ... (establish output) | cmd .. (establish output) |
  // in -> out then set the output as input for the pipe

  console = fork();
  if (console == 0) // in child
  { // before sending the child off to execute this command lets set up the correct
    // input and output file descriptors
    // read our tokens until we hit either a pipe or we end
    for (i = 0; i < realLen; i++){
      if (strcmp("|",tokens[i]) == 0){
        foundPipe = 1;
        pipeIndex = i; // index of the pipe in tokens
      }
    }

    if(foundPipe){ // gonna have to modify our output to be the input end of a pipe
      // get and set all our stdin and stdout
      // now pipe
      // meaning set this stdout as pipe in
      int pid, pd[2];
      pipe(pd); // create a pipe: pd[0]=READ, pd[1]=write
      pid = fork(); // fork a child to share the pipe
      if (pid){ // parent as pipe READER
        close(pd[1]);   // close pipe WRITE end
        dup2(pd[0],0);  // redirect stdin to pipe READ end
        exec(cmd2);
      }
      else{
        close(pd[0]); // close pipe READ end
        dup2(pd[1],1);  // redirect stdout to pipe WRITE end
        exec(cmd1);
      }


    }
    else{ // set our in and out then execute commmand
      // didnt find a pipe so our io redirection inputs end at realLen in tokens
      // lets default to std in for in
      // cmd [filename < << > >>]
      // cat < f1
      // cat f1
      // cat f2 < f1
      // cat <<
      // cat < f1
      // filename
      // >
      // >>
      // <
      // <<

      for(i = cci+1; i < realLen; i++){

      }


      exec(tokens[cci]);
    }

  }
  else
    pid = wait(&status);

}
