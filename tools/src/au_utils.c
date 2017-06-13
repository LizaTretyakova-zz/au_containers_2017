#define _GNU_SOURCE

#include "au_utils.h"
#include "au_cont.h"

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void cleanup(int pipe_fds[2]) {
    if (pipe_fds[0]) {
        close(pipe_fds[0]);
    }
    if (pipe_fds[1]) {
        close(pipe_fds[1]);
    }
}

int error(int pipe_fds[2]) {
    cleanup(pipe_fds);
    return 1;
}

void usage(char **argv) {
    fprintf(stderr, "Usage: %s -u -1 -m . -c /bin/sh ~\n", argv[0]);
}

void clear_resources(char* stack, int pipe_fds[2]) {
    free(stack);
    cleanup(pipe_fds);
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

void fill_empty_config(struct child_config* config) {
    memset(config, 0, sizeof(struct child_config));
    config->fd = -1;
    config->hostname = NULL;
    config->argv = NULL;
    config->daemonize = 0;
}

void process_arguments(struct child_config* config, int argc, char** argv) {
    int c;
    int idx = 0;
    static struct option long_options[] = {
        {"net",     required_argument, 0,  'n' },
        {"cpu",     required_argument, 0,  'c' },
        {0,         0,                 0,  0 }
    };
    while((c = getopt_long(argc, argv, "d", long_options, &idx)) != -1) {
        switch (c) {
            case 'd':
                config->daemonize = 1;
                break;
            case 'n':
                strcpy(config->net, optarg);
                break;
            case 'c':
                strcpy(config->cpu, optarg);
                break;
        }
    }
    strcpy(config->mount_dir, argv[optind]);
}
