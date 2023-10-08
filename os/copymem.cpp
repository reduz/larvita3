#include "copymem.h"

#include <string.h>

void movemem_system(void *to, void *from,int amount) {

  memmove(to,from,amount);  

}
