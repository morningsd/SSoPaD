#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <ftw.h>

using namespace std;

#define CHANGEFILE "/home/demian/study/ss/lab1/mydir"
#define USER_PERMISSIONS S_IRUSR|S_IXUSR // rx
#define ADMIN_PERMISSIONS S_IRWXU // rwx

void list_dir(const char *dir,int op_a,int op_l);
int rmrf(char *path);

int main()
{
  chmod(CHANGEFILE, 0);
  bool logged = false;
  string login;

  do {
    cout << "Enter your login, please: " << "\n";
    cin >> login;
      
    if (login == "user") {
      chmod(CHANGEFILE, USER_PERMISSIONS);
      logged = true;
      break;
    } else if (login == "admin") {
      chmod(CHANGEFILE, ADMIN_PERMISSIONS);
      logged = true;
      break;
    } else {
      cout << "Unknown login, please, try again." << "\n";
    }
      
  } while (!logged);
  cout << "You are successfully logged in as " << login << "\n";
  
  while (true) {
    char command[255];
    char param[255];
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    
    cout << cwd << " " << login << ">";
    cin >> command;

    if (strcmp(command, "cd") == 0) {
      cin >> param;
      cout << param << "\n";
      if (chdir(param) != 0) {
	cout << "Can't change directory to " << param << "\n";
      }
    } else if (strcmp(command, "rm") == 0) {
      cin >> param;
      if (remove(param) != 0) {
	cout << "Can't remove file " << param << "\n";
      }
    } else if (strcmp(command, "ls") == 0) {
      list_dir(".", 0, 0);
    } else if (strcmp(command, "pwd") == 0) {
      cout << cwd << "\n";
    } else if (strcmp(command, "mkdir") == 0) {
      cin >> param;
      if (login == "user") {
	cout << "Can't write to current directory\n";
      } else {
	if (mkdir(param, 0) != 0) {
	  cout << "Can't make directory" << "\n";
	} else if (login == "admin") {
	  chmod(param, ADMIN_PERMISSIONS);
	}
      }
    } else if (strcmp(command, "vi") == 0) {
      cin >> param;
      string command_str;
      if (login == "admin") {
	command_str = string(command);
      } else {
	command_str = "view"; 
      }
      string param_str = string(param);
      string concat = command_str + " " + param_str;
      char result[concat.size() + 1];
      strcpy(result, concat.c_str());
      system(result);
    } else {
      cout << "Unknown command.\n";
    }
  }

  return 0;
}

void list_dir(const char *dir,int op_a,int op_l) {
  struct dirent *d;
  DIR *dh = opendir(dir);

  if (!dh) {
    if (errno == ENOENT) {
      cout << "Directory doesn't exist\n";
    } else {
      cout << "Unable to read directory\n";
    }
    return;
  }

  while ((d = readdir(dh)) != NULL) {
    if (!op_a && d->d_name[0] == '.')
      continue;
    printf("%s  ", d->d_name);
    if (op_l) printf("\n");
  }
  if (!op_l) printf("\n");
}

