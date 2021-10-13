/****
  * File containing unittest for path parser class
  *
  */

// Dependencies
#include <iostream>
#include <string>
#include <pwd.h>
#ifdef WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#define getcwd getcwd
#endif
// Local Dependencies
#include "path_parser.cpp"

// Set namespace
using namespace std;

#define print_status cout<<" + "<<__FUNCTION__;if(flag){cout<<" SUCCEEDED\n";}else{cout<<" FAILED\n";}

void test_absolute_path(){
  // Initialise variable to hold input
  string input = "/nithihs";
  // Initialise variable with expected output
  string expected_output = input;
  // Initialise object
  PathParser obj;
  // Call function to parse path
  string output = obj.parse(input);
  // Initialise flag
  bool flag = true;
  // Check output
  flag = flag && (expected_output == output);
  // Initialise variable to hold input
  input = "~/nithihs";
  // Initialise variable with expected output
  // Get home path
  struct passwd *pw = getpwuid(getuid());
  const char* homedir =  pw->pw_dir;
  expected_output = homedir + input.substr(1);
  // Call function to parse path
  output = obj.parse(input);
  // Check output
  flag = flag && (expected_output == output);
  print_status
}

void test_relative_path(){
  // Initialise variable to hold backslash
  string backslash = "/";
  // Initialise variabole to hold current path
  char currdir[FILENAME_MAX];
  // Get current path
  getcwd( currdir, FILENAME_MAX );
  // Initialise variable to hold input
  string input = "./nithihs";
  // Generate expected_output
  string expected_output = currdir + input.substr(1);
  // Initialise object
  PathParser obj;
  // Call function to parse path
  string output = obj.parse(input);
  // Initialise flag
  bool flag = true;
  // Check output
  flag = flag && (expected_output == output);
  // Initialise variable to hold input
  input = "nithihs/file";
  // Generate expected_output
  expected_output = currdir + backslash + input;
  // Call function to parse path
  output = obj.parse(input);
  // Check output
  flag = flag && (expected_output == output);
  print_status
}

void test_path_with_go_back(){
  // Initialise variable to hold backslash
  string backslash = "/";
  // Initialise variabole to hold current path
  char currdir[FILENAME_MAX];
  // Get current path
  getcwd( currdir, FILENAME_MAX );
  // Initialise variable to hold input
  string input = "./nithihs/file/../file2";
  // Generate expected_output
  string temp = "/nithihs/file2";
  string expected_output = currdir + temp;
  // Initialise object
  PathParser obj;
  // Call function to parse path
  string output = obj.parse(input);
  // Initialise flag
  bool flag = true;
  // Check output
  flag = flag && (expected_output == output);
  // Initialise variable to hold input
  input = "nithihs/file1/file2/../";
  // Generate expected_output
  temp = "nithihs/file1";
  expected_output = currdir + backslash + temp;
  // Call function to parse path
  output = obj.parse(input);
  // Check output
  flag = flag && (expected_output == output);
  print_status
}

int main(){
  test_absolute_path();
  test_relative_path();
  test_path_with_go_back();
}
