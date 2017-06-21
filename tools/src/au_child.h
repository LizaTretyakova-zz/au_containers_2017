#ifndef AU_CHILD
#define AU_CHILD

#include <sys/types.h>

int child(void *arg);
int handle_child_uid_map (pid_t child_pid, uid_t host_uid, gid_t host_gid);

#endif
