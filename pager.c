//
// Created by Jason Yao on 4/25/15.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Indices & global vars for use in a queue */
int head = 0;
int tail = 0;
int size = 0;

/* Your job is to implement the following three functions */

/* Queue Helper functions */

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
 * output: index of the page being replaced
 */
int dequeue ()
{
    int returnValue;

    // Checks to see if the queue is empty
    if (size == 0)
    {
        // Returns a non-valid mem array index number
        return -1;
    }
    else
    {
        // Initializes the return value to the mem[head]
        returnValue = mem[head];

        // Sets the mem[head] to initial 0
        mem[head] = 0;

        // Updates head
        ++head;

        // Checks for array wrapping
        if (head >= mem_size)
            head = 0;

        // Decrements the size
        --size;

        // Returns the value
        return returnValue;
    }
} // End of the dequeue function

/**
 * Implements a fifo page replacement policy
 * Input: none
 * Output: Index of the page to be replaced
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
    return;
} // End of the fifo function


/***************************************************************/
/* input: none */
/* output: page to be replaced */
int lru()
{
    return 0;
}
/***************************************************************/
/* input: none */
/* output: page to be replaced */
int own()
{

    return 0;
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
    unsigned j;

    // Identifies any duplicates in the pages
    for (j = 0; j < mem_size; ++j)
    {
        if(page == mem[j])
        {
            // Found a duplicate, no need to replace
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
            //linkedInsert(page);
            return;
        }
        case 2:
        {
            // Own insert
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
/* Output: page hit or page miss */
int mem_check(int page)
{
    unsigned i;

    for(i = 0; i < mem_size; i++)
        if(mem[i] == page)
            return PAGEHIT;

    return PAGEMISS;

}
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
    const char * extension[] ={".fifo", ".lru", "new"};
    float num_accesses = 0.0; /* total number of page accesses */
    float page_faults = 0.0;
    unsigned victim = 0;  /* page to be replaced */

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
    fscanf(fp,"%d", &current);
    while(!feof(fp))
    {
        num_accesses++;
        if(mem_check(current) == PAGEMISS)
            page_faults++;

        switch(policy)
        {
            case 0: if(IsFull())
                {
                    // Dequeues and enqueues the current value
                    fifo(current);
                }
                else
                    insert(current, policy);
                break;

            case 1: if(IsFull())
                {
                    victim = lru();
                    mem[victim] = current;
                }
                else
                    insert(current, policy);
                break;

            case 2: if( IsFull())
                {
                    victim = own();
                    mem[victim] = current;
                }
                else
                    insert(current, policy);
                break;


            default: printf("Unknown policy ... Exiting\n");
                exit(1);

        }/* end switch-case */

        print_mem(rp);
        fscanf(fp,"%d", &current);

    }/* end while */
    fprintf(rp,"percentage of page faults = %f", page_faults/num_accesses);

    /* wrap-up */
    fclose(fp);
    fclose(rp);
    free(mem);

    return 1;

} // End of the main function

