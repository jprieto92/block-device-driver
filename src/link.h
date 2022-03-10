
#ifndef _LINK_H
#define _LINK_H

	/* 
         * Includes
         */
	#include <stdio.h>
	#include <stdlib.h>
	#include <signal.h>
	#include <string.h>
	#include <sys/types.h> 
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <dlfcn.h> 
	#include <sys/mman.h>
	#include <mqueue.h> 


	/* 
         * Constants 
         */
	#define READ 0
	#define WRITE 1

	#define BLOCK_SIZE 1024

	#define MAX_PROCESS_PER_REQUEST 10


	/* 
         * Datatypes 
         */

	/* Structure to send/receive disk requests */
	typedef struct disk_request 
        {
		pid_t process_requester; /* PID of process requester */
		int operation;		 /* READ or WRITE */
		int block_num;		 /* Block to read/write */
		char buffer[BLOCK_SIZE]; /* Block contents */
		int error;
	} disk_request;


	/* 
         * Functions 
         */

	/* Open & close */
        int link_open   ( mqd_t *q_d, char *q_name, int max_messages, int block ) ;
	int link_close  ( mqd_t *q_d ) ;
	int link_delete ( char * q_name ) ;

	/* Send & receive */
	int link_send    ( mqd_t *q_d, disk_request *current_request ) ;
	int link_receive ( mqd_t *q_d, disk_request *current_request ) ;

#endif 

