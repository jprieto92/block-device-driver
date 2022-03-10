
/*
 *  link.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

#include "link.h"


/*
 * Open & close
 */
int link_open ( mqd_t *q_d, char *q_name, int max_messages, int block )
{
	struct mq_attr q_attr_init;

        memset(&q_attr_init, 0, sizeof(q_attr_init)) ;
	q_attr_init.mq_maxmsg  = max_messages;
	q_attr_init.mq_msgsize = sizeof(disk_request);
	*q_d = mq_open(q_name, O_RDWR| O_CREAT | block, 0600, &q_attr_init);
	if (*q_d < 0)
        {
		perror("link qkernel<->client: ");
		return -1;
	}

        return 1 ;
}

int link_close ( mqd_t *q_d )
{
	mq_close(*q_d);

        return 1;
}

int link_delete( char * q_name )
{
	return mq_unlink(q_name);
}


/*
 * Send & receive
 */
int link_send ( mqd_t *q_d, disk_request *current_request )
{
	int send_res ;

	send_res = mq_send(*q_d, (char*)current_request, sizeof(disk_request), 0);
	if (send_res < 0)
        {
		perror("link qkernel<->client: ");
		return -1;
	}

        return 1;
}

int link_receive ( mqd_t *q_d, disk_request *current_request )
{
        long flags ;
	int recv_res ;

	recv_res = mq_receive(*q_d, (char *)current_request, sizeof(disk_request), 0);
	if (recv_res < 0)
        {
                flags = fcntl(*q_d, F_GETFL, 0) ;
                if (! (flags & O_NONBLOCK) ) {
		     perror("link qkernel<->client: ");
                }
		return -1;
	} 

        return 1;
}

