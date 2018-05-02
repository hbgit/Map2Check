#include <stdio.h>
#include <stdlib.h>

// Aborts if input 007
int main() {
  int ERROR;
  scanf("%d", &ERROR);

  if(ERROR == 0) {
    scanf("%d", &ERROR);
    if(ERROR == 0) {
      scanf("%d", &ERROR);
      if(ERROR == 7) {
        abort();
      }
    }
  }
  
  return 0;
}
