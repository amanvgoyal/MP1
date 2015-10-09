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

Header** free_lists; // lists for each size 2^k, k in [4, max]
int num_lists = 0; // cardinality of above set 
int max_size = 0; // maximum block size (size of blocks in last free list)
void* base_addr;
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
unsigned int init_allocator(unsigned int b, unsigned int len) {
  int num_blocks = len / b;

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

  max_size = pow(2, num_lists);

  //Header** fl = (Header**) malloc(sizeof(Header) * num_blocks);
  //free_lists = (Header**) malloc(sizeof(Header) * num_lists); //????????????
  free_lists = (Header**) malloc(sizeof(Header*) * num_lists);

  // First tier has contiguous chunk
  // This wil be split for allocations. split chunks will be pointed to 
  // by tier headers?
  free_lists[0] = (Header*) malloc(len);
  free_lists[0]->size = len;
  free_lists[0]->next = NULL;
  free_lists[0]->free = true;

  base_addr = (void*)free_lists [0];

  // Init free list here with headers with proper sizes
  // and all next pointers NULL 

  /*  for (int i = 1; i < num_lists; ++i) {
    free_lists[i]->size = 2^(3 + i);
    free_lists[i]->next = NULL;
    free_lists[i]->free = true;
    printf("flsize i size: %d", free_lists[i]->size);
    }*/
  printf("s: %d", free_lists[0]->size);
  
}
int release_allocator() {
  return 0;
}

extern Addr my_malloc(unsigned int _length) {
  int alloc_size = 0;
  if (_length > max_size) {
    printf("Error, trying to allocate more than maximum size!\n");
    abort();
  }
  

  return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  /* Same here! */
  free(_a);
  return 0;
}

