/****
  * File containing code for path parser class
  *
  */

// Dependencies
#include <iostream>
#include <pwd.h>
#ifdef WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#define getcwd getcwd
#endif

// Set namespace
using namespace std;

class PathParser {
  // Initialise variable to hold backslash
  string backslash = "/";
  // Initialise variabole to hold current path
  char currdir[FILENAME_MAX];
  // Get home path
  const char *homedir;
  public:
  PathParser(){
    // Get current path
    getcwd( currdir, FILENAME_MAX );
    // Get home path
    struct passwd *pw = getpwuid(getuid());
    homedir =  pw->pw_dir;
  }
  // Function to parse given string
  string parse(string path){
    if(path[0] == '.' && path[1] == '/'){
      path = currdir + path.substr(1);
    }else if(path[0] == '~' && path[1] == '/'){
      path = homedir + path.substr(1);
    }else if(path[0] == '/'){
      // Do nothing
      path = path;
    }else{
      path = currdir + backslash + path;
    }
    return path;
  }
};
