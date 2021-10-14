/****
  * File containing functions to manage window state
  *
  */

// Dependencies
#include <iostream>
#include <unistd.h>
#include <sys/ioctl.h>

// Set namespace
using namespace std;

// Struct to hold state of window
// This struct should not be modified directly
// Modification of this struct should occur only through functions provided below
struct window_state {
  int max_row;
  int max_col;
  int y_coord;
};

// Function to update state with current window dimensions
void update_window_dimensions(struct window_state *state){
  // Get current window dimensions
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  // Update state
  state->max_row = w.ws_row;
  state->max_col = w.ws_col;
}

// Function to reset cursor to top left
void reset_cursor(struct window_state *state){
  state->y_coord = 1;
}

// Function to Initialise window state with default values
void initialise_window_state(struct window_state *state){
  // Call function to update state with window dimensions
  update_window_dimensions(state);
  // Call function to reset cursor
  reset_cursor(state);
}

void decrement_cursor(struct window_state *state){
  if(state->y_coord>1){
    state->y_coord = state->y_coord-1;
  }
}

// Function to move cursor up by a single row
void move_cursor_up(struct window_state *state){
  decrement_cursor(state);
  cout<<"\033["<<state->y_coord<<";1H";
}

void increment_cursor(struct window_state *state){
  if(state->y_coord<state->max_row-1){
    state->y_coord = state->y_coord+1;
  }
}

// Function to move cursor down by a single row
void move_cursor_down(struct window_state *state){
  increment_cursor(state);
  cout<<"\033["<<state->y_coord<<";1H";
}

// Function to move cursor to bottom
void move_cursor_to_bottom(struct window_state *state){
  cout<<"\033["<<state->max_row<<";1H";
}
