/****
  * File containing class to store traversal history
  *
  */

// Dependencies
#include <iostream>
#include <stdio.h>

// Set namespace
using namespace std;

struct crumb {
  char path[FILENAME_MAX];
  struct crumb* next;
  struct crumb* prev;
};

class Breadcrumb {
  struct crumb* start;
  struct crumb* end;
  struct crumb* curr;
  public:
  Breadcrumb(){
    start = NULL;
    end = NULL;
    curr = NULL;
  }
  // Function to add new path to history
  void push(char path[FILENAME_MAX]){
    // Initialise node
    struct crumb* temp = new crumb;
    // Update values
    strcpy(temp->path, path);
    temp->next = NULL;
    temp->prev = NULL;
    // Check if list is empty
    if(start == NULL && end == NULL){
      // Update pointers
      start = temp;
      end = temp;
      curr = end;
    // Check if current node is at the end
    }else if(curr == end){
      end->next = temp;
      temp->prev = end;
      end = temp;
      curr = end;
    }else{
      while(end != curr){
        struct crumb* t = end;
        end = end->prev;
        free(t);
      }
      end->next = temp;
      temp->prev = end;
      end = temp;
      curr = end;
    }
  }

  // Function to move to previous state
  void move_to_previous(){
    if(curr != start){
      curr = curr->prev;
    }
  }

  // Function to move to next state
  void move_to_next(){
    if(curr != end){
      curr = curr->next;
    }
  }

  // Function to get current state
  void current(char path[FILENAME_MAX]){
    strcpy(path, curr->path);
  }
};
