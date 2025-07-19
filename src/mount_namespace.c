#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>

int main() {
  if (unshare(CLONE_NEWNS) == -1) {
	perror("unshare");
    exit(EXIT_FAILURE);
  }

  if (mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL) == -1) {
    perror("mount -- make private");
    exit(EXIT_FAILURE);
  }
  printf("New mount ns created. You can mount/unmount filesystems here.\n");

  if (mount("tmpfs", "/mnt", "tmpfs", 0, "") == -1) {
    perror("mount tmpfs");
    exit(EXIT_FAILURE);
  }

  printf("Mounted tmpfs at /mnt. Check /mnt directory. \n");

  return 0;
}