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
#include "networking.h"
#include "init.h"

void start_networking() {
    // Create essential network devices
    system("ip link set lo up");
    system("dhclient eth0");
    const char* dev_paths[] = {"/dev/net", "/dev/net/tun"};
    mode_t modes[] = {0755, 0755};
    
    for (int i = 0; i < 2; i++) {
        if (mkdir(dev_paths[i], modes[i]) && i == 0) {
            perror("Failed to create /dev/net");
        }
    }
    
    if (mknod("/dev/net/tun", S_IFCHR | 0666, makedev(10, 200))) {
        perror("Failed to create tun device");
    }

    // Bring up loopback interface
    if (system("ip link set dev lo up") != 0) {
        fprintf(stderr, "Failed to bring up loopback\n");
    }

    // Configure network interfaces
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("/sys/class/net")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_LNK && 
                strcmp(ent->d_name, ".") != 0 && 
                strcmp(ent->d_name, "..") != 0 &&
                strcmp(ent->d_name, "lo") != 0) {
                
                // Bring interface up
                char cmd[256];
                snprintf(cmd, sizeof(cmd), "ip link set dev %s up", ent->d_name);
                if (system(cmd) != 0) {
                    fprintf(stderr, "Failed to bring up %s\n", ent->d_name);
                }

                // Try DHCP configuration
                snprintf(cmd, sizeof(cmd), "udhcpc -i %s -n -q", ent->d_name);
                if (system(cmd) != 0) {
                    fprintf(stderr, "DHCP failed for %s, using link-local\n", ent->d_name);
                    
                    // Fallback to link-local address
                    snprintf(cmd, sizeof(cmd), "ip addr add 169.254.0.1/16 dev %s", ent->d_name);
                    system(cmd);
                }
            }
        }
        closedir(dir);
    } else {
        perror("Cannot access network interfaces");
    }
}