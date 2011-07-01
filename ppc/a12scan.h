/*
	$Id: a12scan.h,v 2.3 1999/03/10 22:34:43 crosser Exp crosser $
*/

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
	$Log: a12scan.h,v $
	Revision 2.3  1999/03/10 22:34:43  crosser
	make app12 and exif scanning in different files

	Revision 2.2  1999/02/28 22:40:48  crosser
	Support Exif file format

	Revision 2.1  1998/10/18 13:18:27  crosser
	Put RCS logs and I.D. into the source

	Revision 2.0  1998/02/13 23:01:13  crosser
	define type off_t for DOS
	
	Revision 1.3  1998/02/08 19:58:38  crosser
	Support low memory: chunked saving etc.
	
	Revision 1.2  1998/01/18 02:16:45  crosser
	DOS support
	
	Revision 1.1  1997/08/17 08:59:54  crosser
	Initial revision
	
*/

#ifndef _A12SCAN_H
#define _A12SCAN_H

#define MAXJPREFIX 8192L

#ifdef DOS
typedef long off_t;
#endif

int a12scan(unsigned char **buf,off_t *bufsize);
unsigned char *a12search(char *key,unsigned char *buf,off_t bufsize);
void a12dump(unsigned char *buf,off_t bufsize);

#endif
