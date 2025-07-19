#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int child_function(void *arg) {
    char new_hostname[] = "new_host";

    if (sethostname(new_hostname, sizeof(new_hostname)) == -1) {
        perror("sethostname");
        return 1;
    }

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        return 1;
    }
    execlp("/bin/bash", "bash", NULL);
    printf("Inside UTS namespace. Hostname: %s\n", hostname);
    sleep(10);
    return 0;
}

int main() {
    int euid = geteuid();
    if (euid != 0) {
        printf("Error: You are not root. EUID: %d\n", euid);
        exit(EXIT_FAILURE);
    }
    const int STACK_SIZE = 1024 * 1024;
    char *stack = malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    printf("parent process hostname before: ");
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("%s\n", hostname);
    }

    pid_t pid = clone(child_function, stack + STACK_SIZE, CLONE_NEWUTS | SIGCHLD, NULL);
    if (pid == -1) {
        perror("clone");
        free(stack);
        exit(1);
    }

    waitpid(pid, NULL,  0);
    printf("Parent process hostname after: ");
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("%s\n", hostname);
    }

    free(stack);
    return 0;
}