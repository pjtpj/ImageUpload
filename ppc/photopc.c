#ifndef LINT
static char *rcsid="$Id: photopc.c,v 2.49 1999/04/11 11:10:09 crosser Exp crosser $";
#endif

/*
	Copyright (c) 1997-1999 Eugene G. Crosser
	Copyright (c) 1998,1999 Bruce D. Lightner (DOS/Windows support)

	You may distribute and/or use for any purpose modified or unmodified
	copies of this software if you preserve the copyright notice above.

	THIS SOFTWARE IS PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY
	KIND, EITHER EXPRESSED OR IMPLIED.  IN NO EVENT WILL THE
	COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES RESULTING FROM THE
	USE OF THIS SOFTWARE.
*/

/*
	$Log: photopc.c,v $
	Revision 2.49  1999/04/11 11:10:09  crosser
	Add comment about the first usleep() call and do it on non-unix only

	Revision 2.48  1999/04/10 16:31:34  lightner
	Change bad switch printout to match bad command printout
	Add usleep(0L) call to calibrate delays at startup

	Revision 2.47  1999/03/22 06:08:44  lightner
	No showhelp() on bad command switches (Windows/DOS errors got lost!)

	Revision 2.46  1999/03/21 20:22:09  crosser
	help message typo
	properly check if the path name is directory if needed

	Revision 2.45  1999/03/13 13:27:43  crosser
	add tzset

	Revision 2.44  1999/03/13 13:10:45  crosser
	fix usage message

	Revision 2.43  1999/03/12 23:22:30  crosser
	cleanup timezone handling

	Revision 2.42  1999/03/12 10:36:41  crosser
	fix writing

	Revision 2.41  1999/03/12 10:06:23  crosser
	repair low memory model and misc changes

	Revision 2.40  1999/03/12 01:31:05  crosser
	cleanups

	Revision 2.39  1999/03/10 22:34:43  crosser
	set utime; cleanup

	Revision 2.38  1999/03/06 13:37:08  crosser
	Convert to autoconf-style

	Revision 2.37  1999/02/26 23:54:42  crosser
	Correct JFIF vs. Exif vs. JPEG things

	Revision 2.36  1999/02/08 09:37:40  crosser
	fix help message

	Revision 2.35  1999/01/17 13:41:34  crosser
	fix -V message

	Revision 2.34  1999/01/17 13:32:03  crosser
	change copyright info

	Revision 2.33  1999/01/17 09:41:45  crosser
	fix bug imposed with audio file handling

	Revision 2.32  1998/12/20 21:49:38  crosser
	implement "audio" command

	Revision 2.31  1998/12/06 08:42:43  crosser
	remove usleep()s - apparently they do not make any difference

	Revision 2.30  1998/12/06 08:40:34  crosser
	better way to obtain picture date for the file name

	Revision 2.29  1998/11/19 11:04:31  crosser
	clean up things when timedate is not available in the file

	Revision 2.28  1998/10/18 19:27:31  crosser
	Insert sleep()s before eph_action, see if it helps Nikon...

	Revision 2.27  1998/10/18 19:07:36  crosser
	renamed "list" to "llist", make new "list" that works on all models.

	Revision 2.26  1998/10/18 13:47:59  crosser
	change 3 and 4 -f patterns

	Revision 2.25  1998/10/18 13:18:27  crosser
	Put RCS logs and I.D. into the source

	Revision 2.24  1998/08/08 22:59:42  crosser
	check error from fclose, change error reportd from fwrite
	
	Revision 2.23  1998/08/08 22:16:33  crosser
	More timezone things
	
	Revision 2.22  1998/08/08 14:00:33  crosser
	make things more Olympus friendly, work on zimezone handling
	
	Revision 2.21  1998/08/01 13:12:41  lightner
	Four port for DOS, minor fix
	
	Revision 2.20  1998/07/21 22:25:07  crosser
	Allow numeric resolutions: come models have more than three
	allregs do not display 14 and 15 reg to avoid hanging
	reporterr() print timeout error if -v specified
	
	Revision 2.19  1998/06/11 07:23:26  crosser
	fix bug with -last interval
	
	Revision 2.18  1998/06/07 18:41:40  crosser
	frame intervals, digital zoom
	
	Revision 2.17  1998/06/07 13:55:20  crosser
	fix download in low memory model
	
	Revision 2.16  1998/03/01 18:57:04  crosser
	preview command
	
	Revision 2.15  1998/02/27 04:37:11  lightner
	Fix progress report bug (total image size too large)
	
	Revision 2.14  1998/02/27 03:44:49  lightner
	Image download progress report bug (total bytes low if ! LOW_MEMORY)
	
	Revision 2.13  1998/02/26 00:50:39  crosser
	change timezone stuff
	change time output routine
	add error callback to suppress timeout messages
	maybe other misc things
	
	Revision 2.12  1998/02/25 22:43:40  crosser
	fix termios data saving
	
	Revision 2.11  1998/02/16 06:17:36  lightner
	Add -V option
	Add -z option (camera time is local time)
	Fix bug with MS dir names of form "A:/"
	Validate timezone processing under MS-DOS/Windows
	Fix spelling of "quiet" (quite->quiet)
	
	Revision 2.10  1998/02/13 23:02:40  crosser
	define type off_t on DOS
	
	Revision 2.9  1998/02/08 19:58:38  crosser
	Support low memory: chunked saving etc.
	
	Revision 2.8  1998/02/06 09:22:13  crosser
	comment buffer roundup, fix ISDIR define
	
	Revision 2.7  1998/02/05 23:31:23  lightner
	Special define for S_ISDIR macro if DOS
	Add extra 2048 bytes when allocating image buffer (avoids realloc())
	
	Revision 2.6  1998/01/27 21:52:55  crosser
	upload command
	
	Revision 2.5  1998/01/18 02:16:45  crosser
	DOS support
	
	Revision 2.4  1998/01/11 21:20:21  crosser
	off command, malloc/free cleanup
	
	Revision 2.3  1998/01/05 22:02:14  crosser
	disable gebug printout about scheduling policy
	
	Revision 2.2  1998/01/04 13:55:57  crosser
	new commands: protect, list
	Change close mode
	
	Revision 2.1  1998/01/03 19:55:24  crosser
	locktoggle command
	new -f variants
	
	Revision 2.0  1998/01/02 19:20:11  crosser
	Added support for Win32
	
	Revision 1.4  1997/12/24 00:36:17  crosser
	fix help message
	
	Revision 1.3  1997/12/24 00:19:13  crosser
	add new commands
	add formatted file names
	show more information in query
	
	Revision 1.2  1997/11/15 19:20:51  crosser
	photopc 600 specific commands/parameters
	and fix for making files when camera time not set
	
	Revision 1.1  1997/08/17 08:59:54  crosser
	Initial revision
	
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "eph_io.h"
#include "a12scan.h"
#include "fnames.h"
#include "ctimez.h"
#include "filetime.h"
#include "revision.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
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
#include <errno.h>
#ifdef UNIX
#ifdef HAVE_SCHED_H
#include <sched.h>
#endif
#ifdef HAVE_UNAME
#include <sys/utsname.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#else
#include "getopt.h"
#include "strcscmp.h"
#endif

#ifdef DOS
#define MS_PROGRAM_NAMES
#pragma warn -par
#pragma warn -sus
#endif

#ifdef MSWINDOWS
#define MS_PROGRAM_NAMES
#define ERRNO GetLastError()
#else
#define ERRNO errno
#endif

#ifdef UNIX
#define WRITEMODE "w"
#define READMODE  "r"
#else
#define WRITEMODE "wb"
#define READMODE  "rb"
#endif

#ifndef S_ISDIR
#define S_ISDIR(st_mode) ((S_IFDIR & (st_mode)) ? 1 : 0)
#endif

#ifdef UNIX
static char *device="/dev/photopc";
#else
static char *device="COM1:";
#endif
static int debug=0;
static int quiet=0;
static int usetimezone=1;
static int setutime=0;
static unsigned long filesize=0L;
static long frame=0L;
static char *nameformat=NULL;
static int switchoff=0;
static FILE *fp = NULL;
static char *fname = NULL;

void file_abort_cleanup(void) {
	if (fp) {
		fclose(fp);
		fp=NULL;
	}
	if (fname) {
		printf("\ndeleting file %s\n", fname);
		unlink(fname);
		free(fname);
		fname=NULL;
	}
}

int probe(eph_iob *iob) {
	long ret;

	if (eph_getint(iob,1,&ret)) return -1;
	else return 0;
}

int setclock(eph_iob *iob,int argc,char *argv[]) {
	time_t now,new;

	(void)time(&now);

	if (!usetimezone) {
		now+=zoneoffset(now);
	}

	if (eph_setint(iob,2,now)) return -1;
	if (eph_getint(iob,2,&new)) return -1;
	if (labs(now-new) > 59L) {
		fprintf(stderr,"time we tried to set does not match the result\n");
		return -1;
	} else {
		return 0;
	}
}

int resolution(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"Lo") == 0) {
		val=1;
	} else if (strcasecmp(argv[1],"Hi") == 0) {
		val=2;
	} else if (strcasecmp(argv[1],"Ext") == 0) {
		val=3;
	} else if ((val=atol(argv[1])) == 0) {
		fprintf(stderr,"bad resolution `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,1,val);
}

int shutter(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"Auto") == 0) {
		val=0L;
	} else if ((strlen(argv[1]) > 2) && (strncmp(argv[1],"1/",2) == 0)) {
		val=atol(argv[1]+2);
		if (val <= 0) {
			fprintf(stderr,"bad shutter speed `%s'\n",argv[1]);
			return -1;
		} else {
			val=1000000L/val;
		}
	} else {
		if ((val=atol(argv[1])) <= 0) {
			fprintf(stderr,"bad shutter speed `%s'\n",argv[1]);
			return -1;
		}
	}

	return eph_setint(iob,3,val);
}

int flash(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"Auto") == 0) {
		val=0;
	} else if (strcasecmp(argv[1],"Force") == 0) {
		val=1;
	} else if (strcasecmp(argv[1],"Off") == 0) {
		val=2;
	} else if (strcasecmp(argv[1],"Antiredeye") == 0) {
		val=3;
	} else if (strcasecmp(argv[1],"Slowsync") == 0) {
		val=4;
	} else {
		fprintf(stderr,"bad flash mode `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,7,val);
}

int autoshut_host(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if ((val=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad timer value `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,0x17,val);
}

int autoshut_field(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if ((val=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad timer value `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,0x18,val);
}

int lcd_autoshut(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if ((val=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad timer value `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,38,val);
}

int lcd_brightness(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (((val=atol(argv[1])) <= 0) || (val > 7)) {
		fprintf(stderr,"bad brighness value `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,35,val);
}

int setid(eph_iob *iob,int argc,char *argv[]) {
	return eph_setvar(iob,0x16,argv[1],strlen(argv[1]));
}

int macro(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"On") == 0) {
		val=1;
	} else if (strcasecmp(argv[1],"Off") == 0) {
		val=2;
	} else {
		fprintf(stderr,"bad macro mode `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,33,val);
}

int color(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"On") == 0) {
		val=1;
	} else if (strcasecmp(argv[1],"Off") == 0) {
		val=2;
	} else {
		fprintf(stderr,"bad color mode `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,6,val);
}

int dzoom(eph_iob *iob,int argc,char *argv[]) {
	long val=0;

	if (strcasecmp(argv[1],"On") == 0) {
		val=8;
	} else if (strcasecmp(argv[1],"Off") == 0) {
		val=0; /* not sure if it is correct */
	} else {
		fprintf(stderr,"bad dzoom mode `%s'\n",argv[1]);
		return -1;
	}

	return eph_setint(iob,72,val);
}

