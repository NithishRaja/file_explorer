/****
  * Main file
  *
  */

// Dependencies
#include <vector>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
// Local Dependencies
#include "file_handler.cpp"
#include "path_parser/path_parser.cpp"

// Set namespace
using namespace std;

int main(){
  // Initialise object to parse path
  PathParser pp;
  // Initialise buffer to hold home path
  struct passwd *pw = getpwuid(getuid());
  char homedir[256];
  strcpy(homedir, pw->pw_dir);
  
  // char currdir[FILENAME_MAX]; //create string buffer to hold path
  // getcwd( currdir, FILENAME_MAX );
  // cout<<currdir<<endl;
  // strtok(currdir, "\n");

  // Create buffer to hold path
  char currdir[FILENAME_MAX];
  getcwd( currdir, FILENAME_MAX );
  // Parse path
  string temp(currdir);
  temp = pp.parse(temp);
  strcpy(currdir, temp.c_str());

  vector<struct file_info_hr> list = get_dir_content(currdir);

  for(int i=0;i<list.size();++i){
    cout.width(12);
    cout<<left<<list[i].permission;
    cout.width(10);
    cout<<left<<list[i].user;
    cout.width(10);
    cout<<left<<list[i].group;
    cout.width(10);
    cout<<left<<list[i].size;
    cout.width(25);
    cout<<left<<list[i].last_modified;
    cout.width(10);
    cout<<left<<list[i].name;
    cout<<endl;
  }

}
