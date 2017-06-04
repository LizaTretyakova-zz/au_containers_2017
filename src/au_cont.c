#define _GNU_SOURCE

#include "au_utils.h"
#include "au_child.h"
#include "au_cont.h"

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <linux/capability.h>
#include <linux/limits.h>
#include <pwd.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static const size_t CHILD_STACK_SIZE = 8 * 1024 * 1024;
static const int CLONE_FLAGS = SIGCHLD
        | CLONE_NEWIPC
        | CLONE_NEWNS
        | CLONE_NEWPID
        | CLONE_NEWUTS
        | CLONE_NEWUSER
        | CLONE_NEWNET;

int main (int argc, char **argv)
{
    char* stack = NULL;

    struct child_config config = {0};
    int err = 0;
    int pipe_fds[2] = {0};
    pid_t child_pid = 0;    

    // create pipe
    pipe2(pipe_fds, O_CLOEXEC);
    // config options
    config.fd = pipe_fds[0];
    config.hostname = "container";
    config.argv = argv + 3;
    config.mount_dir = argv[2];
    sscanf(argv[1], "%d", &config.daemonize);

    // create stack
    if (!(stack = (char*)malloc(CHILD_STACK_SIZE))) {
        perror("stack allocation failed");
        return error(pipe_fds);
    }
    // create child
    if ((child_pid = clone(child, stack + CHILD_STACK_SIZE, CLONE_FLAGS, &config)) == -1) {
        perror("clone");
        err = 1;
        clear_resources(stack, pipe_fds);
        return err;
    }
    close(pipe_fds[0]);
    pipe_fds[0] = 0;
    // tuning
    if (handle_child_uid_map(child_pid)) {
        err = 1;
        kill_and_finish_child(&err, child_pid);
        clear_resources(stack, pipe_fds);
        return err;
    }
    // don't know how to let the python script
    // know my pid here
    write(pipe_fds[1], PROCEED, strlen(PROCEED));

    finish_child(&err, child_pid);
    clear_resources(stack, pipe_fds);

    fprintf(stdout, "%d\n", child_pid);

    return err;
}
