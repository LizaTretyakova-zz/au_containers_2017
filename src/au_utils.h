#ifndef AU_UTILS
#define AU_UTILS

#include "au_cont.h"
#include <sched.h>
#include <sys/types.h>
#include <unistd.h>

void cleanup(int sockets[2]);
int error(int sockets[2]);
void usage(char **argv);
void clear_resources(struct child_config* config, char* stack, int sockets[2]);
void finish_child(int *err, pid_t child_pid);
void kill_and_finish_child(int* err, pid_t child_pid);

#endif
