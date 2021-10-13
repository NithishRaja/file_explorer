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
// Local Dependencies
#include "file_handler.cpp"

// Set namespace
using namespace std;

int main(){

  // const char *homedir = pw->pw_dir;
  // cout<<homedir<<endl;
  // char currdir[FILENAME_MAX]; //create string buffer to hold path
  // getcwd( currdir, FILENAME_MAX );
  // cout<<currdir<<endl;
  // strtok(currdir, "\n");

  char currdir[FILENAME_MAX]; //create string buffer to hold path
  getcwd( currdir, FILENAME_MAX );

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
