#include <stdio.h>
#include <sys/stat.h>

#define CGROUP_PATH "/sys/fs/cgroup/my_cgroup"

// creating a control group involves interacting directly with
// the linux filesystem. Read the below code
int createControlGroup() {
    if (mkdir(CGROUP_PATH, 0755) != 0) {
        return 1;
    }
    printf("Cgroup created at: %s\n", CGROUP_PATH);
    return 0;
}

void setControlGroupLimit(const char *filename, const char *value) {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s/%s", CGROUP_PATH, filename);

}