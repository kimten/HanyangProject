#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h> 
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

static double *distance;
 
int main(void)
{
    distance = mmap(NULL, sizeof *distance, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
 
    *distance = 0;
     float f = 0.0;
int fd;
  fd = open("/sys/class/gpio/export",O_WRONLY);                   
                write(fd,"68",2);                                               
                close(fd);                                                      
    if (fork() == 0) {
        float before = 0.0;
        while (1) {
		shut68_on();
		sleep(1);
		shut68_off();
		sleep(1);	
                }
        }
     else {
        while (1) {
                f  += 1;
                *distance = f;
                sleep(1);
        }
        munmap(distance, sizeof *distance);
    }
    return 0;
}
void shut68_on()                                                                
{                                                                               
                                                                                
                int fd = open("/sys/class/gpio68/direction", O_WRONLY);         
                write(fd,"out",3);                                              
                close(fd);                                                      
                                                                                
                fd = open("/sys/class/gpio/gpio68/value",O_WRONLY);             
                write(fd,"1",1);                                                
                close(fd);                                                      
}                                                                               
void shut68_off()                                                               
{                                                                               
                                                                                
                int fd = open("/sys/class/gpio68/direction", O_WRONLY);         
                write(fd,"out",3);                                              
                close(fd);                                                      
                                                                                
                fd = open("/sys/class/gpio/gpio68/value",O_WRONLY);             
                write(fd,"0",1);                                                
                close(fd);                                                      
}                                                                               
