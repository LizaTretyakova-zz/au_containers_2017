#include "au_child.h"
#include "au_cont.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_MAX 32

int print_stats() {
    char tmp[64];
    if(gethostname(tmp, 64)) {
        perror("gethostname");
        return -1;
    }
    fprintf(stderr, "[AU_CHILD] hostname is set to: %s\n", tmp);
    fprintf(stderr, "[AU_CHILD] uid %d\n[AU_CHILD] euid %d\n[AU_CHILD] gid %d\n[AU_CHILD] egid %d\n",
            getuid(), geteuid(), getgid(), getegid());
    return 0;
}

int map_id(pid_t child_pid, unsigned int id, const char* type) {
    int uid_map = 0;
    char path[PATH_MAX] = {0};

    int res = sprintf(path, "/proc/%d/%s", child_pid, type);
    if(res < 0) {
        perror(type);
        return -1;
    }

    fprintf(stderr, "writing %s...\n", path);
    if ((uid_map = open(path, O_WRONLY)) == -1) {
        perror(type);
        return -1;
    }

    if (dprintf(uid_map, "0 %d 1\n", id) == -1) {
        perror(type);
        close(uid_map);
        return -1;
    }

    close(uid_map);
    return 0;
}

int handle_child_uid_map (pid_t child_pid, uid_t host_uid, gid_t host_gid) {
    int res;

    res = map_id(child_pid, host_uid, "uid_map");
    if(res < 0) {
        return res; // perror already done
    }
    res = map_id(child_pid, host_gid, "gid_map");
    if(res < 0) {
        return res;
    }

    return 0;
}

int daemonize() {
    int fd;
    pid_t sid = setsid();
    if(sid == (pid_t)-1) {
        perror("failed setsid");
        return -1;
    }
    fprintf(stderr, "[AU_CHILD] daemonize sid=%ud\n", sid);
    if(chdir("/") < 0) {
        perror("could not chdir in daemonize");
        return -1;
    }
    fprintf(stderr, "[AU_CHILD] daemonize changed root\n", sid);
    return 0;
}

int pivot_root(const char* new_root_path, const char* old_root_path) {
    return syscall(SYS_pivot_root, new_root_path, old_root_path);
}

int mounts(const char* new_root_path) {
    int len = strlen(new_root_path) + 3 + 1 + 1;
    char old_root_path[len];
    if(sprintf(old_root_path, "%s/old", new_root_path) < 0) {
        perror("error printing old_root_path");
        return -1;
    }
    fprintf(stderr, "Meow!\n");

    if(mkdir(old_root_path, 0777) < 0) {
        perror("mkdir couldn't create old root dir");
        fprintf(stderr, "old_root_path: %s\n%d %d %d %d\n",
                old_root_path, getuid(), geteuid(), getgid(), getegid());
        return -1;
    }
    if(mount(new_root_path, new_root_path, "bind", MS_BIND | MS_REC, NULL)) {
        perror("error mounting new_root_path");
        return -1;
    }
    if(pivot_root(new_root_path, old_root_path) < 0) {
        perror("pivot_root failed");
        return -1;
    }
    if(chdir("/") < 0) {
        perror("chdir failed");
        return -1;
    }
    if(chroot("/") < 0) {
        perror("chroot failed");
        return -1;
    }

    if(mount("nodev", "/proc", "proc", 0, NULL) < 0) {
        perror("could not moun /proc");
        return -1;
    }
    if(mount("nodev", "/sys", "sysfs", 0, NULL) < 0) {
        perror("could not moun /sys");
        return -1;
    }

    umount2("/old", MNT_DETACH);

    if(rmdir("/old") < 0) {
        perror("could not rm /old root");
        return -1;
    }
}

int child(void *arg)
{
    struct child_config *config = arg;
    char msg[32];
    if(read(config->fd, msg, sizeof(PROCEED)) < 0) {
        perror("error reading from pipe");
        return -1;
    }
    if(strcmp(PROCEED, msg)) {
        perror("child received incorrect message");
        exit(EXIT_FAILURE);
    }

    // to rule our little cruel word...
    setuid(0);
    seteuid(0);
    setgid(0);
    setegid(0);

    if(sethostname(config->hostname, strlen(config->hostname)) < 0) {
        perror("Child couldn't set hostname");
        exit(EXIT_FAILURE);
    }

    if(print_stats()) {
        return -1;
    }

    if(mounts(config->mount_dir) < 0) {
        return -1;
    }

    fprintf(stderr, "[AU_CHILD] config->daemonize=%d\n", config->daemonize);
    if(config->daemonize == 1) {
        if(daemonize() < 0) {
            exit(EXIT_FAILURE);
        }
    }

    if (execve(config->argv[0], config->argv, NULL)) {
        fprintf(stderr, "execve failed! %m.\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
