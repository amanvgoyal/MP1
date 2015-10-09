/*
  AMAN, I RECCOMEND USING THE BELOW SITE AS REFERENCE:
  http://homepage.cs.uiowa.edu/~jones/opsys/notes/27.shtml


    File: my_allocator.c

    Author: <your name>
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include "my_allocator.h"
#include <math.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

char* mem;
Header** free_lists; // lists for each size 2^k, k in [4, max]
int num_lists = 0; // cardinality of above set 
int max_size = 0; // maximum block size (size of blocks in last free list)
int mem_size = 0;
int block_size = 0;
char* base_addr;
/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

    /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/
Header* join(Header*) {

}

void split(Header*) {

}

unsigned int init_allocator(unsigned int b, unsigned int len) {
  //  int num_blocks = len / b;
  mem_size = len;
  block_size = b;

  // Prevent user from making blocks larger than total memory
  if (b > len) {
    printf("Error, block size greater than total memory size!\n");
    abort();
  }

  // Figure out how many tiers of free lists we have
  // 1st tier has size 16, second 32, ...
  int size = len;
  //while (len > 31) {
  while (size > b - 1) {
    size /= 2;
    ++num_lists;
  }

  //max_size = pow(2, num_lists);

  mem = (char*) malloc(len);
  free_lists = (Header**) malloc(sizeof(Header*) * num_lists);

  base_addr = mem;
  /*  free_lists[num_lists - 1] = (Header*) base_addr;
      free_lists[num_lists - 1]->free = false;
      free_lists[num_lists - 1]->size = len;
  */

  // Init free list
  printf("num_lists: %d", num_lists);
  for (int i = 0; i < num_lists; ++i) {
    free_lists[i] = (Header*) base_addr;
    free_lists[i]->size = pow(2, log2(b) + i); // Set block size
    free_lists[i]->free = true;
    free_lists[i]->next = NULL;
  }
  

  free_lists[num_lists - 1] = (Header*) base_addr;
  free_lists[num_lists - 1]->free = false;
  free_lists[num_lists - 1]->size = len;
}

int release_allocator() {
  return 0;
}

extern Addr my_malloc(unsigned int _length) {
  int alloc_size = 0;
  if (_length > mem_size) {
    printf("Error, trying to allocate more than maximum size!\n");
    abort();
  }

  // Find size to give user (give)
  int temp = mem_size;
  int need = _length + sizeof(Header);
  int give = pow(2, (int) round (log2(need) + .5));
  /*while (need < temp) {
    temp /= 2;
  }
  give = temp*2;
  */
  if (give < block_size) {give = block_size;}
  if (give > mem_size) {
    printf("give: %d, max: %d\n", give, mem_size);
    printf("YOU NEED TOO MUCH!\n");
    give = block_size; // FIX THIS
  }
  int fl_index = log2(give) - log2(block_size); // index of free list
  printf("need: %d, give: %d, block size: %d\n", need, give, block_size);
  printf("fl_index: %d\n\n", fl_index);
  if (free_lists[fl_index]->free == true) {
    // Must do splitting here...
  }
  
  else {
    Header * current = free_lists[fl_index];
    while (current->next != NULL || current->free == true) {
      //current = c
      printf("free\n");
    }
  }
  
  return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  /* Same here! */
  free(_a);
  return 0;
}

