//  data structures for the program:

struct node				// a node in a linked list
{
	int item;
	struct node *next;			// and a pointer to the next item in the linked list
};

struct Queue			// the Queue will have a pointer to the front
{								// and a pointer to the rear of the queue
	struct node *front;			// the pointers will equal NULL if the list is empty
	struct node *rear;			// or will point at the same location for a 1-item queue
};

//   function prototypes:

void enqueue(struct Queue *q, int item);	// enqueue item to the rear of q
int dequeue(struct Queue *q);				// dequeue and return the first item of q
int empty(struct Queue *q);					// checks to see if the queue is empty (returns 0) or not (returns 1)

