//Use compiler flag "-g" to compile with extra information for gdb
// (set break lines)
//Include stdlib for compiler warnings & NULL
#include <stdlib.h>
#include <stdio.h>
// Type vector
typedef struct vec_struct {
	int *data;
	int size;
	int elements;
} vec;
// Prototypes
int create_vec(vec *v);
int add_element(vec *v, int element);
int get(vec *v, int index);

void main() {

//create vector
vec *vector;
create_vec(&vector);
// add element
add_element(vector, 1337);
// print elements
int value = get(vector, 1);
printf("size: %d\n", vector->size);
printf("elements: %d\n", vector->elements);
printf("value at index 1: %d\n", value);
printf("Add new element 707\n");
add_element(vector, 707);
value = get(vector, 2);
printf("size: %d\n", vector->size);
printf("elements: %d\n", vector->elements);
printf("value at index 2: %d\n", value);

}


int create_vec(vec *v) {
//Allocate memory for vector data
v->data = malloc(sizeof(int));
v->size = 1;
v->elements = 0;
return 0;
}

int add_element(vec *v, int element) {
int *data = realloc(v->data, sizeof(int));
v->data = data;
v->data[v->size + 1] = element;
v->size += 1;
v->elements += 1;
return 0;
}

int get(vec *v, int index) {
return v->data[index];
}
