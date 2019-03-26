#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    struct sysinfo info;
    char name[255];
    if (sysinfo(&info) != 0)
    {
        perror("Could not get system info.");
        exit(1);
    }
    if(gethostname(name, 255) != 0){
        perror("Could not get host name.");
        exit(2);
    }
    printf("Hostname: %s\n", name);
    printf("Uptime: %ld second(s)\n", info.uptime);
    printf("Process count: %d\n", info.procs);
    printf("Total RAM: %lu Byte(s)\n", info.totalram);
    printf("Free RAM: %lu Byte(s)\n", info.freeram);
    printf("Page size: %u Byte(s)\n", info.mem_unit);
    exit(0);
}