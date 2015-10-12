// Using http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html
// as reference
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) {
  // input parameters (basic block size, memory length)
  int b = 128;
  int M = 512000;
  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "b:M:")) != -1) {
    switch(c) {
    case 'b':
      printf("checked b\n");
      b = atoi(optarg);
      break;
    case 'M':
      printf("checked M\n");
      M = atoi(optarg);
      break;
    default:
      printf("flag H for help\n");
      printf("flag b for block size (default 128)\n");
      printf("flag M for mem size (default 512000)\n");
      abort();
    }
  }
  printf("B: %d, M: %d\n", b, M);
  init_allocator(b, M);
  ackerman_main();
  //  void* h1 = my_malloc(1024);
  //printf("Header address: %p\n", h1);
  //my_free(h1);
  /*
    void* h2 = my_malloc(500);
  printf("Header address: %p\n", h2);

  void* h3 = my_malloc(256);
  printf("Header address: %p\n", h3);
  */

  atexit (release_allocator);
  exit (EXIT_SUCCESS);
}
