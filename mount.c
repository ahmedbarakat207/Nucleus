#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         
#include <sys/stat.h>       
#include <sys/sysmacros.h>  
#include <sys/mount.h>      
#include <sys/wait.h> 
#include <dirent.h>
#include <string.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include "mount.h"
#include "init.h"

void mount_sys_dir() {
    print_logs("INFO", "Mounting SYS Directories '/dev', '/proc', '/sys'");

    if (mount("devtmpfs", "/dev", "devtmpfs", MS_NOSUID, NULL) == -1) {
        perror("Failed to mount /dev");
    }
    if (mount("proc", "/proc", "proc", MS_NOSUID|MS_NOEXEC|MS_NODEV, NULL) == -1) {
        perror("Failed to mount /proc");
    }
    if (mount("sysfs", "/sys", "sysfs", MS_NOSUID|MS_NOEXEC|MS_NODEV, NULL) == -1) {
        perror("Failed to mount /sys");
    }
}
void mount_tmpfs() {
	print_logs("INFO", "Mounting tmpfs on /tmp");

	if (mount("tmpfs", "/tmp", "tmpfs", MS_NOSUID|MS_NODEV, NULL) == -1) {
		perror("Failed to mount /tmp");
	}
}

void mount_devpts() {
	print_logs("INFO", "Mounting devpts on /dev/pts");

	if (mount("devpts", "/dev/pts", "devpts", MS_NOSUID|MS_NOEXEC, NULL) == -1) {
		perror("Failed to mount /dev/pts");
	}
}

void create_essential_devices() {
    // Create console device
    mknod("/dev/console", S_IFCHR | 0600, makedev(5, 1));
    
    // Create null device
    mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));
    
    // Create zero device
    mknod("/dev/zero", S_IFCHR | 0666, makedev(1, 5));
    
    // Create random devices
    mknod("/dev/random", S_IFCHR | 0666, makedev(1, 8));
    mknod("/dev/urandom", S_IFCHR | 0666, makedev(1, 9));

    for (int i = 1; i <= 6; i++) {
        char tty[20];
        sprintf(tty, "/dev/tty%d", i);
        mknod(tty, S_IFCHR | 0620, makedev(4, i));
        chown(tty, 0, 5); 
    }
}