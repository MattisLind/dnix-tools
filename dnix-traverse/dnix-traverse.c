#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
typedef unsigned long daddr_t;

#include "diskpar.h"
#include "inode.h"
#include "sysfile.h"
#include "dir.h"





int main (int argc, char ** argv) {
  FILE * image_file;
  int opt;
  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
    case 'd':
      image_file = fopen (optarg, "r");
      if (image_file == NULL) {
	perror ("Failure opening file.");
	exit(EXIT_FAILURE);
      }
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-d disk-image-file]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

}
