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
}

// merge buddies
Header* join(Header* buddy1) {
  //clear(buddy1); ADD THIS IN LATER
  
  int offset = (char*) buddy1 - (char*) base_addr;
  int buddy_offset = offset ^ (buddy1->size);
  Header* buddy_addr = (Header*) base_addr + buddy_offset;

 int tier = log2(buddy1->size) - log2(block_size);
  if (! buddy_addr->free) {
   
    // Take free blocks back, put back in free list
    if (free_lists[tier]) {
      Header* temp = free_lists[tier];
      if (!temp->free) {
	while (temp->next) {
	  temp = temp->next;
	}
	temp->next = buddy1;
      }
      else {
	free_lists[tier] = buddy1;
      }
    }
  
    else {
      free_lists[tier] = buddy1;
    }
  }

  else {
    buddy1->size = 2 * buddy1->size;
    Header* temp = free_lists[tier];
	if (free_lists[tier + 1]->free) {
      free_lists[tier + 1] = buddy1;
      
		if(free_lists[tier] == buddy_addr)//aman: i made few changes in this para.
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
      Header* temp2 = free_lists[tier + 1];
      while (temp2->next) {
	temp2 = temp2->next;
      }
      temp2->next = buddy1;
      join(buddy1);
    }
  }
}

// Split a block 
// void split(int tier, int size)
Header* split(int size_need) {
  int cur_tier = log2(size_need) - log2(block_size);
  Header* temp = free_lists[cur_tier]; //?????

  if (free_lists[cur_tier]) {
    temp = free_lists[cur_tier];
  }
  
  
  int ct = cur_tier;
  //  while (temp == NULL || temp->free == true) {
  while (temp == NULL && ct<num_lists) {
    printf("ct: %d", ct);
    ++ct;
    temp = free_lists[ct];
    //++ct;
  }
  
  /*while(!temp) { //i just commemted ti out, doesn't make sense as of now
    ++ct;
    temp = free_lists[ct];
    printf("Ct: %d\n", ct);
  }*/
  
  int cur_size = pow(2, cur_tier + log2(block_size));
  // should be !=
  if (temp != NULL){//condition if memory is unavailable
    //printf("cbefore: %d, need: %d\n", cur_size, size_need);
    //temp->size = 0;
    cur_size = temp->size;//aman - make sure if you use size , then previously save them in header.
  }
  
  printf("cur: %d, need: %d\n", cur_size, size_need);
  while (cur_size > size_need) {
    printf("IN HERE ! \n");
    int offset = (char*) temp - (char*) base_addr; //i assume this to work
    int buddy_offset = offset ^ (temp->size / 2);
    char* buddy_addr = (char*) base_addr + buddy_offset;
      
    
    // Set 1st buddy
    temp->size = cur_size / 2;
    temp->free = true;
    
    free_lists[cur_tier - 1] = temp;
    Header * buddy2 = (Header*) buddy_addr;
    buddy2->size = cur_size / 2;
    buddy2->free = true;
    buddy2->next = NULL;

    free_lists[cur_tier - 1]->next = buddy2;
        
    /*
    temp->size = cur_size / 2;
    temp->free = true;
    temp->next = (Header*) buddy_addr;
    
    (temp->next)->size = cur_size / 2;
    (temp->next)->free = true;

    free_lists[cur_tier - 1] = temp;
    free_lists[cur_tier]->free = true;*/
    if (cur_size / 2 == size_need) { 
      return temp->next;
    }
    cur_size /= 2;
    temp = temp->next;
  }
  return temp;
  // return?
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
  int need = _length + sizeof(Header); // Size of mem required
  int give = pow(2, (int) round (log2(need) + .5)); // need rounded to 2^k

  // reject if need too much
  if (give < block_size) {give = block_size;}
  if (give > max_size) {
    printf("give: %d, max: %d\n", give, max_size);
    printf("YOU NEED TOO MUCH!\n");
    give = 0; // FIX THIS
    return 0;
  }
  
  int fl_index = log2(give) - log2(block_size); // index of free list
  printf("1need: %d, give: %d, block size: %d\n", need, give, block_size);
  printf("fl_index: %d\n\n", fl_index);
  
  // If valid amt of mem to give and we have room for it
  int ct = num_lists - 1;
      
  Header* hh = split(give);// + sizeof(Header);
  printf("2GIVE: %d, SPLIT SIZE: %d\n", give, hh->size);
  printf("base: %p, hh: %p\n", (void*) base_addr, hh);
  return (void*) hh;
  //return malloc((size_t)_length);
}

extern int my_free(Addr _a) {/*
  Header* h = (Header*) _a - sizeof(Header*);
   h->free = true;
   join(h);*/


  /* Same here! */
  // Reject invalid free requests
  /*if (!_a) {return -1;}
  
  
  // Get beginning of memory before header
  Header* begin = (Header*) ((char*) _a - sizeof(Header));
  int tier = log2(begin->size) - log2(block_size);
  begin->free = true;
  begin->size = 2 * begin->size;
  
  // no block in next highest tier 
  if (!free_lists[tier + 1] && tier + 1 < num_lists) {
    begin->next = NULL;
    free_lists[tier + 1] = begin;
  } 
  
  // exist blocks in next highest tier
  else {
    
  }
  */
  free(_a);
  return 0;
}
