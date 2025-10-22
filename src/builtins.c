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

Arguments* dish_splt_str(char* str) { // TODO: its time to clean this up
	char** args = NULL;
	char indarg[MAX_BUF_SIZE];
	size_t strsize = strlen(str);
	int count = 0;
	int k = 0;
  bool is_str = false;

  Arguments* cur_arg_list = malloc(sizeof(Arguments));
  if (cur_arg_list == NULL) {
    perror("cur_arg_list");
    exit(EXIT_FAILURE);
  }

	for (int i = 0; i <= strsize; i++) {
		char c = str[i];

    if (c == '"' || c == '\'') {
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
  cur_arg_list->args_list = args;
  cur_arg_list->args_size = count;
	args[count] = NULL;

	return make_argument(cur_arg_list);
}

Arguments* make_argument(Arguments* args) {
  args->exec_list = malloc(sizeof(char *) * args->args_size);
  if (args->exec_list == NULL) {
    perror("exec_list");
    exit(EXIT_FAILURE);
  }

  char** tmpargs = args->args_list;
  int exec_list_index = 1;
  if (tmpargs != NULL) {
    args->exec_list_size = 1;
    args->exec_list[0] = args->args_list[0];
  }


  for (int i = 0; i < args->args_size; i++) { // get all executable arguments
    if (strcmp(tmpargs[i], "&&") == 0) {
      args->exec_list[exec_list_index] = tmpargs[i + 1];
      args->exec_list_size++;
      exec_list_index++;
    }
  }

  char **indiv_arg = malloc(sizeof(char *) * args->args_size);
  if (indiv_arg == NULL) {
    perror("indiv_arg (make_argument)");
    exit(EXIT_FAILURE);
  }
  int is_exec = 0;
  int tmp_arg_ind = 0;
  int exec_arg_ind = 0;

// j = current arg passed by user
  args->indiv_arg_list = malloc(sizeof(char**) * args->args_size);
  if (args->indiv_arg_list == NULL) {
    perror("indiv_arg_list");
    exit(EXIT_FAILURE);
  }
  args->indiv_arg_list_size = 0;

  for (int j = 0; j < args->args_size; j++) { // check every word
    char* cur_arg = args->args_list[j];

    for (int k = 0; k < args->exec_list_size; k++) { // compare against every exec arg
      if (strcmp(cur_arg, args->exec_list[k]) == 0) {
        is_exec = 1;
        break;
      }
    }

    if (is_exec == 1) {
      for (int fj = j + 1; fj < args->args_size; fj++) {
        char* tmp_cur = args->args_list[fj];
        if(strcmp(tmp_cur, "&&") == 0) {
          break;
        } 
        if (tmp_arg_ind >= args->args_size) {
          perror("tmp_arg_ind");
          exit(EXIT_FAILURE);
        }

        indiv_arg[tmp_arg_ind++] = tmp_cur;

      }

      indiv_arg[tmp_arg_ind] = NULL;

      if (args->indiv_arg_list_size >= args->args_size) {
        perror("indiv_arg_list_size");
        exit(EXIT_FAILURE);
      }
      args->indiv_arg_list[args->indiv_arg_list_size++] = indiv_arg;
      is_exec = 0;
      tmp_arg_ind = 0;
    }

  }

  return args;
}

void dish_exec(Arguments* args) {
	char* exec_arg = args->args_list[0];
  size_t exec_amount = args->exec_list_size;
	if (exec_arg == NULL) {
		printf("Why so quiet? (No command entered)\n");
    return;
	}

  int checkcmd = 0;

  // looks for cmd with int return type
	for (int i = 0; i < dish_bltin_num_int(); i++) { 
		if (strcmp(exec_arg, dish_builtin_func_int[i]) == 0) {
			builtin_func_int[i](args->args_list);
      checkcmd = 1;
		}
	}
  // if not in (int) arr looks for cmds with string return type
  for (int j = 0; j < dish_bltin_num_char(); j++) {
    if (strcmp(exec_arg, dish_builtin_func_char[j]) == 0) {
      builtin_func_char[j](args->args_list);
      checkcmd = 1;
    }
  }

  if (checkcmd == 0) {
    printf("Command '%s' not found.\n", exec_arg);
    return;
  }
}
