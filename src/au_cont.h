#ifndef AU_CONT
#define AU_CONT

#include <sched.h>

struct child_config {
    int argc;
    uid_t uid;
    int fd;
    char *hostname;
    char **argv;
    char *mount_dir;
};

static const size_t CHILD_STACK_SIZE = 8 * 1024 * 1024;
static const int FLAGS = CLONE_NEWCGROUP
                         | CLONE_NEWIPC
                         | CLONE_NEWNS
                         | CLONE_NEWNET
                         | CLONE_NEWPID
                         | CLONE_NEWUTS
                         | CLONE_NEWUSER;

#endif