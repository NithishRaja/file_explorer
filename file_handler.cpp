/****
  * File containing code to get info of files and directories in given directory
  *
  */

// Dependencies
#include <vector>
#include <queue>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
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
  bool is_dir;
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
      // Get full path
      string temp(currdir);
      temp = temp+"/";
      string tempname(ent->d_name);
      temp = temp+tempname;
      char fullpath[4096];
      strcpy(fullpath, temp.c_str());
      // Check if user has permissions
      if((stat(fullpath, &fileInfo) == 0) && ((fileInfo.st_mode & S_IRUSR) != 0)){
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
        // Update file or directory indicator
        if(fileInfo.st_mode & S_IFDIR){
          newfile.is_dir = true;
        }else{
          newfile.is_dir = false;
        }
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
      // if(stat(ent->d_name, &fileInfo) == 0){
      //
      // }
    }
    closedir(dir);
  }else{
    /* could not open directory */
    cout<<"couldn't open dir\n";
  }
  return res;
}

// Function to search for given file or directory
bool search_entry(char currdir[FILENAME_MAX], char name[4096]){
  string cur_s = ".";
  char cur[FILENAME_MAX];
  strcpy(cur, cur_s.c_str());
  string bac_s = "..";
  char bac[FILENAME_MAX];
  strcpy(bac, bac_s.c_str());
  // Initialise flag
  bool flag = false;
  // Call function to get entries in current path
  vector<struct file_info_hr> list = get_dir_content(currdir);
  // Initialise queue to hold subdirectories
  queue<string> q;
  // Check list for required entry
  for(int i=0;i<list.size();++i){
    // // Skip entry '.'
    // if(strcmp(curr, list[i].name) == 0){
    //   continue;
    // }
    // // Skip entry '..'
    // if(strcmp(back, list[i].name) == 0){
    //   continue;
    // }
    //  && strcmp(curr, list[i].name) != 0 && strcmp(back, list[i].name) != 0
    // Check if entry is a directory
    // cout<<"---"<<list[i].name<<"----"<<name<<"----";
    if(list[i].is_dir && strcmp(cur, list[i].name) != 0 && strcmp(bac, list[i].name) != 0){
      // Get full path
      string temp(currdir);
      temp = temp+"/";
      string tempname(list[i].name);
      temp = temp+tempname;
      // Add directory to queue
      q.push(temp);
    }
    // if(strcmp(curr, list[i].name) == 0 || strcmp(back, list[i].name) == 0){
    //   continue;
    // }
    // cout<<":::"<<list[i].name<<":::"<<name<<";;;;;";
    // Exit loop if match is found
    if(strcmp(name, list[i].name) == 0){
      flag = true;
      break;
    }
  }
  // Iterate over all subdirectories
  while(!q.empty() && !flag){
    // Get full path
    string tt = q.front();
    q.pop();
    char fullpath[4096];
    strcpy(fullpath, tt.c_str());
    // Call function to get entries in current path
    list = get_dir_content(fullpath);
    // Check list for required entry
    for(int i=0;i<list.size();++i){
      // // Skip entry '.'
      // if(strcmp(curr, list[i].name) == 0){
      //   continue;
      // }
      // // Skip entry '..'
      // if(strcmp(back, list[i].name) == 0){
      //   continue;
      // }
      // Check if entry is a directory
      if(list[i].is_dir && strcmp(cur, list[i].name) != 0 && strcmp(bac, list[i].name) != 0){
        // Get full path
        string temp(fullpath);
        temp = temp+"/";
        string tempname(list[i].name);
        temp = temp+tempname;
        // Add directory to queue
        q.push(temp);
      }
      // Exit loop if match is found
      if(strcmp(name, list[i].name) == 0){
        flag = true;
        break;
      }
    }
  }
  return flag;
}

// Function to print files and directories in given window
void print_list(vector<struct file_info_hr> list, int start, int window_size){
  // Clear screen and place cursor at top left
  cout<<"\033[2J\033[1;1H";
  // Iterate over list
  for(int i=start;i<list.size() && i-start<window_size;++i){
    cout.width(12);
    cout<<left<<list[i].permission;
    cout.width(10);
    cout<<left<<list[i].user;
    cout.width(10);
    cout<<left<<list[i].group;
    cout.width(10);
    cout<<left<<list[i].size;
    cout.width(25);
    cout<<left<<list[i].last_modified;
    cout.width(10);
    cout<<left<<list[i].name;
    cout<<endl;
  }
}
