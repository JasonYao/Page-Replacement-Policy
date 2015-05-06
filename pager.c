//
// Created by Jason Yao on 4/25/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PAGEHIT  0
#define PAGEMISS 1

int * mem;
int mem_size = 0;  /* memory size in pages */

/* WARNING: The following code may make you cry. A safety pig has been
* included for your benefit.
*
*
*	 _._ _..._ .-',     _.._(`))
*	'-. `     '  /-._.-'    ',/
*	   )         \            '.
*	  / _    _    |             \
*	 |  a    a    /              |
*	 \   .-.                     ;
*	  '-('' ).-'       ,'       ;
*	     '-;           |      .'
*	        \           \    /
*	        | 7  .__  _.-\   \
*	        | |  |  ``/  /`  /
*	       /,_|  |   /,_/   /
*	          /,_/      '`-'
*
*/

/* Indices & global vars for use in a fifo queue */
int head = 0;
int tail = 0;
int size = 0;

/* Indices & global vars for use in a LRU priority queue */
int LRUHEAD = 0;

/* Indices & global vars for use in my own random replacement */
int ownSize = 0;
int count = 0;

/* fifo Helper functions */

/**
 * Adds the new value to the memory array
 * NOTE: Does not check if the array is full already, so note insertion is blind
 * @param pageData The page number that is being added to the memory block
 * output: void
 */
void enqueue (int pageData)
{
    // Checks if the queue is empty or not
    if (size != 0)
    {
        // Queue is not empty, updates the tail
        ++tail;

        // Checks if there's a need to wrap around:
        if (tail >= mem_size)
        {
            // Resets tail to the start
            tail = 0;
        }
    }
    // Sets the memory page to tail
    mem[tail] = pageData;

    // Updates the size
    ++size;
} // End of the Enqueue function

/**
 * Returns the first value in the memory array queue
 * @param void
 * output: void
 */
void dequeue ()
{
    // We know the queue will never be empty, so no need to check
    // Sets the mem[head] to initial 0
    mem[head] = 0;

    // Updates head
    ++head;

    // Checks for array wrapping
    if (head >= mem_size)
        head = 0;

    // Decrements the size
    --size;
} // End of the dequeue function

/* End of the fifo helper functions */

/* LRU helper functions */

/**
 * Swap function that swaps both the memory page array and the priority array
 * @param a index of the first element to be switched
 * @param b index of the second element to be switched
 * output: void, but alters mem and priority.
 */
void swap(int a, int b)
{
    int tempPage = 0;

    tempPage = mem[a];
    mem[a] = mem[b];
    mem[b] = tempPage;
} // End of swap function

/**
 * Swaps the value all the way until the right
 * @param the index at which the swaps begin
 * output: void
 */
void doShit(int current)
{
    int currentIndex = current;
    while(currentIndex < mem_size - 1)
    {
        swap(currentIndex, currentIndex + 1);
        ++currentIndex;
    }
} // End of the doShit function


/**
 * Inserts a page
 */
void lruEnqueue(int page)
{
    mem[LRUHEAD] = page;

    doShit(LRUHEAD);
} // End of lru Enqueue

/* End of LRU helper functions */

/* own helper functions */

/**
 * Adds a page in the order found, unless it's time to use random
 * @param current The current page that is being inserted
 * @param index The index of the randomized value that was just generated
 * output: void
 */
void ownEnqueue(int current, int index)
{
    if (count <= mem_size - 1)
        mem[ownSize] = current;
    else
        mem[index] = current;
    ++count;
    ++ownSize;
} // End of the ownEnqueue function

/* end of own helper functions */

/* Output: page hit or page miss */
int mem_check(int page)
{
    unsigned i;

    for(i = 0; i < mem_size; i++)
        if(mem[i] == page)
            return PAGEHIT;

    return PAGEMISS;
}

/**
 * Implements a fifo page replacement policy
 * Input: Current the current page being replaced
 * Output: Void
 */
void fifo(int current)
{
    // We know that the most recently used will be the head of the queue
    // Since the wrapper does the isFull() check for us, we know the array is not empty
    if (!mem_check(current))
    {
        // This means we have a page hit, no need to enqueue or dequeue
    }
    else
    {
        // We have a pagemiss, so we dequeue and then enqueue
        dequeue();
        // Enqueues the current value
        enqueue(current);
    }
} // End of the fifo function

/* End of the fifo code */

/**
 * Least recently used page replacement policy, dequeues and enqueues when mem is full
 * @param current The current page that is being inserted
 * output: void
 */
void lru(int current)
{
    unsigned i;

    // Searches for duplicates
    for (i = 0; i < mem_size; ++i)
    {
        if (mem[i] == current)
        {
            // Duplicate is found
            // Shifts it to the right
            doShit(i);
            return;
        }
    }

    // Enqueues the current value, replaces the least recently used
    lruEnqueue(current);
    return;
} // End of the lru function


