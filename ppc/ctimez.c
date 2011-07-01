#ifndef LINT
static char *rcsid="$Id: ctimez.c,v 1.5 1999/03/27 23:47:05 crosser Exp crosser $";
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
	$Log: ctimez.c,v $
	Revision 1.5  1999/03/27 23:47:05  crosser
	cosmetic

	Revision 1.4  1999/03/22 05:59:31  lightner
	Add gltimetz() routine for Win32 localtime/gmtime() NULL pointer returns

	Revision 1.3  1999/03/12 23:22:30  crosser
	clean zone handling

	Revision 1.2  1999/03/10 22:34:43  crosser
	notimezone -> usetimezone

	Revision 1.1  1999/03/09 18:30:22  crosser
	Initial revision

*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
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

#include "ctimez.h"

struct tm *gltimetz(time_t *clock,int usetimezone) {
	struct tm *tm;
	static struct tm null_tm;

	if (usetimezone) tm=localtime(clock);
	else tm=gmtime(clock);
	if (!tm) {	/* Windows can return NULL!! */
		null_tm.tm_sec = 0;	/* Seconds after minute (0-59) */
		null_tm.tm_min = 0;	/* Minutes after hour (0-59) */
		null_tm.tm_hour = 0;	/* Hours since midnight (0-23) */
		null_tm.tm_mday = 1;	/* Day of month (1-31) */
		null_tm.tm_mon = 0;	/* Month (0-11; January=0) */
		null_tm.tm_year = 0;	/* Year (current year minus 1900) */
		null_tm.tm_wday = 1;	/* Day of week (0-6; Sunday=0) */
		null_tm.tm_yday = 0;	/* Day of year (0-365; January 1=0) */
		null_tm.tm_isdst = 0;	/* Always 0 for gmtime */

		tm = &null_tm;
	}
	return tm;
}

char *ctimetz(time_t *clock,int usetimezone) {
	static char tmbuf[80];
	struct tm *camtm;
	char *timefmt;

	if ((*clock) == (time_t)-1) {
		strcpy(tmbuf,"N/A");
		return tmbuf;
	}
	camtm=gltimetz(clock,usetimezone);
#ifdef HAVE_STRFTIME
	if (usetimezone) timefmt="%a %b %d %H:%M:%S %Y %Z";
	else timefmt="%a %b %d %H:%M:%S %Y";
	(void)strftime(tmbuf,sizeof(tmbuf),timefmt,camtm);
#else
	strncpy(tmbuf,asctime(camtm),sizeof(tmbuf));
	tmbuf[sizeof(tmbuf)-1]='\0'; /* paranoia */
	tmbuf[strlen(tmbuf)-1]='\0'; /* remove newline */
#endif
	return tmbuf;
}

time_t zoneoffset(time_t now) {
#ifndef HAVE_TM_ZONE
	struct tm gtm;
	time_t offset;
#endif
	struct tm ltm;

	ltm=*localtime(&now);
#ifdef HAVE_TM_ZONE
	return ltm.tm_gmtoff;
#else
	/* Compute timezone difference in seconds.
	   This is the difference between gmtime() and
	   locatime() return values. */

	gtm=*gmtime(&now);
	/* Assume we are never more than 24 hours away. */
	offset=gtm.tm_yday-ltm.tm_yday;
	if (offset > 1) offset=-24;
	else if (offset < -1) offset=24;
	else offset*=24;
	offset+=gtm.tm_hour-ltm.tm_hour;
	offset*=60;
	offset+=gtm.tm_min-ltm.tm_min;
	offset*=60;
	offset+=gtm.tm_sec-ltm.tm_sec;
	return -offset;
#endif
}
