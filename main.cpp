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
#include "file_handler/file_handler.cpp"
#include "path_parser/pp_wrapper.cpp"
#include "window_state/window_state.cpp"
#include "breadcrumb/breadcrumb.cpp"
#include "command_state/command_state.cpp"

// Set namespace
using namespace std;

class WindowHandler {
  // Create buffer to hold path
  char homedir[FILENAME_MAX];
  char currdir[FILENAME_MAX];
  // Initialise breadcrumb
  Breadcrumb history;
  // Initialise vector to hold list of entries
  vector<struct file_info_hr> list;
  // Initialise struct for handling command mode
  struct command_state c_state;
  // Initialise struct for handling window state
  struct window_state window;
  // Initialise variable to hold list size
  int list_size;
  // Initialise variable to hold window start
  int window_start;
  // Initialise variable for serial port
  int serial_port;
  // Initialise struct for handling terminal io
  struct termios tty_backup, tty_current;
  public:
  WindowHandler(){
    // Initialise buffer to hold home path
    struct passwd *pw = getpwuid(getuid());
    strcpy(homedir, pw->pw_dir);
    getcwd(currdir, FILENAME_MAX);
    // Call function to parse current path
    parse_path(currdir);
    // Call function to get list
    list = get_dir_content(currdir);
    // Update history
    history.push(currdir);

    // Call function to initialise terminal window
    initialise_terminal_window();

    // Call function to initialise command state
    reset_command_state(&c_state);
    // Call function to initialise window state
    initialise_window_state(&window);
    // Get list size
    list_size = list.size();
    // Set window start to default value
    window_start = 0;

    // Call function to print list of files and directories
    print_list(list, window_start, window.max_row-1);
    // Move cursor back to top left
    cout<<"\033[1;1H";
  }

  ~WindowHandler(){
    reset_terminal_window();
  }

  void initialise_terminal_window(){
    // Initialise port
    serial_port = open("/dev/tty", O_RDWR);
    // Check for errors
    if (serial_port < 0) {
      printf("Error %i from open: %s\n", errno, strerror(errno));
    }
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
  }

  void reset_terminal_window(){
    // Reset to old config
    if (tcsetattr(serial_port, TCSANOW, &tty_backup) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
    // Close port
    close(serial_port);
  }

  void clear_command_line(){
    // Move cursor to bottom
    move_cursor_to_bottom(&window);
    // Clear command
    for(int i=0;i<window.max_col;++i){
      cout<<" ";
    }
  }

  void print_command_buffer(){
    // Move cursor to bottom
    move_cursor_to_bottom(&window);
    for(int i=0;i<c_state.counter;++i){
      cout<<c_state.command[i];
    }
  }

  void populate_screen(char dir[FILENAME_MAX]){
    // Call function to get list of files and directories
    list = get_dir_content(dir);
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

  void move_to_path(char dir[FILENAME_MAX]){
    // Update history
    history.push(dir);
    // Call function to populate screen
    populate_screen(dir);
    // Update currdir
    strcpy(currdir, dir);
  }

  void startREPL(){
    // Start REPL
    while(true){
      // Read input
      char ch = getchar();
      // Check mode
      if(!c_state.command_mode_active){
        clear_command_line();
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
          print_command_buffer();
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
          // Call function to move to given path
          move_to_path(homedir);
        }else if(ch == 0x7f){
          // Call function to get path of parent directory
          get_parent_path(currdir);
          move_to_path(currdir);
        }else if(ch == 10){
          // Get selected index
          int selected_index = window_start + window.y_coord-1;
          // Check if selected path is a directory
          if(list[selected_index].is_dir){
            // Call function to get path of child directory
            get_child_path(currdir, list[selected_index].name);
            move_to_path(currdir);
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
          populate_screen(currdir);
        }else if(ch == 68){
          // Call function to move to previous history state
          history.move_to_previous();
          // Get history state
          history.current(currdir);
          populate_screen(currdir);
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
          clear_command_line();
          // Move cursor to original position
          cout<<"\033["<<window.y_coord<<";1H";
        }else if(ch == 10){
          // Check for goto command
          if(c_state.command[1] == 'g'
          && c_state.command[2] == 'o'
          && c_state.command[3] == 't'
          && c_state.command[4] == 'o'){
            // Initialise buffer to hold parameter
            char param[COMMAND_MAX_LENGTH];
            param[0] = 0;
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
            move_to_path(temp_path);
            // Move cursor to bottom
            clear_command_line();
            // Move cursor to original position
            cout<<"\033["<<window.y_coord<<";1H";
          }else if(c_state.command[1] == 's'
          && c_state.command[2] == 'e'
          && c_state.command[3] == 'a'
          && c_state.command[4] == 'r'
          && c_state.command[5] == 'c'
          && c_state.command[6] == 'h'){
            // Initialise buffer to hold parameter
            char param[COMMAND_MAX_LENGTH];
            param[0] = 0;
            // Call function to get first parameter
            get_command_first_argument(param, &c_state);
            // Call function to search
            bool found = search_entry(currdir, param);
            // Clear command
            clear_command_line();
            // Print status
            cout<<"\033["<<window.max_row<<";1H";
            cout<<found;
            cout<<"\033["<<window.y_coord<<";1H";
          }else if(c_state.command[1] == 'c'
          && c_state.command[2] == 'r'
          && c_state.command[3] == 'e'
          && c_state.command[4] == 'a'
          && c_state.command[5] == 't'
          && c_state.command[6] == 'e'
          && c_state.command[7] == '_'
          && c_state.command[8] == 'f'
          && c_state.command[9] == 'i'
          && c_state.command[10] == 'l'
          && c_state.command[11] == 'e'){
            // Initialise buffer to hold parameter
            char param[COMMAND_MAX_LENGTH];
            param[0] = 0;
            // Call function to get first parameter
            get_command_first_argument(param, &c_state);
            // Call function to create file
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
            // Call function to create file
            create_file(temp_path);
            // Clear command
            clear_command_line();
            populate_screen(currdir);
          }else if(c_state.command[1] == 'c'
          && c_state.command[2] == 'r'
          && c_state.command[3] == 'e'
          && c_state.command[4] == 'a'
          && c_state.command[5] == 't'
          && c_state.command[6] == 'e'
          && c_state.command[7] == '_'
          && c_state.command[8] == 'd'
          && c_state.command[9] == 'i'
          && c_state.command[10] == 'r'){
            // Initialise buffer to hold parameter
            char param[COMMAND_MAX_LENGTH];
            param[0] = 0;
            // Call function to get first parameter
            get_command_first_argument(param, &c_state);
            // Call function to create file
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
            // Call function to create directory
            create_directory(temp_path);
            // Clear command
            clear_command_line();
            populate_screen(currdir);
          }else{
            clear_command_line();
            cout<<"\033["<<window.y_coord<<";1H";
          }
          // Switch mode
          turn_off_command_mode(&c_state);
          // Call function to reset command state
          reset_command_state(&c_state);
        }else if(ch == 0x7f){
          // Call function to remove last character in command
          update_command_pop(&c_state);
          clear_command_line();
          print_command_buffer();
          // Move cursor to original position
          // cout<<"\033["<<window.y_coord<<";1H";
        }else{
          // Call function to update command state
          update_command_push(ch, &c_state);
          print_command_buffer();
        }
      }
    }
  }
};

int main(){
  WindowHandler win;
  win.startREPL();
  return 0;
}
