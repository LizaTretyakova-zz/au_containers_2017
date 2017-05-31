#ifndef AU_UTILS
#define AU_UTILS

void cleanup(int sockets[2]);
int error(int sockets[2]);
void usage(char **argv);
void clear_resources(struct child_config* config, char* stack);
void finish_child(int *err, pid_t child_pid);
void kill_and_finish_child(int* err, pid_t child_pid);

#endif
