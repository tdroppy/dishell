#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <errno.h>

#include "builtins.h"

#define MAX_BUF_SIZE 256

void dish_event_loop(char* usrprmpt);
int dish_exit(char** args);
void dish_exec(char** args);
char *dish_get_cwd(char** args);
char *dish_chng_cwd(char** args);
int dish_hi(char** args);
int dish_open(char** args);
void sgchld_handling(int signum);

char dish_cwd[1024];

int main() {
  struct sigaction action;
  sigemptyset(&action.sa_mask);
  action.sa_handler = SIG_IGN;
  sigaction(SIGINT, &action, NULL); // ignore ctrlc
  sigaction(SIGTSTP, &action, NULL); // ignore ctrlz
  sigaction(SIGQUIT, &action, NULL);
  sigaction(SIGTTIN, &action, NULL);
  sigaction(SIGTTOU, &action, NULL);

  action.sa_handler = sgchld_handling;
  action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
  sigaction(SIGCHLD, &action, NULL);

  struct passwd *pw = getpwuid(getuid());
  if (pw == NULL) {
    printf("Failed to retrieve userid");
  }

  char* usrprmpt = malloc(strlen(pw->pw_name) + 6);
  char* prmptend = " O~> \0";

  usrprmpt = pw->pw_name;
  strcat(usrprmpt, prmptend);

	printf("Welcome!\n");
	dish_event_loop(usrprmpt);
}

char *dish_get_cwd(char **args) {
	char buffer[1024];
	char *cwd = getcwd(buffer, sizeof(buffer));
	

	if (cwd == NULL) {
		perror("We seem to be lost.. ");
		return NULL;
	}

	strcpy(dish_cwd, cwd);
	printf("%s\n", dish_cwd);

	return cwd;
}

char *dish_chng_cwd(char **args) {
  char *tmp_pth = args[1];
  if (tmp_pth[0] == '/') {
    tmp_pth[0] = ' ';
    for (int i = 0; i < strlen(tmp_pth); i++) {
      tmp_pth[i] = tmp_pth[i + 1];
    }
  } 
  const char *pth = tmp_pth;

  if (int d = chdir(pth) != 0) {
    perror("Great heavens.. ");
  }
  dish_get_cwd(args);
}

int dish_ls(char **args) {
  char tdir[MAX_BUF_SIZE];
  if (args[1] == NULL) {
    strcpy(tdir, "."); // default to cwd
  }
  else {
    strcpy(tdir, args[1]); // let user target dir to ls
  }
  DIR *cdir = opendir(tdir);
  struct dirent *retdir;
  if (cdir == NULL) {
    perror("Can't open directory: ");
    return 1;
  } 

  while ((retdir = readdir(cdir))) {
    if (strcmp(retdir->d_name, ".") != 0 && strcmp(retdir->d_name, "..") != 0) {;
      char ftype = retdir->d_type;
      if (ftype == DT_UNKNOWN) {
        printf("Filesystem doesn't support dirent file types... \n");
      } else {
        if ((int)ftype == 4 && retdir->d_name[0] != '.') {
          printf("\033[31m%s/    \033[39m", retdir->d_name);
        } else {
          if (  char iftmp = strstr(retdir->d_name, ".tmp")  == NULL && 
                retdir->d_name[0] != '.') {
          printf("%s    ", retdir->d_name);
          }
        }
      }
    }
  }
  printf("\n");

  closedir(cdir);
  return 0;
}

void dish_event_loop(char* usrprmpt) {
	char* buf = malloc(MAX_BUF_SIZE);

  if (buf == NULL) {
    printf("Failed to allocate memory for cmd buffer.\n");
    exit(1);
  }
	while ((buf = readline(usrprmpt)) != NULL) { // infinite loop for REPL
    if (buf[0] != '\0' || buf[0] != '\n') {
      add_history(buf);
    }

		char** args;
		args = dish_splt_str(buf); // builtin
    
    // TODO: add '&&' operation

		dish_exec(args); // look for cmd

		for (int i = 0; args[i] != NULL; i++) {
			free(args[i]);
		}
		free(args);
	}
	// exit stuff
  free(usrprmpt);
	free(buf);
}

void sgchld_handling(int signum) {
  int temp_err = errno;
  printf("Terminating %i\n", signum);
  while(waitpid(-1, NULL, WNOHANG) > 0);
  errno = temp_err;
  }

int dish_run(char** args) {
  pid_t dishprc;

  dishprc = fork();
  if(dishprc<0) {
    perror("Failed to fork");
    exit(EXIT_FAILURE);
  }

  static char *newenv[] = {NULL};

  if (dishprc== 0) { // if child process
    signal(SIGINT,  SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    execvp(args[1], (&args[1]));

    // only runs if exec returns (meaning it failed)
    perror("Execve failed");
    exit(EXIT_FAILURE);
  } else {
    int child_status;
    waitpid(dishprc, &child_status, 0);
  }

}

int dish_hi(char** args) {
  printf("Hello!\n");
  return 1;
}

int dish_exit(char** args) {
	printf("Exiting.. byebye\n");
	exit(0);
}
