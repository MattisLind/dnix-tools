/*

  compile using : c++ dnix-traverse.cpp -o dnix-traverse

  Program to list and extract files from a DNIX FS. Should also be able to write to a DNIX file system. Possibly even add boot code and other things.

 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

typedef unsigned int dnix_daddr_t;
typedef int dnix_time_t;

#pragma pack(1)

#define daddr_t dnix_daddr_t
#define time_t dnix_time_t

#include "../dnix-headers/diskpar.h"
#include "../dnix-headers/inode.h"
#include "../dnix-headers/sysfile.h"
#include "../dnix-headers/dir.h"

#undef daddr_t
#undef time_t
#define daddr_t daddr_t
#define time_t time_t


#define swap32(num) (((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000)) 


class DnixFs {
  struct sysfptr dnixPartitionInfo;
  FILE * image;
 public:
  DnixFs();
  void init( FILE * image );
};


void DnixFs::init(FILE * img) {
  char buffer[26];
  struct tm * tm_info;
  time_t tim;
  image = img;
  // Read the partition info
  fseek (image, 512, SEEK_SET);
  fread ( (void * ) &dnixPartitionInfo, 512, 1, image);
  tim = swap32(dnixPartitionInfo.timestamp);
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  printf ("Pointer to volume descriptor block: %08X\n", swap32(dnixPartitionInfo.vdsp));
  printf ("1:s complement of previous item: %08X\n", swap32(dnixPartitionInfo.vdspn));
  printf ("Volume clean flag: %08X\n", swap32(dnixPartitionInfo.cleanfl));
  printf ("Timestamp %s\n", buffer);
}


DnixFs::DnixFs() {
}


int main (int argc, char ** argv) {
  FILE * image_file;
  int opt;
  class DnixFs dnixFs;
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
  dnixFs.init(image_file);
}



