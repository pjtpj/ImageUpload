#ifndef LINT
static char *rcsid="$Id: filetime.c,v 1.4 1999/03/22 05:59:31 lightner Exp crosser $";
#endif

/*
	Copyright (c) 1997,1998 Eugene G. Crosser
	Copyright (c) 1998 Bruce D. Lightner (DOS/Windows support)

	You may distribute and/or use for any purpose modified or unmodified
	copies of this software if you preserve the copyright notice above.

	THIS SOFTWARE IS PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED.  IN NO EVENT WILL THE
	COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES RESULTING FROM THE
	USE OF THIS SOFTWARE.
*/

/*
	$Log: filetime.c,v $
	Revision 1.4  1999/03/22 05:59:31  lightner
	Use gltimetz() instead of gmtime()/localtime()

	Revision 1.3  1999/03/12 23:22:30  crosser
	support windows

	Revision 1.2  1999/03/12 10:06:23  crosser
	make it work under unix and dos (no windows yet)

	Revision 1.1  1999/03/11 22:57:35  crosser
	Initial revision

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#ifdef DOS
# include <io.h>
# include <fcntl.h>
#else
# ifdef HAVE_UTIME_H
#  include <utime.h>
# endif
# ifdef HAVE_SYS_UTIME_H
#  include <sys/utime.h>
# endif
#endif
#include "ctimez.h"

int setfiletime(char *fname,time_t actime,time_t mtime,int usetimezone) {

#ifdef DOS

	int fd,rc;
	struct tm *ptm;
#ifdef __TURBOC__
	struct ftime ft;
#else
	unsigned short dosdate,dostime;
#endif
/*
        yr = ((lrec.last_mod_file_date >> 9) & 0x7f) + (1980 - YRBASE);
        mo = ((lrec.last_mod_file_date >> 5) & 0x0f) - 1;
        dy = (lrec.last_mod_file_date & 0x1f) - 1;
        hh = (lrec.last_mod_file_time >> 11) & 0x1f;
        mm = (lrec.last_mod_file_time >> 5) & 0x3f;
        ss = (lrec.last_mod_file_time & 0x1f) * 2;
*/

	if ((fd=open(fname,O_RDWR)) == -1) return -1;
	ptm=gltimetz(&mtime,usetimezone);

#ifdef __TURBOC__
	ft.ft_tsec=ptm->tm_sec/2;
	ft.ft_min=ptm->tm_min;
	ft.ft_hour=ptm->tm_hour;
	ft.ft_day=ptm->tm_mday;
	ft.ft_month=ptm->tm_mon+1;
	ft.ft_year=ptm->tm_year-80;
	rc=setftime(fd,&ft);
#else
	dosdate=(ptm->tm_year-80)<<9;
	dosdate|=(ptm->tm_mon+1)<<5;
	dosdate|=(ptm->tm_mday);
	dostime=(ptm->tm_hour)<<11;
	dostime|=(ptm->tm_min)<<5;
	dostime|=(ptm->tm_sec/2);
	asm("movl %0, %%ebx": : "g" (fd));
	asm("movl %0, %%ecx": : "g" (dostime));
	asm("movl %0, %%edx": : "g" (dosdate));
	asm("movl $0x5701, %eax");
	asm("int $0x21": : : "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi");
	asm("movl %%eax, %0": : "g" (rc));
#endif
	close(fd);
	return rc;

#else /* UNIX or Windows */

	struct utimbuf utb;

	utb.actime=actime;
	utb.modtime=mtime;
	return utime(fname,&utb);

#endif
}
