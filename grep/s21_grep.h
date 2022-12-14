#ifndef SRC_GREP_S21_GREP_H_
#define SRC_GREP_S21_GREP_H_

//  #define _GNU_SOURCE
#define _POSIX_C_SOURCE 201701L
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct s_flags {
  char *name;
  int i_flag;
  int n_flag;
  int v_flag;
  int c_flag;
  int l_flag;
  int e_flag;
  int h_flag;
  int s_flag;
  int o_flag;
  int line_number;
  int count;
  char pattern[1024];
  struct s_files_list *files;
  int files_count;
};

struct s_files_list {
  char *file_name;
  struct s_files_list *next;
};

void add_pattern(struct s_flags *flags, char *pattern);
void add_pattern_from_file(struct s_flags *flags, char *file_name);
struct s_files_list *add_file(struct s_files_list *head, char *file_name);
void destroy_files_list(struct s_files_list *head);
void parse_flags(struct s_flags *flags, int argc, char **argv);
void output_result(char *file_name, struct s_flags *flags);

#endif  //  SRC_GREP_S21_GREP_H_
