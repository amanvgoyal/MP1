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
#include <string.h>
#include <stdint.h>
#include "my_allocator.h"
#include <math.h>

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
 
Header* mem;
Header** free_lists; // lists for each size 2^k, k in [4, max]
int num_lists = 0; // cardinality of above set 
int max_size = 0; // maximum block size (size of blocks in last free list)
int mem_size = 0;
int block_size = 0;
Header* base_addr;
bool no_room = false;
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

// clear header portion of a block so we can merge
void clear(Header* buddy1) {
  // DO A MEMSET OF 2*BLOCKSIZE - HEADER SIZE
  
  Header* buddy2_addr = (Header*) ((intptr_t)buddy1 ^ buddy1->size); 
  memset(buddy2_addr, 0, sizeof(Header));
  /*Header b2 = *buddy2_addr;
  Header b1 = *buddy1;
 
  int tier = log2(buddy1->size) - log2(block_size);
 
  // Set up buddy 1 to have 2* size and be free
  buddy1->next = NULL;
  buddy1->size = 2*buddy1->size;
  buddy1->free = true;

  // If we don't already have a block of size 2*buddy1->size
  if (free_lists[tier + 1]->free == true) {
    free_lists[tier + 1] = buddy1;
  }
  
  // If we already have a block(s) of size 2*buddy->size
  // make buddy1 the next header in freelists[tier+1]
  else {
    Header* temp = free_lists[tier + 1];
    while (temp->next) {
      temp = temp->next;
    }
    temp = buddy1;
  }
  */
}

// merge buddies
Header* join(Header* buddy1) {
  //clear(buddy1); ADD THIS IN LATER
  // Header* buddy2 = (Header*) ((intptr_t)buddy1 ^ buddy1->size); 
  
  // int offset = block_addr - origin;
  // int buddy_offset = offst ^ size;
  // char* buddy_addr = origin + buddy_offset;

  int offset = (char*) buddy1 - (char*) base_addr;
  int buddy_offset = offset ^ (buddy1->size);
  char* buddy_addr = (char*) base_addr + buddy_offset;
   
  int tier = log2(buddy1->size) - log2(block_size);
 
  // Set up buddy 1 to have 2* size and be free
  buddy1->next = NULL;
  buddy1->size = 2*buddy1->size;
  buddy1->free = true;

  // If we don't already have a block of size 2*buddy1->size
  if (free_lists[tier + 1]->free == true) {
    free_lists[tier + 1] = buddy1;
  }
  
  // If we already have a block(s) of size 2*buddy->size
  // make buddy1 the next header in freelists[tier+1]
  else {
    Header* temp = free_lists[tier + 1];
    while (temp->next) {
      temp = temp->next;
    }
    temp = buddy1;
  }

}

// Split a block 
void split(int tier) {
  int ct = num_lists - 1;
  int size_needed = pow(2, tier + log2(block_size));

  Header* temp = free_lists[ct];
  // Find a free block >= size_needed
  while (free_lists[ct] && free_lists[ct]->size > size_needed) {
    temp = free_lists[ct];
    //while (temp && temp->free == false) {
    while (true) {
      if (temp->free == true) {break;}
      temp = temp->next;
    }
    --ct;
  }
  printf("End split size: %d\n", temp->size);
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

  max_size = pow(2, num_lists + log2(b)); // Give too much

  mem = (Header*) malloc(len);
  free_lists = (Header**) malloc(sizeof(Header) * num_lists);

  base_addr = mem;
  
  for (int i = 0; i < num_lists - 1; ++i) {free_lists[i] = NULL;}

  free_lists[num_lists - 1] = mem;
  free_lists[num_lists - 1]->next = NULL;
  free_lists[num_lists - 1]->free = true;
  free_lists[num_lists - 1]->size = max_size;
  printf("\nlast: %d\n", free_lists[num_lists - 1]->size);

  return max_size;
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

  int temp = mem_size;
  int need = _length + sizeof(Header); // Size of mem required
  int give = pow(2, (int) round (log2(need) + .5)); // need rounded to 2^k
  
  // Reject/Fix invalid requests
  if (give < block_size) {give = block_size;}
  if (give > max_size) {
    printf("give: %d, max: %d\n", give, max_size);
    printf("YOU NEED TOO MUCH!\n");
    give = 0; // FIX THIS
  }
  
  int fl_index = log2(give) - log2(block_size); // index of free list
  printf("need: %d, give: %d, block size: %d\n", need, give, block_size);
  printf("fl_index: %d\n\n", fl_index);
  
  // If valid amt of mem to give and we have room for it
  int ct = num_lists - 1;
  if (give != 0 && !no_room) {
    // Special case when user needs whole memory
    if (give == max_size && free_lists[num_lists - 1]->free == true) {
      printf("TAKEN THE WHOLE THING!\n");
      free_lists[num_lists - 1]->free = false;
      no_room = true;
      return (void*) base_addr;
    }
       
    // If there exists a block of the size we want
    if (free_lists[fl_index] != NULL) {
      // If the block is free then we can use it 
      if (free_lists[fl_index]->free == true) {
	// Must do splitting here...?
	printf("block of this size not used yet!\n");
      }
  
      // Otherwise look further in the free list for a block of correct size
      else {
	Header * current = free_lists[fl_index];
	printf("add a block of this size\n");
	while (current->next != NULL || current->free == true) {
	  //current = c
	  printf("free\n");
	}
      }
    }
    
    // There is no block of the size we need, we need to split a bigger one
    else {
      printf("NULL BLOCK\n");
      split(fl_index);
    }
  }

  return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  /* Same here! */
  free(_a);
  return 0;
}

