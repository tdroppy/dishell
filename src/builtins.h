#ifndef BUILTINS_H
#define BUILTINS_H

extern char* dish_builtin_func_int[];
extern char* dish_builtin_func_char[];

extern int (*builtin_func_int[]) (char**);
extern char* (*builtin_func_char[]) (char**);

int dish_bltin_num_int();
int dish_bltin_num_char();

char** dish_splt_str(char* str);
void dish_exec(char** args);

extern char* str;
extern char** args;
extern char *dish_get_cwd(char** args);
extern char *dish_chng_cwd(char** args);
extern int dish_exit(char ** args);
extern int dish_hi(char** args);
extern int dish_ls(char** args);
extern int dish_run(char** args);

#endif