int seti(eph_iob *iob,int argc,char *argv[]) {
	long reg;
	long val;

	if ((reg=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad register value \"%s\"\n",argv[1]);
		return -1;
	}
	if (strspn(argv[2],"0123456789") != strlen(argv[2])) {
		fprintf(stderr,"bad set value \"%s\"\n",argv[2]);
		return -1;
	}
	val=atol(argv[2]);
	return eph_setint(iob,reg,val);
}

int setv(eph_iob *iob,int argc,char *argv[]) {
	long reg;

	if ((reg=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad register value \"%s\"\n",argv[1]);
		return -1;
	}
	return eph_setvar(iob,reg,argv[2],strlen(argv[2]));
}

int snapshot(eph_iob *iob,int argc,char *argv[]) {
	char zero=0;

	if (eph_action(iob,2,&zero,1)) return -1;
	else return 0;
}

int erase(eph_iob *iob,int argc,char *argv[]) {
	char zero=0;
	long frame;

	if ((frame=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad frame number %s\n",argv[1]);
		return -1;
	}

	if (eph_setint(iob,4,frame)) return -1;
	if (eph_action(iob,7,&zero,1)) return -1;
	else return 0;
}

int protect(eph_iob *iob,int argc,char *argv[]) {
	char pmode;
	long frame;

	if (strcasecmp(argv[2],"On") == 0) {
		pmode=1;
	} else if (strcasecmp(argv[2],"Off") == 0) {
		pmode=0;
	} else {
		fprintf(stderr,"bad protect mode `%s'\n",argv[1]);
		return -1;
	}
	if ((frame=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad frame number %s\n",argv[1]);
		return -1;
	}

	if (eph_setint(iob,4,frame)) return -1;
	if (eph_action(iob,9,&pmode,1)) return -1;
	else return 0;
}

int eraseall(eph_iob *iob,int argc,char *argv[]) {
	char zero=0;

	if (eph_action(iob,1,&zero,1)) return -1;
	else return 0;
}

int cmd(eph_iob *iob,int argc,char *argv[]) {
	long ccode;
	int intarg=-1;
	char chararg;

	if ((ccode=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad code value \"%s\"\n",argv[1]);
		return -1;
	}
	if (strspn(argv[2],"0123456789") == strlen(argv[2]))
		intarg=atol(argv[2]);
	if ((intarg >= 0) && (intarg <= 255)) {
		chararg=intarg;
		return eph_action(iob,ccode,&chararg,1);
	} else
		return eph_action(iob,ccode,argv[2],strlen(argv[2]));
}

static char *flashval[] = {"Auto","Force","Off","AntiRedeye","SlowSync"};
static char *resval[] = {"-bad-","Low","High","Extended"};

int query(eph_iob *iob,int argc,char *argv[]) {
	unsigned long result;
	char *buffer;
	off_t bufsize;
	int rc;

	buffer=malloc(2048);

	if ((rc=eph_getint(iob,1,&result)) == 0)
		printf("Resolution: %lu - %s\n",(unsigned long)result,
			(result < 4)?resval[result]:"Model-specific");
	else if (rc == DC1)
		printf("Resulution unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,2,&result)) == 0) {
		if ((result == 0L) || (result == (unsigned long)-1L)) {
			printf("Camera time: not set (%ld)\n",result);
		} else {
			printf("Camera time: %s\n",
					ctimetz((time_t*)&result,usetimezone));
		}
	} else if (rc == DC1)
		printf("Camera time unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,3,&result)) == 0)
		printf("Shutter: %lu (1/%lu)\n",(unsigned long)result,
			(unsigned long)(result?1000000/result:0L));
	else if (rc == DC1)
		printf("Shutter unavailable\n");
	else goto failure;
/*
	if ((rc=eph_getint(iob,5,&result)) == 0)
		printf("Reg 5: %lu\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Reg 5 unavailable\n");
	else goto failure;
*/
	if ((rc=eph_getint(iob,6,&result)) == 0)
		printf("Color mode: %lu - %s\n",result,
		(result == 1)?"Color":(result == 2)?"B/W":"Bad value");
	else if (rc == DC1)
		printf("Color mode unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,7,&result)) == 0)
		printf("Flash: %lu - %s\n",(unsigned long)result,
			(result < 5)?flashval[result]:"Bad value");
	else if (rc == DC1)
		printf("Flash mode unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,10,&result)) == 0)
		printf("Frames taken: %lu\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Frames taken unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,11,&result)) == 0)
		printf("Frames left: %lu\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Frames left unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,16,&result)) == 0)
		printf("Battery: %lu%%\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Battery capacity unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,23,&result)) == 0)
		printf("Autoshut on host: %lu sec\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Autoshut on host timer unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,24,&result)) == 0)
		printf("Autoshut on field: %lu sec\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Autoshut on field timer unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,28,&result)) == 0)
		printf("Free memory: %lu bytes\n",(unsigned long)result);
	else if (rc == DC1)
		printf("Free memory size unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,33,&result)) == 0)
		printf("Macro mode: %lu - %s\n",result,
		(result == 1)?"On":(result == 2)?"Off":"Bad value");
	else if (rc == DC1)
		printf("Macro mode unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,35,&result)) == 0)
		printf("LCD brightness: %lu (of 7)\n",(unsigned long)result);
	else if (rc == DC1)
		printf("LCD brightness unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,38,&result)) == 0)
		printf("LCD autoshut: %lu sec\n",(unsigned long)result);
	else if (rc == DC1)
		printf("LCD autoshut timer unavailable\n");
	else goto failure;
/*
	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,0x08,&buffer,&bufsize)) == 0)
		printf("Reg 8: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Reg 8 unavailable\n");
	else goto failure;
*/
	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,0x16,&buffer,&bufsize)) == 0)
		printf("Camera I.D.: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Camera I.D. unavailable\n");
	else goto failure;

	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,0x19,&buffer,&bufsize)) == 0)
		printf("Serial No.: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Serial No. unavailable\n");
	else goto failure;

	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,0x1a,&buffer,&bufsize)) == 0)
		printf("Version: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Version unavailable\n");
	else goto failure;

	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,0x1b,&buffer,&bufsize)) == 0)
		printf("Model: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Model unavailable\n");
	else goto failure;

	bufsize=2048; buffer[0]='\0';
	if ((rc=eph_getvar(iob,48,&buffer,&bufsize)) == 0)
		printf("Manufacturer: \"%s\"\n",buffer);
	else if (rc == DC1)
		printf("Manufacturer unavailable\n");
	else goto failure;

	if ((rc=eph_getint(iob,72,&result)) == 0)
		printf("Digital zoom mode: %lu - %s\n",result,
		(result <= 1)?"Off":"On");
	else if (rc == DC1)
		printf("Digital zoom mode unavailable\n");
	else goto failure;

	free(buffer);
	return 0;

failure:
	free(buffer);
	return -1;
}

int count(eph_iob *iob,int argc,char *argv[]) {
	unsigned long result;

	if (eph_getint(iob,0x0a,&result)) {
		printf("-1\n");
		return -1;
	}
	else printf("%lu\n",result);
	return 0;
}

int geti(eph_iob *iob,int argc,char *argv[]) {
	long res;
	long reg;

	if ((reg=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad register value \"%s\"\n",argv[1]);
		return -1;
	}

	if (eph_getint(iob,reg,&res)) return -1;
	printf("Reg %ld=%ld\n",reg,res);
	return 0;
}

int getv(eph_iob *iob,int argc,char *argv[]) {
	char *buffer=malloc(2048);
	off_t bufsize=2048;
	int i;
	long reg;

	if ((reg=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad register value \"%s\"\n",argv[1]);
		free(buffer);
		return -1;
	}

	buffer[0]='\0';
	if (eph_getvar(iob,reg,&buffer,&bufsize)) {
		free(buffer);
		return -1;
	}
	printf("Reg %ld=\"",reg);
	for (i=0;i<bufsize;i++) {
		if ((buffer[i] >= ' ') && (buffer[i] <= 'z')) {
			printf("%c",buffer[i]);
		} else {
			printf("\\%03o",(unsigned char)buffer[i]);
		}
	}
	printf("\"\n");
	free(buffer);
	return 0;
}

int camsum(eph_iob *iob,int argc,char *argv[]) {
	long res;
	char *buf=malloc(32);
	off_t bufsize=32;
	int i;

	if (eph_getvar(iob,46,&buf,&bufsize)) {
		fprintf(stderr,"cannot get summary data\n");
		return -1;
	}
	for (i=0;i<32;i+=4) {
		res=(long)(buf[i]&0xff)+((long)(buf[i+1]&0xff)<<8)+
			((long)(buf[i+2]&0xff)<<16)+((long)(buf[i+2]&0xff)<<24);
		printf("%d: %08lx\n",i/4,res);
	}
	free(buf);
	return 0;
}

int picsum(eph_iob *iob,int argc,char *argv[]) {
	long res;
	char *buf=malloc(32);
	off_t bufsize=32;
	long frame;
	int i;

	if ((frame=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad frame value \"%s\"\n",argv[1]);
		return -1;
	}
	if (eph_setint(iob,4,frame)) {
		fprintf(stderr,"cannot set frame frame \"%s\"\n",argv[1]);
		return -1;
	}

	if (eph_getvar(iob,47,&buf,&bufsize)) {
		fprintf(stderr,"cannot get summary data\n");
		return -1;
	}
	for (i=0;i<32;i+=4) {
		res=(long)(buf[i]&0xff)+((long)(buf[i+1]&0xff)<<8)+
			((long)(buf[i+2]&0xff)<<16)+((long)(buf[i+2]&0xff)<<24);
		printf("%d: %08lx ",i/4,res);
	}
	printf("\n");
	free(buf);
	return 0;
}

int allregs(eph_iob *iob,int argc,char *argv[]) {
	char *buffer;
	off_t bufsize;
	int i,rc;
	long j,max;
	long res;

	if ((max=atol(argv[1])) <= 0) {
		fprintf(stderr,"bad max register value \"%s\"\n",argv[1]);
		return -1;
	}

	for (j=0;j<=max;j++) {
		printf("%3ld ",j);
		/* do not query for image data because this may hang the
		   camera if frame number is not set beforehand.  On
		   Olympus D-600L even reading length registers hang it. */
		if (((j >= 12) && (j <= 15)) ||
		    (j == 39) || (j == 43) ||
		    (j == 44) || (j == 47)) {
			printf("Reg %ld not queried: dangerous...\n",j);
			continue;
		}
		if ((rc=eph_getint(iob,j,&res)) == DC1) {
			printf("N/A        ");
		} else if (rc) {
			printf("FAIL       ");
		} else {
			printf("%10ld ",res);
		}
		buffer=malloc(2048);
		buffer[0]='\0';
		bufsize=2048;
		if ((rc=eph_getvar(iob,j,&buffer,&bufsize)) == DC1) {
			strcpy(buffer,"N/A");
		} else if (rc) {
			strcpy(buffer,"FAIL");
		}
		printf("\"");
		for (i=0;(i<bufsize) && (i < 8);i++) {
			if ((buffer[i] >= ' ') && (buffer[i] <= 'z')) {
				printf("%c",buffer[i]);
			} else {
				printf("\\%03o",(unsigned char)buffer[i]);
			}
		}
		if (i < bufsize) printf("..");
		printf("\"\n");
		free(buffer);
	}
	return 0;
}

int list(eph_iob *iob,int argc,char *argv[]) {
	char *buffer;
	off_t bufsize;
	int rc;
	long res,j,max;

	if (eph_getint(iob,10,&max)) {
		return -1;
	}
	printf("total %lu\n",max);
	printf("No.    Size R P Date and Time\n");

	for (j=1;j<=max;j++) {
		printf("%3ld ",j);
		if ((rc=eph_setint(iob,4,j)) == DC1) {
			printf("N/A\n");
			break;
		} else if (rc) {
			printf("FAIL\n");
			break;
		}
		if ((rc=eph_getint(iob,12,&res)) == DC1) {
			printf("N/A    ");
		} else if (rc) {
			printf("FAIL   ");
		} else {
			printf("%7lu ",res);
		}
		buffer=malloc(32);
		bufsize=32;
		if ((rc=eph_getvar(iob,47,&buffer,&bufsize)) == DC1) {
			printf("N/A\n");
			break;
		} else if (rc) {
			printf("FAIL\n");
			break;
		}
		res=(long)(buffer[12]&0xff)+((long)(buffer[13]&0xff)<<8)+
			((long)(buffer[14]&0xff)<<16)+((long)(buffer[15]&0xff)<<24);
		printf("%01ld ",res);
		res=(long)(buffer[16]&0xff)+((long)(buffer[17]&0xff)<<8)+
			((long)(buffer[18]&0xff)<<16)+((long)(buffer[19]&0xff)<<24);
		printf("%c ",res?'P':'-');
		res=(long)(buffer[20]&0xff)+((long)(buffer[21]&0xff)<<8)+
			((long)(buffer[22]&0xff)<<16)+((long)(buffer[23]&0xff)<<24);
		if (res != -1L) printf("%8s\n",ctimetz(&res,usetimezone));
		else printf("N/A\n");
	}
	free(buffer);
	return 0;
}

int llist(eph_iob *iob,int argc,char *argv[]) {
	char *buffer,*p;
	unsigned char *ress;
	off_t bufsize,ressize;
	int rc;
	long j,max;
	long res;

	if (eph_getint(iob,10,&max)) {
		return -1;
	}
	printf("total %lu\n",max);
	printf("No. P    Size R F Shuttr Date and Time\n");

	for (j=1;j<=max;j++) {
		printf("%3ld ",j);
		if ((rc=eph_setint(iob,4,j)) == DC1) {
			printf("N/A\n");
			break;
		} else if (rc) {
			printf("FAIL\n");
			break;
		}
		if ((rc=eph_getint(iob,39,&res)) == DC1) {
			printf("N ");
		} else if (rc) {
			printf("F ");
		} else {
			printf("%c ",res?'P':'-');
		}
		if ((rc=eph_getint(iob,12,&res)) == DC1) {
			printf("N/A    ");
		} else if (rc) {
			printf("FAIL   ");
		} else {
			printf("%7lu ",res);
		}
		buffer=malloc(2048);
		buffer[0]='\0';
		bufsize=2048;
		if ((rc=eph_getvar(iob,15,&buffer,&bufsize)) == DC1) {
			printf("N/A\n");
			break;
		} else if (rc) {
			printf("FAIL\n");
			break;
		}
		ress=buffer;
		ressize=bufsize;
		a12scan(&ress,&ressize);
		p=a12search("Resolution",ress,ressize);
		res=-1L;
		if (p) {
			if (sscanf(p,"%lu",&res) != 1) res=-1L;
		}
		if (res != -1L) printf("%ld ",res);
		else printf("N ");
		p=a12search("Flash",ress,ressize);
		res=-1L;
		if (p) {
			if (sscanf(p,"%lu",&res) != 1) res=-1L;
		}
		if (res != -1L) printf("%c ",res?'F':'-');
		else printf("N ");
		p=a12search("Shutter",ress,ressize);
		res=-1L;
		if (p) {
			if (sscanf(p,"%lu",&res) != 1) res=-1L;
		}
		if (res != -1L) printf("1/%-4ld ",res?1000000L/res:0L);
		else printf("N/A   ");
		p=a12search("TimeDate",ress,ressize);
		res=-1L;
		if (p) {
			if (sscanf(p,"%lu",&res) != 1) res=-1L;
		}
		if (res != -1L) printf("%8s\n",ctimetz(&res,usetimezone));
		else printf("N/A\n");
		free(buffer);
	}
	return 0;
}

static void reporterror(int errcode,char *errstr) {
	if ((errcode != ERR_TIMEOUT) || debug)
		fprintf(stderr,"Error %d: %s\n",errcode,errstr);
}

void running(off_t count) {
	if (!quiet && filesize)
	{
		printf("%lu: %lu of %lu\r",(unsigned long)frame,
				(unsigned long)count,(unsigned long)filesize);
		fflush(stdout);
	}
}

int storing(char *data,size_t len) {
	if (fwrite(data,len,1,fp) != 1) {
		perror(fname);
		return -1;
	}
	return 0;
}

int retrfile(eph_iob *iob,int argc,char *argv[],int datatype) {
	char *filenm;
	char *buffer,*buf8i;
	off_t bufsize,buf8isize;
	long length,ilength,tlength,alength;
	off_t got;
	time_t pictime;
	unsigned char *res;
	off_t ressize;
	char *p;
	struct stat st;
	int dreg=0;
#ifndef LOWMEMORY
	int needfillbuffer=0;
#endif

	buf8isize=32;
	if ((buf8i=malloc(buf8isize)) == NULL) {
		perror("malloc buf8i");
		return -1;
	}
	(void)sscanf(argv[1],"%ld",&frame);
	filenm=argv[2];
	if (eph_setint(iob,4,frame)) return -1;
	if (eph_getint(iob,13,&tlength)) return -1;
	if (datatype == AUDIO) {
		buf8isize=32;
		if (eph_getvar(iob,43,&buf8i,&buf8isize) == 0) {
			alength=(long)(buf8i[4]&0xff)+((long)(buf8i[5]&0xff)<<8)+
				((long)(buf8i[6]&0xff)<<16)+((long)(buf8i[7]&0xff)<<24);
		} else
			alength=0;
		if (debug) fprintf(stderr,"Compressed audio length: %lu\n",
					alength);
		filesize=length=alength;
	} else {
		filesize=length=tlength;
		if (datatype == IMAGE) {
			if (eph_getint(iob,0x0c,&ilength)) return -1;
			length+=ilength;
			filesize=ilength;
		}
	}
	pictime=0;
	buf8isize=32;
	if (eph_getvar(iob,47,&buf8i,&buf8isize) == 0) {
		pictime=(long)(buf8i[20]&0xff)+((long)(buf8i[21]&0xff)<<8)+
			((long)(buf8i[22]&0xff)<<16)+((long)(buf8i[23]&0xff)<<24);
	}
	if (debug) fprintf(stderr,"got timestamp from reg47: %lu\n",
				pictime);
	free(buf8i);

/*
		What happens next

	MM  PT  DT | 1   2   3  4  5   6
	-----------+------------++-------
	N	T  | SA  RT  -- || --  WB
	o   !0	I  | BA  RI  -- || --  WB
	r	A  | BA  RA  -- || --  WB
	m  --------+------------++-------
	a	T  | SA  RT  AN || --  WB
	l   0	I  | BA  RI  AN || --  WB
		A  | SA  RT  AN || RA  WB
	-----------+------------++-------
		T  | SA  RT  -- || --  WB
	    !0	I  | --  --  -- || RW  --
	L	A  | --  --  -- || RW  --
	o  --------+------------++-------
	w	T  | SA  RT  AN || --  WB
	    0	I  | SA  RT  AN || RW  --
		A  | SA  RT  AN || RW  --
	-----------+------------++-------
*/

	/*=== Stage 1: allocate buffer ===*/

	bufsize=0;
	buffer=NULL;
#ifdef LOWMEMORY
	if ((datatype == THUMBNAIL) || (pictime == 0))
		bufsize=((tlength-1)/2048+2)*2048;
#else
	if ((pictime == 0) && (datatype == AUDIO))
		needfillbuffer=1;
	if ((datatype == THUMBNAIL)
	    || ((pictime == 0) && (datatype == AUDIO)))
		bufsize=((tlength-1)/2048+2)*2048;
	else
		bufsize=((length-1)/2048+2)*2048;
#endif
	/*				 ^ we should have had 1 here if
		we knew strict file size.  Although, we do not.  All we
		know is the size of image data chunk, and the size of
		thumbnail data chunk.  For now, we add extra 2048 in
		hope that extra JPEG data would not be more than that.
		If it is, the buffer will be automatically realloc()'ed
		anyway.
	*/
	if (bufsize) {
		buffer=malloc(bufsize);
		if (buffer == NULL) {
			fprintf(stderr,"could not alloc %lu bytes\n",
					(unsigned long)bufsize);
			return -1;
		}

	/*=== Stage 2: read data into the buffer ===*/

		got=bufsize;
#ifdef LOWMEMORY
		dreg=15;
#else
		switch (datatype) {
		case THUMBNAIL:
			dreg=15;
			break;
		case IMAGE:
			dreg=14;
			break;
		case AUDIO:
			if (pictime) dreg=44;
			else dreg=15;
			break;
		}
#endif
		if (eph_getvar(iob,dreg,&buffer,&got)) {
			free(buffer);
			return -1;
		}
		if (!quiet) printf("\n");
	}

	/*=== Stage 3: analyze data in the buffer ===*/

	if (pictime == 0) {
		res=buffer;
		ressize=bufsize;
		a12scan(&res,&ressize);
		p=a12search("TimeDate",res,ressize);
		pictime=(time_t)-1;
		if (p) {
			if (sscanf(p,"%lu",&pictime) != 1)
				pictime=(time_t)-1;
		}
		if (debug) fprintf(stderr,"got timestamp from file: %lu\n",
					pictime);
	}
	if (!quiet) {
		printf("taken %s\n",ctimetz(&pictime,usetimezone));
	}

	/*=== Stage 4: setup output file ===*/

	if (strcmp(filenm,"-") == 0) {
		fp=stdout;
	} else {
		if ((stat(filenm,&st) == 0) && (S_ISDIR(st.st_mode))) {
			int flen;

			flen=strlen(filenm)+MAXFORMAT+5;
			fname=malloc(flen);
			if (fname == NULL) {
				if (buffer) free(buffer);
				return -1;
			}
			makename(fname,flen,filenm,datatype,pictime,
					nameformat,usetimezone);
			if (!quiet) printf("file \"%s\"\n",fname);
		} else {
			fname=malloc(strlen(filenm)+1);
			if (fname == NULL) {
				if (buffer) free(buffer);
				return -1;
			}
			strcpy(fname,filenm);
		}
		if ((fp=fopen(fname,WRITEMODE)) == NULL) {
			perror(fname);
			if (fname) free(fname); fname=NULL;
			if (buffer) free(buffer);
			return -1;
		}
	}

	/*=== Stage 5: read data into the buffer or right out to file ===*/

#ifdef LOWMEMORY
	if (datatype != THUMBNAIL) {
		if (buffer) free(buffer);
		buffer=NULL;
		bufsize=0;
#else
	if (needfillbuffer) {
#endif
		/* we still need to read the data itself */
		if (eph_setint(iob,4,frame)) return -1;
		/* seems that some Agfa model needs setting reg4 again */
		dreg=(datatype==AUDIO)?44:14; /* cannot be thumbnail here */
		got=bufsize;
		if (eph_getvar(iob,dreg,buffer?&buffer:NULL,&got)) {
			file_abort_cleanup();
			free(buffer);
			return -1;
		}
		if (!quiet) printf("\n");
	}

	/*=== Stage 6: write out the buffer contents */

#ifdef LOWMEMORY
	if (datatype == THUMBNAIL)
#endif
				  {
		/* data is still in the buffer, just write it out */
		if (fwrite(buffer,(size_t)got,1,fp) != 1) {
			perror(fname);
			file_abort_cleanup();
			free(buffer);
			return -1;
		}
	}

	if (fp) {
		if (fclose(fp) < 0) {
			perror(fname);
		}
		fp=NULL;
	}
	if (fname) {
		if (setutime) {
			if (setfiletime(fname,pictime,pictime,usetimezone))
				perror("setfiletime");
		}
		free(fname); fname=NULL;
	}
	if (buffer) free(buffer);
	return 0;
}

int image(eph_iob *iob,int argc,char *argv[]) {
	return retrfile(iob,argc,argv,IMAGE);
}

int thumbnail(eph_iob *iob,int argc,char *argv[]) {
	return retrfile(iob,argc,argv,THUMBNAIL);
}

int audio(eph_iob *iob,int argc,char *argv[]) {
	return retrfile(iob,argc,argv,AUDIO);
}

int preview(eph_iob *iob,int argc,char *argv[]) {
	FILE *fp=NULL;
	char *buffer=NULL;
	off_t bufsize;
	char zero=0;
	int rc=-1;

	bufsize=2048;
	buffer=malloc(bufsize);
	if (buffer == NULL) {
		fprintf(stderr,"could not alloc %lu bytes\n",
				(unsigned long)bufsize);
		goto exit;
	}
	if (strcmp(argv[1],"-") == 0) fp=stdout;
	else if ((fp=fopen(argv[1],WRITEMODE)) == NULL) {
		fprintf(stderr,"could not open file %s: %s\n",
			argv[1],strerror(ERRNO));
		goto exit;
	}
	if (eph_action(iob,5,&zero,1)) goto exit;
	if (eph_setint(iob,4,0L)) goto exit;
	if (eph_getvar(iob,14,&buffer,&bufsize)) goto exit;
	if (fwrite(buffer,bufsize,1,fp) != 1) {
		perror(fname);
		goto exit;
	} else rc=0;

exit:
	if (buffer) free(buffer);
	if (fp && !(fp == stdout)) {
		if (fclose(fp) < 0) {
			perror(fname);
		}
	}
	return rc;
}

#ifndef LOWMEMORY
int upload(eph_iob *iob,int argc,char *argv[]) {
	FILE *fp=NULL;
	char *buffer=NULL;
	struct stat stbuf;
	unsigned long res;
	char zero=0;
	int rc=-1;

	if (stat(argv[1],&stbuf)) {
		fprintf(stderr,"upload cannot stat file \"%s\": %s\n",
				argv[1],strerror(errno));
		goto failure;
	}
	if ((fp=fopen(argv[1],READMODE)) == NULL) {
		fprintf(stderr,"upload cannot open file \"%s\": %s\n",
				argv[1],strerror(errno));
		goto failure;
	}
	if (stbuf.st_size == (size_t)0) {
		fprintf(stderr,"upload file \"%s\" has zero length\n",
				argv[1]);
		goto failure;
	}
	buffer=(char*)malloc(stbuf.st_size);
	if (buffer == NULL) {
		fprintf(stderr,"upload cannot allocate %lu bytes: %s\n",
				(unsigned long)stbuf.st_size,strerror(errno));
		goto failure;
	}
	if (fread(buffer,stbuf.st_size,1,fp) != 1) {
		fprintf(stderr,"upload cannot read from %s: %s\n",
				argv[1],strerror(errno));
		goto failure;
	}

	if (eph_getint(iob,28,&res)) {
		fprintf(stderr,"upload cannot get free memory\n");
		goto failure;
	}

	if (res < stbuf.st_size) {
		fprintf(stderr,"upload free memory %lu less than file size%lu\n",
				res,(unsigned long)stbuf.st_size);
		goto failure;
	}

	if (eph_setint(iob,32,0x0FEC000E)) {
		fprintf(stderr,"upload cannot cast magic spell\n");
		goto failure;
	}

	frame=0;
	filesize=stbuf.st_size;

	if (eph_setvar(iob,29,buffer,stbuf.st_size)) {
		if (!quiet) printf("\n");
		fprintf(stderr,"upload cannot write image\n");
		goto failure;
	}
	if (!quiet) printf("\n");

	if (eph_action(iob,11,&zero,1)) {
		fprintf(stderr,"upload cannot store image\n");
		goto failure;
	}

	rc=0;

failure:
	if (buffer) free(buffer);
	if (fp) fclose(fp);
	return rc;
}
#endif /* LOWMEMORY */

int off(eph_iob *iob,int argc,char *argv[]) {
	switchoff=1;
	return 0;
}

/* -------------------------------------------------------------------- */

int iterate(int (*executor)(eph_iob *iob,int argc,char *argv[]),
		eph_iob *iob,int argc,char *argv[],int intvl,int filearg) {
	int rc=0;
	long bot,top,i,step;
	char buf[16],*p;
	struct stat st;

	if (intvl == 0) return (*executor)(iob,argc,argv);

	if (filearg &&
	    (strchr(argv[intvl],',') || strchr(argv[intvl],'-'))) {
		if (stat(argv[filearg],&st) || !S_ISDIR(st.st_mode)) {
			fprintf(stderr,"Must specify directory for range\n");
			return -1;
		}
	}

	for (p=strtok(argv[intvl],",");p;p=strtok(NULL,",")) {
		if (strcasecmp(p,"all") == 0) {
			bot=1;
			top=-1;
		} else if ((strcasecmp(p+strlen(p)-5,"-last") == 0) &&
			   (sscanf(p,"%ld-last",&bot) == 1)) {
			top=-1;
		} else if (sscanf(p,"%ld-%ld",&bot,&top) == 2) {
			/* do nothing */ ;
		} else if (sscanf(p,"%ld",&bot) == 1) {
			top=bot;
		} else {
			fprintf(stderr,"bad frames interval \"%s\"\n",p);
			return -1;
		}
		if (bot < 1) {
			fprintf(stderr,"Bottom frame must be 1 or greater\n");
			return -1;
		}
		if (top < 1) {
			if (eph_getint(iob,0x0a,&top)) {
				fprintf(stderr,"could not get number of frames\n");
				return -1;
			}
			if (!quiet) printf("Top frame No. %ld\n",top);
		}
		if (top >= bot) step=1;
		else step=-1;
		top+=step;
		argv[intvl]=buf;	/* it's a bad taste to modify
					arg vector, but we won't use
					this argument anymore, so who
					cares... */
		for (i=bot;i!=top;i+=step) {
			sprintf(buf,"%ld",i);
			if ((rc=(*executor)(iob,argc,argv))) return rc;
		}
	}

	return rc;
} 

/* -------------------------------------------------------------------- */

struct _cmdlist {
	char *cmd;		/* Command name */
	int argc;		/* No of args for this command */
	int intvl;		/* Which arg is frame number/range */
	int filearg;		/* Which arg is the file/directory name */
	int (*executor)(eph_iob *iob,int argc,char *argv[]);
	char *help;
} cmdlist[] = {
	{"",		0,0,0,	NULL,		"== Query parameters =="	},
	{"query",	0,0,0,	query,		""			},
	{"count",	0,0,0,	count,		""			},
	{"list",	0,0,0,	list,		""			},
	{"llist",	0,0,0,	llist,		""			},
	{"geti",	1,0,0,	geti,		"-<reg-no>"		},
	{"getv",	1,0,0,	getv,		"-<reg-no>"		},
	{"camsum",	0,0,0,	camsum,		"-"			},
	{"picsum",	1,1,0,	picsum,		"-<frame-interval>"	},
	{"allregs",	1,0,0,	allregs,	"-<max-reg-no>"			},
	{"",		0,0,0,	NULL,		"== Do some actions =="	},
	{"erase",	1,1,0,	erase,		"<frame-interval>"		},
	{"protect",	2,1,0,	protect,	"`On' | `Off' <frame-interval>"	},
	{"snapshot",	0,0,0,	snapshot,	""			},
	{"eraseall",	0,0,0,	eraseall,	""			},
	{"",		0,0,0,	NULL,		"== Retreive files =="	},
	{"image",	2,1,2,	image,		"`All' | <frame-interval> <filename>"	},
	{"thumbnail",	2,1,2,	thumbnail,	"`All' | <frame-interval> <filename>"	},
	{"audio",	2,1,2,	audio,		"`All' | <frame-interval> <filename>"	},
	{"preview",	1,0,0,	preview,	"<filename>"	},
#ifndef LOWMEMORY
	{"",		0,0,0,	NULL,		"== Upload files =="	},
	{"upload",	1,0,0,	upload,		"<filename>"		},
#endif
	{"",		0,0,0,	NULL,		"== Set parameters =="			},
	{"resolution",	1,0,0,	resolution,	"`Hi' | `Lo' | `Ext'"		},
	{"clock",	0,0,0,	setclock,	""			},
	{"shutter",	1,0,0,	shutter,	"`Auto' | <microseconds> | 1/<fraction-of-second>"			},
	{"flash",	1,0,0,	flash,		"`Auto' | `Force' | `Off' | `AntiRedeye' | `SlowSync'"},
	{"id",		1,0,0,	setid,		"<string>"		},
	{"autoshut-host",1,0,0,	autoshut_host,	"<seconds>"		},
	{"autoshut-field",1,0,0,autoshut_field,	"<seconds>"		},
	{"lcd-autoshut",1,0,0,	lcd_autoshut,	"<seconds>"		},
	{"lcd-brightness",1,0,0,lcd_brightness,	"1 to 7"		},
	{"macro",	1,0,0,	macro,		"`On' | `Off'"		},
	{"color",	1,0,0,	color,		"`On' | `Off'"		},
	{"dzoom",	1,0,0,	dzoom,		"`On' | `Off'"		},
	{"seti",	2,0,0,	seti,		"-<reg-no> <value-to-set>"	},
	{"setv",	2,0,0,	setv,		"-<reg-no> <value-to-set>"	},
	{"cmd",		2,0,0,	cmd,		"-<cmd-code> <arg>"	},
	{"off",		0,0,0,	off,		""			},
	{"",		0,0,0,	NULL,		""			},
	{NULL,		0,0,0,	NULL,		NULL			}
};

/* -------------------------------------------------------------------- */

void showhelp(char *name) {
	int i;
	printf("usage: %s [-h] [-V] [-v[v]] [-q] [-z] [-t] [-f timeformat] [-s speed] [-l device] [command [params]] ...\n\n",name);
	printf("Options:\n\n");
	printf("\t-h\t- show this help screen\n");
	printf("\t-V\t- show software version number\n");
	printf("\t-v\t- increase debugging verbosity\n");
	printf("\t-q\t- do not show running download indicator\n");
	printf("\t-z\t- suppress timezone conversions (camera's clock is local time)\n");
	printf("\t-t\t- set modification time of the files to the time of snapshot\n");
	printf("\t-f fmt\t- create file names using strftime(3) with the time of snapshot\n");
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
	printf("\t-s baud\t- set port speed to 9600,19200,38400,57600 or 115200\n");
	printf("\t\t\tdefault is %d\n",MAX_SPEED);
	printf("\t-l dev\t- use device name instead of default %s\n\n",device);
	printf("Commands:\n");
	for (i=0;cmdlist[i].cmd;i++) {
		if (cmdlist[i].help[0] == '-') continue;
		printf("%-20.20s%s\n",cmdlist[i].cmd,cmdlist[i].help);
	}
	printf("EXAMPLE:\n%s id \"Eugene Crosser www.average.org\" query\n",name);
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
	int c,i,rc=0;
	char *cmd;
	long speed=0;
	eph_iob *iob;
	char *program = argv[0];
#ifdef MS_PROGRAM_NAMES
	char *q;

	/* remove program extension */
	if ((q = strrchr(program, '.')) != NULL)
		*q = '\0';
	/* remove directory */
	if ((q = strrchr(program, '/')) != NULL)
		program = q + 1;
	if ((q = strrchr(program, '\\')) != NULL)
		program = q + 1;
#endif

#if HAVE_SCHED_SETSCHEDULER
	if (geteuid() == 0) {
		/* Try to set realtime priority */
		struct sched_param sp;
		int rc,minp,maxp;

		minp=sched_get_priority_min(SCHED_FIFO);
		maxp=sched_get_priority_max(SCHED_FIFO);
		sp.sched_priority=minp+(maxp-minp)/2;
		if ((rc=sched_setscheduler(0,SCHED_FIFO,&sp)) == -1)
			fprintf(stderr,"failed to set realtime priority: %s\n",
				strerror(errno));
#if 0
		if ((rc=sched_getscheduler(0)) == -1)
			fprintf(stderr,"getscheduler: %s\n",strerror(errno));
		else
			if (sched_getparam(0,&sp) == -1)
				fprintf(stderr,"sched_getparm: %s\n",
					strerror(errno));
			else
				printf("New scheduling policy: %d, prio %d\n",
					rc,sp.sched_priority);
#endif
		/* Drop supervisor privelege */
		(void)seteuid(getuid());
	}
#endif /* HAVE_SCHED_SETSCHEDULER */

	while ((c=getopt(argc,argv,"l:s:f:vqhVzt")) != EOF)
	switch (c) {
	case 'l':
		device=optarg;
		break;
	case 's':
		speed=atol(optarg);
		break;
	case 'f':
		nameformat=optarg;
		break;
	case 'v':
		debug++;
		break;
	case 'q':
		quiet=1;
		break;
	case 'h':
		showhelp(program);
		return 0;
	case 'V':
		showversion(program);
		return 0;
	case 'z':
		usetimezone=0;
		break;
	case 't':
		setutime=1;
		break;
	default:
		fprintf(stderr," run \"%s -h\" for help\n",program);
		return 1;
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

#ifndef UNIX
	/* Under DOS and Win32, usleep() is done with a local
	   function that does self-calibration on first invokation.
	   That's why the first call may cause overly big delay and
	   break negotiation with the camera. */
	usleep(0L);	/* calibrate delay loops as required */
#endif

#ifdef LOWMEMORY
	if (atexit(file_abort_cleanup) == -1)
		perror("error setting atexit function");
#endif

#ifdef UNIX
	if (*device != '/') {
		char *p;
		p=device;
		device=malloc(strlen("/dev/")+strlen(p)+1);
		strcpy(device,"/dev/");
		strcat(device,p);
	}
#endif

	iob=eph_new(reporterror,NULL,running,storing,debug);
	if (!iob) {
		fprintf(stderr,"eph_new failed\n");
		return 1;
	}
	if (eph_open(iob,device,speed)) {
		fprintf(stderr,"eph_open failed\n");
		return 1;
	}

	if ((rc=probe(iob))) {
		fprintf(stderr,"probe failed\n");
		goto exit;
	}

	while ((cmd=argv[optind])) {
		if (switchoff) {
			fprintf(stderr,"commands after \"off\" ignored\n");
			goto exit;
		}
		for (i=0;
		     cmdlist[i].cmd&&strcmp(cmd,cmdlist[i].cmd);
		     i++) /* nothing */ ;
		if (cmdlist[i].cmd == NULL) {
			fprintf(stderr,"bad command \"%s\" ignored,",cmd);
			fprintf(stderr," run \"%s -h\" for help\n",program);
			optind++;
			continue;
		}
		if ((optind + cmdlist[i].argc) >= argc) {
			fprintf(stderr,"too few arguments for \"%s\" command\n",cmd);
			rc=1;
			goto exit;
		}
		if ((rc=iterate(cmdlist[i].executor,
				iob,
				cmdlist[i].argc+1,
				argv+optind,
				cmdlist[i].intvl,
				cmdlist[i].filearg))) {
			fprintf(stderr,"command \"%s\" failed, abort\n",cmd);
			goto exit;
		}
		optind+=cmdlist[i].argc;
		optind++;
	}

exit:
/*
	On older models, action 04 with zero paramater terminates
	session but leaves the camera ON.  On Olympus 600 (at least)
	this command not only terminates session but also turns the
	camera off.  So, the "off" command will turn off newer models
	and *just* terminate session more quickly on older.  You may
	want to terminate the command array by "off" on models like
	PhotoPC 600 to decrease power consumption when the session is
	over.
*/
	eph_close(iob,switchoff);
	eph_free(iob);
	return rc;
}
