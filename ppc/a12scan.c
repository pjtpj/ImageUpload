#ifndef LINT
static char *rcsid="$Id: a12scan.c,v 1.12 1999/03/10 22:34:43 crosser Exp crosser $";
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
	$Log: a12scan.c,v $
	Revision 1.12  1999/03/10 22:34:43  crosser
	make app12 and exif scanning in different files

	Revision 1.11  1999/03/06 13:37:08  crosser
	Convert to autoconf-style

	Revision 1.10  1999/03/06 10:24:16  crosser
	support Exif

	Revision 1.9  1999/02/28 22:40:48  crosser
	Support Exif file format

	Revision 1.8  1998/11/20 20:14:27  crosser
	scan APP12 block without vendor name section

	Revision 1.7  1998/11/19 11:04:31  crosser
	do cleaner if "not found"

	Revision 1.6  1998/10/18 13:18:27  crosser
	Put RCS logs and I.D. into the source

	Revision 1.5  1998/08/08 22:16:33  crosser
	Make jscan return the complete app12 contents, don't skip vendor info
	
	Revision 1.4  1998/02/08 19:58:38  crosser
	Support low memory: chunked saving etc.
	
	Revision 1.3  1998/01/18 02:16:45  crosser
	DOS support
	
	Revision 1.2  1997/12/24 00:19:13  crosser
	check return value from jscan
	
	Revision 1.1  1997/08/17 08:59:54  crosser
	Initial revision
	
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
/* #include <stdio.h> pjt */
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif

#include "a12scan.h"

/* pjt: */
#include <string.h>
#include "eph_io.h"
#include "eph_priv.h"

int a12scan(unsigned char **buf,off_t *bufsize) {
	unsigned char *p;
	off_t length=0;

	p=*buf;
	while (p < ((*buf)+(*bufsize))) {
		while ((p < ((*buf)+(*bufsize))) && (*p != 0xff)) p++;
		while ((p < ((*buf)+(*bufsize))) && (*p == 0xff)) p++;
		if (*p == 0xD8) continue;
		length=(p[1]<<8)+p[2];
		if (*p == 0xec) {
			p+=3;
			length-=3;
			break;
		}
		p+=length;
	}
	if (p >= ((*buf)+(*bufsize))) {
		(*bufsize)=0;
		(*buf)=NULL;
		return -1;
	}

	(*bufsize)=length;
	(*buf)=p;
	return 0;
}

unsigned char *a12search(char *key,unsigned char *buf,off_t bufsize) {
	unsigned char *p=buf;

	/* skip company name which is model-dependant */
	while ((p < (buf+bufsize)) && (*p)) p++;
	while ((p < (buf+bufsize)) && (*p == '\0')) p++;
	if (p >= (buf+bufsize)) p=buf;
	while (p < (buf+bufsize)) {
		if (strncmp(p,key,strlen(key)) == 0) break;
		while ((p < (buf+bufsize)) && (*p != '\n')) p++;
		p++;
	}
	if (p >= (buf+bufsize)) return NULL;
	if (*(p+strlen(key)) != '=') return NULL;
	return p+strlen(key)+1;
}

void a12dump(unsigned char *buf,off_t bufsize) {
	unsigned char *p=buf;

	EPHTRACE("%s\n",p);
	while ((p < (buf+bufsize)) && (*p)) p++;
	while ((p < (buf+bufsize)) && (*p == '\0')) p++;
	EPHTRACE("%s\n",p);
}
