#include <stddef.h>
#ifndef BUILTINS_H
#define BUILTINS_H

typedef struct Arguments {
  char** args_list;
  size_t args_size;
  char** exec_list; // 1 index after '&&'
  size_t exec_list_size;
  char ***indiv_arg_list;
  size_t indiv_arg_list_size;
} Arguments;

extern char* dish_builtin_func_int[];
extern char* dish_builtin_func_char[];

extern int (*builtin_func_int[]) (char**);
extern char* (*builtin_func_char[]) (char**);

int dish_bltin_num_int();
int dish_bltin_num_char();

Arguments* dish_splt_str(char* str);
void dish_exec(Arguments* args);
Arguments* make_argument(Arguments* args);

extern char* str;
extern char** args;
extern char *dish_get_cwd(char** args);
extern char *dish_chng_cwd(char** args);
extern int dish_exit(char ** args);
extern int dish_hi(char** args);
extern int dish_ls(char** args);
extern int dish_run(char** args);

#endif
