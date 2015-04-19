#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGEHIT  0
#define PAGEMISS 1

int * mem;
int mem_size = 0;  /* memory size in pages */


/* Your job is to implement the following three functions */

/* input: none */
/* output: page to be replaced */
int fifo()
{
  
  return 0;
}
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

/***************************************************************/
/* input: page number  */
/* output: NONE */
void insert(int page)
{
   unsigned i;
   
   for(i = 0; i < mem_size; i++)
     if(!mem[i])
     {
        mem[i] = page;
	return;
     }
     
   printf("Memory full and is not suppose to be!!\n");
   exit(1);
}



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
      case 0: if( IsFull())
	      {
		victim = fifo();
		mem[victim] = current;
	      }
	      else
		insert(current);
	      break;
	     
      case 1: if( IsFull())
	      {
		victim = lru();
		mem[victim] = current;
	      }
	      else
		insert(current);
	      break;
	      
      case 2: if( IsFull())
	      {
		victim = own();
		mem[victim] = current;
	      }
	      else
		insert(current);
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

}