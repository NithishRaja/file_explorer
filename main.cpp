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

void print_list(vector<struct file_info_hr> list){
  // Clear screen and place cursor at top left
  cout<<"\033[2J\033[1;1H";
  // Iterate over list
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

int main(){
  // Initialise object to parse path
  PathParser pp;
  // Initialise buffer to hold home path
  struct passwd *pw = getpwuid(getuid());
  char homedir[FILENAME_MAX];
  strcpy(homedir, pw->pw_dir);

  // Create buffer to hold path
  char currdir[FILENAME_MAX];
  getcwd(currdir, FILENAME_MAX);
  // Parse path
  string temp(currdir);
  temp = pp.parse(temp);
  strcpy(currdir, temp.c_str());
  // Call function to get list
  vector<struct file_info_hr> list = get_dir_content(currdir);

  // Initialise port
  int serial_port = open("/dev/tty", O_RDWR);
  // Check for errors
  if (serial_port < 0) {
    printf("Error %i from open: %s\n", errno, strerror(errno));
  }
  // Initialise struct for handling terminal io
  struct termios tty_backup, tty_current;
  // Get current terminal config
  if(tcgetattr(serial_port, &tty_backup) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  }
  tty_current = tty_backup;
  // Update terminal config
  tty_current.c_lflag &= ~ICANON;
  tty_current.c_lflag &= ~ECHO;
  tty_current.c_lflag &= ~ECHOE;
  tty_current.c_lflag &= ~ECHONL;
  // Update terminal with new config
  if (tcsetattr(serial_port, TCSANOW, &tty_current) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }

  // Initialise flag to track current mode
  bool command_mode = false;

  // Get window dimensions
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int max_row = w.ws_row;
  int max_col = w.ws_col;
  // Get list size
  int list_size = list.size();

  // Call function to print list of files and directories
  print_list(list);
  // Move cursor back to top left
  cout<<"\033[1;1H";
  // Initialise variable to hold y coordinate of cursor
  int y_coord = 1;

  // Start REPL
  while(true){
    // Read input
    char ch = getchar();
    if(ch == ':'){
      cout<<"\033["<<max_row<<";1H";
      cout<<":";
    }else if(ch == 'k'){
      cout<<'k';
    }else if(ch == 'l'){
      cout<<'l';
    }else if(ch == 0x7f){
      cout<<"back";
    }else if(ch == 10){
      cout<<"selected: "<<list[y_coord-1].name;
    }else if(ch == 65){
      // cout<<"up";
      if(y_coord>0){
        --y_coord;
        cout<<"\033[A";
      }
    }else if(ch == 66){
      // cout<<"down";
      if(y_coord<max_row-1 && y_coord<list_size){
        ++y_coord;
        cout<<"\033[B";
      }
    }
  }

  // Reset to old config
  if (tcsetattr(serial_port, TCSANOW, &tty_backup) != 0) {
    printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  }
  // Close port
  close(serial_port);
  return 0;
}
