#ifndef LINT
static char *rcsid="$Id: epinfo.c,v 2.12 1999/03/27 23:47:23 crosser Exp crosser $";
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
	$Log: epinfo.c,v $
	Revision 2.12  1999/03/27 23:47:23  crosser
	change gmtime to gltimetz (possible null pointer under Windows)

	Revision 2.11  1999/03/21 20:22:09  crosser
	help message typo

	Revision 2.10  1999/03/13 13:27:43  crosser
	add tzset

	Revision 2.9  1999/03/12 23:22:30  crosser
	clean zone handling

	Revision 2.8  1999/03/12 10:06:23  crosser
	new setfiletime

	Revision 2.7  1999/03/10 22:34:43  crosser
	lots of new functionality (rename, setutime) etc.

	Revision 2.6  1999/03/06 13:37:08  crosser
	Convert to autoconf-style

	Revision 2.5  1999/02/28 22:40:48  crosser
	Support Exif file format

	Revision 2.4  1998/10/18 13:18:27  crosser
	Put RCS logs and I.D. into the source

	Revision 2.3  1998/02/13 23:02:40  crosser
	use proper open mode on DOS
	
	Revision 2.2  1998/02/08 19:58:38  crosser
	Support low memory: chunked saving etc.
	
	Revision 2.1  1998/01/18 02:16:45  crosser
	DOS support
	
	Revision 2.0  1998/01/02 19:20:11  crosser
	Added support for Win32
	
	Revision 1.2  1997/12/24 00:19:13  crosser
	Do not check vendor identifier
	
	Revision 1.1  1997/08/17 08:59:54  crosser
	Initial revision
	
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif
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
#include "revision.h"
#include "a12scan.h"
#include "exifscan.h"
#include "fnames.h"
#include "ctimez.h"
#include "filetime.h"

#ifdef UNIX
#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#define READMODE "r"
#else
#include "getopt.h"
#include "strcscmp.h"
#define READMODE "rb"
#define MS_PROGRAM_NAMES
#endif

#ifdef DOS
#define MS_PROGRAM_NAMES
#pragma warn -par
#pragma warn -sus
#endif

static int usetimezone=1;
static int setutime=0;
static int dorename=0;
static char *nameformat=NULL;
static char *tag=NULL;

void showhelp(char *name) {
	printf("usage: %s [-h] [-V] [-z] [-r] [-t] [-f fmt] [-T TagName] <file> ...\n\n",name);
	printf("Options:\n\n");
	printf("\t-h\t- show this help screen\n");
	printf("\t-V\t- show software version number\n");
	printf("\t-z\t- suppress timezone conversions (camera's clock is local time)\n");
	printf("\t-r\t- rename files to the timestamp\n");
	printf("\t-t\t- set modification time of the files to the time of snapshot\n");
	printf("\t-f fmt\t- rename files using strftime(3) with the time of snapshot\n");
	printf("\t\t\t fmt = 'd' - name is seconds since the epoch in decimal\n");
	printf("\t\t\t fmt = 'x' - name is seconds since the epoch in hex\n");
#ifdef HAVE_STRFTIME
	printf("\t\t\t fmt = '1' - name is MMDD_CCC (default)\n");
	printf("\t\t\t fmt = '2' - name is YYMMDDCC\n");
	printf("\t\t\t fmt = '3' - name is YYYY_MM_DD-HH_MM_SS\n");
	printf("\t\t\t fmt = '4' - name is YYYY/MM/DD/HH_MM_SS\n");
	printf("\t\t\t otherwise make file names using strftime(3),\n");
	printf("\t\t\t if fmt contains `%%%%NNNd' then place count there\n");
#endif
}

void showversion(char *name) {
#ifdef UNIX
#if HAVE_UNAME
#define OS uts.sysname
	struct utsname uts;

	if (uname(&uts)) {
		strncpy(uts.sysname,"Unix",sizeof(uts.sysname));
		uts.sysname[sizeof(uts.sysname)-1]='\0';
	}
#else
#define OS "Generic UNIX"
#endif
#endif

#ifdef DOS
#define OS "MS-DOS"
#endif

#ifdef MSWINDOWS
#define OS "Windows 95/NT"
#endif

#ifndef OS
#define OS "Unknown OS"
#endif

	printf("%s version %s (%s) build %s (%s memory model)\n",
			name,VERSION,OS,__DATE__,
#ifdef LOWMEMORY
			"low"
#else
			"normal"
#endif
		);
	printf("     Copyright (c) 1997-1999 Eugene Crosser and Bruce Lightner\n");
	printf("     URL: http://www.average.org/digicam/\n");
}

