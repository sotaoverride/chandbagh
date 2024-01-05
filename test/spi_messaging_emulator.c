#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(void);


static char buff_slave_rx[3] = {0xaa, 0xbb, 0xbb};
static char buff_slave_tx[3] = {0xca, 0xd5, 0xd5};
static char buff_master_rx[3] =  {0xdd, 0xee, 0xcc};
static char buff_master_tx[3] = {0x5a, 0xad, 0xfc};


static void * slave_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	read(fd, buff_slave_rx + start, bytes);
	return NULL;
}

static void * master_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	read(fd, buff_master_rx + start, bytes);
	return NULL;
}

static void * slave_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	write(fd, buff_slave_tx + start, bytes);
	return NULL;
}

static void * master_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	write(fd, buff_slave_tx + start, bytes);
	return NULL;
}
static master_read_write(void *pfd) {
	master_sockpair_write(pfd, 1, 0);
	master_sockpair_read(pfd, 1, 0);
	master_sockpair_write(pfd, 2, 1);
	master_sockpair_read(pfd, 2, 1);
	printf("0x%x", buff_master_rx[0]);
}
static slave_read_write(void *pfd) {
	slave_sockpair_write(pfd, 1, 0);
	slave_sockpair_read(pfd,1,0);
	slave_sockpair_write(pfd, 2, 1);
	slave_sockpair_read(pfd, 2,1);
	printf("0x%x", buff_slave_rx[0]);
}
static void _spi_messaging_test() {
	while(1){
	int fd[2];
	pthread_t master_thr;
	pthread_t slave_thr;

	printf("socketpair msg passing for messages\n");

	socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
	pthread_create(&master_thr, NULL, master_read_write, (void *)(&(fd[1])));
	pthread_create(&slave_thr, NULL, slave_read_write, (void *)(&(fd[0])));
	pthread_join(master_thr, NULL);
	pthread_join(slave_thr, NULL);
	close(fd[0]);
	close(fd[1]);
	}
}

int main(void){
	printf("0x%x", buff_master_tx[0]);
	_spi_messaging_test();
	return 0;
}