/***************************************************************/
/* input: none */
/* output: void */
void own(int current)
{
    if (!mem_check(current))
    {
        // This means we have a page hit, no need to enqueue or dequeue
        return;
    }
    else
    {
        // We have a pagemiss, so we dequeue and then enqueue
        //LOL IDGAF ANYMORE
        //random int between 0 and 9

        int replacementValue = rand() % mem_size;

        --ownSize;
        // Enqueues the current value
        ownEnqueue(current, replacementValue);
    }
}
/***************************************************************/
/* Input: file handle
/* Output: non
/* This function writes the memory content to a file */

void print_mem(FILE *file)
{
    int i;

    for(i = 0; i < mem_size; i++)
        fprintf(file, "%d ", mem[i]);

    fprintf(file, "\n");
}

/**
 * Insertion function that covers all 3 possible insertion methods
 * @param: page The page that we are inserting into memory
 * @param: policy The policy type that we are implementing insertion for
 *          e.g. fifo, lru, own
 * Output: void
 */
void insert(int page, int policy)
{
    unsigned i;

    // Searches for duplicates
    for (i = 0; i < mem_size; ++i)
    {
        if (mem[i] == page)
        {
            // Duplicate is found
            // Shifts it to the right
            doShit(i);
            return;
        }
    }

    // Goes through and adds the page to the memory array
    switch(policy)
    {
        case 0:
        {
            // Fifo enqueue
            enqueue(page);
            return;
        }
        case 1:
        {
            // Lru insert
            lruEnqueue(page);
            return;
        }
        case 2:
        {
            // Own insert
            ownEnqueue(page, 0);
            return;
        }
        default:
        {
            // Memory is full
            printf("Memory full and is not suppose to be!!\n");
            exit(1);
        }
    } // End of the switch statement
} // End of the insert function

/***************************************************************/
/* Input:  page number */
/***************************************************************/
/* Input: none
/* Ouput: 1 if memory is full, 0 otherwise */
int IsFull()
{
    unsigned i;

    for(i = 0; i < mem_size; i++)
        if(!mem[i])
            return 0;

    return 1;
}

/***************************************************************/
int main(int argc, char * argv[])
{
    int policy; /* replacement policy */
    int current;  /* current page accessed */
    FILE * fp; /* The file containing the page accesses */
    FILE * rp; /* output file */
    char filename[30]={""};
    const char * extension[] ={".fifo", ".lru", ".own"};
    float num_accesses = 0.0; /* total number of page accesses */
    float page_faults = 0.0;
    srand(time(NULL));

    /* Getting and checking the input from the command line */
    if(argc != 4)
    {
        printf("usage: pager policy size filename\n");
        exit(1);
    }

    policy = atoi(argv[1]);
    mem_size = atoi(argv[2]);

    if( policy < 0 || policy > 2)
    {
        printf("policy must be 0, 1, or 2\n");
        exit(1);
    }

    if(mem_size <= 0 )
    {
        printf("Size must be a positive integer.\n");
        exit(1);
    }

    /* Allocate and initialize the memory */
    mem = (int *)calloc(mem_size, sizeof(int));
    if(!mem)
    {
        printf("Cannot allocate mem\n");
        exit(1);
    }

    /* open the memory access file */
    fp = fopen(argv[3], "r");
    if(!fp)
    {
        printf("Cannot open file %s\n", argv[3]);
        exit(1);
    }

    /* Create the output file */
    strcat(filename, argv[3]);
    strcat(filename,extension[policy]);
    rp = fopen(filename, "w");
    if(!rp)
    {
        printf("Cannot create file %s\n", filename);
        exit(1);
    }

    /* The main loop of the program */
    while(fscanf(fp,"%d", &current) == 1)
    {
        num_accesses++;
        if(mem_check(current) == PAGEMISS)
            page_faults++;

        switch(policy)
        {
            case 0:
                if(IsFull())
                {
                    // Dequeues and enqueues the current value
                    fifo(current);
                }
                else
                    insert(current, policy);
                break;

            case 1: if(IsFull())
                {
                    // Dequeues and enqueues the current value
                    lru(current);
                }
                else
                    insert(current, policy);
                break;

            case 2: if( IsFull())
                {
                    own(current);
                }
                else
                    insert(current, policy);
                break;


            default: printf("Unknown policy ... Exiting\n");
                exit(1);

        }/* end switch-case */

        print_mem(rp);

    }/* end while */
    fprintf(rp,"percentage of page faults = %f", page_faults/num_accesses);

    /* wrap-up */
    fclose(fp);
    fclose(rp);
    free(mem);

    return 1;

} // End of the main function
