#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    setuid(0);
    seteuid(0);
    setgid(0);
    setegid(0);
    fprintf(stderr, "set id's\n");
    return 0;
}
