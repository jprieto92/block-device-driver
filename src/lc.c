
/*
 *  lc.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

#include "lc.h"


int syscall_open ( mqd_t *q_req, mqd_t *q_res, int client_id )
{
    char q_name[1024] ;
    int ret ;

    sprintf(q_name, "/%d_q_client_req", getuid()) ;
    ret = link_open(q_req, q_name, 10, 0) ;
    if (ret < 0) return -1;

    sprintf(q_name, "/%d_%d", getuid(), getpid()) ;
    ret = link_open(q_res, q_name, 10, 0) ;
    if (ret < 0) return -1;

    return 1;
}

int syscall_write ( mqd_t *q_req, mqd_t *q_res, int client_id, int block_id, char *block )
{
    disk_request current_request ;
    int ret ;

    current_request.process_requester = client_id ;
    current_request.operation = WRITE ;
    current_request.block_num = block_id ;
    current_request.error = 0 ;
    
    /* memcpy block data to request */
    memcpy(current_request.buffer, block, BLOCK_SIZE);

    ret = link_send   (q_req, &current_request) ;
    if (ret < 0) return -1;

    ret = link_receive(q_res, &current_request) ;
    if (ret < 0) return -1;
    
    if (current_request.error != 0) return -1;

    if (current_request.error < 0) return -1;

    memcpy(block, current_request.buffer, BLOCK_SIZE) ;

    return 1;
}

int syscall_read ( mqd_t *q_req, mqd_t *q_res, int client_id, int block_id, char *block )
{
    disk_request current_request ;
    int ret ;

    current_request.process_requester = client_id ;
    current_request.operation = READ ;
    current_request.block_num = block_id ;
    current_request.error = 0 ;

    ret = link_send   (q_req, &current_request) ;
    if (ret < 0) return -1;

    ret = link_receive(q_res, &current_request) ;
    if (ret < 0) return -1;

    if (current_request.error < 0) return -1;

    memcpy(block, current_request.buffer, BLOCK_SIZE) ;

    return 1;
}

int syscall_close ( mqd_t *q_req, mqd_t *q_res )
{
    int ret ;
    char q_name[256];

    ret = link_close(q_req) ;
    if (ret < 0) return -1;

    ret = link_close(q_res) ;
    if (ret < 0) return -1;
    
    sprintf(q_name, "/%d_%d", getuid(), getpid()) ;
    link_delete(q_name) ;

    return 1;
}

