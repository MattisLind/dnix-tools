/*
 * Inode structure as it appears on disk.
 * The 40 address bytes:
 *	39 used; 13 addresses
 *	of 3 bytes each.
 */
#ifndef _rhSYS_FS_DNIX_INODE
#define _rhSYS_FS_DNIX_INODE

struct dinode
{
	unsigned short	di_mode;	/* mode and type of file */
	short	di_nlink;	/* number of links to file */
	short	di_uid;		/* owner's user id */
	short	di_gid;		/* owner's group id */
	daddr_t	di_size;	/* number of bytes infile */
	char	di_addr[40];	/* disk block addresses */
	time_t	di_atime;	/* time last accessed */
	time_t	di_mtime;	/* time last modified */
	time_t	di_ctime;	/* time inode last modified */
};
#endif /* _rhSYS_FS_DNIX_INODE */

