#ifndef LINT
static char *rcsid="$Id: fnames.c,v 1.4 1999/03/27 23:52:29 crosser Exp crosser $";
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
	$Log: fnames.c,v $
	Revision 1.4  1999/03/27 23:52:29  crosser
	remove old check for valid pictm because it's not needed with gltimetz

	Revision 1.3  1999/03/22 05:59:31  lightner
	Include <windows.h> for Win32 compile
	Use gltimetz() instead of gmtime()/localtime()

	Revision 1.2  1999/03/10 22:34:43  crosser
	clenup

	Revision 1.1  1999/03/09 18:36:34  crosser
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
#include <errno.h>
#include "fnames.h"
#include "ctimez.h"

#if defined(MSWINDOWS)
#include <windows.h>
#endif

#ifdef MSWINDOWS
#define MS_PROGRAM_NAMES
#define ERRNO GetLastError()
#else
#define ERRNO errno
#endif

static char *shortcut[][4] = {
	{
		"%m%d_%%03d.jpg",
		"%y%m%d%%02d.jpg",
		"%Y_%m_%d-%H_%M_%S.jpg",
		"%Y/%m/%d/%H_%M_%S.jpg"
	},
	{
		"%m%d-%%03d.jpg",
		"%y%m%d%%02d.jpt",
		"%Y_%m_%d-%H_%M_%S-t.jpg",
		"%Y/%m/%d/%H_%M_%S-t.jpg"
	},
	{
		"%m%d_%%03d.wav",
		"%y%m%d%%02d.wav",
		"%Y_%m_%d-%H_%M_%S.wav",
		"%Y/%m/%d/%H_%M_%S.wav"
	}
};
#define MAXSHORTCUT '4'
static int ftype=0;

void makename(char *fname,int flen,char *filenm,int datatype,
			time_t pictime,char *nameformat,int usetimezone) {
	int count;
	struct stat st;
	struct tm *pictm;
	int hascount=0;
	int width=0;
	int maxcount;
	char *ifslash;
	char *p;

	if ((filenm[0] == '\0') ||
	    (filenm[strlen(filenm)-1] == '\\') ||
	    (filenm[strlen(filenm)-1] == '/' )) {
		ifslash="";
	} else {
		ifslash="/";
	}

	pictm=gltimetz(&pictime,usetimezone);

	if (nameformat) {
		if (strlen(nameformat) == 1) {
			if (*nameformat == 'd') {
				ftype=1;
				nameformat="%010lu";
			} else if (*nameformat == 'x') {
				ftype=1;
				nameformat="%08lx";
#ifdef HAVE_STRFTIME
			} else if ((*nameformat >= '1') &&
			    (*nameformat <= MAXSHORTCUT)) {
				ftype=2;
				nameformat=
					shortcut[datatype][(*nameformat)-'1'];
#endif
			} else {
				fprintf(stderr,
					"invalid shortcut `-f %c', using default\n",
					*nameformat);
				nameformat=NULL;
			}
		} else {
#ifdef HAVE_STRFTIME
			ftype=2;
#else
			fprintf(stderr,
				"format `-f %s' unsupported, using default\n",
				nameformat);
			nameformat=NULL;
#endif
		}
	}

	if (nameformat && (ftype == 2)) {
		/* see if there is a field for count */
		enum {deflt,escaped,pc1,pc2,digit} state=deflt;

		for (p=nameformat;*p;p++) switch (state) {
		case deflt:
			switch (*p) {
			case '\\': state=escaped; break;
			case '%': state=pc1; break;
			default: state=deflt; break;
			}
			break;
		case escaped:
			state=deflt;
			break;
		case pc1:
			switch (*p) {
			case '%': state=pc2; break;
			default: state=deflt; break;
			}
			break;
		case pc2:
			if ((*p >= '0') && (*p <= '9')) {
				state=digit;
				width*=10;
				width+=(*p-'0');
			} else
				state=deflt;
			break;
		case digit:
			if ((*p >= '0') && (*p <= '9')) {
				state=digit;
				width*=10;
				width+=(*p-'0');
			} else
				state=deflt;
			if (*p == 'd') hascount++;
			break;
		}
	}

	if (hascount > 1) {
		fprintf(stderr,"bad `-f %s' option: more than one count fields\n",
				nameformat);
		hascount=0;
		width=0;
	}

	if (hascount && ((width < 1) || (width > 8))) {
		fprintf(stderr,"bad count field width in `-f %s' option\n",
				nameformat);
		hascount=0;
		width=0;
	}

	switch (width) {
	case 1: maxcount=10; break;
	case 2: maxcount=100; break;
	default: maxcount=1000; break;
	}

	for (count=1;count<maxcount;count++) {
		if (ftype == 0) {
			sprintf(fname,"%s%s%02d%02d%s%03d.%s",
					filenm,
					ifslash,
					pictm->tm_mon+1,
					pictm->tm_mday,
					(datatype==THUMBNAIL)?"-":"_",
					count,
					(datatype==AUDIO)?"wav":"jpg");
			if ((stat(fname,&st) < 0) && (ERRNO == ENOENT)) {
				break;
			}
		} else if (ftype == 1) {
			char timestr[MAXFORMAT+1];
			char ext[5];

			sprintf(timestr,nameformat,pictime);
			if (count == 1) ext[0]='\0';
			else sprintf(ext,".%03d",count-1);
			sprintf(fname,"%s%s%s%s",
					filenm,
					ifslash,
					timestr,
					ext);
			if ((stat(fname,&st) < 0) && (ERRNO == ENOENT)) {
				break;
			}
#ifdef HAVE_STRFTIME
		} else if (ftype == 2) {
			char timestr[MAXFORMAT+1];
			char timestr2[MAXFORMAT+1];
			char ext[5];

			if (hascount) {
				(void)strftime(timestr2,MAXFORMAT-width,
					nameformat,pictm);
				sprintf(timestr,timestr2,count);
			} else {
				(void)strftime(timestr,MAXFORMAT,
					nameformat,pictm);
			}
			if ((hascount) || (count == 1)) ext[0]='\0';
			else sprintf(ext,".%03d",count-1);
			sprintf(fname,"%s%s%s%s",
					filenm,
					ifslash,
					timestr,
					ext);
			if ((stat(fname,&st) < 0) && (ERRNO == ENOENT)) {
				break;
			}
#endif /* HAVE_STRFTIME */
		}
	}

	/* Just for case (we should not have overwritten the buffer) */
	fname[flen-1]='\0';

	/* OK, now time to create intermediate directories */
#ifdef HAVE_MKDIR
	for (p=fname+strlen(filenm);*p;p++) if (*p == '/') {
		*p='\0';
		(void)mkdir(fname,0777);
		*p='/';
	}
#endif
}

