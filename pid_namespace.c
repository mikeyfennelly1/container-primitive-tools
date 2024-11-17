#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int child_function(void *arg) {
    printf("Inside a new namespace! PID: %d\n", getpid());
    execlp("/bin/bash", "bash", NULL);
    perror("execlp");
    sleep(100000);
    return 1;
}

int main() {
    int euid = geteuid();
    if (euid != 0) {
        printf("You need to be superuser. File ran by pid: %d\n", euid);
        exit(0);
    }
    const int STACK_SIZE = 1024 * 1024;
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    printf("Parent process PID: %d\n", getpid());

    pid_t pid = clone(child_function, stack + STACK_SIZE, CLONE_NEWPID | SIGCHLD, NULL);

    if (pid == -1) {
        perror("clone");
        free(stack);
        exit(1);
    }
    waitpid(pid, NULL, 0);
    free(stack);
    return 0;
}