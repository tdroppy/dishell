#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "builtins.h"

#define MAX_BUF_SIZE 256

void dish_event_loop();
int dish_exit(char** args);
void dish_exec(char** args);
char *dish_get_cwd(char** args);
char *dish_chng_cwd(char** args);
int dish_hi(char** args);

char dish_cwd[1024];

int main() {
	printf("Welcome!\n");
	dish_event_loop();
}

char *dish_get_cwd(char **args) {
	char buffer[1024];
	char *cwd = getcwd(buffer, sizeof(buffer));
	

	if (cwd == NULL) {
		perror("We seem to be lost..: ");
		return NULL;
	}

	strcpy(dish_cwd, cwd);
	printf("%s\n", dish_cwd);

	return cwd;
}

char *dish_chng_cwd(char **args) {
  const char *pth = args[1];
  if (int d = chdir(pth) != 0) {
    perror("Great heavens..");
  }
  dish_get_cwd(args);
}

void dish_event_loop() {
	char* buf = malloc(MAX_BUF_SIZE);
	while ((buf = readline("()> ")) != NULL) { // infinite loop for REPL
    if (buf[0] != '\0' || buf[0] != '\n') {
      add_history(buf);
    }

		char** args;
		args = dish_splt_str(buf); // builtin

    //if (strcmp(args[0], "^[[A")) {
      //printf("up arrow\n");
    //}
		dish_exec(args); // look for cmd

		for (int i = 0; args[i] != NULL; i++) {
			free(args[i]);
		}
		free(args);
	}
	// exit stuff
	free(buf);
}

int dish_hi(char** args) {
  printf("Hello!\n");
  return 1;
}

int dish_exit(char** args) {
	printf("Exiting.. byebye\n");
	exit(0);
}
