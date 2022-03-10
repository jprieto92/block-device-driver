
/*
 *  create_disk.c
 *
 *  DrvSim. Version 2.0
 *
 *  INFODSO@ARCOS.INF.UC3M.ES
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main ( int argc, char *argv[] )
{
	
	char * dummy_block;
	
	if (argc != 3)
        {
		printf("ERROR: Incorrect number of arguments:\n");
		printf("Syntax: ./%s <block size> <number of blocks>\n", argv[0]);
		return -1;
	}

	int block_size = atoi(argv[1]);
	int num_blocks = atoi(argv[2]);

	int fd = open("disk.dat", O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (fd < 0)
        {
            perror("open:");
            return -1;
	}
	
	dummy_block = (char*)malloc(block_size);
        if (NULL == dummy_block)
        {
            perror("malloc:");
            return -1;
        }
	
	memset(dummy_block, '0', block_size);
	
	int i;
	int total_write, write_result;
	for (i = 0; i < num_blocks; i++)
        {
		total_write = 0;
		do {
			write_result = write(fd, dummy_block+total_write, block_size-total_write);
			total_write = total_write + write_result;
		} while (total_write < block_size && write_result >= 0);
	}
	
	free(dummy_block);
        close(fd);
	
	return 0;
}
