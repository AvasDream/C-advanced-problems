//Include stdlib for compiler warnings & NULL
#include <stdlib.h>

void main() {

int *pointer;
int integer = 1337;
pointer = malloc(sizeof(integer));
//free(pointer);
pointer = NULL;
*pointer;
}
