#define _GNU_SOURCE

#include "au_utils.h"
#include "au_cont.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void cleanup(int sockets[2]) {
    if (sockets[0]) {
        close(sockets[0]);
    }
    if (sockets[1]) {
        close(sockets[1]);
    }
}

int error(int sockets[2]) {
    cleanup(sockets);
    return 1;
}

void usage(char **argv) {
    fprintf(stderr, "Usage: %s -u -1 -m . -c /bin/sh ~\n", argv[0]);
}

void clear_resources(struct child_config* config, char* stack, int sockets[2]) {
    free_resources(config);
    free(stack);
    cleanup(sockets);
}

void finish_child(int *err, pid_t child_pid) {
    int child_status = 0;
    waitpid(child_pid, &child_status, 0);
    *err |= WEXITSTATUS(child_status);
}

void kill_and_finish_child(int* err, pid_t child_pid) {
    if (child_pid) {
        kill(child_pid, SIGKILL);
    }
    finish_child(err, child_pid);
}

int parse_parameters(int argc, char **argv, struct child_config* config) {
    int option = 0;
    int last_optind = 0;
    while ((option = getopt(argc, argv, "c:m:u:"))) {
        if(option == 'c') {
            config->argc = argc - last_optind - 1;
            config->argv = &argv[argc - config->argc];
            break;
        } else if(option == 'm') {
            config->mount_dir = optarg;
        } else if(option == 'u') {
            if (sscanf(optarg, "%d", &config->uid) != 1) {
                fprintf(stderr, "badly-formatted uid: %s\n", optarg);
                usage(argv);
                return 0;
            }
        } else {
            usage(argv);
            return 0;
        }
        last_optind = optind;
    }
    return 1;
}
