#ifndef AU_CONT
#define AU_CONT

#include <sched.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

struct child_config {
    int fd;
    char *hostname;
    char **argv;
    char mount_dir[64];
    int daemonize;
    char net[32];
    char cpu[32];
};

static const char* PROCEED = "proceed";

#endif
