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
#include "init.h"

void print_logs(const char* status, const char* msg) {
    printf("[%s] %s\n", status, msg);
}

int run_shell() {
    print_logs("INFO", "Running Bash");
    
    if (fork() == 0) {
        char* args[] = {"/bin/bash", NULL};
        execv("/bin/bash", args);

        print_logs("ERROR", "Failed to run bash, Trying sh");
        char* sh_args[] = {"/bin/sh", NULL};
        execv("/bin/sh", sh_args);
        print_logs("ERROR", "Failed to run sh, Exiting");
        exit(1);
    } else {
        int status;
        wait(&status);
        return WEXITSTATUS(status);
    }
}

void check_for_gui() {
	if (access("/usr/bin/startx", F_OK) == 0) {
		print_logs("INFO", "GUI detected, starting X server");
		if (fork() == 0) {
			char* args[] = {"/usr/bin/startx", NULL};
			execv("/usr/bin/startx", args);
			perror("Failed to start X server");
			exit(1);
		} else {
			wait(NULL);
		}
	} else {
		print_logs("INFO", "No GUI detected, running in console mode");
	}
}
void init(){
    print_logs("INFO", "Initializing system...");

    // Create essential directories
    const char* dirs[] = {"/dev", "/proc", "/sys", "/tmp", "/var/log"};
    for (int i = 0; i < (int)(sizeof(dirs) / sizeof(dirs[0])); i++) {
        if (mkdir(dirs[i], 0755) == -1 ) {
            perror("Failed to create directory");
    }
}

    // Create /var/run and /var/lock
    mkdir("/var/run", 0755);
    mkdir("/var/lock", 0755);
}