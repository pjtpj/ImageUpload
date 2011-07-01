/*
	$Id: exifscan.h,v 1.1 1999/03/09 18:19:39 crosser Exp crosser $
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
	$Log: exifscan.h,v $
	Revision 1.1  1999/03/09 18:19:39  crosser
	Initial revision

*/

#ifndef _EXIFSCAN_H
#define _EXIFSCAN_H

#ifdef DOS
typedef long off_t;
#endif

int exifscan(unsigned char **buf,off_t *bufsize);
unsigned char *exifsearch(char *key,unsigned char *buf,off_t bufsize);
void exifdump(unsigned char *buf,off_t bufsize);

#endif
