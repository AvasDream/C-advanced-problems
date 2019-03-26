#include <sys/sysinfo.h>
#include <unistd.h>
#include <stdlib.h>
#include "sysinfo.h"

extern int print_info(char * buffer, size_t * size)
{
    struct sysinfo info;
    char name[255];
    *size = 0;
    if (sysinfo(&info) != 0)
    {
        perror("Could not get system info.");
        return 1;
    }
    if(gethostname(name, 255) != 0){
        perror("Could not get host name.");
        return 2;
    }
    *size += sprintf(&buffer[*size], "Hostname: %s\n", name);
    *size += sprintf(&buffer[*size], "Uptime: %ld second(s)\n", info.uptime);
    *size += sprintf(&buffer[*size], "Process count: %d\n", info.procs);
    *size += sprintf(&buffer[*size], "Total RAM: %lu Byte(s)\n", info.totalram);
    *size += sprintf(&buffer[*size], "Free RAM: %lu Byte(s)\n", info.freeram);
    *size += sprintf(&buffer[*size], "Page size: %u Byte(s)\n", info.mem_unit);
    return 0;
}