#include "s21_grep.h"

int main(int argc, char **argv) {
  struct s_flags flags = {argv[0], 0, 0, 0, 0,   0,    0, 0,
                          0,       0, 1, 0, {0}, NULL, 0};
  struct s_files_list *temp = NULL;

  // получение и разделение регулярок, флагов и файлов
  parse_flags(&flags, argc, argv);

  // вывод результата поиска по каждому файлу
  temp = flags.files;
  while (temp != NULL) {
    output_result(temp->file_name, &flags);
    temp = temp->next;
  }

  destroy_files_list(flags.files);
  return 0;
}

void add_pattern(struct s_flags *flags, char *pattern) {
  char buffer[1024] = {0};
  if (strlen(flags->pattern) == 0) {
    memcpy(flags->pattern, pattern, strlen(pattern) + 1);
  } else {
    strcat(buffer, flags->pattern);
    strcat(buffer, "|");
    strcat(buffer, pattern);
    memcpy(flags->pattern, buffer, strlen(buffer));
  }
}

void add_pattern_from_file(struct s_flags *flags, char *file_name) {
  FILE *fp;
  char str[1024] = {0};
  char *ch;
  fp = fopen(file_name, "rb");
  if (fp == NULL) {
    if (!flags->s_flag)
      fprintf(stderr, "%s: %s: No such file or directory\n", flags->name,
              file_name);
    return;
  }

  while (!feof(fp)) {
    if (fgets(str, 1024, fp)) {
      if ((ch = strchr(str, '\n')) != NULL) {
        *ch = '\0';
      }
      add_pattern(flags, str);
    }
  }
  fclose(fp);
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

void parse_flags(struct s_flags *flags, int argc, char **argv) {
  int c;
  int pat = 0;
  struct s_files_list *tail = NULL;

  while ((c = getopt(argc, argv, "nicvle:f:hso")) != -1) {
    switch (c) {
      case 'n':
        flags->n_flag = 1;
        break;
      case 'i':
        flags->i_flag = 1;
        break;
      case 'c':
        flags->c_flag = 1;
        break;
      case 'v':
        flags->v_flag = 1;
        break;
      case 'l':
        flags->l_flag = 1;
        break;
      case 'e':
        add_pattern(flags, optarg);
        pat = 1;
        break;
      case 'f':
        add_pattern_from_file(flags, optarg);
        if (strlen(flags->pattern) > 0) pat = 1;
        break;
      case 'h':
        flags->h_flag = 1;
        break;
      case 's':
        opterr = 0;
        flags->s_flag = 1;
        break;
      case 'o':
        flags->o_flag = 1;
        break;
      default:
        printf("Usage: s21_grep [OPTION]... PATTERN [FILE]...\n");
        break;
    }
  }
  while (optind < argc) {
    if (pat == 0) {
      add_pattern(flags, argv[optind++]);
      pat = 1;
      continue;
    }
    if (!flags->files) {
      tail = add_file(flags->files, argv[optind]);
      if (tail == NULL) {
        fprintf(stderr, "Error malloc in file-list.");
        exit(1);
      }

      flags->files = tail;
    } else {
      tail = add_file(tail, argv[optind]);
    }
    flags->files_count++;
    optind++;
  }
}

void output_result(char *file_name, struct s_flags *flags) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  int l_flag = 0;
  int nread;
  int result;
  regex_t pattern;

  fp = fopen(file_name, "rb");
  if (fp == NULL) {
    if (!flags->s_flag)
      fprintf(stderr, "%s: %s: No such file or directory\n", flags->name,
              file_name);
    return;
  }

  result =
      regcomp(&pattern, flags->pattern,
              flags->i_flag == 1 ? REG_ICASE | REG_EXTENDED : REG_EXTENDED);
  if (result != 0) {
    if (!flags->s_flag) fprintf(stderr, "Error compilation regex!\n");
    if (fp) fclose(fp);
    regfree(&pattern);
    return;
  }

  if (flags->v_flag && flags->o_flag && !flags->l_flag && !flags->c_flag) {
    if (fp) fclose(fp);
    regfree(&pattern);
    return;
  }

  while ((nread = getline(&line, &len, fp)) != -1) {
    int c_flag = 0;
    char *ch = NULL;

    if ((ch = strchr(line, '\n')) != NULL) {
      *ch = '\0';
    }

    result = regexec(&pattern, line, 0, NULL, 0);

    if (flags->o_flag && !flags->v_flag) {
      size_t nmatch = 2;
      regmatch_t temp[2];
      regoff_t length;
      result = regexec(&pattern, line, nmatch, temp, 0);
      char *pstr = line;
      while (result != REG_NOMATCH) {
        length = temp[0].rm_eo - temp[0].rm_so;
        int length_int = length;

        if (flags->l_flag) {
          l_flag = 1;
        } else if (flags->c_flag) {
          c_flag = 1;
        } else {
          if (flags->files_count > 1 && !flags->h_flag) {
            printf("%s:", file_name);
          }
          if (flags->n_flag) {
            printf("%d:%.*s\n", flags->line_number, length_int,
                   pstr + temp[0].rm_so);
          } else {
            printf("%.*s\n", length_int, pstr + temp[0].rm_so);
          }
        }
        pstr += temp[0].rm_eo;
        result = regexec(&pattern, pstr, nmatch, temp, 0);  // ищем совпадения
      }

      if (c_flag) {
        flags->count++;
        c_flag = 0;
      }
      flags->line_number++;
      continue;
    }

    if (result == flags->v_flag) {
      if (flags->l_flag) {
        printf("%s\n", file_name);
        flags->line_number++;
        flags->count++;
        break;
      }

      if (flags->n_flag && flags->o_flag) {
        flags->count++;
        continue;
      }

      if (flags->c_flag == 0) {
        if (flags->files_count > 1 && !flags->h_flag) {
          printf("%s:", file_name);
        }
        if (flags->n_flag && !flags->o_flag) {
          printf("%d:%s\n", flags->line_number, line);
        } else {
          printf("%s\n", line);
        }
      }
      flags->count++;
    }
    flags->line_number++;
  }

  if (l_flag) {
    printf("%s\n", file_name);
    l_flag = 0;
  }

  if (flags->c_flag && !flags->l_flag) {
    if (flags->files_count > 1 && !flags->h_flag) {
      printf("%s:", file_name);
    }
    printf("%d\n", flags->count);
    flags->count = 0;
  }

  if (line) free(line);
  if (fp) fclose(fp);
  flags->line_number = 1;
  regfree(&pattern);
}
