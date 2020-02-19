/*
 *	Copyright (C) Dataindustrier DIAB AB, Sweden, 1986
 */
/*
 *	Structures used by filehandler
 *	SCCSID(@(#) diskpar.h	5.2.2.1)
 */
#ifndef _rhSYS_FS_DNIX_DISKPAR
#define _rhSYS_FS_DNIX_DISKPAR

/*
 * Disk definition equs
 */
/*
 * Fixed inode numbers
 */
#define INOBDSP	((ino_t)1)	/* i number of badspot file */
#define INOROOT	((ino_t)2)	/* i number of all roots */
#define INOLIST	((ino_t)3)	/* i number of inode list */
#define INOBITM	((ino_t)4)	/* i number of bitmap file */
#define INOVDSF	((ino_t)5)	/* i number of volume description file */

#define INO_LAST INOVDSF	/* Last of the fixed inode numbers */

/*
 *	First part of a file system volume
 */
#define BOOTREC	((daddr_t)0)	/* First 512 bytes set aside for booting */
#define SYSFPTR	((daddr_t)512)	/* 512 bytes for pointer record */
#define DRVPAR	((daddr_t)1024)	/* 512 bytes set aside for disk driver use */
#define VUTIL	((daddr_t)1536)	/* 512 bytes reserved */
#define LOCFREE	((daddr_t)2048)	/* possible start of file system */
/*
 *	structure of the pointer record at SYSFPTR
 */
struct sysfptr {
	daddr_t	vdsp;		/* Pointer to volume descriptor block */
	daddr_t	vdspn;		/* 1:s complement of previous item */
	daddr_t	cleanfl;	/* Volume clean flag */
	dnix_time_t	timestamp;	/* Timestamp */
	daddr_t	x[124];		/* Reserved */
};

#define CLEANFL	((daddr_t)0x76543210)	/* Volume clean magic number */
#endif /* _rhSYS_FS_DNIX_DISKPAR */

