#ifndef AU_UTILS
#define AU_UTILS

#include "au_cont.h"
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

void cleanup(int pipe_fds[2]);
int error(int pipe_fds[2]);
void usage(char **argv);
void clear_resources(char* stack, int pipe_fds[2]);
void finish_child(int *err, pid_t child_pid);
void kill_and_finish_child(int* err, pid_t child_pid);
void fill_empty_config(struct child_config* config);
void process_arguments(struct child_config* config, int argc, char** argv);

#endif
