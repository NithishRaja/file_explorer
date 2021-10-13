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

struct Word {
  string text;
  struct Word* next;
  struct Word* prev;
};

class PathParser {
  // Initialise variable to hold backslash
  string backslash = "/";
  // Initialise variabole to hold current path
  char currdir[FILENAME_MAX];
  // Get home path
  const char *homedir;
  // Initialise empty start and end pointers
  struct Word* start;
  struct Word* end;
  public:
  PathParser(){
    // Get current path
    getcwd( currdir, FILENAME_MAX );
    // Get home path
    struct passwd *pw = getpwuid(getuid());
    homedir =  pw->pw_dir;
    // Set pointers to NULL
    start = NULL;
    end = NULL;
  }

  // Function to parse given string
  string parse(string path){
    // Call function to clear list
    clear();
    // Check if path is relative or absolute
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
    // Call function to shorten path
    return shorten(path);
  }

  // Function to shorten path
  string shorten(string path){
    // Get path length
    int len = path.length();
    // Initialise variable to hold current word
    string current_word = "";
    // Traverse path
    for(int i=1;i<len;++i){
      if(path[i] == '/'){
        // Call function to insert current word into list
        push(current_word);
        // Reset current word
        current_word = "";
      }else if(i <= len-3 && path[i] == '.' && path[i+1] == '.' && path[i+2] == '/'){
        // Call function to remove word at end of list
        pop();
        // Update iterator
        i = i+2;
      }else if(i <= len-2 && path[i] == '.' && path[i+1] == '/'){
        // Update iterator
        i = i+1;
      }else{
        // Update current word
        current_word = current_word + path[i];
      }
    }
    // Insert current word if it is not empty
    if(current_word != ""){
      // Call function to insert current word into list
      push(current_word);
      // Reset current word
      current_word = "";
    }
    // Call function to traverse list and assemble path
    return get_path();
  }

  // Function to insert word into list
  void push(string word){
    // Initialise new node
    struct Word* node = new Word;
    // Set default values
    node->text = word;
    node->next = NULL;
    node->prev = NULL;
    // Check if list is empty
    if(start == NULL){
      // Point start and end pointers to new node
      start = node;
      end = node;
    }else{
      // Add node to end of list
      end->next = node;
      node->prev = end;
      end = end->next;
    }
  }

  // Function to remove last word from list
  void pop(){
    // Initialise pointer to end of list
    struct Word* temp = end;
    // Check if list size is 1
    if(start == end){
      // Set start and end pointers to NULL
      start = NULL;
      end = NULL;
    }else{
      // Move back end pointer
      end->prev->next = end->next;
      end = end->prev;
    }
    // temp->next = NULL;
    // temp->prev = NULL;
    // // Free memory
    // free(temp);
  }

  // Function to get path from list
  string get_path(){
    // Initialise pointer to start of list
    struct Word* temp = start;
    // Initialise path string
    string path = "";
    // Traverse list
    while(temp != NULL){
      // Update path string
      path = path + backslash + temp->text;
      // if(temp->next != NULL){
      //   path = path + backslash;
      // }
      // Move to next element in list
      temp = temp->next;
    }
    // Return path
    return path;
  }

  // Function to clear list
  void clear(){
    // Traverse list
    while(end != NULL){
      // Initialise pointer to end of list
      struct Word* temp = end;
      // Move to next element in list
      end = end->prev;
      free(temp);
    }
    start = NULL;
    end = NULL;
  }

  // Function to print contents of list
  void print_list(){
    struct Word* temp = start;
    // Traverse list
    while(temp != NULL){
      cout<<temp->text<<endl;
      // Move to next element in list
      temp = temp->next;
    }
  }
};
