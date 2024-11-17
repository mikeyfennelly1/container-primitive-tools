#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define CGROUP_PATH "/sys/fs/cgroup/my_cgroup"

bool dirExists(const char *path) {
    struct stat info;

    if (stat(path, &info) != 0) {
        return false;
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        return false;
    }
}

int create_cgroup() {
    bool existsAlready = dirExists(CGROUP_PATH);
    if (!existsAlready) {
        if (mkdir(CGROUP_PATH, 0755) != 0) {
            perror("mkdir");
            exit(EXIT_FAILURE);
        }
        printf("Cgroup created at: %s\n", CGROUP_PATH);
    }
    return 0;
}

void set_cgroup_limit(const char *filename, const char *value) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", CGROUP_PATH, filename);
    int fd = open(filepath, O_WRONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (write(fd, value, strlen(value)) < 0) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    close(fd);
}

void assign_to_cgroup(pid_t pid) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/cgroup.procs", CGROUP_PATH);

    FILE *file = fopen(filepath, "w");
    if (!file) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d", pid);
    fclose(file);
    printf("Assigned process %d to the cgroup\n", pid);
}

// int main() {
//     int euid = geteuid();
//     if (euid != 0) {
//         perror("Elevated permissions required.");
//         exit(EXIT_FAILURE);
//     }
//     create_cgroup();
//
//     set_cgroup_limit("cpu.max", "100000 100000");
//
//     assign_to_cgroup(getpid());
//
//     printf("cgroup created, sim workload\n");
//     while (1) {
//         sleep(1000000);
//     }
//
//     return 0;
// }