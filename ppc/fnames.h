/*
	$Id: fnames.h,v 1.2 1999/03/10 22:34:43 crosser Exp crosser $
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
	$Log: fnames.h,v $
	Revision 1.2  1999/03/10 22:34:43  crosser
	clenup

	Revision 1.1  1999/03/09 18:36:34  crosser
	Initial revision

*/

#ifndef FNAMES_H
#define FNAMES_H

#define MAXFORMAT 200

/* datatype values */
#define IMAGE		0
#define THUMBNAIL	1
#define AUDIO		2

void makename(char *fname,int flen,char *filenm,int datatype,
			time_t pictime,char *nameformat,int usetimezone);

#endif
