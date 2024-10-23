#include "s21_grep.h"

int main(int argc, char *argv[]) {
  struct options *opt = calloc(1, sizeof(struct options));
  opt->patterns = calloc(0, sizeof(char *));
  opt->files = calloc(0, sizeof(char *));
  int optcheck = 0;

  parser(argc, argv, opt, optcheck);
  reader(opt);

  for (int i = 0; i < opt->count_p; ++i) {
    free(opt->patterns[i]);
  }
  for (int i = 0; i < opt->count_f; ++i) {
    free(opt->files[i]);
  }

  free(opt->patterns);
  opt->patterns = NULL;
  free(opt->files);
  opt->files = NULL;
  free(opt);
  return 0;
}

int parser(int argc, char *argv[], struct options *opt, int optcheck) {
  int flag = 0;
  while ((optcheck = getopt_long(argc, argv, "e:ivclnhsf:o", NULL, NULL)) !=
         -1) {
    flag = flags(opt, &optcheck);
  }
  if (opt->e == 0 && opt->f == 0 && opt->count_p == 0 && argc > 1) {
    opt->patterns = realloc(opt->patterns, (opt->count_p + 1) * sizeof(char *));
    opt->patterns[opt->count_p] =
        calloc(strlen(argv[optind]) + 1, sizeof(char));
    strcpy(opt->patterns[opt->count_p], argv[optind]);
    optind++;
    opt->count_p = opt->count_p + 1;
  }
  while (optind < argc) {
    opt->files = realloc(opt->files, (opt->count_f + 1) * sizeof(char *));
    opt->files[opt->count_f] = calloc(strlen(argv[optind]) + 1, sizeof(char));
    strcpy(opt->files[opt->count_f], argv[optind]);
    optind++;
    opt->count_f = opt->count_f + 1;
  }
  for (int i = 0; i < opt->count_p; ++i) {
    for (int j = i + 1; j < opt->count_p; ++j) {
      if (strcmp(opt->patterns[i], opt->patterns[j]) == 0) {
        strcpy(opt->patterns[j], "\0");
      }
    }
  }
  return flag;
}

int flags(struct options *opt, int const *optcheck) {
  int flag = 0;
  switch (*optcheck) {
    case 'e':
      opt->e = 1;
      opt->patterns =
          realloc(opt->patterns, (opt->count_p + 1) * sizeof(char *));
      opt->patterns[opt->count_p] = calloc(strlen(optarg) + 1, sizeof(char));
      strcpy(opt->patterns[opt->count_p], optarg);
      opt->count_p = opt->count_p + 1;
      break;
    case 'i':
      opt->i = 1;
      break;
    case 'v':
      opt->v = 1;
      break;
    case 'c':
      opt->c = 1;
      break;
    case 'l':
      opt->l = 1;
      break;
    case 'n':
      opt->n = 1;
      break;
    case 'h':
      opt->h = 1;
      break;
    case 's':
      opt->s = 1;
      break;
    case 'f':
      opt->f = 1;
      flag = pattern_file(opt);
      break;
    case 'o':
      opt->o = 1;
      break;
    default:
      flag = 1;
      break;
  }
  return flag;
}

int pattern_file(struct options *opt) {
  int flag = 0;
  FILE *file = fopen(optarg, "r");
  if (file == NULL) {
    if (opt->s == 0) {
      printf("grep: %s: No such file or directory", optarg);
    }
    flag = 1;
  } else {
    char buffer[256] = {0};
    while (fgets(buffer, 256, file) != NULL) {
      if (strlen(buffer) != 1) {
        buffer[strcspn(buffer, "\n")] = '\0';
      }
      opt->patterns =
          realloc(opt->patterns, (opt->count_p + 1) * sizeof(char *));
      opt->patterns[opt->count_p] = calloc(strlen(buffer) + 1, sizeof(char));
      strcpy(opt->patterns[opt->count_p], buffer);
      opt->count_p = opt->count_p + 1;
    }
    fclose(file);
  }
  return flag;
}

