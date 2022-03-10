
#ifndef _DISK_DRIVER_H
#define _DISK_DRIVER_H

	/* 
         * Includes
         */

	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
        #include <pthread.h>

	#include "link.h"
	#include "queue.h"
	

	/* 
         * Functions 
         */

	int mount_driver();
	int unmount_driver();

        int disk_driver_block_request              ( pid_t p_id, int operation, int block_id, char *block, int error ) ;
        int disk_driver_hardware_interrupt_handler ( pid_t p_id, int operation, int block_id, char *block, int error ) ;

#endif

