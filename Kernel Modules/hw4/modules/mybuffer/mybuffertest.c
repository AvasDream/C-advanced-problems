#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int main(int argc, char const *argv[])
{
    int i = 0;
    char const text[4][10] = {"Birne", "Apfel", "Physalis", "Kartoffel"} ;
    int counter = 0;
    int child;
    char buffer[512];
    printf("Started Test.\n");
    if(fork() == 0) {
        ++counter;
        printf("Started Thread 2\n");
    }
    if(fork() == 0) {
        counter += 2;
        printf("%d\n", counter);
        if(counter == 2) {
            printf("Started Thread 3\n");
        }
        else if (counter == 3){
            printf("Started Thread 4\n");
        }
        sleep(2);
        for(i;i<20;i++){
            FILE *file = fopen("/dev/mybuffer", "w");
            int j = 0;
            while(j != 10){
                fputc(text[i % 4][j], file);
                j++;
            }
            fclose(file);
            pthread_yield();
        } 
    }
    else{
        for(i;i<20;i++){
            FILE *file = fopen("/dev/mybuffer", "r");
            int j = 0;
            int retVal = 0;
            memset(buffer, 0, 512);
            fgets(buffer, 512, file);
            
            for(j = 0; j < 4; j++) {
                if((retVal = strcmp(text[j], buffer))== 0){
                    break;
                }
            }
            if(retVal != 0){
                printf("%s\n", buffer);
                printf("RACECONDITION!\n");
                exit(0);
            } 
            fclose(file);
            pthread_yield();
        } 
    }
    return -1;
}
