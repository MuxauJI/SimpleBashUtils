#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct s_files_list {
  char *file_name;
  struct s_files_list *next;
};

struct s_flags {
  char *name;
  int b_flag;
  int n_flag;
  int s_flag;
  int T_flag;
  int E_flag;
  int v_flag;
  int line_number;
  struct s_files_list *files;
  int space;
  int line;
};

struct s_files_list *add_file(struct s_files_list *head, char *file_name);
void parse_flags(struct s_flags *flags, char *flag);
void destroy_files_list(struct s_files_list *head);
void output_result(char *file_name, struct s_flags *flags);

#endif  //  SRC_CAT_S21_CAT_H_
