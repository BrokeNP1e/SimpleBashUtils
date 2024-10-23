#include "s21_cat.h"

int main(int argc, char *argv[]) {
  struct options opt = {0};
  int number_file[argc];
  int numbers = 0;

  if (parser(argc, argv, &opt, number_file, &numbers) == 1 ||
      reader(argv, &opt, number_file, &numbers) == 1) {
    printf("n/a\n");
  }

  return 0;
}

int parser(int argc, char *argv[], struct options *opt, int number_file[],
           int *numbers) {
  int flag = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] != '-') {
      int j = 1;
      while (argv[i][j] != '\0' && flag == 0) {
        switch (argv[i][j]) {
          case 'b':
            opt->b = 1;
            break;
          case 'e':
            opt->e = 1;
            opt->v = 1;
            break;
          case 'E':
            opt->e = 1;
            break;
          case 'n':
            opt->n = 1;
            break;
          case 's':
            opt->s = 1;
            break;
          case 't':
            opt->t = 1;
            opt->v = 1;
            break;
          case 'T':
            opt->t = 1;
            break;
          case 'v':
            opt->v = 1;
            break;
          default:
            flag = 1;
            break;
        }
        j++;
      }
    } else if (strcmp(argv[i], "--number-nonblank") == 0) {
      opt->b = 1;
    } else if (strcmp(argv[i], "--number") == 0) {
      opt->n = 1;
    } else if (strcmp(argv[i], "--squeeze-blank") == 0) {
      opt->s = 1;
    } else if (argv[i][0] != '-' && argv[i][0] != '/' && argv[i][0] != '\\' &&
               argv[i][0] != ':' && argv[i][0] != '\'' && argv[i][0] != '"' &&
               argv[i][0] != '?' && argv[i][0] != '*' && argv[i][0] != '.') {
      number_file[*numbers] = i;
      *numbers = *numbers + 1;
    } else {
      flag = 1;
    }
  }
  return flag;
}

int reader(char *argv[], struct options *opt, int const number_file[],
           int const *numbers) {
  int flag = 0;
  int i = 0;
  char c;
  char prev_c = -1;
  char prev2_c = -1;
  int count_b = 1;
  int count_n = 1;
  while (i < *numbers && flag == 0) {
    FILE *file = fopen(argv[number_file[i]], "r");
    if (file == NULL) {
      flag = 1;
    } else {
      while ((c = (char)fgetc(file)) != EOF) {
        if (processing(opt, c, prev_c, prev2_c, count_n, count_b) == 0) {
          printf("%c", c);
        }
        if (c == '\n') {
          count_n++;
        }
        if (c == '\n' && c != prev_c && prev_c != -1) {
          count_b++;
        }
        prev2_c = prev_c;
        prev_c = c;
      }
      fclose(file);
    }
    i++;
    prev_c = -1;
    prev2_c = -1;
    count_b = 1;
    count_n = 1;
  }
  return flag;
}

int processing(struct options *opt, char c, char prev_c, char prev2_c,
               int count_n, int count_b) {
  int flag = 0;
  if (opt->s == 1 && prev_c == '\n' && c == prev_c &&
      (prev2_c == '\n' || prev2_c == -1)) {
    flag = 2;
  } else {
    if ((opt->n == 1 || opt->b == 1) &&
        ((prev_c == -1 && prev2_c == -1) || prev_c == '\n')) {
      if (opt->n == 1 && opt->b == 0) {
        printf("%6d\t", count_n);
      } else if (opt->b == 1 && prev_c != c &&
                 (((prev_c == -1 && prev2_c == -1) && c != '\n') ||
                  prev_c == '\n')) {
        printf("%6d\t", count_b);
      } else if (opt->e == 1 && c == '\n') {
        printf("      \t");
      }
    }
    if (opt->e == 1 && c == '\n') {
      printf("$");
    }
    if (opt->t == 1 && c == '\t') {
      printf("^I");
      flag = 1;
    }
    if (opt->v == 1 && ((c >= 0 && c <= 31 && c != 10 && c != 9) || c == 127)) {
      if (c == 127) {
        printf("^%c", c - 64);
      } else {
        printf("^%c", c + 64);
      }
      flag = 1;
    }
  }
  return flag;
}