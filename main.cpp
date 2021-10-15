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
#include "command_state/command_state.cpp"

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

  // Initialise struct for handling command mode
  struct command_state c_state;
  // Call function to initialise command state
  reset_command_state(&c_state);

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

  // Start REPL
  while(true){
    // Read input
    char ch = getchar();
    // Check mode
    if(!c_state.command_mode_active){
      // Exit when q is pressed
      if(ch == 'q'){
        break;
      }else if(ch == ':'){
        // Move cursor to bottom
        move_cursor_to_bottom(&window);
        // Switch mode
        turn_on_command_mode(&c_state);
        // Update command buffer
        update_command_push(':', &c_state);
        // Print command buffer
        for(int i=0;i<c_state.counter;++i){
          cout<<c_state.command[i];
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
        turn_off_command_mode(&c_state);
        // Call function to reset command state
        reset_command_state(&c_state);
        // Move cursor to bottom
        move_cursor_to_bottom(&window);
        // cout<<"\033["<<max_row<<";1H";
        for(int i=0;i<window.max_col;++i){
          cout<<" ";
        }
        // Move cursor to original position
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 10){
        // Check for goto command
        if(c_state.command[1] == 'g' && c_state.command[2] == 'o' && c_state.command[3] == 't' && c_state.command[4] == 'o'){
          // Initialise buffer to hold parameter
          char param[COMMAND_MAX_LENGTH];
          // Call function to get first parameter
          get_command_first_argument(param, &c_state);
          char temp_path[FILENAME_MAX];
          // Check if path is absolute
          if(param[0] == '/' || param[0] == '~'){
            strcpy(temp_path, param);
            parse_path(temp_path);
          }else{
            strcpy(temp_path, currdir);
            // Call function to get absolute path
            get_child_path(temp_path, param);
          }
          // Update history
          history.push(temp_path);
          // Call function to get list of files and directories
          list = get_dir_content(temp_path);
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
        // Switch mode
        turn_off_command_mode(&c_state);
        // Call function to reset command state
        reset_command_state(&c_state);
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        for(int i=0;i<window.max_col;++i){
          cout<<" ";
        }
        // Move cursor to original position
        cout<<"\033["<<window.y_coord<<";1H";
      }else if(ch == 0x7f){
        // Call function to remove last character in command
        update_command_pop(&c_state);
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        // Clear command
        for(int i=0;i<window.max_col;++i){
          cout<<" ";
        }
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        // Print command buffer
        for(int i=0;i<c_state.counter;++i){
          cout<<c_state.command[i];
        }
      }else{
        // Move cursor to bottom
        cout<<"\033["<<window.max_row<<";1H";
        // Call function to update command state
        update_command_push(ch, &c_state);
        // Print command buffer
        for(int i=0;i<c_state.counter;++i){
          cout<<c_state.command[i];
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
