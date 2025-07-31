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

void print_logs(const char* status, const char* msg);
void init();
void check_for_gui();
int run_shell();
