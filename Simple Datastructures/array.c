//Include stdlib for compiler warnings & NULL
#include <stdlib.h>

void main() {

int *pointer;
int integer = 1337;
pointer = malloc(sizeof(integer)*100);
pointer = NULL;
//free(pointer);
}
