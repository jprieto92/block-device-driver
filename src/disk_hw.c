
/*
 *  disk_hw.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */


// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include "link.h"


// constants
#define TRANSFER_TIME  1000000
#define SEEK_TIME       500000


// global variables (stats)
long stats_time_lseek_acc  = 0 ;
long stats_time_transfer_acc = 0 ;
long stats_lseeks_acc      = 0 ;
long stats_requests_acc    = 0 ;


// ending the application
void handle_sigint()
{
        char fname[1024] ;

	printf("\t\t\tDISK_HW: SIGINT received and now exiting\n");

        sprintf(fname, "/%d_q_diskhw_req", getuid()) ;
	link_delete(fname);

        sprintf(fname, "/%d_q_diskhw_res", getuid()) ;
	link_delete(fname);

        printf("\nDISK_HW: Statistics:\n");
	printf("DISK_HW: TOTAL_SEEK_TIME_MICROSEC %ld\n",     stats_time_lseek_acc) ;
	printf("DISK_HW: TOTAL_TRANSFER_TIME_MICROSEC %ld\n", stats_time_transfer_acc) ;
	printf("DISK_HW: TOTAL_SEEKS %ld\n",                  stats_lseeks_acc) ;
	printf("DISK_HW: TOTAL_REQUESTS %ld\n",               stats_requests_acc) ;

	exit(0);
}

// main function
int main ( int argc, char *argv[] )
{
    char fname[1024] ;
    int  last_block, current_block ;
    int  sim_dsk ;
    int  ret, readed, written ;
    int  NUM_BLOCKS ;
    struct stat st ;

    mqd_t q_req, q_res ;
    disk_request current_request ;

    
    // To detect the number of disk blocks
    ret = stat("disk.dat", &st);
    if (ret < 0) {
        perror("stat:") ;
        return -1;
    }

    NUM_BLOCKS = st.st_size / 1024 ;


    // To handle Ctrl+C signal
    signal(SIGINT, handle_sigint);

    sprintf(fname, "/%d_q_diskhw_req", getuid()) ;
    link_delete(fname) ;
    ret = link_open(&q_req, fname,1, 0) ;
    if (ret < 0) return -1;

    sprintf(fname, "/%d_q_diskhw_res", getuid()) ;
    link_delete(fname) ;
    ret = link_open(&q_res, fname,1, 0) ;
    if (ret < 0) return -1;


    // To process requests
    last_block = 0 ;
    ret = link_receive(&q_req, &current_request) ;

    while (1)
    {
        current_block = current_request.block_num ;
	printf("\t\t\tDISK_HW: BLOCK_REQUESTED %d\n", current_block);

        // check errors + send back error
	if (current_block >= NUM_BLOCKS)
        {
	    printf("\t\t\tDISK_HW: ERROR on requested block %d -> greater than disk size %d\n", current_block, NUM_BLOCKS);
	    current_request.error = -1;
	    ret = link_send(&q_res, &current_request) ;
            continue;
	}

	// sleep: simulates access time
	usleep(SEEK_TIME*(abs(current_block - last_block)) + TRANSFER_TIME) ;

        stats_lseeks_acc        +=                 abs(current_block - last_block) ;
        stats_requests_acc      +=                 1 ;
        stats_time_lseek_acc    += SEEK_TIME     * abs(current_block - last_block) ;
        stats_time_transfer_acc += TRANSFER_TIME * 1 ;
	
	// send back data/ack
	if (READ == current_request.operation)
	{
		memset(&(current_request.buffer), 'x', BLOCK_SIZE) ;

		sim_dsk = open("disk.dat", O_RDWR);
		if (sim_dsk < 0) 
		{
		    fprintf(stderr, "DISK_HW: ERROR opening disk.dat\n");
		    return -1;
		}
		
		ret = lseek(sim_dsk, current_block*BLOCK_SIZE, SEEK_SET);

		readed = 0;
		do 
		{
		    ret = read(sim_dsk, 
			      &(current_request.buffer)+readed, 
			      BLOCK_SIZE-readed);
		    readed = readed + ret;
		} while (readed < BLOCK_SIZE && ret >= 0);

		close(sim_dsk) ;

		if (ret < 0) 
		     current_request.error = -1;
		else current_request.error =  0;

		printf("\t\t\tDISK_HW: BYTES_READED %d\n", readed);
		ret = link_send(&q_res, &current_request) ;
	}
	else if (WRITE == current_request.operation )
	{
		sim_dsk = open("disk.dat", O_RDWR);
		if (sim_dsk < 0) 
		{
		    fprintf(stderr, "DISK_HW: ERROR opening disk.dat \n");
		    return -1;
		}

		ret = lseek(sim_dsk, current_block*BLOCK_SIZE, SEEK_SET);

		written = 0;
		do 
		{
		    ret = write(sim_dsk, 
			      &(current_request.buffer)+written, 
			      BLOCK_SIZE-written);
		    written = written + ret;
		} while (written < BLOCK_SIZE && ret >= 0);

		close(sim_dsk) ;

		if (ret < 0) 
		     current_request.error = -1;
		else current_request.error =  0;
		
		printf("\t\t\tDISK_HW: BYTES_WRITTEN %d\n", written);
		ret = link_send(&q_res, &current_request) ;
	}
	    
	// receive next request
	last_block = current_block ;
	ret = link_receive(&q_req, &current_request) ;
    }

    return 1;
}

