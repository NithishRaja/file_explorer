/****
  * Main file
  *
  */

// Dependencies
#include <dirent.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
// #include <termios.h>
#ifdef WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#define getcwd getcwd
#endif
// Local Dependencies
#include "helpers.cpp"

// Set namespace
using namespace std;

void listdir(){
  struct stat fileInfo;

  DIR *dir;
  struct dirent *ent;

  struct passwd *pw = getpwuid(getuid());

  const char *homedir = pw->pw_dir;
  cout<<homedir<<endl;
  char currdir[FILENAME_MAX]; //create string buffer to hold path
  getcwd( currdir, FILENAME_MAX );
  cout<<currdir<<endl;
  // strtok(currdir, "\n");
  if((dir = opendir(currdir)) != NULL){
    /* print all the files and directories within directory */
    while ((ent = readdir(dir)) != NULL){
      if(stat(ent->d_name, &fileInfo) == 0){
        struct passwd *pw = getpwuid(fileInfo.st_uid);
        struct group  *gr = getgrgid(fileInfo.st_gid);
        char tim[80];
        convert_time_to_human_readable(fileInfo.st_mtime, tim);
        char size[80];
        convert_size_to_human_readable(fileInfo.st_size, size);
        char mode[80];
        convert_permission_to_human_readable(fileInfo.st_mode, mode);
        cout.width(12);
        cout<<left<<mode;
        cout.width(10);
        cout<<left<<pw->pw_name;
        cout.width(10);
        cout<<left<<gr->gr_name;
        cout.width(10);
        cout<<left<<size;
        cout.width(25);
        cout<<left<<tim;
        cout.width(10);
        cout<<left<<ent->d_name<<endl;
        // break;
      }
    }
    closedir(dir);
  }else{
    /* could not open directory */
    cout<<"couldn't open dir\n";
  }
}

int main(){
  listdir();
}
