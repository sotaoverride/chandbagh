#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * SYNOPSIS
#include <mqueue.h>

int mq_notify(mqd_t mqdes, const struct sigevent *sevp);

Link with -lrt.

*/
void startprocesses(char* buff, mqd_t* mqd, struct mq_attr* attr)
{
	struct message {
		int x;
		char y;
		int z;
	};

	int relen;
	pid_t p;
	p = fork();
	unsigned int priority = 0;
	if(p == -1)
	{
		perror("fork fail");
		exit(1);
	}
	// child process because return value zero
	else if ( p == 0){
		struct message msg;
		msg.x = 0;
		msg.y = 'q';
		msg.z = -1;

		/* Sending a struct works identically to a char array */
		while(1)
			mq_send (*mqd, (const char *)&msg, sizeof (struct message), 10);
	}


	// parent process because return value non-zero.
	else{
		while(1){
			printf("Hello from Parent!\n");
			/* When reading, use a char* buffer and explicitly cast */
			 relen =  ((mq_receive (*mqd, buff, attr->mq_msgsize, &priority)));
			if (relen == -1) perror("mq_receive");
			else if (relen != sizeof(struct message)) printf ("unexpected length\n");
			else
			{
				/* Retrieve message fields here */
				struct message* ptr = (struct message*) buff;
				printf("Contents of structure %d are %c, %d\n", ptr->x, ptr->y, ptr->z);

				memset( buff, '\0', attr->mq_msgsize );
			}
		}
	}
}
int main () {
	/* Create and open a message queue for writing and reading*/
	mqd_t mqd = mq_open ("/OpenCSF_MQ", O_CREAT | O_NONBLOCK | O_RDWR,  0600, NULL);

	/* Ensure the creation was successful */
	if (mqd == -1)
	{
		perror ("mq_open");
		exit (1);
	}
	/* Get the message queue attributes */
	struct mq_attr attr;
	if (mq_getattr (mqd, &attr) == -1) {perror("mq_getattr error"); exit (-1);}

	char *buffer = calloc (attr.mq_msgsize, 1);
	assert (buffer != NULL);


	startprocesses(buffer, &mqd, &attr);
	free (buffer);
	buffer = NULL;
	mq_close (mqd);
	return 0;
}
