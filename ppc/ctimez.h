/*
	$Id: ctimez.h,v 1.4 1999/03/22 05:59:31 lightner Exp crosser $
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
	$Log: ctimez.h,v $
	Revision 1.4  1999/03/22 05:59:31  lightner
	Add prototype for gltimetz()

	Revision 1.3  1999/03/12 23:22:30  crosser
	clean zone handling

	Revision 1.2  1999/03/10 22:34:43  crosser
	notimezone -> usetimezone

	Revision 1.1  1999/03/09 18:30:22  crosser
	Initial revision

*/

#ifndef CTIMEZ_H
#define CTIMEZ_H

struct tm *gltimetz(time_t *clock,int usetimezone);
char *ctimetz(time_t *clock,int usetimezone);
time_t zoneoffset(time_t now);

#endif
