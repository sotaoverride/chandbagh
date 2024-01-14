#include <ctype.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "messaging_queue.h"
#include <pthread.h>
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)
int global_number_of_threads;
pthread_mutex_t mutx;


/*Create the messaging bus/queue*/

struct Queue *queue;

/*Generate thread number randomly*/

int genRandoms(int lower, int upper)
{
        int num = (rand() %
        (upper - lower + 1)) + lower;
//        printf("random number:%d \n", num);
	return num;
}

struct thread_info {            /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
};

/* Enqueue messages thread function. */

static void * 
enqueue_bus(void *arg)
{
	while(1){
	int tmp = genRandoms(0, global_number_of_threads);
	pthread_mutex_lock(&mutx);
	enqueue(queue, tmp);
	pthread_mutex_unlock(&mutx);
	}
	return NULL;
			
}

/*check incoming messages for each thread*/

static void *
check_messages(void* arg)
{
    struct thread_info *tinfo = arg;
    int tmp;
    while(1){
	pthread_mutex_lock(&mutx);
	if(empty(queue)!=0){
	tmp = peek(queue);
	pthread_mutex_unlock(&mutx);
    	while ( tinfo->thread_num != tmp){}
	
	printf("Thread %d found a message addressed to it from the queue\n", tinfo->thread_num);
	pthread_mutex_lock(&mutx);
        dequeue(queue);
	pthread_mutex_lock(&mutx);
	}


    }

   return NULL;
}




/* Thread start function: Check if message on head of the queue is for you (this thread)
 * display address near top of our stack,
   and return upper-cased copy of argv_string. */

static void *
thread_start(void *arg)
{
    struct thread_info *tinfo = arg;
    char *uargv;
    printf("Thread %d: top of stack near %p; argv_string=%s\n",
           tinfo->thread_num, (void *) &tinfo, tinfo->argv_string);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
        handle_error("strdup");

    for (char *p = uargv; *p != '\0'; p++)
        *p = toupper(*p);

    return uargv;
}

int main(int argc, char *argv[])
{
    int                 s, opt;
    void                *res;
    size_t              num_threads;
    ssize_t             stack_size;
    pthread_attr_t      attr;
    struct thread_info  *tinfo;
    struct thread_info  *tinfo2;
    queue = (struct Queue *) malloc(sizeof(struct Queue));	// allocate the queue
    queue->front = NULL;			// initialize the queue's pointers to NULL
    queue->rear = NULL;


    /* The "-s" option specifies a stack size for our threads. */

    stack_size = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            stack_size = strtoul(optarg, NULL, 0);
            break;

        default:
            fprintf(stderr, "Usage: %s [-s stack-size] arg...\n",
                    argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    num_threads = argc - optind;
    global_number_of_threads = num_threads;

    /* Initialize thread creation attributes. */

    s = pthread_attr_init(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_init");

    if (stack_size > 0) {
        s = pthread_attr_setstacksize(&attr, stack_size);
        if (s != 0)
            handle_error_en(s, "pthread_attr_setstacksize");
    }

    /* Allocate memory for pthread_create() arguments. */

    tinfo = calloc(num_threads, sizeof(*tinfo));
    if (tinfo == NULL)
        handle_error("calloc");
    tinfo2 = calloc(num_threads, sizeof(*tinfo));
    if (tinfo == NULL)
        handle_error("calloc");

    /* Create one thread that enqueues messages addressed to a thread. */
    struct thread_info ti2;
    ti2.thread_num = num_threads + 1;
    s = pthread_create(&ti2.thread_id, &attr, &enqueue_bus, &ti2);
    if (s != 0)
	    handle_error_en(s, "pthread_create");

    /* Create one thread for each command-line argument. */

    for (size_t tnum = 0; tnum < num_threads; tnum++) {
        tinfo[tnum].thread_num = tnum + 1;
        tinfo[tnum].argv_string = argv[optind + tnum];

        /* The pthread_create() call stores the thread ID into
           corresponding element of tinfo[]. */

        s = pthread_create(&tinfo[tnum].thread_id, &attr,
                           &thread_start, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    for (size_t tnum = 0; tnum < num_threads; tnum++) {
        tinfo2[tnum].thread_num = tnum + 1;
        tinfo2[tnum].argv_string = argv[optind + tnum];

        /*check incoming messages*/
	s = pthread_create(&tinfo2[tnum].thread_id, &attr,
                           &check_messages, &tinfo2[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }
    /* Destroy the thread attributes object, since it is no
       longer needed. */

    s = pthread_attr_destroy(&attr);
    if (s != 0)
        handle_error_en(s, "pthread_attr_destroy");

    /* Now join with each thread, and display its returned value. */

    for (size_t tnum = 0; tnum < num_threads; tnum++) {
        s = pthread_join(tinfo[tnum].thread_id, &res);
        if (s != 0)
            handle_error_en(s, "pthread_join");

        printf("Joined with thread %d; returned value was %s\n",
               tinfo[tnum].thread_num, (char *) res);
        free(res);      /* Free memory allocated by thread */
    }
    s = pthread_join(ti2.thread_id, &res);
    if (s !=0)
	    handle_error_en(s, "pthread_join");
    printf("Joined with thread %d; returned value was %s\n",
               ti2.thread_num, (char *) res);
        free(res);
    free(tinfo);
    exit(EXIT_SUCCESS);
}

