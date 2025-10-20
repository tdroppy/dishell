#include "builtins.h"
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_BUF_SIZE 256

// list of commands (needs to have same index as func)
char* dish_builtin_func_int[] = {
	"quit",
  "hi",
  "ls",
  "run",
};

char* dish_builtin_func_char[] = {
  "cwd",
  "cd",
};

// list of functions (needs to have same index as commands)
int (*builtin_func_int[]) (char**) = {
	&dish_exit,
  &dish_hi,
  &dish_ls,
  &dish_run,
};

char* (*builtin_func_char[]) (char**) = {
  &dish_get_cwd,
  &dish_chng_cwd,
};

// sizing of arrays (used in forloop in dish_exec())
int dish_bltin_num_int() {
	return sizeof(dish_builtin_func_int) / sizeof(char*);
}

int dish_bltin_num_char() {
  return sizeof(dish_builtin_func_char) / sizeof(char*);
}

char** dish_splt_str(char* str) {
	char** args = NULL;
	char indarg[MAX_BUF_SIZE];
	size_t strsize = strlen(str);
	int count = 0;
	int k = 0;
  bool is_str = false;

	for (int i = 0; i <= strsize; i++) {
		char c = str[i];

    if (c == '"' || c == '\'' || c == '\\') {
      is_str = !is_str;
      continue;
    }

		if (c != ' ' && c != '\0') {
      if (k < MAX_BUF_SIZE) {
        indarg[k++] = c;
      }
		}

    else if ((c == ' ' || c == '\0') && is_str) {
      if (k < MAX_BUF_SIZE) {
        indarg[k++] = c;
      }
    }

		else if (k > 0) {
			if (k < MAX_BUF_SIZE) {
        indarg[k] = '\0';
      }

			char **tmpargs = realloc(args, (count + 1) * sizeof(char*));
      if (tmpargs == NULL) {
        printf("Resize (realloc) for args failed\n");
        free(args);
        exit(EXIT_FAILURE);
      }
      args = tmpargs;

			args[count] = malloc(k + 1);
      if (args[count] == NULL) {
        for (int j = 0; j < count; j++) {
          free(args[j]);
        }
        free(args);
        exit(EXIT_FAILURE);
      }
			strcpy(args[count], indarg); // not valid in MSVC
			count++;
			k = 0;
		}
	}

	char **tmpargs = realloc(args, (count + 1) * sizeof(char*));
  if (tmpargs == NULL) {
    printf("Resize (realloc) for args failed\n");
    free(args);
    exit(EXIT_FAILURE);
  }
  args = tmpargs;
	args[count] = NULL;

	return args;
}

void dish_exec(char** args) {
	char* exec_arg = args[0];
	if (exec_arg == NULL) {
		printf("Why so quiet? (No command entered)\n");
    return;
	}

  int checkcmd = 0;

  // looks for cmd with int return type
	for (int i = 0; i < dish_bltin_num_int(); i++) { 
		if (strcmp(exec_arg, dish_builtin_func_int[i]) == 0) {
			builtin_func_int[i](args);
      checkcmd = 1;
		}
	}
  // if not in (int) arr looks for cmds with string return type
  for (int j = 0; j < dish_bltin_num_char(); j++) {
    if (strcmp(exec_arg, dish_builtin_func_char[j]) == 0) {
      builtin_func_char[j](args);
      checkcmd = 1;
    }
  }

  if (checkcmd == 0) {
    printf("Command '%s' not found.\n", exec_arg);
  }
}
