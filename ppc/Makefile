# Generated automatically from Makefile.in by configure.
#	$Id: Makefile.in,v 2.14 1999/03/22 05:59:31 lightner Exp crosser $

#	Copyright (c) 1997,1998 Eugene G. Crosser
#	Copyright (c) 1998 Bruce D. Lightner (DOS/Windows support)
#
#	You may distribute and/or use for any purpose modified or unmodified
#	copies of this software if you preserve the copyright notice above.
#
#	THIS SOFTWARE IS PROVIDED AS IS AND COME WITH NO WARRANTY OF ANY
#	KIND, EITHER EXPRESSED OR IMPLIED.  IN NO EVENT WILL THE
#	COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES RESULTING FROM THE
#	USE OF THIS SOFTWARE.

#	$Log: Makefile.in,v $
#	Revision 2.14  1999/03/22 05:59:31  lightner
#	Add dependencies for ctimez.h
#
#	Revision 2.13  1999/03/12 10:06:23  crosser
#	setfiletime
#
#	Revision 2.12  1999/03/10 22:34:43  crosser
#	sync with structure changes
#
#	Revision 2.11  1999/03/06 13:37:08  crosser
#	Convert to autoconf-style
#
#	Revision 2.10  1998/12/18 09:29:37  crosser
#	fix dir perms in install.lib
#
#	Revision 2.9  1998/10/18 18:59:16  crosser
#	fix a glitch
#
#	Revision 2.8  1998/10/18 13:18:27  crosser
#	Put RCS logs and I.D. into the source
#
#	Revision 2.7  1998/08/01 12:30:50  crosser
#	Create dirs during install
#	
#	Revision 2.6  1998/02/27 08:38:06  crosser
#	correct DEFS for other OS-es
#	
#	Revision 2.5  1998/02/16 06:11:22  lightner
#	Add dependency for revision.h
#	
#	Revision 2.4  1998/02/08 19:58:38  crosser
#	Support low memory: chunked saving etc.
#	
#	Revision 2.3  1998/01/27 21:52:23  crosser
#	add NANOSLEEP
#	
#	Revision 2.2  1998/01/18 02:16:45  crosser
#	DOS support
#	HAVE_NANOSLEEP
#	
#	Revision 2.1  1998/01/04 13:55:57  crosser
#	add HAVE_PRIOCTL
#	
#	Revision 2.0  1998/01/02 19:20:11  crosser
#	Added support for Win32
#	
#	Revision 1.2  1997/12/24 00:19:13  crosser
#	add HAVE_STRFTIME option
#	
#	Revision 1.1  1997/08/17 08:59:54  crosser
#	Initial revision
#

# -DUNIX
# Use UNIX specific I/O and other things.

# -DDOS
# Use DOS specific I/O and other things.  Do not define here, use
# Makefile.dos file instead.

# -DMSWINDOWS
# Use Windows specific I/O and other things.  Do not define here, use
# Makefile.win file instead.

# -DLOWMEMORY
# Try to work even if the whole image dill not fit in addressable
# memory (e.g DOS or XENIX/286)

OPTS	= -DUNIX

MANEXT	= 1

##### No need to touch anything below this line #####

srcdir = .

CC = gcc
CFLAGS = -g -O -Wall
LDFLAGS = 
AR = /usr/bin/ar
RANLIB = ranlib
INSTALL = /usr/bin/install -c
LN = /bin/ln
ECHO = /bin/echo

DEFS = -DHAVE_CONFIG_H
LIBS = 

prefix = /usr/local
exec_prefix = ${prefix}

sbindir = ${exec_prefix}/sbin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include
sysconfdir = ${prefix}/etc
bindir = ${exec_prefix}/bin
mandir = ${prefix}/man

LIBOBJS	= eph_read.o eph_iob.o eph_err.o eph_cmd.o eph_io.o eph_open.o

.c.o:
	${CC} ${CFLAGS} ${DEFS} ${OPTS} -c $<

all:	libeph_io.a photopc epinfo

libeph_io.a:	${LIBOBJS}
	${AR} r $@ $?
	${RANLIB} $@

photopc:	photopc.o a12scan.o fnames.o ctimez.o filetime.o libeph_io.a
	${CC} ${LDGLAGS} -o $@ photopc.o a12scan.o fnames.o ctimez.o filetime.o -L. -leph_io ${LIBS}

epinfo:	epinfo.o a12scan.o exifscan.o fnames.o ctimez.o filetime.o
	${CC} ${LDGLAGS} -o $@ epinfo.o a12scan.o exifscan.o fnames.o ctimez.o filetime.o ${LIBS}

install:	all
	${INSTALL} -d -m 755 ${bindir}
	${INSTALL} -m 755 photopc ${bindir}
	${INSTALL} -m 755 epinfo ${bindir}
	${INSTALL} -d -m 755 ${mandir}/man${MANEXT}
	${INSTALL} -m 644 photopc.man ${mandir}/man${MANEXT}/photopc.${MANEXT}
	${INSTALL} -m 644 epinfo.man ${mandir}/man${MANEXT}/epinfo.${MANEXT}

install.lib:	libeph_io.a
	${INSTALL} -d -m 755 ${libdir}
	${INSTALL} -d -m 755 ${includedir}
	${INSTALL} -m 644 libeph_io.a ${libdir}
	${INSTALL} -m 644 eph_io.h ${includedir}

clean:
	rm -f *.o config.log *.bak

gclean:	clean
	rm -f libeph_io.a photopc epinfo config.status config.cache config.h Makefile

Makefile: config.status Makefile.in
	CONFIG_FILES=$@ CONFIG_HEADERS= ./config.status

config.status: configure
	./config.status --recheck

configure: configure.in
	cd $(srcdir) && autoconf

# DO NOT DELETE

eph_cmd.o: eph_io.h
eph_cmd.o: eph_priv.h
eph_err.o: eph_io.h
eph_err.o: eph_priv.h
eph_io.o: eph_io.h
eph_io.o: eph_priv.h
eph_io.o: usleep.h
eph_io.o: comio.h
eph_iob.o: eph_io.h
eph_open.o: eph_io.h
eph_open.o: eph_priv.h
eph_open.o: usleep.h
eph_open.o: comio.h
eph_open.o: usleep.h
eph_read.o: eph_io.h
eph_read.o: comio.h
eph_read.o: usleep.h
epinfo.o: a12scan.h
epinfo.o: exifscan.h
epinfo.o: fnames.h
epinfo.o: ctimez.h
epinfo.o: filetime.h
epinfo.o: strcscmp.h
epinfo.o: revision.h
getopt.o: getopt.h
a12scan.o: a12scan.h
exifscan.o: exifscan.h
fnames.o: fnames.h
fnames.o: ctimez.h
ctimez.o: ctimez.h
filetime.o: filetime.h
filetime.o: ctimez.h
photopc.o: eph_io.h
photopc.o: a12scan.h
photopc.o: fnames.h
photopc.o: ctimez.h
photopc.o: filetime.h
photopc.o: getopt.h
photopc.o: strcscmp.h
photopc.o: revision.h
