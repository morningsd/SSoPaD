#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <ftw.h>
#include <fstream>

using namespace std;

#define WORKING_DIRECTORY "/home/demian/study/ss/lab1" // path from which you start the program
#define CHANGEFILE "/home/demian/study/ss/lab1/mydir"// working directory + name of the directory
//#define USER_PERMISSIONS S_IRUSR|S_IXUSR // rx
//#define ADMIN_PERMISSIONS S_IRWXU // rwx
#define PERMISSIONS S_IRWXU //rwx
#define AUTH_FILE "/home/demian/study/ss/lab1/.auth" // file with metadata

void list_dir(const char *dir,int op_a,int op_l);
int rmrf(char *path);
int create_file(string path, string role);
int read_file(string path, string role);


int main()
{
  chmod(CHANGEFILE, 0);
  bool logged = false;
  string login;

  do {
    cout << "Enter your login, please: " << "\n";
    cin >> login;
      
    if (login == "user") {
      //chmod(CHANGEFILE, USER_PERMISSIONS);
      logged = true;
      break;
    } else if (login == "admin") {
      //chmod(CHANGEFILE, ADMIN_PERMISSIONS);
      logged = true;
      break;
    } else {
      cout << "Unknown login, please, try again." << "\n";
    }
      
  } while (!logged);
  cout << "You are successfully logged in as " << login << "\n";
  chmod(CHANGEFILE, PERMISSIONS);
  chdir(CHANGEFILE);
  
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
      char new_cwd[1024];
      if (chdir(param) != 0) {
	cout << "Can't change directory to " << param << "\n";
      }
      getcwd(new_cwd, sizeof(new_cwd));
      string new_cwd_str(new_cwd);
      if (!(new_cwd_str.rfind(CHANGEFILE, 0) == 0)) {
	cout << "Can't exit current working directory\n";
	chdir(cwd);
	continue;
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
	  //chmod(param, ADMIN_PERMISSIONS);
	}
      }
    } else if (strcmp(command, "vi") == 0) {
      cin >> param;
      string command_str;
      command_str = string(command);
     
      string param_str = string(param);
      string concat = command_str + " " + param_str;
      char result[concat.size() + 1];
      strcpy(result, concat.c_str());
      if (create_file(string(cwd) + "/" + param_str, login) == 0) {
	system(result);
      } else {
	cout << "Can't call vi " << param_str << "\n";
      }
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

int create_file(string path, string role) {
  int res = read_file(path, role);
  if (res == 1) {
    cout << "Success\n";
    return 0;
  } else if (res == -1) {
    cout << "User " << role << " have no access to file: " << path << "\n";
    return -1;
  }
  
  ofstream fs(AUTH_FILE, ios_base::app);

  if (!fs) {
    cout << "Cannot open the output file.\n";
    return -1;
  }
  
  fs << role << " " << path << "\n";
  
  fs.close();

  return 0;
}

int read_file(string path, string role) {
  cout << "path: " << path + " role: " << role << "\n";
  fstream fs(AUTH_FILE, ios::in);
  if (!fs.is_open()) {
    return -2;
  }
  string check_file = role + " " + path;
  string auth_role;
  string auth_path;
  while(fs) {
    fs >> auth_role >> auth_path;
    //cout << "auth_role: " << auth_role << "\n";
    //cout << "auth_path: " << auth_path << "\n";
    if (auth_path == path) {
      if ((role == "admin") || (auth_role == role)) {
	return 1;
      } else {
	return -1;
      }
    }
  }
  return 0;
}