int main(int argc,char *argv[]) {
	char *fname;
	char newname[MAXFORMAT+1];
	FILE *fp;
	struct stat st;
	unsigned char *buf,*res,*p,*q;
	off_t bufsize,ressize;
	int c,rc=0;
	char *program = argv[0];
	enum {NONE,APP12,EXIF} fformat;
	time_t ptime;
	struct tm *ptm,ptms;

#ifdef MS_PROGRAM_NAMES
	/* remove program extension */
	if ((q = strrchr(program, '.')) != NULL)
		*q = '\0';
	/* remove directory */
	if ((q = strrchr(program, '/')) != NULL)
		program = q + 1;
	if ((q = strrchr(program, '\\')) != NULL)
		program = q + 1;
#endif

	while ((c=getopt(argc,argv,"hVzrtf:T:")) != EOF)
	switch (c) {
	case 'f':
		nameformat=optarg;
		break;
	case 'T':
		tag=optarg;
		break;
	case 'z':
		usetimezone=0;
		break;
	case 't':
		setutime=1;
		break;
	case 'r':
		dorename=1;
		break;
	case 'h':
		showhelp(program);
		return 0;
	case 'V':
		showversion(program);
		return 0;
	default:
		showhelp(program);
		return 1;
	}

	if (!argv[optind]) {
		showhelp(program);
		return -1;
	}

#ifdef DOS
	/* MS/DOS has silly assumptions about the timezone if TZ variable
	   is not set.  Hope the following will help in most cases, the
	   results should be at least predictable... */
	if (getenv("TZ") == NULL) {
		putenv("TZ=GMT0");
		usetimezone = 0;
	}
#endif
	tzset();

	while ((fname=argv[optind++])) {

		if ((fp=fopen(fname,READMODE)) == NULL) {
			perror(fname);
			rc++;
			continue;
		}

		if (fstat(fileno(fp),&st)) {
			perror(fname);
			fclose(fp);
			rc++;
			continue;
		}

#ifdef LOWMEMORY
		if (st.st_size > MAXJPREFIX)
			bufsize=MAXJPREFIX;
		else
#endif
			bufsize=st.st_size;
		buf=malloc(bufsize);
		if (buf == NULL) {
			perror("malloc");
			fclose(fp);
			rc++;
			continue;
		}

		if (fread(buf,bufsize,1,fp) != 1) {
			perror("read");
			free(buf);
			fclose(fp);
			rc++;
			continue;
		}
		fclose(fp);

		res=buf;
		ressize=bufsize;
		fformat=NONE;
		if (a12scan(&res,&ressize) == 0)
			fformat=APP12;
		else {
			res=buf;
			ressize=bufsize;
			if (exifscan(&res,&ressize) == 0)
				fformat=EXIF;
		}
		if (fformat == NONE) {
			fprintf(stderr,"No camera-specific information in the file\n");
			free(buf);
			rc++;
			continue;
		}

		if (dorename || setutime) {
			if (tag) {
				fprintf(stderr,"-T cannot be used with -r and -t\n");
				free(buf);
				rc++;
				continue;
			}
			if (fformat == APP12) {
				p=a12search("TimeDate",res,ressize);
				if ((p == NULL) ||
				    (sscanf(p,"%lu",&ptime) != 1)) {
					fprintf(stderr,
						"TimeDate key not found\n");
					ptime=-1;
				}
			} else {
				p=exifsearch("DateTime",res,ressize);
				if ((p == NULL) ||
				    (sscanf(p,"\"%d:%d:%d %d:%d:%d\"",
						&ptms.tm_year,
						&ptms.tm_mon,
						&ptms.tm_mday,
						&ptms.tm_hour,
						&ptms.tm_min,
						&ptms.tm_sec) != 6)) {
					fprintf(stderr,
						"DateTime key not found\n");
					ptime=-1;
				} else {
					if (ptms.tm_mon) ptms.tm_mon-=1;
					if (ptms.tm_year) ptms.tm_year-=1900;
					ptime=mktime(&ptms);
					ptime+=zoneoffset(ptime);
				}
			}
			if (ptime == -1) {
				fprintf(stderr,"cannot rename/setutime\n");
				free(buf);
				rc++;
				continue;
			}
			if (dorename) {
				makename(newname,MAXFORMAT,"",IMAGE,
					ptime,nameformat,usetimezone);
				printf("rename %s -> %s\n",fname,newname);
#ifdef HAVE_RENAME
				if (rename(fname,newname)) {
					perror("rename");
					rc++;
				} else fname=newname; /* for setutime */
#else
				if (link(fname,newname)) {
					perror("link");
					rc++;
				} else {
					if (unlink(fname))
						perror("unlink");
					fname=newname; /* for setutime */
				}
#endif
			}
			if (setutime) {
				printf("set modtime %s: %s\n",
					fname,ctimetz(&ptime,usetimezone));
				if (setfiletime(fname,st.st_atime,ptime,
							usetimezone)) {
					perror("setfiletime");
					rc++;
				}
			}
		} else if (tag && (fformat == APP12) &&
			   (strcmp(tag,"DateTime") == 0)) {
			p=a12search("TimeDate",res,ressize);
			if (p && (sscanf(p,"%lu",&ptime) == 1)) {
				ptm=gltimetz(&ptime,0);
				printf("\"%04d:%02d:%02d %02d:%02d:%02d\"\n",
					ptm->tm_year+1900,
					ptm->tm_mon+1,
					ptm->tm_mday,
					ptm->tm_hour,
					ptm->tm_min,
					ptm->tm_sec);
			} else {
				fprintf(stderr,"TimeDate key not found\n");
				free(buf);
				rc++;
				continue;
			}
		} else if (tag) {
			if (fformat == APP12)
				p=a12search(tag,res,ressize);
			else p=exifsearch(tag,res,ressize);
			if (p == NULL) {
				fprintf(stderr,"Key not found\n");
				free(buf);
				rc++;
				continue;
			}
			if ((q=strchr(p,'\n'))) *q='\0';
			if ((q=strchr(p,'\r'))) *q='\0';
			printf("%s\n",p);
		} else {
			if (fformat == APP12)
				a12dump(res,ressize);
			else exifdump(res,ressize);
		}

		free(buf);
	}
	return rc;
}
