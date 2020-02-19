/*

  compile using : c++ dnix-traverse.cpp -o dnix-traverse

  Program to list and extract files from a DNIX FS. Should also be able to write to a DNIX file system. Possibly even add boot code and other things.

 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef unsigned long daddr_t;


#pragma pack(push, 1)

#include "../dnix-headers/diskpar.h"
#include "../dnix-headers/inode.h"
#include "../dnix-headers/sysfile.h"
#include "../dnix-headers/dir.h"

#pragma pack(pop)



class DnixFs {
  struct sysfptr dnixPartitionInfo;
  FILE * image;
 public:
  DnixFs( FILE * image);
};


struct sysfptr {
	daddr_t	vdsp;		/* Pointer to volume descriptor block */
	daddr_t	vdspn;		/* 1:s complement of previous item */
	daddr_t	cleanfl;	/* Volume clean flag */
	time_t	timestamp;	/* Timestamp */
	daddr_t	x[124];		/* Reserved */
};


DixFs:DnixFs(FILE * img) {
  char buffer[26];
  image = img;
  // Read the partition info
  fread ( (void * ) dnixPartitionInfo, 512, 1 image);
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(dnixPartitionInfo.timestamp))
  printf ("Pointer to volume descriptor block: %08lX", dnixPartitionInfo.vdsp);
  printf ("1:s complement of previous item: %08lX", dnixPartitionInfo.vdspn);
  printf ("Volume clean flag: %08lX", dnixPartitionInfo.cleanfl);
  printf ("Timestamp %s", buffer);

}


int main (int argc, char ** argv) {
  FILE * image_file;
  int opt;
  printf("1\n");
  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
    case 'd':
      printf("1");
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



