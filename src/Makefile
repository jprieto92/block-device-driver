
#
# INFODSO@ARCOS.INF.UC3M.ES
#	nanokernel (v2.0) Makefile 
#


# flags
MAKEFLAGS=-k
CC=gcc
CFLAGS=-Wall -Werror -g -I.


# main rule
all: boot lc.a client pkernel disk_hw create_disk

# boot rule
boot: boot.o link.o
	$(CC) $(CFLAGS)  -o $@ $< link.o -lrt

# lc.a rule
lc.a: lc.o
	ar rv lc.a lc.o
	ranlib lc.a

# client rule
client: client.o lc.a link.o
	$(CC) $(CFLAGS)  -o $@ $< lc.a link.o -lrt

# disk_hw rule
disk_hw: disk_hw.o link.o
	$(CC) $(CFLAGS)  -o $@ $< link.o -lrt

# pkernel rule
pkernel: pkernel.o disk_driver.o queue.o link.o
	$(CC) $(CFLAGS)  -o $@ $< disk_driver.o queue.o link.o -lrt
	
# create_disk rule
create_disk: create_disk.o
	$(CC) $(CFLAGS) -o $@  $<

# .c -> .o rule
%.o :: %.c 
	$(CC) $(CFLAGS)  -c -fPIC -o $@ $<
	

# clean rule
clean:
	rm -f *.o 
	rm -f boot
	rm -f lc.a
	rm -f client
	rm -f pkernel
	rm -f disk_hw
	rm -f create_disk

