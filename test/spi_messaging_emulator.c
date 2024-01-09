#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdbool.h>
int main(void);


static unsigned char buff_slave_rx[3] = {0xaa, 0xbb, 0xbb};
static unsigned char buff_slave_tx[3] = {0xca, 0xd5, 0xd5};
static unsigned char buff_master_rx[3] =  {0xdd, 0xee, 0xcc};
static unsigned char buff_master_tx[3] = {0x5a, 0xad, 0xfc};
volatile static bool gpio22 = false;

static void * slave_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	int rc = read(fd, buff_slave_rx + start, bytes);
	if (rc == -1) if (errno == EAGAIN || errno == EWOULDBLOCK) printf("read failed %d \n", rc);
	return NULL;
}

static void * master_sockpair_read(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	int rc = read(fd, buff_slave_rx + start, bytes);
	if (rc == -1) if (errno == EAGAIN || errno == EWOULDBLOCK) printf("read failed %d \n", rc);
	return NULL;
}

static void * slave_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	int rc = write(fd, buff_slave_tx + start, bytes);
	if (rc == -1) printf("write error return code  %d \n", errno);
	return NULL;
}

static void * master_sockpair_write(void *pfd, int bytes, int start) {
	int fd = *((int *)pfd);
	int rc = write(fd, buff_slave_tx + start, bytes);
	if (rc == -1) printf("write error return code  %d \n", errno);
	return NULL;
}
static void * master_read_write(void *pfd) {
	while(1){
	gpio22 =false;
	master_sockpair_write(pfd, 1, 0);
	master_sockpair_read(pfd, 1, 0);
	gpio22 = true;
	printf("0x%x", buff_master_rx[0]);
	gpio22 = false;
	master_sockpair_write(pfd, 2, 1);
	master_sockpair_read(pfd, 2, 1);
	gpio22 = true;
	//should read 0xd5 0xd5
	printf("0x%x", buff_master_rx[1]);
	printf("0x%x \n", buff_master_rx[2]);
	}
	return NULL;
}
static void * slave_read_write(void *pfd) {
	while(1){
	while(!gpio22){}
	while(gpio22){}
	
	slave_sockpair_write(pfd, 1, 0);
	slave_sockpair_read(pfd,1,0);
	//should read 0xca
	if (buff_slave_rx[0] == 0xca){
	printf("0x%x", buff_slave_rx[0]);
	slave_sockpair_write(pfd, 2, 1);
	slave_sockpair_read(pfd, 2,1);
	}
	//should read 0xad 0xfc
	
	printf("0x%x", buff_slave_rx[1]);
	printf("0x%x \n", buff_slave_rx[2]);
	}
	return NULL;
}
static void _spi_messaging_test() {
	int fd[2];
	pthread_t master_thr;
	pthread_t slave_thr;
	socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, fd);
	pthread_create(&master_thr, NULL, master_read_write, (void *)(&(fd[1])));
	pthread_create(&slave_thr, NULL, slave_read_write, (void *)(&(fd[0])));
	pthread_join(master_thr, NULL);
	pthread_join(slave_thr, NULL);
	printf("socketpair msg passing for messages\n");
}

int main(void){
	printf("0x%x", buff_master_tx[0]);
	_spi_messaging_test();
	return 0;
}
