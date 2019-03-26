#include <stdio.h>
#include <stdlib.h>

void my_exit(int exitcode);

static FILE *file[257];
int main(int argc, char* argv[]) {
    int i;

    for(i = 0; i < 257; i++){
        file[i] = fopen("/dev/openclose0", "r");
    }
    if(fopen("/dev/openclose0", "r") != NULL) {
        printf("Could open more than 257 times.\n");
        my_exit(-1);
    }
    fclose(file[0]);
    if(fopen("/dev/openclose0", "r") == NULL) {
        printf("Could not reopen after close.\n");
        my_exit(-2);
    }
    my_exit(0);
}

void my_exit(int exitcode) {
    int i = 0;
    for(i; i < 257; i++){
        fclose(file[i]);
    }
    if(exitcode != 0) {
        printf("TEST FAILED");
    }
    else {
        printf("TEST SUCCEEDED");
    }
    exit(exitcode);
} 