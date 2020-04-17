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
#include <limits.h>     /* PATH_MAX */

#include <errno.h>

typedef unsigned int daddr_t;
typedef int dnix_time_t;
typedef int dnix_ino_t;

#pragma pack(1)

#define time_t dnix_time_t
#define ino_t dnix_ino_t

#include "../dnix-headers/diskpar.h"
#include "../dnix-headers/inode.h"
#include "../dnix-headers/sysfile.h"
#include "../dnix-headers/dir.h"

#undef ino_t
#undef time_t
#define time_t time_t

#include <sys/stat.h>   /* mkdir(2) */

#include <utime.h>


#define swap32(num) (((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000)) 

#define swap16(num) (((num >> 8) & 0xff) | ((num << 8) & 0xff00))

class DnixFs {
  struct sysfptr dnixPartitionInfo;
  struct sysfile sysfile;
  FILE * image;
 public:
  DnixFs();
  void init( FILE * image );
  bool readInode (int inumber, struct dinode * inode);
};


bool DnixFs:: readInode (int inumber, struct dinode * inode) {
  struct dinode dinode;
  char buffer[26];
  struct tm * tm_info;
  time_t tim;
  int i;
  if (((inumber-1) * (sizeof dinode) + sysfile.s_inadr) > sysfile.s_insiz) {
    return false;
  }
  fseek (image, (inumber-1) * (sizeof dinode) + sysfile.s_inadr, SEEK_SET);
  fread ( (void * ) &dinode, sizeof dinode, 1, image);

  
  inode->di_mode = swap16(dinode.di_mode);
  inode->di_nlink = swap16(dinode.di_nlink);
  inode->di_uid = swap16(dinode.di_uid);
  inode->di_gid = swap16(dinode.di_gid);
  inode->di_size = swap32(dinode.di_size);
  inode->di_atime = swap32(dinode.di_atime);
  inode->di_mtime = swap32(dinode.di_mtime);
  inode->di_ctime = swap32(dinode.di_ctime);
  memcpy((void *) inode->di_addr, (void *) dinode.di_addr, 40);

  tim = inode->di_atime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  tim = inode->di_mtime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  tim = inode->di_ctime;
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

  return true;

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
  

  tim = sysfile.s_time;
  
  tm_info = localtime((time_t *) ( &tim));
  strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Read a file - block number as specified  - store in buf

struct BlockCache {
  int disk_address;
  int block[512];
};

class DnixFile {
  struct dinode ino;
  FILE * image;
  struct BlockCache blockCache [3];
  void updateBlockCache(long disk_address, int level);
public:
  void init( FILE * image, struct dinode * inode );
  void readFileBlock( int block_no, void * buf );
};


void DnixFile::updateBlockCache(long disk_address, int level) {
  if (disk_address != blockCache[level].disk_address) {
      fseek (image, disk_address, SEEK_SET);
      fread (blockCache[level].block, 2048, 1, image);
  }
}

void DnixFile::init(FILE * img, struct dinode * inode) {
  memcpy (&ino, inode, sizeof (struct dinode));
  image = img;
}

void DnixFile::readFileBlock ( int block_no, void * buf ) {
  long disk_address;
  if (block_no<11) {
    // direct blocks
    disk_address = (((0xff& ino.di_addr[block_no*3]) <<16 ) | ((0xff & ino.di_addr[block_no*3 + 1]) <<8 ) | (ino.di_addr[block_no*3 + 2] & 0xff)) <<8;
  } else if ((block_no > 10) && (block_no < 522)) {
    disk_address = (((0xff& ino.di_addr[30]) <<16 ) | ((0xff & ino.di_addr[31]) <<8 ) | (ino.di_addr[32] & 0xff))<<8;
    updateBlockCache(disk_address, 0);
    disk_address = swap32(blockCache[0].block[block_no-11]);
  } else if ((block_no >= 522) && (block_no < 262666)) {
    // Second level of indirect block
    disk_address = (((0xff& ino.di_addr[33]) <<16 ) | ((0xff & ino.di_addr[34]) <<8 ) | (ino.di_addr[35] & 0xff))<<8;
    updateBlockCache(disk_address, 1);
    disk_address = swap32(blockCache[1].block[(block_no-522)>>9]);
    updateBlockCache(disk_address, 0);
    disk_address = swap32(blockCache[0].block[(block_no-522) & 0x1ff]);
  } else {
    // Third level of indirect block 
    disk_address = (((0xff& ino.di_addr[36]) <<16 ) | ((0xff & ino.di_addr[37]) <<8 ) | (ino.di_addr[38] & 0xff))<<8;
    updateBlockCache(disk_address, 2);
    disk_address = swap32(blockCache[2].block[(block_no-262666) >> 18]);
    updateBlockCache(disk_address, 1);
    disk_address = swap32(blockCache[1].block[((block_no-262666) >>9) & 0x1ff]);
    updateBlockCache(disk_address, 0);
    disk_address = swap32(blockCache[0].block[(block_no-262266) & 0x1ff]);
  }
  fseek (image, disk_address, SEEK_SET);
  fread (buf, 2048, 1, image);
}


DnixFs::DnixFs() {
}


int mkdir_p(const char *path)
{
  /* Adapted from http://stackoverflow.com/a/2336245/119527 */
  const size_t len = strlen(path);
  char _path[PATH_MAX];
  char *p; 

  errno = 0;

  /* Copy string so its mutable */
  if (len > sizeof(_path)-1) {
    errno = ENAMETOOLONG;
    return -1; 
  }   
  strcpy(_path, path);

  /* Iterate the string */
  for (p = _path + 1; *p; p++) {
    if (*p == '/') {
      /* Temporarily truncate */
      *p = '\0';

      if (mkdir(_path, S_IRWXU) != 0) {
	if (errno != EEXIST)
	  return -1; 
      }

      *p = '/';
    }
  }   

  if (mkdir(_path, S_IRWXU) != 0) {
    if (errno != EEXIST)
      return -1; 
  }   

  return 0;
}



void readDir(int inumber, FILE * image_file, class DnixFs * dnixFs, char * path) {
  char p [1024];
  const char * slash = "/";
  struct direct * dir;
  class DnixFile * file = new class DnixFile;
  int size;
  FILE *output;
  int block_no=0;
  int dir_cnt;
  bool ret;
  struct utimbuf timebuf;
  struct dinode inode;
  ret = dnixFs->readInode(inumber, &inode);
  if (!ret) return;
  dir = (struct direct *) malloc (2048);
  file->init(image_file, &inode);
  size = inode.di_size;
  if (inode.di_mode & 0x4000) {
    mkdir_p(path);
    block_no = 0;
    do {
      file->readFileBlock(block_no, (void * ) dir);
      dir_cnt=0;
      do {
	if (((strncmp(dir[dir_cnt].d_name,".",1)!=0) && (strncmp(dir[dir_cnt].d_name,"..",2)!=0)) || (strlen(dir[dir_cnt].d_name) > 2)) {
	  memset(p,0,sizeof p);
	  strcpy(p, path);
	  strcat(p, slash);
	  strncat(p, dir[dir_cnt].d_name,14);
	  readDir(swap16(dir[dir_cnt].d_ino), image_file, dnixFs, p);
	}
	dir_cnt++;
      } while ((dir_cnt < 128) && (swap16(dir[dir_cnt].d_ino) != 0)); 
      size -= 2048;
      block_no ++;
    } while (size > 0 && swap16(dir[255].d_ino) !=0 );
  } else {    
    // Ordinary file
    output = fopen (path, "w");
    do {
      file->readFileBlock(block_no, (void * ) dir);
      if (size >= 2048) {
	fwrite(dir, 1, 2048, output);
      } else {
	fwrite(dir, 1, size, output);
      }
      size -= 2048;
      block_no ++;
    } while (size > 0);
    fclose(output);
  }
  chmod (path, inode.di_mode & 07777);
  chown (path, inode.di_gid, inode.di_uid);
  timebuf.actime = inode.di_atime;
  timebuf.modtime = inode.di_mtime;
  utime(path, &timebuf);
  delete file;
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
  readDir(INOROOT, image_file, &dnixFs, (char *) ".");
}



