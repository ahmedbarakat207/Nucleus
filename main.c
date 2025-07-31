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
#include "mount.h"
#include "init.h"
#include "services.h"

int main() {
    init();
    mount_sys_dir();  
    create_essential_devices();    
	mount_tmpfs();
	mount_devpts();
    start_networking();
    setup_signal_handlers();
    handle_hotplug();
    watchdog_init();
    run_service("/sbin/udevd", (char*[]){"/sbin/udevd", "--daemon", NULL});  
	check_for_gui();
    return run_shell(); 
}
