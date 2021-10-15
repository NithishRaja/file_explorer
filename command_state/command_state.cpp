/****
  * File containing functionality to track command state
  *
  */

// Dependencies
#include <iostream>
#include <vector>
#include <string.h>

// Set namespace
using namespace std;

#define COMMAND_MAX_LENGTH 4096

// Struct to hold state of command
// This struct should not be modified directly
// Modification of this struct should occur only through functions provided below
struct command_state {
  bool command_mode_active;
  char command[COMMAND_MAX_LENGTH];
  int counter;
};

// Function to initialise command state with default values
void reset_command_state(struct command_state* state){
  state->counter = 0;
  state->command_mode_active = false;
}

// Function to set command mode active flag to false
void turn_off_command_mode(struct command_state* state){
  state->command_mode_active = false;
}

// Function to set command mode active flag to true
void turn_on_command_mode(struct command_state* state){
  state->command_mode_active = true;
}

// Function to add character to end of command
// Function assumes that command will never overflow
void update_command_push(char new_ch, struct command_state* state){
  // Update command
  state->command[state->counter] = new_ch;
  // Increment counter
  state->counter = state->counter+1;
}

// Function to remove last element from command
void update_command_pop(struct command_state* state){
  if(state->counter > 1){
    // Increment counter
    state->counter = state->counter-1;
  }
}

// Function to update buffer with current command
void get_command(char output_buffer[COMMAND_MAX_LENGTH], struct command_state* state){
  strcpy(output_buffer, state->command);
}

// Function to get first argument in command
void get_command_first_argument(char output_buffer[COMMAND_MAX_LENGTH], struct command_state* state){
  output_buffer[0] = 0;
  // Traverse command till first blank space is encountered
  int i=0;
  while(state->command[i] != ' ' && i<state->counter){
    ++i;
  }
  // Traverse till second blank space is encountered and store it in output buffer
  ++i;
  int j=0;
  while(state->command[i] != ' ' && i<state->counter){
    output_buffer[j] = state->command[i];
    ++j;
    ++i;
  }
  output_buffer[j] = 0;
}

// Function to get second argument in command
void get_command_second_argument(char output_buffer[COMMAND_MAX_LENGTH], struct command_state* state){
  output_buffer[0] = 0;
  // Traverse command till first blank space is encountered
  int i=0;
  while(state->command[i] != ' ' && i<state->counter){
    ++i;
  }
  // Traverse command till second blank space is encountered
  ++i;
  while(state->command[i] != ' ' && i<state->counter){
    ++i;
  }
  // Traverse till third blank space is encountered and store it in output buffer
  ++i;
  int j=0;
  while(state->command[i] != ' ' && i<state->counter){
    output_buffer[j] = state->command[i];
    ++j;
    ++i;
  }
  output_buffer[j] = 0;
}

vector<string> get_command_all_argument(struct command_state* state){
  // Initialise vector
  vector<string> res;
  // Traverse command till first blank space is encountered
  int i=0;
  while(state->command[i] != ' ' && i<state->counter){
    ++i;
  }
  while(i<state->counter){
    char output_buffer[COMMAND_MAX_LENGTH];
    output_buffer[0] = 0;
    // Traverse command till second blank space is encountered
    ++i;
    int j=0;
    while(state->command[i] != ' ' && i<state->counter){
      output_buffer[j] = state->command[i];
      ++j;
      ++i;
    }
    output_buffer[j] = 0;
    string temp(output_buffer);
    res.push_back(temp);
  }
  return res;
}
