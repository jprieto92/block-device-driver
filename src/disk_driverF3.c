
/*
 *  disk_driver.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

/*
* JAVIER PRIETO CEPEDA   - 100307011
* MARIN  LUCIAN PRIALA   - 100303625
* SERGIO RUIZ   JIMENEZ  - 100303582
*/


#include "disk_driver.h"


#define FALSE 0
#define TRUE 1
#define FX 3
/* 
 * Elements available within pKernel
 */

int send_data_to_process   ( pid_t p_id, int operation, int block_id, char *block, int error ) ;
int request_data_to_device (             int operation, int block_id, char *block, int error ) ;
int is_disk_processing     ( void ) ;


/*
 * Elements to be implemented
 */
 
struct queue *peticiones;  // Cola de peticiones que llegan al disco

pthread_mutex_t mutex;     // pid_t peticion_en_curso;

struct request * req_actual;

int mount_driver ( void )
{
	/*Inicializamos la cola de peticiones que conendra queue_elements*/
	peticiones = queue_new();
	pthread_mutex_init(&mutex,NULL);

	return 1;
}

int unmount_driver ( void )
{
	/*Destruimos la cola*/
	queue_destroy(peticiones);
	pthread_mutex_destroy(&mutex);
    
	return 1;
}


int disk_driver_block_request ( pid_t p_id, int operation, int block_id, char *block, int error)
{
	request req;
	int ret;
	
	pthread_mutex_lock(&mutex);

	// busco petición para impachetabil...  	
	request *aux = find_request_impachetabil(peticiones, block_id, operation);
	if (   (aux != NULL) && 
	    (FX == 3) &&
	     ( (operation == READ) || ((operation == WRITE) && (aux->pid_process[0] != req_actual->pid_process[0])))
	   )
	{ // împachetabil...
		aux->pid_process[aux->pidOcupados] = p_id;
		aux->pidOcupados +=1;
		if (aux->operation == WRITE) 
       		   memcpy(aux->block, block, BLOCK_SIZE);
		printf("\tDRIVER: request re-enqueued, pid: %d, block_id: %d\n",p_id,block_id);
	} else { // encolar si no impachetabil
		req.operation = operation;
		req.block_id = block_id;
		memcpy(req.block, block,BLOCK_SIZE);
		req.pid_process[0] = p_id;
		req.pidOcupados = 1;
		req.error = error;

		if ((FX == 2) || (FX == 3)){
		peticiones = enqueue_ordered(peticiones, &req);
		printf("\tDRIVER: request o-enqueued, pid: %d, block_id: %d\n",p_id,block_id);
		}
		else if (FX == 1){
			peticiones = enqueue(peticiones, &req);
			printf("\tDRIVER: request enqueued, pid: %d, block_id: %d\n",p_id,block_id);
		}
	}
 
	if(is_disk_processing() == 0)
	{
		printf("\tDRIVER: request sent, pid: %d, block_id: %d\n",p_id,block_id);
 	        req_actual = find_request(peticiones, block_id, operation);
		ret = request_data_to_device(operation, block_id, block, error);
		pthread_mutex_unlock(&mutex);
		return ret;
	}
	
	pthread_mutex_unlock(&mutex);
	return 1;
}

int disk_driver_hardware_interrupt_handler( pid_t p_id, int operation, int block_id, char *block, int error )
{
	request *r1, *req;
	int i;
	
	pthread_mutex_lock(&mutex);

	/*Obtenemos la peticion que se ha resuelto*/
 	r1 = find_request(peticiones, block_id, operation);
	do {
		req = next_request(peticiones, r1);
		remove_request(peticiones, r1);
		printf("\tDRIVER: request dequeued, block_id: %d\n", block_id);

		/* mandar los datos */
		for (i=0; i<r1->pidOcupados; i++)
		{
			send_data_to_process(r1->pid_process[i], operation, block_id, block, error);
			printf("\tDRIVER: sending data, pid: %d, block_id: %d\n", r1->pid_process[i], block_id);
		}
		r1 = req;
	} while ((r1 != NULL) && (r1->operation == operation) && (r1->block_id == block_id)  &&  (FX == 3));
  
 	/*Si la cola no esta vacia asignamos trabajo en caso de estar libre el dispositivo*/
 	if( (NULL != req) && (is_disk_processing() == 0))
 	{
		req_actual = req;
 		request_data_to_device(req->operation, req->block_id, req->block, req->error);
		printf("\tDRIVER: pending sent, block_id: %d\n",req->block_id);
 	}
  
 	pthread_mutex_unlock(&mutex);
 	return 1;
}

