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


static void * slave_sockpair_read(void *pfd, int bytes) {
	int fd = *((int *)pfd);
	long index;
	for (index=1; index < bytes; index++) {
		read(fd, buff_slave_rx[index-1], 1);
	}
	return NULL;
}

static void * master_sockpair_read(void *pfd, int bytes) {
	int fd = *((int *)pfd);
	long index;
	for (index =1; index < bytes; index++){

		read(fd, buff_master_rx[index-1], 1);
	}
	return NULL;
}

static void * slave_sockpair_write(void *pfd, int bytes) {
	int fd = *((int *)pfd);
	long index;
	for (index=1; index < bytes; index++) {
		write(fd, buff_slave_tx[index], 1);
	}
	return NULL;
}

static void * master_sockpair_write(void *pfd, int bytes) {
	int fd = *((int *)pfd);
	long index;
	for (index=1; index < bytes; index++) {
		write(fd, buff_master_tx[index], 1);
	}
	return NULL;
}

static void _spi_messaging_test() {
	int fd[2];
	pthread_t master_read_thr;
	pthread_t slave_read_thr;
	pthread_t master_write_thr;
	pthread_t slave_write_thr;

	printf("socketpair msg passing for messages\n");

	socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	pthread_create(&master_read_thr, NULL, master_sockpair_read, (void *)(&(fd[0])));
	pthread_create(&slave_read_thr, NULL, slave_sockpair_read, (void *)(&(fd[1])));
	pthread_create(&master_write_thr, NULL, master_sockpair_write, (void *)(&(fd[0])));
	pthread_create(&slave_write_thr, NULL, slave_sockpair_write, (void *)(&(fd[1])));
	close(fd[0]);
	close(fd[1]);
}
