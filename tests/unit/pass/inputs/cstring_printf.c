#include <stdio.h>
int main(void) {
  char s[4] = "abc";
  char t[8] = "abcdefg";
  int n = 3;
  printf("%s %.3s %d\n", s, t, n);
  puts(s);
  return 0;
}
