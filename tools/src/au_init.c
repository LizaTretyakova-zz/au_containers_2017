#include "au_init.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int set_ids(void) {
    fprintf(stderr, "%d %d %d %d\n",
            getuid(), geteuid(), getgid(), getegid());
    if(setuid(0)) {
        perror("setuid");
        return -1;
    }
    if(seteuid(0)) {
        perror("seteuid");
        return -1;
    }
    if(setgid(0)) {
        perror("setgid");
        return -1;
    }
    if(setegid(0)) {
        perror("setegid");
        return -1;
    }
    return 0;
}
