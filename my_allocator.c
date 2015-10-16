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
 
struct header* mem;
struct header** free_lists; // lists for each size 2^k, k in [4, max]
int num_lists = 0; // cardinality of above set 
int max_size = 0; // maximum block size (size of blocks in last free list)
int mem_size = 0; // size of whole memory chunk
int block_size = 0; // size of basic block 
struct header* base_addr; // beginning address of memory chunk

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

// clear struct header portion of a block so we can merge
void clear(struct header* buddy1) {
  struct header* buddy2_addr = (struct header*) ((intptr_t)buddy1 ^ buddy1->size); 
  memset(buddy2_addr, 0, sizeof(struct header));
}

// merge buddies
struct header* join(struct header* buddy1) {
  int offset = (char*) buddy1 - (char*) base_addr;
  int buddy_offset = offset ^ (buddy1->size);
  char* buddy = (char*) base_addr + buddy_offset;

  struct header* buddy_addr = (struct header*) buddy;

  int tier = log2(buddy1->size) - log2(block_size);//formula to calculate the correct tier in freelist
  if (buddy_addr) {
  if (! buddy_addr->free) {// check if buddy is not free
    // Take free blocks back, put back in free list
    if (free_lists[tier]) {//check if tier for buddy1 in free list is not null
      struct header* temp = free_lists[tier];
      if (!temp->free) {
	while (temp->next) {//iterate to the last block pointed by that tier
	  temp = temp->next;
	}
	temp->next = buddy1;//put the address of buddy in next of last block
      }
      else {
	free_lists[tier] = buddy1;//if corresponding tier in free list was null, directly put adrres of buddy1 there
      }
    }
  
    else {
      free_lists[tier] = buddy1;//if corresponding tier in free list was null, directly put adrres of buddy1 there.
    }
  }
}

  else {//if the buddy of the block is free, nso we join them
    buddy1->size = 2 * buddy1->size;
    struct header* temp = free_lists[tier];
    if (free_lists[tier + 1]->free) {
      free_lists[tier + 1] = buddy1;
      
      if(free_lists[tier] == buddy_addr)
	free_lists[tier] = buddy_addr->next;
      else{
	temp = free_lists[tier];
	while(temp->next != buddy_addr)
	  temp = temp->next;
	temp->next = buddy_addr -> next;
      }
      buddy1->next = NULL;
    }

    else { 
      struct header* temp2 = free_lists[tier + 1];
      while (temp2->next) {
	temp2 = temp2->next;
      }
      temp2->next = buddy1;
      join(buddy1);//call the function again to check and join if any more joins are possible
    }
  }
}

// Split a block 
// void split(int tier, int size)
struct header* split(int size_need) {
  int cur_tier = log2(size_need) - log2(block_size);//to calculate the tier in free list for required memory
  struct header* temp = free_lists[cur_tier]; 

  if (free_lists[cur_tier]) {//if the tier of corresponding memory in free list is not null
    temp = free_lists[cur_tier];
  }
  
  // move temp to a suitable free list tier
  int ct = cur_tier;
  while (temp == NULL && ct < num_lists) {
    ++ct;
    temp = free_lists[ct];
  }
  
  int cur_size = pow(2, cur_tier + log2(block_size));
  if (temp != NULL){//condition if memory is unavailable
    //printf("cbefore: %d, need: %d\n", cur_size, size_need);
    cur_size = temp->size;
  }
  
  printf("cur: %d, need: %d\n", cur_size, size_need);
  while (cur_size > size_need) {
    // Find where 2nd buddy will go 
    char* buddy_addr = (char*) temp + (temp->size / 2);

    // Set 1st buddy
    temp->size = cur_size / 2;
    temp->free = true;
    
    // Set 2nd buddy
    free_lists[cur_tier - 1] = temp;
    printf("CUR SIZE / 2: %d\n", cur_size / 2);
    struct header * buddy2 = (struct header*) buddy_addr;
    buddy2->size = cur_size / 2;
    buddy2->free = true;
    buddy2->next = NULL;

    //printf("numlists: %d, cur_tier: %d\n", num_lists, cur_tier);
    free_lists[cur_tier - 1]->next = buddy2;
     
    if (cur_size / 2 == size_need) { 
      return buddy2;
    }
    cur_size /= 2;
    temp = temp->next;
  }
  return temp;
}

unsigned int init_allocator(unsigned int b, unsigned int len) {
  int alloc_size = 0;

  // Prevent user from making blocks larger than total memory
  if (b > len) {
    printf("Error, block size greater than total memory size!\n");
    abort();
  }
  //sizeof(struct struct header)
  // Round sizes to powers of 2 for convenience
  max_size = pow(2, (int) round(log2(len) - .5));
  block_size = pow(2, (int) round(log2(b) - .5));

  // Get # free list tiers
  int size = max_size;
  while (size > block_size - 1) {
    size /= 2;
    ++num_lists;
  }

  // Set up memory chunk and free lists
  mem = (struct header*) malloc(max_size);
  free_lists = (struct header**) malloc(sizeof(struct header) * num_lists);

  base_addr = mem;
  
  for (int i = 0; i < num_lists - 1; ++i) {free_lists[i] = NULL;}

  // Set up last free list to contain whole memory (max size)
  free_lists[num_lists - 1] = mem;
  free_lists[num_lists - 1]->next = NULL;
  free_lists[num_lists - 1]->free = true;
  free_lists[num_lists - 1]->size = max_size;
  printf("\nNew mem size: %d\n", free_lists[num_lists - 1]->size);

  return max_size;
}

// Finally give our free list and memory chunk back to OS
void release_allocator() {
  free(free_lists);
  free(mem);
}

extern Addr my_malloc(unsigned int _length) {
  int alloc_size = 0;
  int need = _length + sizeof(struct header); // Size of mem required
  int give = pow(2, (int) round (log2(need) + .5)); // need rounded to 2^k

  // reject if need too much
  if (give < block_size) {give = block_size;}
  if (give > max_size) {
    printf("give: %d, max: %d\n", give, max_size);
    printf("YOU NEED TOO MUCH!\n");
    give = 0; 

    return 0;
  }
  
  int fl_index = log2(give) - log2(block_size); // index of free list
  printf("1 - need: %d, give: %d, block size: %d\n", need, give, block_size);
  printf("fl_index: %d\n\n", fl_index);
  
  // If valid amt of mem to give and we have room for it
  int ct = num_lists - 1;
      
  struct header* hh = split(give);
  printf("2 - GIVE: %d, SPLIT SIZE: %d\n", give, hh->size);
  printf("base: %p, hh: %p\n", (void*) base_addr, hh);
  return (void*) hh + sizeof(struct header);
  //return malloc((size_t)_length);
}

extern int my_free(Addr _a) {
  // Reject invalid free requests
  if (!_a) {return -1;}
 

  // using the blow commented block causes segmentation faults from join
  // if we use our malloc logic rather than the c lib malloc

  /*
  // Get beginning of memory before struct header
  struct header* begin = (struct header*) ((char*) _a);// - sizeof(struct header));
  begin->free = true;
  join(begin);
  */
  
  // using this when we use our malloc logic gives invalid pointer errors
  //free(_a);
  return 0;

}
