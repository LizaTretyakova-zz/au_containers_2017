#define _GNU_SOURCE

#include "au_utils.h"
#include "au_cont.h"

#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <sched.h>
#include <seccomp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/capability.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <linux/capability.h>
#include <linux/limits.h>

//<<capabilities>>

//<<mounts>>

//<<syscalls>>

//<<resources>>

//<<child>>

int main (int argc, char **argv)
{
    char* stack = NULL;

    struct child_config config = {0};
    int err = 0;
    int sockets[2] = {0};
    pid_t child_pid = 0;

// config options
    int res = parse_parameters(argc, argv);
    if (res == 0 || !config.argc || !config.mount_dir) {
        usage(argv);
        return error(sockets);
    }
    config.hostname = "container";

// <<namespaces>>
    // Do I need a socketpair?
    if (socketpair(AF_LOCAL, SOCK_SEQPACKET, 0, sockets)) {
        perror("socketpair");
        return error(sockets);
    }
    if (fcntl(sockets[0], F_SETFD, FD_CLOEXEC)) {
        perror("fcntl");
        return error(sockets);
    }
    config.fd = sockets[1];
    if (!(stack = (char*)malloc(STACK_SIZE))) {
        perror("stack allocation failed");
        return error(sockets);
    }
    if (resources(&config)) {
        err = 1;
        clear_resources(&config, stack);
        return err;
    }
    if ((child_pid = clone(child, stack + CHILD_STACK_SIZE, 
                            flags | SIGCHLD, &config)) == -1) {
        perror("clone");
        err = 1;
        clear_resources(&config, stack);
        return err;
    }
    close(sockets[1]);
    sockets[1] = 0;
    if (handle_child_uid_map(child_pid, sockets[0])) {
        err = 1;
        kill_and_finish_child(&err, child_pid);
        return err;
    }

    finish_child(&err, child_pid);
    return err;
}
