#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <ftw.h>
#include <fstream>
#include <ctime>

using namespace std;

#define WORKING_DIRECTORY "/home/demian/study/SSoPaD/lab1" // path from which you start the program
#define CHANGEFILE "/home/demian/study/SSoPaD/lab1/mydir"// working directory + name of the directory
//#define USER_PERMISSIONS S_IRUSR|S_IXUSR // rx
#define ADMIN_PERMISSIONS S_IRWXU // rwx
#define PERMISSIONS S_IRWXU //rwx
#define AUTH_FILE "/home/demian/study/SSoPaD/lab1/.auth" // file with metadata
#define JOURNAL_FILE "/home/demian/study/SSoPaD/lab1/mydir/admin/journal.txt" // admin journal
#define SECURITY_MONITOR "/home/demian/study/SSoPaD/lab1/mydir/admin/monitor.txt"


void list_dir(const char *dir,int op_a,int op_l);
int rmrf(char *path);
int create_file(string path, string role);
int read_file(string path, string role);
int check_user(string login);
int check_pin();
int write_to_monitor(string role, string message);

string current_user;
string current_password;
string current_pin;
string current_fail_counter;
int last_check_minute;


int main()
{
  chmod(CHANGEFILE, ADMIN_PERMISSIONS);
  bool logged = false;
  string login;

  do {
    cout << "Enter your login, please: " << "\n";
    cin >> login;
    int res = check_user(login);
    
    if (res == 0) {
      logged = true;
      break;
    } else if (res == -1 || res == -2) {
      return -1;
    } 
    chmod(CHANGEFILE, ADMIN_PERMISSIONS);
      
  } while (!logged);
  cout << "You are successfully logged in as " << login << "\n";
  chmod(CHANGEFILE, PERMISSIONS);
  chdir(CHANGEFILE);

  const std::time_t now = std::time(nullptr);
  const std::tm calendar_time = *std::localtime(std::addressof(now));
  last_check_minute = calendar_time.tm_min;
  
  while (true) {

    //if (check_pin() == -1) {
      //return -1;
      //}
    
    const std::time_t now_2 = std::time(nullptr);
    const std::tm calendar_time_2 = *std::localtime( std::addressof(now_2));
    int check_minute = calendar_time_2.tm_min;
    if (check_minute > last_check_minute + 2) {
      int res = check_pin();
      if (res == -1) {
	return -1;
      }
      last_check_minute = check_minute;
    }
    
    
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
      if (login == "user" && (strcmp(param, "admin") == 0)) {
	write_to_monitor(current_user, "tried to access admin directory");
	cout << "Can't change directory to " << param << "\n";
      } else if (chdir(param) != 0) {
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
      if (mkdir(param, 0777) != 0) {
	cout << "Can't make directory" << "\n";
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
	write_to_monitor(current_user, "tried to call vi on " + param_str + " file");
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

int write_to_monitor(string role, string message) {
  ofstream fs(SECURITY_MONITOR, ios_base::app);

  if (!fs) {
    cout << "Cannot write logs to security monitor file\n";
    return -1;
  }
  const std::time_t now_3 = std::time(nullptr);
  const std::tm cal = *std::localtime( std::addressof(now_3));
  int curr_month = cal.tm_mon;
  curr_month = (curr_month + 1) % 12;
  fs << cal.tm_mday<<"/" << curr_month << " " << cal.tm_hour <<":"<< cal.tm_min <<":"<< cal.tm_sec<< " --- " << role << " " << message << "\n";

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

int check_user(string login) {
  fstream fs(JOURNAL_FILE, ios::in);
  if (!fs.is_open()) {
    return -2;
  }
  string user;
  string password;
  string pin;
  string fail_counter;
  while (fs) {
    fs >> user >> password >> pin >> fail_counter;
    if (login == user) {
      string user_password;
      cout << "Enter password, please: ";
      cin >> user_password;
      if (password == user_password) {
	current_user = user;
	current_password = password;
	current_pin = pin;
	current_fail_counter = fail_counter;
	cout << "Access granted! You are welcome\n";
	return 0;
      } else {
	cout << "Wrong password\n";
	return -1;
      }
    }
    
  }
  cout << "No such user exists: " << login << "\n";
  return -1;
}


int check_pin() {
  while (true) {
    string pin;
    cout << "Enter your pin, please: ";
    cin >> pin;

    if (stoi(current_fail_counter) > 3) {
      cout << "Please contact administrator to continue your work\n";
      return -1;
    }
    
    if (current_pin == pin) {
      cout << "All is good\n";
      return 0;
    }
    cout << "Wrong pin\n";
    
    ostringstream text;
    ifstream in_file(JOURNAL_FILE);

    text << in_file.rdbuf();
    string str = text.str();
    string str_search = current_user + " " + current_password + " " + current_pin + " " + current_fail_counter;
    int ctr = stoi(current_fail_counter);
    ctr++;
    string str_replace = current_user + " " + current_password + " " + current_pin + " " + to_string(ctr);
    size_t pos = str.find(str_search);
    str.replace(pos, string(str_search).length(), str_replace);
    in_file.close();

    ofstream out_file(JOURNAL_FILE);
    out_file << str;    
    
    return -1;
  }
}
