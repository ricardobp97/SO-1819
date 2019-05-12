#include <stdio.h>

int main(int argc, char const *argv[]) {
  for(int i = 0; i < 100; i++) {
    printf("3 %d\n",i);
  }
  for(int i = 0; i < 100; i++) {
    printf("3 %d\n",-i);
  }
  printf("3\n");
  return 0;
}
