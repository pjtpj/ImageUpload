#ifndef LINT
static char *rcsid="$Id: eph_io.c,v 2.13 1999/03/27 23:54:37 crosser Exp crosser $";
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
	$Log: eph_io.c,v $
	Revision 2.13  1999/03/27 23:54:37  crosser
	backed out adding WRTDELAY: already done in eph_priv.h

	Revision 2.12  1999/03/22 05:59:31  lightner
	Add WRTDELAY to SMART CHUNKED WRITE's WRITE()

	Revision 2.11  1999/03/06 13:37:08  crosser
	Convert to autoconf-style

	Revision 2.10  1999/02/10 22:09:36  crosser
	strerror needs string.h with glibc2

	Revision 2.9  1998/10/18 13:18:27  crosser
	Put RCS logs and I.D. into the source

	Revision 2.8  1998/08/08 14:00:33  crosser
	fix typo
	
	Revision 2.7  1998/08/01 13:16:18  crosser
	move delays where they belong.
	
	Revision 2.6  1998/08/01 13:12:41  lightner
	change Windows logic and timeouts
	
	Revision 2.5  1998/08/01 12:30:50  crosser
	flushinput function
	
	Revision 2.4  1998/01/27 21:52:55  crosser
	add multi-block write
	
	Revision 2.3  1998/01/18 02:16:45  crosser
	DOS support
	
	Revision 2.2  1998/01/05 19:48:46  lightner
	Win32 syntax error fixed: i->length
	
	Revision 2.1  1998/01/03 19:57:13  crosser
	Fix Windows things, improve error reporting
	
	Revision 2.0  1998/01/02 19:20:11  crosser
	Added support for Win32
	
	Revision 1.3  1997/12/24 00:19:13  crosser
	Change default speed to 115200
	
	Revision 1.2  1997/09/12 09:42:35  crosser
	skip possible NULs prior to `camera signature'
	
	Revision 1.1  1997/08/17 08:59:54  crosser
	Initial revision
	
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "eph_io.h"
#include "eph_priv.h"
#include <sys/types.h>
#ifdef UNIX
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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif
/* #include <errno.h> pjt */
/* #include <stdio.h> pjt */
/* #include <string.h> pjt */

#ifndef UNIX
#include "usleep.h"
#endif

#ifdef INT16
typedef unsigned INT16 uint16;
#endif

#ifdef MSWINDOWS
#define ERRNO GetLastError()
#else
#define ERRNO errno
#endif

#ifdef HAVE_NANOSLEEP

void shortsleep(unsigned long useconds) {
	struct timespec ts={tv_sec: (long int)(useconds/1000000),
			tv_nsec: (long int)(useconds%1000000)*1000ul};

	nanosleep(&ts,NULL);
}

#else /* HAVE_NANOSLEEP */

#define shortsleep(x) usleep(x)

#endif /* HAVE_NANOSLEEP */

#if !defined(BYTE_BY_BYTE_WRITE) && !defined(SINGLE_BURST_WRITE)
static struct _chunk {
	size_t offset;
	size_t size;
	unsigned long delay;
} chunk[] = {
	{	0L,	1L,	WRTPKTDELAY	},
	{	1L,	3L,	WRTCMDDELAY	},
	{	4L,	0L,	WRTPRMDELAY	}
};
#define MAXCHUNK 3
#endif

/*
	System-specific WRITE implementation
*/

#if defined(DOS)

#include "comio.h"

#define WRITE(x,y,z) (dos_write(x,y,z) != z)

int dos_write(int fd, void *buf, int length) {
	int i;
	unsigned char *p = buf;

	ttflui();	/* flush input buffer */
	for (i = 0; i < length; ++i) {
		ttoc(*p++);
	}
	return length;
}

#elif defined(MSWINDOWS)

#define WRITE(x,y,z) win_write(x,y,z)

int win_write(HANDLE fd,void *buf,DWORD length) {
	DWORD nwrit;

	if (!WriteFile(fd,buf,length,&nwrit,NULL) || (nwrit != length))
		return 1;
	else
		return 0;
}

#elif defined(UNIX)

#define WRITE(x,y,z) (write(x,y,z) != z)

#else
#error "platform not defined"
#endif

int eph_writepkt(eph_iob *iob,int typ,int seq,char *data,size_t length) {
	uint16 crc=0;
	unsigned char buf[2054];
	int i=0,j;

	if (length > (sizeof(buf)-6)) {
		eph_error(iob,ERR_DATA_TOO_LONG,
			"trying to write %ld in one pkt",(long)length);
		return -1;
	}

	buf[i++]=typ;
	buf[i++]=seq;
	buf[i++]=length&0xff;
	buf[i++]=length>>8;
	for (j=0;j<length;j++) {
		crc+=(unsigned char)data[j];
		buf[i++]=data[j];
	}
	buf[i++]=crc&0xff;
	buf[i++]=crc>>8;
	if (iob->debug) {
		EPHTRACE("> (%d)",i);
		for (j=0;j<i;j++) {
			EPHTRACE(" %02x",buf[j]);
		}
		EPHTRACE("\n");
	}

#if defined(SINGLE_BURST_WRITE)
	shortsleep(WRTPKTDELAY);
	if (WRITE(iob->fd,buf,i)) {
		eph_error(iob,ERRNO,"pkt write error %s",strerror(ERRNO));
		return -1;
	}
#elif defined(BYTE_BY_BYTE_WRITE)
/*
   It looks like the camera hates if data is sent at 115200 in one chunk.
   Thierry Bousch <bousch%linotte.uucp@topo.math.u-psud.fr> recommended
   to send data byte-by-byte with small delays.  This really helps.
*/
	shortsleep(WRTPKTDELAY);
	for (j=0;j<i;j++) {
		shortsleep(WRTDELAY);
		if (WRITE(iob->fd,buf+j,1)) {
			eph_error(iob,ERRNO,"pkt write error %s",strerror(ERRNO));
			return -1;
		}
/*
   tcdrain()s do not seem to be necessary, at least on Linux...
		if (tcdrain(iob->fd)) {
			eph_error(iob,ERRNO,"pkt tcdrain error %s",strerror(ERRNO));
			return -1;
		}
*/
	}
#else /* SMART CHUNKED WRITE */
	for (j=0;j<MAXCHUNK;j++) {
		size_t sz=(chunk[j].size)?(chunk[j].size)
						:(i-chunk[j].offset);
		shortsleep(chunk[j].delay);
		if (WRITE(iob->fd,buf+chunk[j].offset,sz)) {
			eph_error(iob,ERRNO,"pkt write chunk %d(%d) error %s",
					j,(int)sz,strerror(ERRNO));
			return -1;
		}
	}
#endif
	return 0;
}

void eph_writeinit(eph_iob *iob) {
	unsigned char init=0;

	if (iob->debug) EPHTRACE("> INIT 00\n");
	shortsleep(WRTDELAY);
	if (WRITE(iob->fd,&init,1)) {
		eph_error(iob,ERRNO,"init write error %s",strerror(ERRNO));
	}
}

void eph_writeack(eph_iob *iob) {
	unsigned char ack=ACK;

	if (iob->debug) EPHTRACE("> ACK 06\n");
	shortsleep(WRTDELAY);
	if (WRITE(iob->fd,&ack,1)) {
		eph_error(iob,ERRNO,"ack write errot %s",strerror(ERRNO));
	}
}

void eph_writenak(eph_iob *iob) {
	unsigned char nak=NAK;

	if (iob->debug) EPHTRACE("> NAK 11\n");
	shortsleep(WRTDELAY);
	if (WRITE(iob->fd,&nak,1)) {
		eph_error(iob,ERRNO,"nak write error %s",strerror(ERRNO));
	}
}

int eph_readpkt(eph_iob *iob,eph_pkthdr *pkthdr,char *buffer,size_t *bufsize,long timeout_usec) {
	uint16 length,got;
	uint16 crc1=0,crc2;
	unsigned char buf[4];
	int i,rc;

	i=eph_readt(iob,buf,1,timeout_usec,&rc);
	if (iob->debug)
		EPHTRACE ("pktstart: i=%d rc=%d char=0x%02x\n",i,rc,*buf);
	if (i < 0) {
		eph_error(iob,ERRNO,"pkt start read error %s",strerror(ERRNO));
		return -1;
	} else if ((i == 0) && (rc == 0)) {
		eph_error(iob,ERR_TIMEOUT,"pkt start read timeout (%ld)",
				timeout_usec);
		return -2;
	} else if (i != 1) {
		eph_error(iob,ERR_BADREAD,"pkt start read %d, expected 1",i);
		return -1;
	}
	pkthdr->typ=buf[0];
	if ((*buf != PKT_DATA) && (*buf != PKT_LAST)) {
		if ((*buf != NAK) && (*buf != DC1))
			eph_error(iob,ERR_BADDATA,"pkt start got 0x%02x",*buf);
		return *buf;
	}
	got=0;
	while ((i=eph_readt(iob,buf+1+got,3-got,DATATIMEOUT,&rc)) > 0) {
		got+=i;
	}
	if (got != 3) {
		if (i < 0) {
			eph_error(iob,ERRNO,"pkt hdr read error %s (got %d)",
					strerror(ERRNO),got);
			return -1;
		} else if ((i == 0) && (rc == 0)) {
			eph_error(iob,ERR_TIMEOUT,"pkt hdr read timeout (%ld)",
					DATATIMEOUT);
			return -2;
		} else {
			eph_error(iob,ERR_BADREAD,"pkt hdr read return %d rc %d",
					i,rc);
			return -1;
		}
	}
	if (iob->debug) EPHTRACE("header: %02x %02x %02x %02x\n",
				buf[0],buf[1],buf[2],buf[3]);
	pkthdr->seq=buf[1];
	length=(buf[3]<<8)|buf[2];
	if (length > *bufsize) {
		eph_error(iob,ERR_DATA_TOO_LONG,
			"length in pkt header %lu bigger than buffer size %lu",
			(unsigned long)length,(unsigned long)*bufsize);
		return -1;
	}

	got=0;
	while ((i=eph_readt(iob,buffer+got,length-got,iob->timeout,&rc)) > 0) {
		got+=i;
	}
	if (got != length) {
		if (i < 0) {
			eph_error(iob,ERRNO,"pkt data read error %s",strerror(ERRNO));
			return -1;
		} else if ((i == 0) && (rc == 0)) {
			eph_error(iob,ERR_TIMEOUT,"pkt data read timeout (%ld)",
				iob->timeout);
			return -2;
		} else {
			eph_error(iob,ERR_BADREAD,
				"pkt read return %d, rc %d",i,rc);
			return -1;
		}
	}

	for (i=0;i<length;i++) {
		crc1+=(unsigned char)buffer[i];
	}

	got=0;
	while ((i=eph_readt(iob,buf+got,2-got,DATATIMEOUT,&rc)) > 0) {
		got+=i;
	}
	if (iob->debug)
		EPHTRACE ("crc: %02x %02x i=%d rc=%d\n",buf[0],buf[1],i,rc);
	if (got != 2) {
		if (i < 0) {
			eph_error(iob,ERRNO,"pkt crc read error %s",strerror(ERRNO));
			return -1;
		} else if ((i == 0) && (rc == 0)) {
			eph_error(iob,ERR_TIMEOUT,"pkt crc read timeout (%ld)",
					DATATIMEOUT);
			return -2;
		} else {
			eph_error(iob,ERR_BADREAD,"pkt crc read return %d rc %d",
					i,rc);
			return -1;
		}
	}

	crc2=(buf[1]<<8)|buf[0];
	if (crc1 != crc2) {
		if (iob->debug) EPHTRACE("crc %04x != %04x\n",crc1,crc2);
		eph_error(iob,ERR_BADCRC,
			"crc received=0x%04x counted=0x%04x",crc2,crc1);
		return -1;
	}
	if (iob->debug) {
		int j;

		EPHTRACE("< %d,%d (%d)",pkthdr->typ,pkthdr->seq,length);
		if (iob->debug > 1) for (j=0;j<length;j++) {
			EPHTRACE(" %02x",(unsigned char)buffer[j]);
		} else EPHTRACE(" ...");
		EPHTRACE("\n");
		EPHTRACE("< %d,%d (%d)",pkthdr->typ,pkthdr->seq,length);
		if (iob->debug > 1) for (j=0;j<length;j++) {
			EPHTRACE(" %c ",(buffer[j] >= ' ' && buffer[j] < 127)
							? buffer[j] : '.');
		} else EPHTRACE(" ...");
		EPHTRACE("\n");
	}
	(*bufsize)=length;
	return 0;
}

int eph_flushinput(eph_iob *iob) {
	unsigned char buf;
	int i,rc;

	i=eph_readt(iob,&buf,1,0L,&rc);
	if (iob->debug)
		EPHTRACE ("< %02x amount=%d rc=%d\n",buf,i,rc);
	if (i < 0) {
		eph_error(iob,ERRNO,"flushinput read error %s",strerror(ERRNO));
		return -1;
	} else if ((i == 0) && (rc == 0)) {
		if (iob->debug)
			EPHTRACE ("flushed: read %d amount=%d rc=%d\n",buf,i,rc);
		return 0;
	} else {
		eph_error(iob,ERR_BADREAD,"flushinput read %d expected 0",i);
		return -1;
	}
}

int eph_waitchar(eph_iob *iob,unsigned long timeout_usec) {
	unsigned char buf;
	int i,rc;

	i=eph_readt(iob,&buf,1,timeout_usec,&rc);
	if (iob->debug)
		EPHTRACE ("< %02x amount=%d rc=%d\n",buf,i,rc);
	if (i < 0) {
		eph_error(iob,ERRNO,"waitchar read error %s",strerror(ERRNO));
		return -1;
	} else if ((i == 0) && (rc == 0)) {
		eph_error(iob,ERR_TIMEOUT,"waitchar read timeout (%ld)",
				timeout_usec);
		return -2;
	} else if (i != 1) {
		eph_error(iob,ERR_BADREAD,"waitchar read %d expected 1",i);
		return -1;
	}
	return buf;
}

int eph_waitack(eph_iob *iob,long timeout_usec) {
	int rc;
	if ((rc=eph_waitchar(iob,timeout_usec)) == ACK) return 0;
	if ((rc != DC1) && (rc != NAK))
		eph_error(iob,ERR_BADREAD,"eph_waitack got %d",rc);
	return rc;
}

int eph_waitcomplete(eph_iob *iob) {
	int rc;
	if ((rc=eph_waitchar(iob,CMDTIMEOUT)) == 0x05) return 0;
	if ((rc != DC1) && (rc != NAK))
		eph_error(iob,ERR_BADREAD,"eph_waitcomplete got %d",rc);
	return rc;
}

int eph_waitsig(eph_iob *iob) {
	int rc,count=SKIPNULS;
	while (((rc=eph_waitchar(iob,INITTIMEOUT)) == 0) && (count-- > 0)) ;
	if (rc == 0x15) return 0;
	eph_error(iob,ERR_BADREAD,"eph_waitsig got %d",rc);
	return rc;
}

int eph_waiteot(eph_iob *iob) {
	int rc;
	if ((rc=eph_waitchar(iob,EODTIMEOUT)) == 0xff) return 0;
	if ((rc != DC1) && (rc != NAK))
		eph_error(iob,ERR_BADREAD,"eph_waiteot got %d",rc);
	return rc;
}

/* pjt: */

#ifdef _DEBUG

#include <stdio.h>
#include <stdarg.h>

void EphTrace(const char* lpszFormat, ...)
{
	va_list args;
	char szBuffer[512];

	va_start(args, lpszFormat);
	
	_vsnprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);

	OutputDebugString(szBuffer);
	va_end(args);
}

#endif