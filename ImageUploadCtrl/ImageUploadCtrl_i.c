

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Sat Mar 03 01:11:46 2007
 */
/* Compiler settings for .\ImageUploadCtrl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IImageUpload,0xA2DADCBF,0xFF08,0x4772,0x8F,0x46,0xD9,0xE6,0xD1,0x08,0x6B,0x74);


MIDL_DEFINE_GUID(IID, LIBID_ImageUploadCtrlLib,0x282CF87D,0x14E9,0x4146,0x83,0x97,0x61,0x8E,0xBC,0x81,0x6D,0xC9);


MIDL_DEFINE_GUID(IID, DIID__IImageUploadEvents,0x0730AD53,0xD0F9,0x4F75,0xA3,0xEE,0xC0,0x6B,0xCB,0x34,0xF1,0x40);


MIDL_DEFINE_GUID(CLSID, CLSID_ImageUpload,0x652FE09F,0x3289,0x414A,0xB1,0xD7,0x2E,0x66,0x1F,0xA6,0x11,0x9B);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



