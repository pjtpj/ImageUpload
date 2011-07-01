#ifndef LINT
static char *rcsid="$Id: exifscan.c,v 1.3 1999/03/12 10:06:23 crosser Exp crosser $";
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
	$Log: exifscan.c,v $
	Revision 1.3  1999/03/12 10:06:23  crosser
	avoid strncmp

	Revision 1.2  1999/03/10 22:34:43  crosser
	separate from app12 scan

	Revision 1.1  1999/03/09 18:19:39  crosser
	Initial revision

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <sys/types.h>
#include <stdio.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#include <string.h>
#endif
#include "exifscan.h"

int exifscan(unsigned char **buf,off_t *bufsize) {
	unsigned char *p;
	off_t length=0;

	p=*buf;
	while (p < ((*buf)+(*bufsize))) {
		while ((p < ((*buf)+(*bufsize))) && (*p != 0xff)) p++;
		while ((p < ((*buf)+(*bufsize))) && (*p == 0xff)) p++;
		if (*p == 0xD8) continue;
		length=(p[1]<<8)+p[2];
		if ((*p == 0xe1) && (strcasecmp(p+3,"Exif")==0)) {
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

static struct _exifdict {
	int tag;
	char *name;
} exifdict[] = {
	{0xFE,		"NewSubFileType"},
	{0x100,		"ImageWidth"},
	{0x101,		"ImageLength"},
	{0x102,		"BitsPerSample"},
	{0x103,		"Compression"},
	{0x106,		"PhotometricInterpretation"},
	{0x10A,		"FillOrder"},
	{0x10D,		"DocumentName"},
	{0x10E,		"ImageDescription"},
	{0x10F,		"Make"},
	{0x110,		"Model"},
	{0x111,		"StripOffsets"},
	{0x112,		"Orientation"},
	{0x115,		"SamplesPerPixel"},
	{0x116,		"RowsPerStrip"},
	{0x117,		"StripByteCounts"},
	{0x11A,		"XResolution"},
	{0x11B,		"YResolution"},
	{0x11C,		"PlanarConfiguration"},
	{0x128,		"ResolutionUnit"},
	{0x12D,		"TransferFunction"},
	{0x131,		"Software"},
	{0x132,		"DateTime"},
	{0x13B,		"Artist"},
	{0x13E,		"WhitePoint"},
	{0x13F,		"PrimaryChromaticities"},
	{0x156,		"TransferRange"},
	{0x200,		"JPEGProc"},
	{0x201,		"JPEGInterchangeFormat"},
	{0x202,		"JPEGInterchangeFormatLength"},
	{0x211,		"YCbCrCoefficients"},
	{0x212,		"YCbCrSubSampling"},
	{0x213,		"YCbCrPositioning"},
	{0x214,		"ReferenceBlackWhite"},
	{0x828D,	"CFARepeatPatternDim"},
	{0x828E,	"CFAPattern"},
	{0x828F,	"BatteryLevel"},
	{0x8298,	"Copyright"},
	{0x829A,	"ExposureTime"},
	{0x829D,	"FNumber"},
	{0x83BB,	"IPTC/NAA"},
	{0x8769,	"ExifOffset"},
	{0x8773,	"InterColorProfile"},
	{0x8822,	"ExposureProgram"},
	{0x8824,	"SpectralSensitivity"},
	{0x8825,	"GPSInfo"},
	{0x8827,	"ISOSpeedRatings"},
	{0x8828,	"OECF"},
	{0x9000,	"ExifVersion"},
	{0x9003,	"DateTimeOriginal"},
	{0x9004,	"DateTimeDigitized"},
	{0x9101,	"ComponentsConfiguration"},
	{0x9102,	"CompressedBitsPerPixel"},
	{0x9201,	"ShutterSpeedValue"},
	{0x9202,	"ApertureValue"},
	{0x9203,	"BrightnessValue"},
	{0x9204,	"ExposureBiasValue"},
	{0x9205,	"MaxApertureValue"},
	{0x9206,	"SubjectDistance"},
	{0x9207,	"MeteringMode"},
	{0x9208,	"LightSource"},
	{0x9209,	"Flash"},
	{0x920A,	"FocalLength"},
	{0x927C,	"MakerNote"},
	{0x9286,	"UserComment"},
	{0x9290,	"SubSecTime"},
	{0x9291,	"SubSecTimeOriginal"},
	{0x9292,	"SubSecTimeDigitized"},
	{0xA000,	"FlashPixVersion"},
	{0xA001,	"ColorSpace"},
	{0xA002,	"ExifImageWidth"},
	{0xA003,	"ExifImageLength"},
	{0xA005,	"InteroperabilityOffset"},
	{0xA20B,	"FlashEnergy"},
	{0xA20C,	"SpatialFrequencyResponse"},
	{0xA20E,	"FocalPlaneXResolution"},
	{0xA20F,	"FocalPlaneYResolution"},
	{0xA210,	"FocalPlaneResolutionUnit"},
	{0xA214,	"SubjectLocation"},
	{0xA215,	"ExposureIndex"},
	{0xA217,	"SensingMethod"},
	{0xA300,	"FileSource"},
	{0xA301,	"SceneType"},
	{0,		NULL}
};

static int typlens[] = {1,1,2,4,8,1,1,2,4,8};

static int s2n_intel(unsigned char *data,int length,int sign) {
	int i,msb,val=0;
	for (i=0;i<length;i++) {
		val |= (data[i] << (i*8));
	}
	if (sign) {
		msb=1<<(length*8-1);
		if (val & msb) val-=(msb<<1);
	}
	return val;
}

static int s2n_motorola(unsigned char *data,int length,int sign) {
	int i,msb,val=0;
	for (i=0;i<length;i++) {
		val = (val << 8) | data[i];
	}
	if (sign) {
		msb=1<<(length*8-1);
		if (val & msb) val-=(msb<<1);
	}
	return val;
}

static int (*s2n)(unsigned char *data,int length,int signed);
#define S2N(x,y,z) ((*s2n)(x,y,z))

static unsigned char *exifwalk(unsigned char *data,int where,
			unsigned char *(*each)(
				unsigned char *data,int offset,int tag,
				int typ,int typlen,int count,void *priv),
			void *priv);

static unsigned char *exifstr(unsigned char *data,int offset,int tag,
				int typ,int typlen,int count) {
	static unsigned char *buf=NULL;
	static int buflen=0;
	int len;

	if (typ == 2) {
		len=count+3;
		if (len > buflen) {
			if (buf) free(buf);
			buf=(unsigned char*)malloc(len);
		}
		if (buf == NULL) return "<no memory>";
		sprintf(buf,"\"%s\"",data+offset);
	} else {
		int k;
		int sig;

		len=count*22+3;
		if (len > buflen) {
			if (buf) free(buf);
			buf=(unsigned char*)malloc(len);
		}
		if (buf == NULL) return "<no memory>";
		buf[0]='\0';
		if ((typ == 6) || (typ >= 8)) sig=1;
		else sig=0;
		if (count > 1) sprintf(buf,"(");
		for (k=0;k<count;k++) {
			if ((typ == 5) || (typ == 10)) {
				sprintf(buf+strlen(buf),"%s%d/%d",
					k?",":"",
					S2N(data+offset+typlen*k,4,sig),
					S2N(data+offset+4+typlen*k,4,sig));
			} else {
				sprintf(buf+strlen(buf),"%s%d",
					k?",":"",
					S2N(data+offset+typlen*k,typlen,sig));
			}
		}
		if (count > 1) sprintf(buf+strlen(buf),")");
	}
	return buf;
}

static unsigned char *exifmatch(unsigned char *data,int offset,int tag,
				int typ,int typlen,int count,void *priv) {
	int i;
	int needtag=0;
	unsigned char *rc;

	for (i=0;exifdict[i].tag;i++) {
		if (strcmp(exifdict[i].name,(char*)priv) == 0) {
			needtag=exifdict[i].tag;
			break;
		}
	}
	if (tag == 0x8769) {
		rc=exifwalk(data,offset,exifmatch,priv);
		if (rc) return rc;
	} else if (tag == 0xA005) {
		rc=exifwalk(data,offset,exifmatch,priv);
		if (rc) return rc;
	}
	if (needtag == tag)
		return exifstr(data,offset,tag,typ,typlen,count);
	else return NULL;
}

static unsigned char *exifprt(unsigned char *data,int offset,int tag,
				int typ,int typlen,int count,void *priv) {
	unsigned char *rc;
	int i;
	char *tagname=NULL,badtag[32];

	for (i=0;exifdict[i].tag;i++) {
		if (exifdict[i].tag == tag) {
			tagname=exifdict[i].name;
			break;
		}
	}
	if (tagname == NULL) {
		sprintf(badtag,"TAG#%d",tag);
		tagname=badtag;
	}
	if (tag == 0x8769) {
		printf("<Exif SubIFD>\n");
		rc=exifwalk(data,offset,exifprt,priv);
		printf("</Exif SubIFD>\n");
		if (rc) return rc;
	} else if (tag == 0xA005) {
		printf("<Interoperability SubIFD>\n");
		rc=exifwalk(data,offset,exifprt,priv);
		printf("</Interoperability SubIFD>\n");
		if (rc) return rc;
	} else {
		printf("%s=%s\n",tagname,
			exifstr(data,offset,tag,typ,typlen,count));
	}
	return NULL;
}

static unsigned char *exifwalk(unsigned char *data,int where,
			unsigned char *(*each)(
				unsigned char *data,int offset,int tag,
				int typ,int typlen,int count,void *priv),
			void *priv) {
	unsigned char *rc=NULL;
	int i;

	if (where == 0) {
		switch (data[0]) {
		case 'I':	s2n=s2n_intel; break;
		case 'M':	s2n=s2n_motorola; break;
		default:	return NULL;
		}
		where=S2N(data+4,4,0);
	} else where=S2N(data+where,4,0);
	for (i=where;i;i=S2N(data+i+2+(12*S2N(data+i,2,0)),4,0)) {
		int ifsnum=S2N(data+i,2,0);
		int j;

		for (j=0;j<ifsnum;j++) {
			int typlen,count;
			int offset;
			int entry=i+2+12*j;
			int tag=S2N(data+entry,2,0);
			int typ=S2N(data+entry+2,2,0);
			if ((typ < 1) || (typ > 10)) continue;
			typlen=typlens[typ-1];
			count=S2N(data+entry+4,4,0);
			offset=entry+8;
			if (count*typlen > 4) offset=S2N(data+offset,4,0);
			if ((rc=(*each)(data,offset,tag,typ,
							typlen,count,priv)))
				return rc;
		}
	}
	return NULL;
}

unsigned char *exifsearch(char *key,unsigned char *buf,off_t bufsize) {
	if (buf == NULL) return NULL;
	return exifwalk(buf+6,0,exifmatch,(void*)key);
}

void exifdump(unsigned char *buf,off_t bufsize) {
	if (buf == NULL) return;
	exifwalk(buf+6,0,exifprt,NULL);
}
