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
#include "path_parser/pp_wrapper.cpp"
#include "window_state/window_state.cpp"
#include "breadcrumb/breadcrumb.cpp"

// Set namespace
using namespace std;

int main(){
  // Initialise buffer to hold home path
  struct passwd *pw = getpwuid(getuid());
  char homedir[FILENAME_MAX];
  strcpy(homedir, pw->pw_dir);
  // Create buffer to hold path
  char currdir[FILENAME_MAX];
  getcwd(currdir, FILENAME_MAX);

  // Initialise breadcrumb
  Breadcrumb history;

  // Call function to parse current path
  parse_path(currdir);
  // Call function to get list
  vector<struct file_info_hr> list = get_dir_content(currdir);
  // Update history
  history.push(currdir);

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

  // Initialise struct for handling window state
  struct window_state window;
  // Call function to initialise window state
  initialise_window_state(&window);

  // Get list size
  int list_size = list.size();
  // Initialise variable to hold window start
  int window_start = 0;

  // Call function to print list of files and directories
  print_list(list, window_start, window.max_row-1);
  // Move cursor back to top left
  cout<<"\033[1;1H";
  // Initialise buffer to hold command
  char command_buffer[200];
  int counter = 0;

  // Start REPL
  while(true){
    // Read input
    char ch = getchar();
    // Check mode
    if(!command_mode){
      if(ch == ':'){
        // Switch mode
        command_mode = true;
        // Move cursor to bottom
        move_cursor_to_bottom(&window);
        // Update command buffer
        command_buffer[counter++] = ':';
        // Print command buffer
        for(int i=0;i<counter;++i){
          cout<<command_buffer[i];
        }
      }else if(ch == 'k'){
        // Update window start
        if(window_start > 0){
          --window_start;
          print_list(list, window_start, window.max_row-1);
          move_cursor_down(&window);
        }
      }else if(ch == 'l'){
        if(window_start + window.max_row-1 < list_size){
          ++window_start;
          print_list(list, window_start, window.max_row-1);
          move_cursor_up(&window);
        }
      }else if(ch == 'h'){
        // Update history
        history.push(homedir);
        // Call function to get list of files and directories
        list = get_dir_content(homedir);
        // Calculate list size
        list_size = list.size();
        // Call function to reset cursor
        reset_cursor(&window);
        // Reset window start
        window_start = 0;
        // Call function to print list
        print_list(list, window_start, window.max_row-1);
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 0x7f){
        // Call function to get path of parent directory
        get_parent_path(currdir);
        // Update history
        history.push(currdir);
        // Call function to get list of files and directories
        list = get_dir_content(currdir);
        // Calculate list size
        list_size = list.size();
        // Call function to reset cursor
        reset_cursor(&window);
        // Reset window start
        window_start = 0;
        // Call function to print list
        print_list(list, window_start, window.max_row-1);
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 10){
        // Get selected index
        int selected_index = window_start + window.y_coord-1;
        // Check if selected path is a directory
        if(list[selected_index].is_dir){
          // Call function to get path of child directory
          get_child_path(currdir, list[selected_index].name);
          // Update history
          history.push(currdir);
          // Call function to get list of files and directories
          list = get_dir_content(currdir);
          // Calculate list size
          list_size = list.size();
          // Call function to reset cursor
          reset_cursor(&window);
          // Reset window start
          window_start = 0;
          // Call function to print list
          print_list(list, window_start, window.max_row-1);
          cout<<"\033["<<window.y_coord<<";1H";
        }else{
          // Declare variable to hold file path
          char file_path[FILENAME_MAX];
          // Copy current path
          strcpy(file_path, currdir);
          // Call function to get path of child directory
          get_child_path(file_path, list[selected_index].name);
          // Call function to open file in editor
          open_in_editor(file_path);
        }
      }else if(ch == 65){
        move_cursor_up(&window);
      }else if(ch == 66){
        // Check if cursor position is within list
        if(window.y_coord < list_size){
          move_cursor_down(&window);
        }
      }else if(ch == 67){
        // Call function to move to next history state
        history.move_to_next();
        // Get history state
        history.current(currdir);
        // Call function to get list of files and directories
        list = get_dir_content(currdir);
        // Calculate list size
        list_size = list.size();
        // Call function to reset cursor
        reset_cursor(&window);
        // Reset window start
        window_start = 0;
        // Call function to print list
        print_list(list, window_start, window.max_row-1);
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 68){
        // Call function to move to previous history state
        history.move_to_previous();
        // Get history state
        history.current(currdir);
        // Call function to get list of files and directories
        list = get_dir_content(currdir);
        // Calculate list size
        list_size = list.size();
        // Call function to reset cursor
        reset_cursor(&window);
        // Reset window start
        window_start = 0;
        // Call function to print list
        print_list(list, window_start, window.max_row-1);
        cout<<"\033["<<window.y_coord<<";1H";
      }
    }else{
      // Check if ESC is pressed
      if(ch == 27){
        // Switch mode
        command_mode = false;
        // Empty command buffer
        counter = 0;
        // Move cursor to bottom
        move_cursor_to_bottom(&window);
        // cout<<"\033["<<max_row<<";1H";
        for(int i=0;i<window.max_col;++i){
          cout<<" ";
        }
        // Move cursor to original position
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 10){
        // Execute command
        // Switch mode
        command_mode = false;
        // Empty command buffer
        counter = 0;
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        for(int i=0;i<window.max_col;++i){
          cout<<" ";
        }
        // Move cursor to original position
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 0x7f){
        // Remove last character in command
        --counter;
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        // Clear command
        for(int i=0;i<window.max_col-1;++i){
          cout<<"";
        }
        // Print command buffer
        for(int i=0;i<counter;++i){
          cout<<command_buffer[i];
        }
      }else{
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        // Update command buffer
        command_buffer[counter++] = ch;
        // Print command buffer
        for(int i=0;i<counter;++i){
          cout<<command_buffer[i];
        }
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
