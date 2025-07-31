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
#include "services.h"
#include <sys/reboot.h>
#include <pthread.h>

void system_halt() {
    kill(-1, SIGTERM);
    sleep(1);
    kill(-1, SIGKILL);
    sleep(1);

    umount("/run");
    umount("/dev");
    umount("/sys");
    umount("/proc");
    umount("/tmp");
    umount("/var/log");
    sync();
    sleep(1);
    reboot(RB_POWER_OFF);
}

void start_login() {
    execl("/bin/login", "login", "-f", "root", (char *)0);
    // If login fails, power off the system
    system_halt();
}

void run_service(const char* path, char* const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execv(path, argv);
        exit(1);
    } else if (pid < 0) {
        perror("Failed to fork service");
    } else {
        int status;
        waitpid(pid, &status, 0);   
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Service %s exited with status %d\n", path, WEXITSTATUS(status));
        }   
    }
}

void handle_term_signal(int sig) {
    (void)sig; // Suppress unused parameter warning
    sync();
    reboot(RB_POWER_OFF);
}

void setup_signal_handlers() {
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, handle_term_signal);
    signal(SIGHUP, SIG_IGN);
}


void handle_hotplug() {
    system("udevadm monitor --subsystem-match=usb");
}

void* watchdog_keepalive(void *arg) {
    int fd = *(int*)arg;
    while (1) {
        write(fd, "\0", 1);  // Keepalive ping
        sleep(10);
    }
    return NULL;
}

void watchdog_init(int sig) {
    int wd_fd = open("/dev/watchdog", O_WRONLY);
    if (wd_fd > 0) {
        // Start watchdog thread
        pthread_t thread;
        pthread_create(&thread, NULL, watchdog_keepalive, &wd_fd);
    }
}

