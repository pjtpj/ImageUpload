/*
	$Id: filetime.h,v 1.1 1999/03/11 22:57:35 crosser Exp crosser $
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
	$Log: filetime.h,v $
	Revision 1.1  1999/03/11 22:57:35  crosser
	Initial revision

*/

#ifndef FILETIME_H
#define FILETIME_H

int setfiletime(char *fname,time_t actime,time_t mtime,int usetimezone);

#endif
