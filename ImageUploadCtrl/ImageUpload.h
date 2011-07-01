// ImageUpload.h : Declaration of the CImageUpload
#pragma once
#include "resource.h"       // main symbols
#include "../Common/DCImage.h"
#include "../Common/DCCamera.h"
#include <atlctl.h>
#include "ImageUploadCtrl.h"
#include "_IImageUploadEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// CImageUpload
class ATL_NO_VTABLE CImageUpload :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IImageUpload, &IID_IImageUpload, &LIBID_ImageUploadCtrlLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IPersistStreamInitImpl<CImageUpload>,
	public IOleControlImpl<CImageUpload>,
	public IOleObjectImpl<CImageUpload>,
	public IOleInPlaceActiveObjectImpl<CImageUpload>,
	public IViewObjectExImpl<CImageUpload>,
	public IOleInPlaceObjectWindowlessImpl<CImageUpload>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CImageUpload>,
	public CProxy_IImageUploadEvents<CImageUpload>,
	public IPersistStorageImpl<CImageUpload>,
	public ISpecifyPropertyPagesImpl<CImageUpload>,
	public IQuickActivateImpl<CImageUpload>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CImageUpload>,
#endif
	public IProvideClassInfo2Impl<&CLSID_ImageUpload, &__uuidof(_IImageUploadEvents), &LIBID_ImageUploadCtrlLib>,
	public IObjectSafetyImpl<CImageUpload, INTERFACESAFE_FOR_UNTRUSTED_CALLER | INTERFACESAFE_FOR_UNTRUSTED_DATA>,
	public CComCoClass<CImageUpload, &CLSID_ImageUpload>,
	public CComControl<CImageUpload>
{
public:
	CImageUpload();

public:
	CAMERA_TYPE m_fCameraType;
	RESIZE_MODE m_fResizeMode;
	STRETCH_MODE m_fStretchMode;
	long m_cxDesignWidth;
	long m_cyDesignHeight;

protected:
	CDCImage m_dci;
	CComBSTR m_strImageURL;
	CComBSTR m_strUploadImageURL;
	CComBSTR m_strStatus;
	HANDLE m_hDownloadThread;
	BOOL m_fCancelDownload;

	void CheckDesignSize();

public:

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_IMAGEUPLOAD)

BEGIN_COM_MAP(CImageUpload)
	COM_INTERFACE_ENTRY(IImageUpload)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(CImageUpload)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CImageUpload)
	CONNECTION_POINT_ENTRY(__uuidof(_IImageUploadEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CImageUpload)
	CHAIN_MSG_MAP(CComControl<CImageUpload>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IImageUpload,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// Misc
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

// IImageUpload
public:
	STDMETHOD(GetImage)(void);
	STDMETHOD(get_ResizeMode)(LONG* pVal);
	STDMETHOD(put_ResizeMode)(LONG newVal);
	STDMETHOD(get_StretchMode)(LONG* pVal);
	STDMETHOD(put_StretchMode)(LONG newVal);
	STDMETHOD(get_DesignWidth)(LONG* pVal);
	STDMETHOD(put_DesignWidth)(LONG newVal);
	STDMETHOD(get_DesignHeight)(LONG* pVal);
	STDMETHOD(put_DesignHeight)(LONG newVal);
	STDMETHOD(get_ImageURL)(BSTR* pVal);
	STDMETHOD(put_ImageURL)(BSTR newVal);
	STDMETHOD(get_UploadImageURL)(BSTR* pVal);
	STDMETHOD(put_UploadImageURL)(BSTR newVal);
	STDMETHOD(get_CameraType)(LONG* pVal);
	STDMETHOD(put_CameraType)(LONG newVal);

	HRESULT OnDraw(ATL_DRAWINFO& di);
	LRESULT OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDownloadComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

friend DWORD WINAPI DownloadImageThread(LPVOID pv);
};

OBJECT_ENTRY_AUTO(__uuidof(ImageUpload), CImageUpload)
