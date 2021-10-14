/****
  * File containing helper functions
  *
  */

// Dependencies
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string.h>

// Set namespace
using namespace std;

// Function to convert time to human readable format
void convert_time_to_human_readable(time_t t, char timbuf[80]){
  struct tm lt;
  localtime_r(&t, &lt);
  char tim[80];
  strftime(tim, sizeof(tim), "%c", &lt);
  string res(tim);
  strcpy(timbuf, res.c_str());
}

// Function to convert size to human readable format
void convert_size_to_human_readable(double size, char sizebuf[80]){
  string units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
  int counter = 0;
  while(size >= 1024){
    size /= 1024;
    ++counter;
  }
  string res = "";
  stringstream ss;
  ss << size;
  ss >> res;
  res = res+units[counter];
  strcpy(sizebuf, res.c_str());
}

// Function to convert permissions to human readable format
void convert_permission_to_human_readable(mode_t perm, char permbuf[80]){
  char modeval[11];
  modeval[0] = (perm & S_IFDIR) ? 'd' : '-';
  modeval[1] = (perm & S_IRUSR) ? 'r' : '-';
  modeval[2] = (perm & S_IWUSR) ? 'w' : '-';
  modeval[3] = (perm & S_IXUSR) ? 'x' : '-';
  modeval[4] = (perm & S_IRGRP) ? 'r' : '-';
  modeval[5] = (perm & S_IWGRP) ? 'w' : '-';
  modeval[6] = (perm & S_IXGRP) ? 'x' : '-';
  modeval[7] = (perm & S_IROTH) ? 'r' : '-';
  modeval[8] = (perm & S_IWOTH) ? 'w' : '-';
  modeval[9] = (perm & S_IXOTH) ? 'x' : '-';
  modeval[10] = '\0';
  string res(modeval);
  strcpy(permbuf, res.c_str());
}

// Function to open file in editor
void open_in_editor(char path[FILENAME_MAX]){
  char* args[3];
  string cmd = "vi";
  args[0] = (char*)cmd.c_str();
  args[1] = path;
  args[2] = NULL;
  // Call fork
  pid_t pid = fork();
  // Check if child process
  if(pid == 0){
    if(execvp(args[0], args) == -1){
      perror("exec");
    }
  }else{
    // Wait for child process
    wait(0);
  }
}
