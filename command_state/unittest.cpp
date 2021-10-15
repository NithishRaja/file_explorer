/****
  * File containing unittest for command state
  *
  */

// Dependencies
#include <iostream>
// Local Dependencies
#include "command_state.cpp"

// Set namespace
using namespace std;

#define print_status cout<<" + "<<__FUNCTION__;if(flag){cout<<" SUCCEEDED\n";}else{cout<<" FAILED\n";}

void test_command_state_push_pop(){
  // Initialise state
  struct command_state state;
  // Call function to initialise command state
  reset_command_state(&state);
  // Initialise input
  char input[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
  // Add entries
  for(int i=0;i<sizeof(input)/sizeof(input[0]);++i){
    update_command_push(input[i], &state);
  }
  // Declare variable to store output
  char output[COMMAND_MAX_LENGTH];
  // Call function to get full command
  get_command(output, &state);
  // Initialise expected output
  char expected_output1[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
  // Initialise flag
  bool flag = true;
  // Check output
  for(int i=0;i<sizeof(expected_output1)/sizeof(expected_output1[0]);++i){
    flag = flag && (expected_output1[i] == output[i]);
  }
  update_command_pop(&state);
  update_command_pop(&state);
  // Call function to get full command
  get_command(output, &state);
  // Initialise expected output
  char expected_output2[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g'};
  // Check output
  for(int i=0;i<sizeof(expected_output2)/sizeof(expected_output2[0]);++i){
    flag = flag && (expected_output2[i] == output[i]);
  }
  update_command_push('l', &state);
  update_command_push('l', &state);
  update_command_push('l', &state);
  // Call function to get full command
  get_command(output, &state);
  // Initialise expected output
  char expected_output3[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'l', 'l', 'l'};
  // Check output
  for(int i=0;i<sizeof(expected_output3)/sizeof(expected_output3[0]);++i){
    flag = flag && (expected_output3[i] == output[i]);
  }
  print_status
}

void test_command_get_first_param(){
  // Initialise state
  struct command_state state;
  // Call function to initialise command state
  reset_command_state(&state);
  // Initialise input
  string inp = ":goto ~/";
  char input[COMMAND_MAX_LENGTH];
  strcpy(input, inp.c_str());
  // Add entries
  for(int i=0;i<sizeof(input)/sizeof(input[0]);++i){
    update_command_push(input[i], &state);
  }
  // Declare variable to store output
  char output[COMMAND_MAX_LENGTH];
  // Call function to get full command
  get_first_argument(output, &state);
  // Initialise expected output
  char expected_output1[] = {'~', '/'};
  // Initialise flag
  bool flag = true;
  // Check output
  for(int i=0;i<sizeof(expected_output1)/sizeof(expected_output1[0]);++i){
    flag = flag && (expected_output1[i] == output[i]);
  }
  // Call function to initialise command state
  reset_command_state(&state);
  // Initialise input
  inp = ":goto ~/home ./../";
  strcpy(input, inp.c_str());
  // Add entries
  for(int i=0;i<sizeof(input)/sizeof(input[0]);++i){
    update_command_push(input[i], &state);
  }
  // Call function to get full command
  get_first_argument(output, &state);
  // Initialise expected output
  char expected_output2[] = {'~', '/', 'h', 'o', 'm', 'e'};
  // Check output
  for(int i=0;i<sizeof(expected_output2)/sizeof(expected_output2[0]);++i){
    flag = flag && (expected_output2[i] == output[i]);
  }
  print_status
}

int main(){
  test_command_state_push_pop();
  test_command_get_first_param();
}
