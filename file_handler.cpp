/****
  * File containing code to get info of files and directories in given directory
  *
  */

// Dependencies
#include <vector>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#ifdef WINDOWS
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#define getcwd getcwd
#endif
// Local Dependencies
#include "helpers.cpp"

// Define struct for holding file info in human readable format
struct file_info_hr {
  char permission[80];
  char user[80];
  char group[80];
  char size[80];
  char last_modified[80];
  char name[256];
};

// Function to get vector containing info on all files and directories in given directory
vector<struct file_info_hr> get_dir_content(char currdir[FILENAME_MAX]){
  // initialize vector
  vector<struct file_info_hr> res;
  struct stat fileInfo;
  // Initialize struct
  DIR *dir;
  struct dirent *ent;
  struct passwd *pw = getpwuid(getuid());

  // Check if directory can be opened
  if((dir = opendir(currdir)) != NULL){
    // Iterate over all files and directories in given directory
    while ((ent = readdir(dir)) != NULL){
      if(stat(ent->d_name, &fileInfo) == 0){
        // Get last modified time of file
        char tim[80];
        convert_time_to_human_readable(fileInfo.st_mtime, tim);
        // Get size of file
        char size[80];
        convert_size_to_human_readable(fileInfo.st_size, size);
        // Get permissions of file
        char mode[80];
        convert_permission_to_human_readable(fileInfo.st_mode, mode);
        // Get file user
        struct passwd *pw = getpwuid(fileInfo.st_uid);
        // Get file group
        struct group  *gr = getgrgid(fileInfo.st_gid);
        // Initialize struct to hold file info
        struct file_info_hr newfile;
        // Add file info to struct
        strcpy(newfile.name, ent->d_name);
        // newfile.name = ent->d_name;
        strcpy(newfile.user, pw->pw_name);
        // newfile.user = pw->pw_name;
        strcpy(newfile.group, gr->gr_name);
        // newfile.group = gr->gr_name;
        strcpy(newfile.size, size);
        // newfile.size = size;
        strcpy(newfile.last_modified, tim);
        // newfile.last_modified = tim;
        strcpy(newfile.permission, mode);
        // newfile.permission = mode;
        // Push struct into vector
        res.push_back(newfile);
      }
    }
    closedir(dir);
  }else{
    /* could not open directory */
    cout<<"couldn't open dir\n";
  }
  return res;
}
