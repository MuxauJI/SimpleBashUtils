#include "s21_cat.h"

int main(int argc, char **argv) {
  struct s_flags flags = {argv[0], 0, 0, 0, 0, 0, 0, 1, NULL, 0, 0};
  struct s_files_list *temp = NULL;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '\0') {
      parse_flags(&flags, argv[i]);
    } else {
      if (!flags.files) {
        temp = add_file(flags.files, argv[i]);
        if (temp == NULL) {
          fprintf(stderr, "Error malloc in file-list.");
          exit(1);
        }
        flags.files = temp;
      } else {
        temp = add_file(temp, argv[i]);
      }
    }
  }

  temp = flags.files;
  if (temp == NULL) {
    output_result("", &flags);
  } else {
    while (temp != NULL) {
      output_result(temp->file_name, &flags);
      temp = temp->next;
    }
  }
  destroy_files_list(flags.files);
  return 0;
}

struct s_files_list *add_file(struct s_files_list *head, char *file_name) {
  struct s_files_list *new, *tmp;
  new = malloc(sizeof(struct s_files_list) * 1);
  if (new == NULL) {
    return NULL;
  }
  new->file_name = file_name;
  new->next = NULL;
  if (head != NULL) {
    tmp = head;
    while (tmp->next != NULL) {
      tmp->next = tmp;
    }
    tmp->next = new;
  }
  return new;
}

void parse_flags(struct s_flags *flags, char *flag) {
  if (strlen(flag) > 2 && flag[0] == '-' && flag[1] != '-') {
    int i = 1;
    char *temp[2];
    while (flag[i] != '\0') {
      sprintf((char *)temp, "-%c", flag[i]);
      parse_flags(flags, (char *)temp);
      i++;
    }
  }

  if (strcmp(flag, "-b") == 0 || strcmp(flag, "--number-nonblank") == 0) {
    flags->b_flag = 1;
    flags->n_flag = 1;
  } else if (strcmp(flag, "-e") == 0) {
    flags->v_flag = 1;
    flags->E_flag = 1;
  } else if (strcmp(flag, "-n") == 0 || strcmp(flag, "--number") == 0) {
    flags->n_flag = 1;
  } else if (strcmp(flag, "-s") == 0 || strcmp(flag, "--squeeze-blank") == 0) {
    flags->s_flag = 1;
  } else if (strcmp(flag, "-t") == 0) {
    flags->T_flag = 1;
    flags->v_flag = 1;
  } else if (strcmp(flag, "-v") == 0) {
    flags->v_flag = 1;
  } else if (strcmp(flag, "-T") == 0) {
    flags->T_flag = 1;
  } else if (strcmp(flag, "-E") == 0) {
    flags->E_flag = 1;
  }
}
void destroy_files_list(struct s_files_list *head) {
  struct s_files_list *temp = head, *next;
  while (temp != NULL) {
    next = temp->next;
    free(temp);
    temp = next;
  }
  next = NULL;
  head = NULL;
}

void output_result(char *file_name, struct s_flags *flags) {
  FILE *fp;
  DIR *dir;
  int ch;

  if ((dir = opendir(file_name)) != NULL) {
    fprintf(stderr, "%s: %s: Is a directory\n", flags->name, file_name);
    closedir(dir);
    return;
  }
  fp = fopen(file_name, "rb");
  if (fp == NULL && strcmp(file_name, "") > 0 && strcmp(file_name, "-") > 0) {
    fprintf(stderr, "%s: %s: No such file or directory\n", flags->name,
            file_name);
    return;
  }

  while ((ch = fgetc((fp == NULL ? stdin : fp))) != EOF) {
    if (ch == '\n') {
      if (flags->line == 0) {
        if (flags->s_flag == 1 && flags->space) {
          continue;
        }
        flags->space = 1;
      }
      if (flags->n_flag == 1 && flags->b_flag == 0 && flags->line == 0) {
        printf("%6d\t", flags->line_number++);
      }
      if (flags->E_flag == 1) putchar('$');
      putchar('\n');
      flags->line = 0;
      continue;
    }
    if (flags->n_flag == 1 && flags->line == 0) {
      printf("%6d\t", flags->line_number++);
    }
    flags->line = 1;
    if (flags->v_flag == 1) {
      if (flags->T_flag == 0 && ch == '\t') {
        putchar(ch);
      } else {
        if (ch > 127) {
          printf("M-");
          if (ch >= 160) {
            if (ch < 255) {
              ch -= 128;
            } else {
              ch = 127;
            }
          } else {
            printf("^");
            ch -= 64;
          }
        }
        if (ch < 32) {
          printf("^%c", ch + 64);
        } else if (ch == 127) {
          printf("^?");
        } else {
          putchar(ch);
        }
      }
    } else {
      if (flags->T_flag == 1 && ch == '\t') {
        printf("^I");
      } else {
        putchar(ch);
      }
    }
    flags->space = 0;
  }
  if (fp != NULL) {
    fclose(fp);
  }
}
