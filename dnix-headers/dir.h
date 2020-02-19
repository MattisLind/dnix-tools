/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _rhSYS_FS_DNIX_DIR
#define _rhSYS_FS_DNIX_DIR

#ident	"@(#)kern-port:sys/fs/dnix/dir.h	10.1"
#ifndef	DIRSIZ
#define	DIRSIZ	14
#endif
struct	direct
{
	unsigned short	d_ino;
	char	d_name[DIRSIZ];
};
#endif /* _rhSYS_FS_DNIX_DIR */

