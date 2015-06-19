#include <stdio.h>
#include <unistd.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#define BAUDRATE B115200   // Change as needed, keep B
#define BAUDRATE 9600
#define UART "/dev/ttyO4" //Beaglebone Black serial port
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1


/* NOT SEGMENTFALUT */


int main(void) {

        /* Initialize the PRU */
        //printf(">> Initializing PRU\n");
        tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
        prussdrv_init();

        /* Open PRU Interrupt */
        if (prussdrv_open (PRU_EVTOUT_0)) {
                // Handle failure
                fprintf(stderr, ">> PRU open failed\n");
                return 1;
        }

        /* Get the interrupt initialized */
        prussdrv_pruintc_init(&pruss_intc_initdata);

        /* Get pointers to PRU local memory */
        void *pruDataMem;
        prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pruDataMem);
        unsigned int *pruData = (unsigned int *) pruDataMem;
        float sendData[10000];

        /* Execute code on PRU */
        //printf(">> Executing HCSR-04 code\n");
        prussdrv_exec_program(0, "hcsr04.bin");

        /* Get measurements */
        int i = 0;
        while (i++<10000) {

                // Wait for the PRU interrupt
                prussdrv_pru_wait_event (PRU_EVTOUT_0);
                prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);

                // Print the distance received from the sonar
                // At 20 degrees in dry air the speed of sound is 342.2 cm/sec
                // so it takes 29.12 us to make 1 cm, i.e. 58.44 us for a roundtrip of 1 cm
                printf("Distance = %f cm\n", (float) pruData[0] / 58.44);
                sendData[i] = pruData[0]/58.44;
                int count = 0;
                if(i>1){
                        if(sendData[i] < 4.8) {

                                int fd, c, res;
                                struct termios oldtio, newtio;
                                char buf[250];
				
			
				

                                int ret = system("echo uart4 > /sys/devices/bone_capemgr.9/slots | ./hcsr04");
                                fd = open(UART, O_RDWR | O_NOCTTY);

                                if (fd < 0) { perror(UART); exit(-1); }

                                bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

                                    /* BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
                                       CRTSCTS : output hardware flow control (only used if the cable has
                                                 all necessary lines. See sect. 7 of Serial-HOWTO)
                                       CS8     : 8n1 (8bit,no parity,1 stopbit)
                                       CLOCAL  : local connection, no modem contol
                                       CREAD   : enable receiving characters */
                                newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

                                    /* IGNPAR  : ignore bytes with parity errors
                                       otherwise make device raw (no other input processing) */
                                newtio.c_iflag = IGNPAR;

                                    /*  Raw output  */
                                newtio.c_oflag = 0;

                                    /* ICANON  : enable canonical input
                                       disable all echo functionality, and don't send signals to calling program */
                                newtio.c_lflag = ICANON;



                                
                                    /* now clean the modem line and activate the settings for the port */
                                tcflush(fd, TCIFLUSH);
                                tcsetattr(fd,TCSANOW,&newtio);
                                    // NMEA command to ouput all sentences
                                    // Note that this code & format values in manual are hexadecimal



                                sprintf(buf,"%s","aaa");
                                write(fd, buf, sizeof(char));
                                //    printf("%s",buf);

                                //   write(fd, buf, strlen(buf));
                                tcsetattr(fd, TCSANOW, &oldtio);
                        }
                }
                
                sleep(1);
        }

        prussdrv_exit();
        printf(">> PRU Disabled.\r\n");

        return (0);

}
