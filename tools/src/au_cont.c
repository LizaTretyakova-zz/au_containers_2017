#define _GNU_SOURCE

#include "au_child.h"
#include "au_cont.h"
#include "au_init.h"
#include "au_utils.h"

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
    int err = 0;
    pid_t child_pid = 0;
    struct child_config config = {0};

    int pipe_fds[2] = {0};
    pipe2(pipe_fds, O_CLOEXEC);


    // config options
    fill_empty_config(&config);
    config.fd = pipe_fds[0];
    config.hostname = "container";
    process_arguments(&config, argc, argv);
    config.argv = argv + optind + 1;

    // create stack
    if (!(stack = (char*)malloc(CHILD_STACK_SIZE))) {
        perror("stack allocation failed");
        return error(pipe_fds);
    }

    // create child
    if ((child_pid = clone(child, stack + CHILD_STACK_SIZE, CLONE_FLAGS, &config)) == -1) {
        perror("clone");
        clear_resources(stack, pipe_fds);
        return 1;
    }
    if(close(pipe_fds[0])) {
        clear_resources(stack, pipe_fds);
        return 1;
    }
    pipe_fds[0] = 0;

    // set all the proper id's
    if(set_ids()) {
        clear_resources(stack, pipe_fds);
        return 1;
    }

    // setup workplace
    char child_pid_str[32];
    sprintf(child_pid_str, "%d", child_pid);
    execv("/test/aucont/src/au_setup_workspace.py",
          (char* []){"/test/aucont/src/au_setup_workspace.py",
            config.mount_dir,
            child_pid_str,
            NULL});
    strcpy(config.mount_dir, "/test/images/");
    strcat(config.mount_dir, child_pid_str);
    strcat(config.mount_dir, "/");

    // UID/GID mappings
    if (handle_child_uid_map(child_pid)) {
        err = 1;
        kill_and_finish_child(&err, child_pid);
        clear_resources(stack, pipe_fds);
        return err;
    }

    // cgroups and CPU usage
    if(strlen(config.cpu)) {
        execv("/test/aucont/src/au_setup_cpu.py",
              (char* []){"/test/aucont/src/au_setup_cpu.py",
              config.cpu,
              child_pid_str,
              NULL});
    }

    //network
    if(strlen(config.net)) {
        execv("/test/aucont/src/au_setup_network.py",
              (char* []){"/test/aucont/src/au_setup_network.py",
              config.net,
              child_pid_str,
              NULL});
    }

    write(pipe_fds[1], PROCEED, strlen(PROCEED));

    finish_child(&err, child_pid);
    clear_resources(stack, pipe_fds);

    fprintf(stdout, "%d\n", child_pid);

    return err;
}