int reader(struct options *opt) {
  int flag = 0;
  int i = 0;
  char buffer[1024] = {0};
  while (i < opt->count_f) {
    FILE *file = fopen(opt->files[i], "r");
    if (file == NULL) {
      if (opt->s == 0) {
        printf("grep: %s: No such file or directory", opt->files[i]);
      }
      flag = 1;
    } else {
      while (fgets(buffer, 1024, file) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
        opt->number_line = opt->number_line + 1;
        flag = regex_func(opt, buffer, i);
      }
      opt->number_line = 0;
      fclose(file);
      if (opt->c == 1) {
        if (opt->h == 0 && opt->count_f > 1) {
          printf("%s:%d\n", opt->files[i], opt->count_flag_c);
        } else {
          printf("%d\n", opt->count_flag_c);
        }
      }
      if (opt->l == 1 && opt->l_check != 0) {
        printf("%s\n", opt->files[i]);
      }
    }

    i++;
    opt->l_check = 0;
    opt->count_flag_c = 0;
  }
  return flag;
}

int regex_func(struct options *opt, char *buffer, int number_file) {
  int flag = 0;
  int check_flag = 0;
  int flag_v = 0;
  regex_t regex = {0};
  regmatch_t pmatch = {0};
  for (int i = 0; i < opt->count_p; ++i) {
    flag = flag_i(opt, i, &regex);
    flag_v = regexec_v(buffer, regex, pmatch);
    if (opt->v == 1) {
      if (flag_v == 0) {
        flag_v = 1;
      } else {
        flag_v = 0;
      }
    }
    if (opt->c == 1 && flag_v == 0) {
      check_flag = 1;
      if (opt->l_check == 0) {
        opt->count_flag_c = opt->count_flag_c + 1;
      }
    }
    if (opt->l == 1 && flag_v == 0) {
      check_flag = 1;
      if (opt->l_check == 0) {
        opt->l_check = 1;
      }
    }
    if (opt->o == 1 && opt->v == 0 && opt->c == 0 && opt->l == 0) {
      check_flag = 1;
      flag_o(opt, buffer, number_file, regex, pmatch);
    }
    regfree(&regex);
    if (check_flag == 0 && flag_v == 0) {
      check_flag = 1;
      if (opt->count_f > 1 && opt->h == 0) {
        printf("%s:", opt->files[number_file]);
      }
      if (opt->n == 1) {
        flag_n(opt, flag_v);
      }
      printf("%s\n", buffer);
    }
  }
  return flag;
}

int flag_i(struct options *opt, int number_pattern, regex_t *regex) {
  int flag = 0;
  if (opt->i == 1) {
    if (regcomp(regex, opt->patterns[number_pattern], REG_ICASE) != 0) {
      flag = 1;
    }
  } else {
    if (regcomp(regex, opt->patterns[number_pattern], 0) != 0) {
      flag = 1;
    }
  }
  return flag;
}

void flag_n(struct options *opt, int flag_v) {
  if (opt->n == 1 && opt->o == 0) {
    if (flag_v == 0) {
      printf("%d:", opt->number_line);
    }
  }
}

void flag_o(struct options *opt, char *buffer, int number_file, regex_t regex,
            regmatch_t pmatch) {
  long long start_check = 0;
  while (regexec(&regex, buffer + start_check, 1, &pmatch, 0) == 0) {
    if (opt->h == 0 && opt->count_f > 1) {
      printf("%s:", opt->files[number_file]);
    }
    if (opt->n == 1) {
      printf("%d:", opt->number_line);
    }
    printf("%.*s\n", (int)(pmatch.rm_eo - pmatch.rm_so),
           buffer + start_check + pmatch.rm_so);
    start_check = start_check + pmatch.rm_eo;
  }
}

int regexec_v(char *buffer, regex_t regex, regmatch_t pmatch) {
  int check_line = 1;
  if (regexec(&regex, buffer, 1, &pmatch, 0) == 0) {
    check_line = 0;
  }
  return check_line;
}