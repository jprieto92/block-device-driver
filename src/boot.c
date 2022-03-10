
/*
 *  boot.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mqueue.h>

int main ( int argc, char *argv[] )
{
    pid_t driver_id, diskhw_id, client_id ;
    int   i, j, n_clients;
    int   status;
    int  *children;
    int   wait_return;
    char  argv_name[1024] ;
    char  argv_1[1024] ;
    char *argv_clients[4] ;


    /* 1) check/get parameters... */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <number of_clients>\n", argv[0]) ;
        exit(-1) ;
    }

    n_clients = atoi(argv[1]);
    children = malloc(n_clients * sizeof(int));


    /* 2) boot disk (simulation) */
    printf("BOOT: initializing the disk hardware...\n");
    diskhw_id = fork() ;
    switch (diskhw_id)
    {
        case -1: // error
                 perror("ERROR in fork") ;
                 exit(-1) ;
                 break ;

        case  0: // son
		 sprintf(argv_name, "disk_hw") ;
                 argv_clients[0] = argv_name ;
                 argv_clients[1] = NULL ;

                 execvp("./disk_hw", argv_clients);
                 perror("execvp: ") ;
                 exit(-1) ;
                 break ;

        default: // father
                 break ;
    }

    /* 3) boot pkernel (simulation) */
    printf("BOOT: initializing the pkernel...\n");

    sleep(1) ;
    driver_id = fork() ;
    switch (driver_id)
    {
        case -1: // error
                 perror("ERROR in fork") ;
                 exit(-1) ;
                 break ;

        case  0: // son
		 sprintf(argv_name, "pkernel") ;

                 argv_clients[0] = argv_name ;
                 argv_clients[1] = NULL ;

                 execvp("./pkernel", argv_clients);
                 perror("execvp: ") ;
                 exit(-1) ;
                 break ;
    }


    /* 4) start up clients (simulation) */
    printf("BOOT: creating the clients\n");
    
    // Create as clients as wanted
    for(i = 0; i < n_clients; i++)
    {
	client_id = fork() ;
	children[i] = client_id;
	switch (client_id)
	{
	    case -1: // error
		      perror("ERROR in fork") ;
		      exit(-1) ;
		      break ;

	    case  0: // son
		      sprintf(argv_name, "client") ;
		      sprintf(argv_1,    "%d", i) ;

		      argv_clients[0] = argv_name ;
		      argv_clients[1] = argv_1 ;
		      argv_clients[2] = NULL ;

		      execvp("./client", argv_clients);
		      perror("execvp: ") ;
		      exit(-1) ;
		      break ;

	    default: // father
		      break ;
	}
    }

    /* 5) Wait for all processes */
    for(j=0; j<n_clients; j++)
    {
	wait_return = waitpid(children[j], &status, 0);
	printf("BOOT: process %d returned %d, status %d\n", children[j], wait_return, status);
    }
    kill(diskhw_id, SIGINT);
    kill(driver_id, SIGINT);


    /* 6) end */
    return 1;
}

