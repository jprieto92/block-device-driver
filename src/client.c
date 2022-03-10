
/*
 *  client.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "lc.h"


/*
 *  Simple Block API for clients
 */

void  OPEN_FILE   ( void ) ;
void  CLOSE_FILE  ( void ) ;
void *FILL_BLOCK  ( char *block, char byte ) ;
void  READ_BLOCK  ( char *block, int block_id ) ;
void  WRITE_BLOCK ( char *block, int block_id ) ;


/*
 *  Client
 */

int main ( int argc, char *argv[] )
{
    char   block[BLOCK_SIZE] ;
    int    tag_client;
    //int i ;


    /* 1) To check/get parameters... */
    if (argc > 2)
    {
        fprintf(stderr, "Usage: %s [client id]\n", argv[0]) ;
        exit(-1) ;
    }

    if (2 == argc)
         tag_client = atoi(argv[1]) ;
    else tag_client = 0 ;


    /* 2) To perform requests... */
    OPEN_FILE();
    FILL_BLOCK(block, 'b') ;
    if(tag_client == 0){
    	READ_BLOCK(block, 15) ;
    }
    
    if(tag_client % 2 == 0){
	usleep(10000);
    	WRITE_BLOCK(block, 15) ;
    }
    
    if(tag_client % 2 == 1){
	usleep(20000);
    	READ_BLOCK(block, 15) ;
    }
    
    if(tag_client % 2 == 0){
	usleep(30000);
    	WRITE_BLOCK(block, 15) ;
    }

    
       /*
       
          int a = 64;
          char b = (char) a;
    */
/*    for (i=0; i<5; i=i+1)
    {
         FILL_BLOCK(block, 'b') ;
         
	 if(tag_client % 2 == 1){
          WRITE_BLOCK(block, 0) ;
        }
         if (tag_client % 2){
           READ_BLOCK(block, 0) ;
           READ_BLOCK(block, 0) ;
           sleep(1) ;
         }
    }*/
    CLOSE_FILE() ;


    /* 3) Client ends */
    return 1 ;
}


/*
 *  Simple Block API IMPLEMENTATION for clients
 */

mqd_t  q_req, q_res ;

void *FILL_BLOCK ( char *block, char byte )
{
    return memset(block,byte,BLOCK_SIZE) ;
}

void  OPEN_FILE  ( void )
{
    int ret ;

    sleep(1) ; /* wait for pkernel to boot... */

    ret = syscall_open(&q_req, &q_res, getpid()) ;
    if (ret < 0) 
        exit(-1) ;
}

void  CLOSE_FILE  ( void )
{
    int ret ;

    ret = syscall_close(&q_req, &q_res) ;
    if (ret < 0) 
        exit(-1) ;

    printf("CLIENT %d: END\n", getpid()) ;
}

void  READ_BLOCK ( char *block, int block_id )
{
    int ret ;
    int id_client ;

    id_client = getpid() ;
    printf("CLIENT %d: request read block %d\n", id_client, block_id);

    ret = syscall_read(&q_req, &q_res, id_client, block_id, block) ;
    if (ret < 0)
	fprintf(stderr, "CLIENT %d: ERROR on syscall_read\n", id_client);

    printf("CLIENT %d: block content after read request: '%c' ... '%c'\n", id_client, block[0], block[BLOCK_SIZE-1]);
}

void  WRITE_BLOCK ( char *block, int block_id )
{
    int ret ;
    int id_client ;

    id_client = getpid() ;
    printf("CLIENT %d: request update block %d\n", id_client, block_id);

    ret = syscall_write(&q_req, &q_res, id_client, block_id, block) ;
    if (ret < 0)
	fprintf(stderr, "CLIENT %d: ERROR on syscall_write\n", id_client);

    printf("CLIENT %d: block content after write request: '%c' ... '%c'\n", id_client, block[0], block[BLOCK_SIZE-1]);
}

