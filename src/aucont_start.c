#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

static const size_t CHILD_STACK_SIZE = 8 * 1024 * 1024;
static const int FLAGS = CLONE_NEWCGROUP 
                         | CLONE_NEWIPC 
                         | CLONE_NEWNS 
                         | CLONE_NEWNET 
                         | CLONE_NEWPID 
                         | CLONE_NEWUTS 
                         | CLONE_NEWUSER
                         | SIGCHLD;


int main(int argc, char *argv[]) {
    int res;
    char* img_path;
    void* stack;
/*
    // Process the flags
    int opt;
    while ((opt = getopt(argc, argv, "imnpuU")) != -1) {
        switch (opt) {
        }
    }
*/

    if (optind >= argci - 1) {
        fprintf(stderr, "IMAGE_PATH and/or CMD parameters are missing.\n");
        exit(1);
    }

    // Get the path to the image
    img_path = argv[optind];

    // Create new namespaces
    stack = malloc(CHILD_STACK_SIZE);
    stack = (void*)((char*)stack + CHILD_STACK_SIZE);
    res = clone(, stack, flags, );

    // Set hostname
    res = sethostname("container", 9);

    res = execv(argv[optind + 1], argv + optind + 1);
    if (res == -1) {
        perror("aucont_start -- execv");
        exit(1);
    }  
}
