/****
  * File containing unittest for window state functionalities
  *
  */

// Dependencies
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>
// Local Dependencies
#include "window_state.cpp"

// Set namespace
using namespace std;

#define print_status cout<<" + "<<__FUNCTION__;if(flag){cout<<" SUCCEEDED\n";}else{cout<<" FAILED\n";}

void test_initialisation(){
  // Calculate expected values
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  int y_coord = 1;
  // Declare window state
  struct window_state window;
  // Call function to initialise window state
  initialise_window_state(&window);
  // Initialise flag
  bool flag = true;
  // Compare expected output and actual output
  flag = flag && (w.ws_row == window.max_row);
  flag = flag && (w.ws_col == window.max_col);
  flag = flag && (y_coord == window.y_coord);
  print_status
}

void test_cursor_movement(){
  // Declare window state
  struct window_state window;
  // Call function to initialise window state
  initialise_window_state(&window);
  // Copy y_coord
  int y_coord = window.y_coord;
  // Initialise flag
  bool flag = true;
  // Call function to increment cursor
  increment_cursor(&window);
  flag = flag && (++y_coord == window.y_coord);
  // Call function to decrement cursor
  decrement_cursor(&window);
  flag = flag && (--y_coord == window.y_coord);
  print_status
}

int main(){
  test_initialisation();
  test_cursor_movement();
  // Return successfully
  return 0;
}
