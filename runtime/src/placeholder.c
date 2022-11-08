#include <stdio.h>

// Add function named dummyPrint with signature void(int) to llvm to have this linked in.
void dummyPrint(int i) {
  printf("I'm a function! %d\n", i);
}


// TODO buffer struct
// TODO stream state value

void rt_print_int(int i) {
  printf("%d", i);
}

void rt_print_real(float d) {
  printf("%g", d);
}

void rt_print_char(char c) {
  printf("%c", c);
}

void rt_print_bool(int b) {
  printf("%c", b ? 'T' : 'F');
}

void rt_scan_int(int *i, int *state, char *buffer) {
  scanf("%d", i);
}

void rt_scan_real(float *d, int *state, char *buffer) {
  scanf("%g", d);
}

void rt_scan_char(char *c, int *state, char *buffer) {
  scanf("%c", c);
}

void rt_scan_bool(int *b, int *state, char *buffer) {
  char c;
  scanf("%c", &c);
  if (c == 'T') *b = 1;
  else *b = 0;
}
