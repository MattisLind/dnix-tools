/*

  compile using : c++ dnix-traverse.cpp -o dnix-traverse

  Program to list and extract files from a DNIX FS. Should also be able to write to a DNIX file system. Possibly even add boot code and other things.

 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

typedef unsigned int daddr_t;
typedef int dnix_time_t;
typedef int ino_t;

#pragma pack(1)

//#define daddr_t dnix_daddr_t
#define time_t dnix_time_t
//#define ino_t dnix_ino_t

#include "../dnix-headers/diskpar.h"
#include "../dnix-headers/inode.h"
#include "../dnix-headers/sysfile.h"
#include "../dnix-headers/dir.h"

//#undef daddr_t
#undef time_t
//#define daddr_t daddr_t
#define time_t time_t


#define swap32(num) (((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000)) 

#define swap16(num) (((num >> 8) & 0xff) | ((num << 8) & 0xff00))

class DnixFs {
  struct sysfptr dnixPartitionInfo;
  struct sysfile sysfile;
  FILE * image;
 public:
  DnixFs();
  void init( FILE * image );
  void readInode (int inumber, struct dinode * inode);
};


void DnixFs:: readInode (int inumber, struct dinode * inode) {
  struct dinode dinode;
  char buffer[26];
  struct tm * tm_info;
  time_t tim;
  int i;
  printf ("inumber  %04X\n", inumber);
  printf ("insiz %04X\n", sysfile.s_insiz);
  printf ("inadr %04X\n", sysfile.s_inadr);
  printf ("Address to read inode from %04lX\n", (inumber-1) * (sizeof dinode) + sysfile.s_inadr);
  
  fseek (image, (inumber-1) * (sizeof dinode) + sysfile.s_inadr, SEEK_SET);
  fread ( (void * ) &dinode, sizeof dinode, 1, image);

  
  inode->di_mode = swap16(dinode.di_mode);
  inode->di_nlink = swap16(dinode.di_nlink);
  inode->di_uid = swap16(dinode.di_uid);
  inode->di_gid = swap16(dinode.di_gid);
  inode->di_size = swap32(dinode.di_size);
  inode->di_atime = swap32(dinode.di_atime);
  inode->di_atime = swap32(dinode.di_atime);
  inode->di_mtime = swap32(dinode.di_mtime);
  inode->di_ctime = swap32(dinode.di_ctime);
  memcpy((void *) inode->di_addr, (void *) dinode.di_addr, 40);

  printf("mode and type of file %04X\n", inode->di_mode);
  printf("number of links to file %d\n", inode->di_nlink);
  printf("owner's user id %d\n", inode->di_uid);
  printf("owner's group id %d\n", inode->di_gid);
  printf("number of bytes in file %d\n", inode->di_size);

  tim = inode->di_atime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  printf("time last accessed %s\n", buffer);

  tim = inode->di_mtime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  printf("time last modified %s\n", buffer);
  tim = inode->di_ctime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  printf("time inode last modified %s\n", buffer);


}


void DnixFs::init(FILE * img) {
  char buffer[26];
  struct tm * tm_info;
  daddr_t superblock_address;
  struct sysfile s;
  time_t tim;
  image = img;
  // Read the partition info
  
  fseek (image, SYSFPTR, SEEK_SET);
  fread ( (void * ) &dnixPartitionInfo, 512, 1, image);
  tim = swap32(dnixPartitionInfo.timestamp);
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  superblock_address = swap32(dnixPartitionInfo.vdsp);
  printf ("Pointer to volume descriptor block: %08X\n", superblock_address);
  printf ("1:s complement of previous item: %08X\n", swap32(dnixPartitionInfo.vdspn));
  printf ("Volume clean flag: %08X\n", swap32(dnixPartitionInfo.cleanfl));
  printf ("Timestamp %s\n", buffer);
  
  fseek (image, superblock_address, SEEK_SET);
  fread ( (void *) &s, sizeof sysfile, 1, image);
  memcpy ( (void *) &sysfile, (void *) &s, sizeof sysfile);
  
  sysfile.s_bmadr = swap32(s.s_bmadr);
  sysfile.s_inadr = swap32(s.s_inadr);
  sysfile.s_rtadr = swap32(s.s_rtadr);
  sysfile.s_swadr = swap32(s.s_swadr);
  sysfile.s_vlsiz = swap32(s.s_vlsiz);
  sysfile.s_bmsiz = swap32(s.s_bmsiz);
  sysfile.s_swsiz = swap32(s.s_swsiz);
  sysfile.s_bksiz = swap32(s.s_bksiz);
  sysfile.s_disiz = swap32(s.s_disiz);
  sysfile.s_insiz = swap32(s.s_insiz);
  sysfile.s_time = swap32(s.s_time); 
  
  printf ("Bitmap pointer: %08X\n", sysfile.s_bmadr);
  printf ("Inode list pointer: %08X\n", sysfile.s_inadr);
  printf ("Root file pointer: %08X\n", sysfile.s_rtadr);
  printf ("Swap area: %08X\n", sysfile.s_swadr);
  printf ("Volume size: %d\n", sysfile.s_vlsiz);
  printf ("Bitmap size: %d\n", sysfile.s_bmsiz);
  printf ("Swap area size: %d\n", sysfile.s_swsiz);
  printf ("Block size: %d\n", sysfile.s_bksiz);
  printf ("Size of a directory entry: %d\n", sysfile.s_disiz);
  printf ("Default size of inode list: %08X\n", sysfile.s_insiz);

  tim = sysfile.s_time;
  
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
  printf ("Time initiated: %s\n", buffer);
}

// Read a file - block number as specified  - store in buf


class DnixFile {
  struct dinode ino;
  FILE * image;
  char indir_level_one [2048];
  int  indir_level_one_block_no;
  char indir_level_two [2048];
  int  indir_level_two_block_no;
  char indir_level_three [2048];
  int  indir_level_three_block_no;
public:
  void init( FILE * image, struct dinode * inode );
  void readFileBlock( int block_no, void * buf );
};


void DnixFile::init(FILE * image, struct dinode * inode) {
  memcpy (&ino, inode, sizeof (struct dinode));
}

void DnixFile::readFileBlock ( int block_no, void * buf ) {
  int disk_address;
  if (block_no<11) {
    // direct blocks
    disk_address = ino.di_addr[block_no*3] * 65536 + ino.di_addr[block_no*3 + 1]*256 + ino.di_addr[block_no*3 + 2];
  } else if ((block_no > 10) && (block_no < 692)) {
    // address of first indirect block is in block 11.
    disk_address = ino.di_addr[30] * 65536 + ino.di_addr[31]*256 + ino.di_addr[32];
    if (disk_address != indir_level_one_block_no) {
      fseek (image, disk_address, SEEK_SET);
      fread (indir_level_one, 2048, 1, image);
    }
    disk_address = indir_level_one[(block_no-10)*3] * 65536 + indir_level_one[(block_no-10)*3 + 1]*256 + indir_level_one[(block_no-10)*3 + 2];

    // First level of indirect block
  } else if ((block_no > 693) && (block_no < 465816)) {
    
    // Second level of indirect block
    disk_address = ino.di_addr[33] * 65536 + ino.di_addr[34]*256 + ino.di_addr[35];
    if (disk_address != indir_level_two_block_no) {
      fseek (image, disk_address, SEEK_SET);
      fread (indir_level_two, 2048, 1, image);
      indir_level_two_block_no = disk_address;
    }
    disk_address = indir_level_two[((block_no-693)/682)*3] * 65536 + indir_level_two[((block_no-693)/682)*3 + 1]*256 + indir_level_two[((block_no-693)/682)*3 + 2];
    if (disk_address != indir_level_one_block_no) {
      fseek (image, disk_address, SEEK_SET);
      fread (indir_level_one, 2048, 1, image);
      indir_level_one_block_no = disk_address;
    }
    disk_address = indir_level_one[(block_no-10)*3] * 65536 + indir_level_one[(block_no-10)*3 + 1]*256 + indir_level_one[(block_no-10)*3 + 2];

  } else {
    // Third level of indirect block 
    disk_address = ino.di_addr[36] * 65536 + ino.di_addr[37]*256 + ino.di_addr[38];
    if (disk_address != indir_level_one_block_no) {
      fseek (image, disk_address, SEEK_SET);
      fread (indir_level_one, 2048, 1, image);
    }
    disk_address = ino.di_addr[(block_no-10)*3] * 65536 + ino.di_addr[(block_no-10)*3 + 1]*256 + ino.di_addr[(block_no-10)*3 + 2];

  }
  fseek (image, disk_address, SEEK_SET);
  fread (buf, 2048, 1, image);
}


DnixFs::DnixFs() {
}


int main (int argc, char ** argv) {
  FILE * image_file;
  int block_no;
  int size;
  class DnixFile file;
  struct direct * dir;
  int dir_cnt;
  int dir_block_cnt;
  struct dinode inode;
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
  dnixFs.readInode(INOROOT, &inode);
  if (inode.di_mode & 0x8000) {
    // Directory
    // Allocate memory
    dir = (struct direct *) malloc (2048);
    file.init(image_file, &inode);
    size = inode.di_size;
    block_no = 0;
    do {
      file.readFileBlock(block_no, (void * ) dir);
      // process all the entries in the directory
      dir_cnt=0;
      do {
	printf ("%s\n", dir[dir_cnt].d_name);
	dir_cnt++;
      } while (dir_cnt < 256 && dir[dir_cnt].d_ino != 0); 
      size -= 2048;
      block_no ++;
    } while (size > 0 && dir[255].d_ino !=0 );
  } else {
    // Ordinary file
  }
  
}



