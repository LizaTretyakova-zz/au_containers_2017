#ifndef AU_CONT
#define AU_CONT

#include <sched.h>
#include <signal.h>
#include <sys/types.h>

struct child_config {
    int fd;
    char *hostname;
    char **argv;
    char *mount_dir;
    int daemonize;
};

static const char* PROCEED = "proceed";

#endif
