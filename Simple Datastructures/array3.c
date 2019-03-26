//Include stdlib for compiler warnings & NULL
#include <stdlib.h>

void main() {

int *pointer;
int integer = 1337;
pointer = malloc(sizeof(integer)*100);
pointer[3] = 13;
pointer[21] = 99;
pointer[10] = 1;
pointer[20] = "AAAA"; // overwritting content
printf("%d", pointer[21]);
printf("%d", pointer[20]);
//pointer[4] = NULL;
pointer = NULL;
//free(pointer);
}
