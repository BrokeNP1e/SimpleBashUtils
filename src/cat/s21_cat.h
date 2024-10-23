#ifndef C3_SIMPLEBASHUTILS_1_S21_CAT_H
#define C3_SIMPLEBASHUTILS_1_S21_CAT_H

#include <stdio.h>
#include <string.h>

struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
};

int parser(int argc, char *argv[], struct options *opt, int number_file[],
           int *numbers);

int reader(char *argv[], struct options *opt, int const number_file[],
           int const *numbers);

int processing(struct options *opt, char c, char prev_c, char prev2_c,
               int count_n, int count_b);

#endif  // C3_SIMPLEBASHUTILS_1_S21_CAT_H
