/****
  * File containing wrapper functions to use char arrays with path parser
  *
  */

// Dependencies
#include <iostream>

// Local Dependencies
#include "path_parser.cpp"

void parse_path(char path[FILENAME_MAX]){
  // Initialise object to parse path
  PathParser pp;
  // Parse path
  string temp(path);
  temp = pp.parse(temp);
  strcpy(path, temp.c_str());
}

void get_parent_path(char path[FILENAME_MAX]){
  // Initialise object to parse path
  PathParser pp;

  string temp(path);
  temp = pp.parse(temp+"/../");
  strcpy(path, temp.c_str());
}

void get_child_path(char path[FILENAME_MAX], char name[256]){
  // Initialise object to parse path
  PathParser pp;

  string temp(path);
  temp = temp+"/";
  string tempname(name);
  temp = pp.parse(temp+tempname);
  strcpy(path, temp.c_str());
}
