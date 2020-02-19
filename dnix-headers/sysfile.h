/*
 *	Copyright (C) Dataindustrier DIAB AB, Sweden, 1986
 */
#ifndef _rhSYS_FS_DNIX_SYSFILE
#define _rhSYS_FS_DNIX_SYSFILE

/*
 *	Structures used by filehandler
 *	SCCSID(@(#) sysfile.h	5.2.2.1)
 */
/*
 *	Structure of the system-file
 *
 *	This is a read-only structure for some vital volume parameters
 *	All numerical values are 32-bit	stored MSByte ... LSByte.
 *	Last byte is an overall checksum computed as
 *	the sum of all bytes modulo 256 and negated(so it will sum to 0).
 *
 *	Soft checksum stuff:
 *	If the "s_ckflg" flag is set, then soft checksum is enabled on this
 *	volume. This checksum is computed by XOR'ing each data block
 *	(s_bksiz bytes each) into a n-byte integer. The value of n is given by
 *	the value of "s_chkflg". n is preferrably four (4) on a 32-bit CPU.
 *	The checksum's are stored at s_vlsiz and beyond, one n-byte value
 *	for each logical block. There is a utility program to turn this
 *	feature on or off.
 */
struct sysfile {
	char	s_fhnd[16];	/* Filehandler spec */
	daddr_t	s_bmadr;	/* Bitmap pointer */
	daddr_t	s_inadr;	/* Inode list pointer */
	daddr_t	s_rtadr;	/* Root file pointer */
	daddr_t	s_swadr;	/* Swap area */
	daddr_t	s_vlsiz;	/* Volume size */
	daddr_t	s_bmsiz;	/* Bitmap size */
	daddr_t	s_swsiz;	/* Swap area size */
	daddr_t	s_bksiz;	/* Block size */
	daddr_t	s_disiz;	/* Size of a directory entry */
	daddr_t	s_insiz;	/* Default size of inode list */
	time_t	s_time;		/* Time initiated */
	char	s_fname[8];	/* Filsys name */
	char	s_fpack[8];	/* Filsys pack name */
	daddr_t	s_[44];		/* Reserved */
	char	s_c[2];		/* Reserved */
	char	s_ckflg;	/* Checksum on this volume */
	char	s_chksum;	/* Check sum of all bytes */
};
#define filsys	sysfile		/* To make diab and ATT compatible */
#endif /* _rhSYS_FS_DNIX_SYSFILE */

