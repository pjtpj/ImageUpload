

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ImageUploadCtrl_h__
#define __ImageUploadCtrl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IImageUpload_FWD_DEFINED__
#define __IImageUpload_FWD_DEFINED__
typedef interface IImageUpload IImageUpload;
#endif 	/* __IImageUpload_FWD_DEFINED__ */


#ifndef ___IImageUploadEvents_FWD_DEFINED__
#define ___IImageUploadEvents_FWD_DEFINED__
typedef interface _IImageUploadEvents _IImageUploadEvents;
#endif 	/* ___IImageUploadEvents_FWD_DEFINED__ */


#ifndef __ImageUpload_FWD_DEFINED__
#define __ImageUpload_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageUpload ImageUpload;
#else
typedef struct ImageUpload ImageUpload;
#endif /* __cplusplus */

#endif 	/* __ImageUpload_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IImageUpload_INTERFACE_DEFINED__
#define __IImageUpload_INTERFACE_DEFINED__

/* interface IImageUpload */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IImageUpload;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A2DADCBF-FF08-4772-8F46-D9E6D1086B74")
    IImageUpload : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetImage( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ResizeMode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ResizeMode( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_StretchMode( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_StretchMode( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DesignWidth( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DesignWidth( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DesignHeight( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DesignHeight( 
            /* [in] */ LONG newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImageURL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ImageURL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UploadImageURL( 
            /* [retval][out] */ BSTR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UploadImageURL( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CameraType( 
            /* [retval][out] */ LONG *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CameraType( 
            /* [in] */ LONG newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IImageUploadVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IImageUpload * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IImageUpload * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IImageUpload * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IImageUpload * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IImageUpload * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IImageUpload * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IImageUpload * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetImage )( 
            IImageUpload * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ResizeMode )( 
            IImageUpload * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ResizeMode )( 
            IImageUpload * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_StretchMode )( 
            IImageUpload * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_StretchMode )( 
            IImageUpload * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DesignWidth )( 
            IImageUpload * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DesignWidth )( 
            IImageUpload * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_DesignHeight )( 
            IImageUpload * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_DesignHeight )( 
            IImageUpload * This,
            /* [in] */ LONG newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImageURL )( 
            IImageUpload * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ImageURL )( 
            IImageUpload * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UploadImageURL )( 
            IImageUpload * This,
            /* [retval][out] */ BSTR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UploadImageURL )( 
            IImageUpload * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CameraType )( 
            IImageUpload * This,
            /* [retval][out] */ LONG *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CameraType )( 
            IImageUpload * This,
            /* [in] */ LONG newVal);
        
        END_INTERFACE
    } IImageUploadVtbl;

    interface IImageUpload
    {
        CONST_VTBL struct IImageUploadVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IImageUpload_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IImageUpload_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IImageUpload_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IImageUpload_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IImageUpload_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IImageUpload_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IImageUpload_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IImageUpload_GetImage(This)	\
    (This)->lpVtbl -> GetImage(This)

#define IImageUpload_get_ResizeMode(This,pVal)	\
    (This)->lpVtbl -> get_ResizeMode(This,pVal)

#define IImageUpload_put_ResizeMode(This,newVal)	\
    (This)->lpVtbl -> put_ResizeMode(This,newVal)

#define IImageUpload_get_StretchMode(This,pVal)	\
    (This)->lpVtbl -> get_StretchMode(This,pVal)

#define IImageUpload_put_StretchMode(This,newVal)	\
    (This)->lpVtbl -> put_StretchMode(This,newVal)

#define IImageUpload_get_DesignWidth(This,pVal)	\
    (This)->lpVtbl -> get_DesignWidth(This,pVal)

#define IImageUpload_put_DesignWidth(This,newVal)	\
    (This)->lpVtbl -> put_DesignWidth(This,newVal)

#define IImageUpload_get_DesignHeight(This,pVal)	\
    (This)->lpVtbl -> get_DesignHeight(This,pVal)

#define IImageUpload_put_DesignHeight(This,newVal)	\
    (This)->lpVtbl -> put_DesignHeight(This,newVal)

#define IImageUpload_get_ImageURL(This,pVal)	\
    (This)->lpVtbl -> get_ImageURL(This,pVal)

#define IImageUpload_put_ImageURL(This,newVal)	\
    (This)->lpVtbl -> put_ImageURL(This,newVal)

#define IImageUpload_get_UploadImageURL(This,pVal)	\
    (This)->lpVtbl -> get_UploadImageURL(This,pVal)

#define IImageUpload_put_UploadImageURL(This,newVal)	\
    (This)->lpVtbl -> put_UploadImageURL(This,newVal)

#define IImageUpload_get_CameraType(This,pVal)	\
    (This)->lpVtbl -> get_CameraType(This,pVal)

#define IImageUpload_put_CameraType(This,newVal)	\
    (This)->lpVtbl -> put_CameraType(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IImageUpload_GetImage_Proxy( 
    IImageUpload * This);


void __RPC_STUB IImageUpload_GetImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_ResizeMode_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IImageUpload_get_ResizeMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_ResizeMode_Proxy( 
    IImageUpload * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IImageUpload_put_ResizeMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_StretchMode_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IImageUpload_get_StretchMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_StretchMode_Proxy( 
    IImageUpload * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IImageUpload_put_StretchMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_DesignWidth_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IImageUpload_get_DesignWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_DesignWidth_Proxy( 
    IImageUpload * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IImageUpload_put_DesignWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_DesignHeight_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IImageUpload_get_DesignHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_DesignHeight_Proxy( 
    IImageUpload * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IImageUpload_put_DesignHeight_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_ImageURL_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IImageUpload_get_ImageURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_ImageURL_Proxy( 
    IImageUpload * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IImageUpload_put_ImageURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_UploadImageURL_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ BSTR *pVal);


void __RPC_STUB IImageUpload_get_UploadImageURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_UploadImageURL_Proxy( 
    IImageUpload * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IImageUpload_put_UploadImageURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IImageUpload_get_CameraType_Proxy( 
    IImageUpload * This,
    /* [retval][out] */ LONG *pVal);


void __RPC_STUB IImageUpload_get_CameraType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IImageUpload_put_CameraType_Proxy( 
    IImageUpload * This,
    /* [in] */ LONG newVal);


void __RPC_STUB IImageUpload_put_CameraType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IImageUpload_INTERFACE_DEFINED__ */



#ifndef __ImageUploadCtrlLib_LIBRARY_DEFINED__
#define __ImageUploadCtrlLib_LIBRARY_DEFINED__

/* library ImageUploadCtrlLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ImageUploadCtrlLib;

#ifndef ___IImageUploadEvents_DISPINTERFACE_DEFINED__
#define ___IImageUploadEvents_DISPINTERFACE_DEFINED__

/* dispinterface _IImageUploadEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__IImageUploadEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("0730AD53-D0F9-4F75-A3EE-C06BCB34F140")
    _IImageUploadEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _IImageUploadEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _IImageUploadEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _IImageUploadEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _IImageUploadEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _IImageUploadEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _IImageUploadEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _IImageUploadEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _IImageUploadEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _IImageUploadEventsVtbl;

    interface _IImageUploadEvents
    {
        CONST_VTBL struct _IImageUploadEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _IImageUploadEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _IImageUploadEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _IImageUploadEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _IImageUploadEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _IImageUploadEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _IImageUploadEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _IImageUploadEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___IImageUploadEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ImageUpload;

#ifdef __cplusplus

class DECLSPEC_UUID("652FE09F-3289-414A-B1D7-2E661FA6119B")
ImageUpload;
#endif
#endif /* __ImageUploadCtrlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


