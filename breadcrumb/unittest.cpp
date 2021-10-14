/****
  * File containing unittest for breadcrumb class
  *
  */

// Dependencies
#include <iostream>
#include <stdio.h>
#include <string.h>
// Local Dependencies
#include "breadcrumb.cpp"

// Set namespace
using namespace std;

#define print_status cout<<" + "<<__FUNCTION__;if(flag){cout<<" SUCCEEDED\n";}else{cout<<" FAILED\n";}

void test_traversal(){
  // Initialise object
  Breadcrumb obj;
  // Initialise char array to hold input
  char inp[FILENAME_MAX];
  strcpy(inp, "1");
  obj.push(inp);
  strcpy(inp, "2");
  obj.push(inp);
  strcpy(inp, "3");
  obj.push(inp);
  // Initialise buffer to hold output
  char out[FILENAME_MAX];
  obj.current(out);
  // Initialise flag
  bool flag = true;
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "2");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "1");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_next();
  obj.current(out);
  strcpy(inp, "2");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_next();
  obj.current(out);
  strcpy(inp, "3");
  flag = flag && (strcmp(out, inp) == 0);
  print_status
}

void test_intermediate_insert(){
  // Initialise object
  Breadcrumb obj;
  // Initialise char array to hold input
  char inp[FILENAME_MAX];
  // Initialise buffer to hold output
  char out[FILENAME_MAX];
  strcpy(inp, "1");
  obj.push(inp);
  strcpy(inp, "2");
  obj.push(inp);
  strcpy(inp, "3");
  obj.push(inp);
  strcpy(inp, "4");
  obj.push(inp);
  strcpy(inp, "5");
  obj.push(inp);
  obj.move_to_previous();
  obj.move_to_previous();
  strcpy(inp, "6");
  obj.push(inp);
  strcpy(inp, "7");
  obj.push(inp);
  // Initialise flag
  bool flag = true;
  obj.current(out);
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "6");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "3");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "2");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  strcpy(inp, "1");
  flag = flag && (strcmp(out, inp) == 0);
  obj.move_to_previous();
  obj.current(out);
  print_status
}

int main(){
  test_traversal();
  test_intermediate_insert();
  // Return successfully
  return 0;
}
