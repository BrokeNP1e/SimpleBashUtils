#ifndef C3_SIMPLEBASHUTILS_1_S21_GREP_H
#define C3_SIMPLEBASHUTILS_1_S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct options {
  int e;
  int i;
  int v;
  int c;
  int count_flag_c;
  int l;
  int l_check;
  int n;
  int number_line;
  int h;
  int s;
  int f;
  int o;
  int count_p;
  int count_f;
  char **patterns;
  char **files;
};

int parser(int argc, char *argv[], struct options *opt, int optcheck);

int flags(struct options *opt, int const *optcheck);

int pattern_file(struct options *opt);

int reader(struct options *opt);

int regex_func(struct options *opt, char *buffer, int number_file);

int flag_i(struct options *opt, int number_pattern, regex_t *regex);

void flag_o(struct options *opt, char *buffer, int number_file, regex_t regex,
            regmatch_t pmatch);

void flag_n(struct options *opt, int flag_v);

int regexec_v(char *buffer, regex_t regex, regmatch_t pmatch);

#endif  // C3_SIMPLEBASHUTILS_1_S21_GREP_H
