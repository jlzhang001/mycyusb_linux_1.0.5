/************************************************************************************************
\***********************************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../include/cyusb.h"

/********** Cut and paste the following & modify as required  **********/
static const char * program_name;
static const char *const short_options = "hvt:";
static const struct option long_options[] = {
		{ "help",	0,	NULL,	'h'	},
		{ "version",	0,	NULL,	'v'	},
		{ "timeout",    1,      NULL,   't',	},
		{ NULL,		0,	NULL,	 0	}
};

static int next_option;

static void print_usage(FILE *stream, int exit_code)
{
	fprintf(stream, "Usage: %s options\n", program_name);
	fprintf(stream, 
		"  -h  --help           Display this usage information.\n"
		"  -v  --version        Print version.\n"
		"  -t  --timeout	timeout in seconds, 0 for indefinite wait.\n");

	exit(exit_code);
}
/***********************************************************************/

static FILE *fp = stdout;
static int timeout_provided;
static int timeout = 0;
static cyusb_handle *h1 = NULL;

static void validate_inputs(void)
{
	if ( (timeout_provided) && (timeout < 0) ) {
	   fprintf(stderr,"Must provide a positive value for timeout in seconds\n");
	   print_usage(stdout, 1);
	}   
}

static void *reader(void *arg1)
{
	printf("\nThis is reader!!!\n");
	int r;
	unsigned char buf[512];
	int transferred = 0;

	int fd_infile;
	fd_infile = open("/home/jlzhang/Cypress/FX3_SDK_Linux_v1.3.4/mycyusb_linux_1.0.5/src/infile.txt",O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR); 
        if (-1 == fd_infile) //
           {
	      printf("open file failed!!!");	   
              return (void*)-1 ; 
           }   

	memset(buf,'\0',512);
	while (1) {
		r = cyusb_bulk_transfer(h1, 0x81, buf, 512, &transferred, timeout * 1000);
		if ( r == 0 ) {
		   printf("%s \n", buf);
                   r = write(fd_infile, buf, transferred);
                   if (r < 0)
                       printf ("writefile_error returned %d\n", r);

		   memset(buf,'\0',512);
		   continue;
		}
		else {
			cyusb_error(r);
			cyusb_close();
			return NULL;
		}

	} 
	close(fd_infile);

}

static void * writer(void *arg2)
{
	printf("\n This is writer!!!\n");
	int r, nbr;
	unsigned char buf[512];
	int transferred = 0;
        int fd_outfile;

	fd_outfile = open("/home/jlzhang/Cypress/FX3_SDK_Linux_v1.3.4/mycyusb_linux_1.0.5/src/outfile.txt", O_RDONLY);
	printf("fd_outfile=%d\n", fd_outfile);
	if ( fd_outfile < 0 ) {
             printf("open file failed!!!");
	     return (void *) -1;
	}

	memset(buf,'\0',512);
	//while ( nbr = read(0,buf,512) ) {
	while ( nbr = read(fd_outfile,buf,512) ) {
		printf("nbr= %d \n", nbr);
		r = cyusb_bulk_transfer(h1, 0x01, buf, nbr, &transferred, timeout * 1000);
                printf("\n cyusb_bulk_transfer \n");

		if ( r == 0 ) {
			memset(buf,'\0',512);
			continue;
		}
		else {
			cyusb_error(r);
			cyusb_close();
			return NULL;
		}
	} 
        close(fd_outfile);
}


int main(int argc, char **argv)
{
	int r;
	char user_input = 'n';
	pthread_t tid1, tid2;

	program_name = argv[0];
	
	while ( (next_option = getopt_long(argc, argv, short_options, 
					   long_options, NULL) ) != -1 ) {
		switch ( next_option ) {
			case 'h': /* -h or --help  */
				  print_usage(stdout, 0);
			case 'v': /* -v or --version */
				  printf("%s (Ver 1.0)\n",program_name);
				  printf("Write by Zhang Jianli@NAOC\n");
				  exit(0);
			case 't': /* -t or --timeout  */
				  timeout_provided = 1;
				  timeout = atoi(optarg);
				  break;
			case '?': /* Invalid option */
				  print_usage(stdout, 1);
			default : /* Something else, unexpected */
				  abort();
		}
	} 

	validate_inputs();

	r = cyusb_open();
	if ( r < 0 ) {
	   printf("Error opening library\n");
	   return -1;
	}
	else if ( r == 0 ) {
		printf("No device found\n");
		return 0;
	}
	if ( r > 1 ) {
		printf("More than 1 devices of interest found. Disconnect unwanted devices\n");
		return 0;
	}
	h1 = cyusb_gethandle(0);
	if ( cyusb_getvendor(h1) != 0x04b4 ) {
	  	printf("Cypress chipset not detected\n");
		cyusb_close();
	  	return 0;
	}
	r = cyusb_kernel_driver_active(h1, 0);
	if ( r != 0 ) {
	   printf("kernel driver active. Exitting\n");
	   cyusb_close();
	   return 0;
	}
	r = cyusb_claim_interface(h1, 0);
	if ( r != 0 ) {
	   printf("Error in claiming interface\n");
	   cyusb_close();
	   return 0;
	}
	else printf("Successfully claimed interface\n");
	r = pthread_create(&tid1, NULL, reader, NULL);
	r = pthread_create(&tid2, NULL, writer, NULL);
	while ( 1) {
		pause();
	}
	cyusb_close();
	return 0;
}
