#include "au_cont.h"

#include <stdio.h>
#include <sys/mount.h>
#include <sys/syscall.h>
#include <unistd.h>

static const int PATH_MAX = 32;

int map_id(pid_t child_pid, unsigned int id, const char* type) {
    int uid_map = 0;
    char path[PATH_MAX] = {0};

    int res = sprintf(path, "/proc/%d/%s", child_pid, type);
    if(res < 0) {
        perror(type);
        return -1;
    }

    fprintf(stderr, "writing %s...", path);
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

int handle_child_uid_map (pid_t child_pid) {
    uid_t host_uid = getuid();
    gid_t host_gid = getgid();
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
    if(chdir("/") < 0) {
        perror("could not chdir in daemonize");
        return -1;
    }
    return 0;
}

int pivot_root(const char* new_root_path, const char* old_root_path) {
    return syscall(SYS_pivot_root, new_root_path, old_root_path);
}

int mounts(const char* new_root_path) {
    int len = strlen(new_root_path) + 3 + 1;
    char old_root_path[len];
    if(sprintf(old_root_path, "%s/old", new_root_path) < 0) {
        perror("error printing old_root_path");
        return -1;
    }
    if(mkdir(old_root_path, 0777) < 0) {
        perror("mkdir couldn't create old root dir");
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

    // to rule our little cruel word...
    setuid(0);
    seteuid(0);
    setgid(0);
    setegid(0);

    if(sethostname(config->hostname, strlen(config->hostname)) < 0) {

    }
    if(mounts(config->mount_dir) < 0) {
        return -1;
    }

    if(config->daemonize == 1) {
        daemonize();
    }

    if (execve(config->argv[0], config->argv, NULL)) {
        fprintf(stderr, "execve failed! %m.\n");
        return -1;
    }
    return 0;
}