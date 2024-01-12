#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "messaging_queue.h"

#define handle_error_en(en, msg) \ do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \ do { perror(msg); exit(EXIT_FAILURE); } while (0)

struct thread_info {    /* Used as argument to thread_start() */
    pthread_t thread_id;        /* ID returned by pthread_create() */
    int       thread_num;       /* Application-defined thread # */
    char     *argv_string;      /* From command-line argument */
    struct Queue* queue_ptr; /*messaing bus/queue pointer*/
};

/*thread function to enqueue bus*/
static void * 
enqueue_bus(void *arg)
{
	enqueue(queue, &(genRamdom(0, global_number_of_thread));
	return NULL;
			
}
static void *
deqeue_bus(void *arg){
	int tmp = dequeue(queue);
	message_for_id = tmp;
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

    if (tinfo->thread_num == message_for_id)
	    printf("Thread %d found a messages addressed to it from the queue\n", tinfo->thread_num);

    printf("Thread %d: top of stack near %p; argv_string=%s\n",
           tinfo->thread_num, (void *) &tinfo, tinfo->argv_string);

    uargv = strdup(tinfo->argv_string);
    if (uargv == NULL)
        handle_error("strdup");

    for (char *p = uargv; *p != '\0'; p++)
        *p = toupper(*p);

    return uargv;
}

/*Generate thread number randomly*/
int genRandoms(int lower, int upper,
                            int count)
{
        int num = (rand() %
        (upper - lower + 1)) + lower;
        printf("random number:%d \n", num);
	return num;
}
/*global to publish thread address for thread a message is addressed to*/
int message_for_id;
int global_number_of_thread;

int main(int argc, char *argv[])
{
    int                 s, opt;
    void                *res;
    size_t              num_threads;
    ssize_t             stack_size;
    pthread_attr_t      attr;
    struct thread_info  *tinfo;

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

    /*create one thread that enqueues the queue*/
    struct thread_info ti;
    s = pthread_create(ti.thread_id, &attr, &equeue_bus, 100);
    if (s != 0)
	    handle_error_en(s, "pthread_create");
    
    /*create one thread that dequeues the queue*/
    struct thread_info ti;
    s = pthread_create(ti.thread_id, &attr, &dequeue_bus, 100);
    if (s != 0)
	    handle_error_en(s, "pthread_create");
    /*Create the messaging bus/queue*/
    struct Queue *q;			// the queue itself
    int i;						// an int for the data
    q = (struct Queue *) malloc(sizeof(struct Queue));	// allocate the queue
    q->front = NULL;			// initialize the queue's pointers to NULL
    q->rear = NULL;

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

    free(tinfo);
    exit(EXIT_SUCCESS);
}
