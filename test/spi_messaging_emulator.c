#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
static char buff_slave_rx[3];
static char buff_slave_tx[3];
static char buff_master_rx[3];
static char buff_master_tx[3];


static void * slave_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	long index;
	for (index=0; index < bytes; index++) {
		read(fd, buff_slave_rx[index+start], 1);
	}
	return NULL;
}

static void * master_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	long index;
	for (index =0; index < bytes; index++){

		read(fd, buff_master_rx[index+start], 1);
	}
	return NULL;
}

static void * slave_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	long index;
	for (index=0; index < bytes; index++) {
		write(fd, buff_slave_tx[index+1], 1);
	}
	return NULL;
}

static void * master_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	long index;
	for (index=0; index < bytes; index++) {
		write(fd, buff_master_tx[index+start], 1);
	}
	return NULL;
}
static request_fw(void *pfd) {
	master_sockpair_write(pfd, 1, 0);
	master_sockpair_read(pfd,1,0);
	master_sockpair_write(pfd, 2, 1);
	master_sockpair_read(pfd, 2, 1);
}
static response_fw(void *pfd) {
	slave_sockpair_write(pfd, 1, 0);
	slave_sockpair_read(pfd,1,0);
	slave_sockpair_write(pfd,2,1);
	slave_sockpair_read(pfd, 2, 1);
	
}
static void _spi_messaging_test() {
	int fd[2];
	pthread_t master_thr;
	pthread_t slave_thr;

	printf("socketpair msg passing for messages\n");

	socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	pthread_create(&master_thr, NULL, request_fw, (void *)(&(fd[0])));
	pthread_create(&slave_thr, NULL, response_fw, (void *)(&(fd[1])));
	close(fd[0]);
	close(fd[1]);
}
