kill_and_finish_child:
    //    if (child_pid) {
    //        kill(child_pid, SIGKILL);
    //    }
finish_child:
    //    int child_status = 0;
    //    waitpid(child_pid, &child_status, 0);
    //    err |= WEXITSTATUS(child_status);
clear_resources:
    //    free_resources(&config);
    //    free(stack);
          
    //    goto cleanup;
usage:
    //    fprintf(stderr, "Usage: %s -u -1 -m . -c /bin/sh ~\n", argv[0]);
error:
    //    err = 1;
cleanup:
    //    if (sockets[0]) close(sockets[0]);
    //    if (sockets[1]) close(sockets[1]);

/* -*- compile-command: "gcc -Wall -Werror -lcap -lseccomp contained.c -o contained" -*- */

