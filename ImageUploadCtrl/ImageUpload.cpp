// ImageUpload.cpp : Implementation of CImageUpload
#include "stdafx.h"
#include "ImageUpload.h"
#include "../Common/GetImageDlg.h"
#include "../Common/HTTPClient.h"

// CImageUpload ---------------------------------------------------------------

UINT WM_DCDOWNLOADCOMPLETE = ::RegisterWindowMessage(_T("dcuploadDownloadComplete"));

CImageUpload::CImageUpload()
{
	m_bWindowOnly = TRUE;
	m_bAutoSize = TRUE;
	m_bRecomposeOnResize = TRUE;

	// These defaults are typically overridden by the web application and
	// do not need to be loaded or saved in the registry
	m_fCameraType = CT_USB;
	m_fResizeMode = RM_DESIGN;
	m_fStretchMode = SM_RESAMPLE;
	m_cxDesignWidth = -1;
	m_cyDesignHeight = -1;

	m_hDownloadThread = NULL;
	m_fCancelDownload = FALSE;
}

void CImageUpload::CheckDesignSize()
{
	if(m_cxDesignWidth < 0 || m_cyDesignHeight < 0)
	{
		SIZE sizePixels;
		AtlHiMetricToPixel(&m_sizeExtent, &sizePixels);
		m_cxDesignWidth = sizePixels.cx;
		m_cyDesignHeight = sizePixels.cy;
	}
}

STDMETHODIMP CImageUpload::GetImage()
{
	CheckDesignSize();
	CGetImageDlg gidlg;
	gidlg.m_fCameraType = m_fCameraType;
	gidlg.m_fResizeMode = m_fResizeMode;
	gidlg.m_fStretchMode = m_fStretchMode;
	gidlg.m_cxDesignWidth = m_cxDesignWidth;
	gidlg.m_cyDesignHeight = m_cyDesignHeight;
	gidlg.m_strUploadImageURL = m_strUploadImageURL;
	if(gidlg.DoModal() == IDOK)
	{
		SIZE sizeTemp = m_sizeExtent;
		SIZE sizeExtent = { gidlg.m_cxImage, gidlg.m_cyImage };
		AtlPixelToHiMetric(&sizeExtent, &m_sizeExtent);

		IOleClientSite* pClientSite = NULL;
		if(SUCCEEDED(GetClientSite(&pClientSite)))
		{
			if(!SUCCEEDED(pClientSite->RequestNewObjectLayout()))
				m_sizeExtent = sizeTemp;
			pClientSite->Release();
		}

		CComBSTR strTemp = m_strImageURL;
		put_ImageURL(strTemp);
	}

	return S_OK;
}

STDMETHODIMP CImageUpload::get_ResizeMode(long *pVal)
{
	*pVal = m_fResizeMode;
	return S_OK;
}

STDMETHODIMP CImageUpload::put_ResizeMode(long newVal)
{
	if(newVal > 0 && newVal <= RM_IMAGE)
	{
		m_fResizeMode = (RESIZE_MODE)newVal;
		return S_OK;
	}
	else
	{
		return Error(_T("Unknown Resize Mode"));
	}
}

STDMETHODIMP CImageUpload::get_StretchMode(long *pVal)
{
	*pVal = m_fStretchMode;
	return S_OK;
}

STDMETHODIMP CImageUpload::put_StretchMode(long newVal)
{
	if(newVal > 0 && newVal <= SM_CROP)
	{
		m_fStretchMode = (STRETCH_MODE)newVal;
		return S_OK;
	}
	else
	{
		return Error(_T("Unknown Stretch Mode"));
	}
}

STDMETHODIMP CImageUpload::get_DesignWidth(long *pVal)
{
	CheckDesignSize();
	*pVal = m_cxDesignWidth;
	return S_OK;
}

STDMETHODIMP CImageUpload::put_DesignWidth(long newVal)
{
	m_cxDesignWidth = newVal < 0 ? -newVal : newVal;

	SIZE sizeTemp = m_sizeExtent;
	SIZE sizeExtent = { m_cxDesignWidth, m_cyDesignHeight };
	AtlPixelToHiMetric(&sizeExtent, &m_sizeExtent);

	IOleClientSite* pClientSite = NULL;
	if(SUCCEEDED(GetClientSite(&pClientSite)))
	{
		if(!SUCCEEDED(pClientSite->RequestNewObjectLayout()))
			m_sizeExtent = sizeTemp;
		pClientSite->Release();
	}

	return S_OK;
}

STDMETHODIMP CImageUpload::get_DesignHeight(long *pVal)
{
	CheckDesignSize();
	*pVal = m_cyDesignHeight;
	return S_OK;
}

STDMETHODIMP CImageUpload::put_DesignHeight(long newVal)
{
	m_cyDesignHeight = newVal < 0 ? -newVal : newVal;

	SIZE sizeTemp = m_sizeExtent;
	SIZE sizeExtent = { m_cxDesignWidth, m_cyDesignHeight };
	AtlPixelToHiMetric(&sizeExtent, &m_sizeExtent);

	IOleClientSite* pClientSite = NULL;
	if(SUCCEEDED(GetClientSite(&pClientSite)))
	{
		if(!SUCCEEDED(pClientSite->RequestNewObjectLayout()))
			m_sizeExtent = sizeTemp;
		pClientSite->Release();
	}

	return S_OK;
}

STDMETHODIMP CImageUpload::get_UploadImageURL(BSTR *pVal)
{
	*pVal = m_strUploadImageURL.Copy();

	return S_OK;
}

STDMETHODIMP CImageUpload::put_UploadImageURL(BSTR newVal)
{
	m_strUploadImageURL = newVal;

	return S_OK;
}

STDMETHODIMP CImageUpload::get_ImageURL(BSTR *pVal)
{
	*pVal = m_strImageURL.Copy();

	return S_OK;
}

STDMETHODIMP CImageUpload::put_ImageURL(BSTR newVal)
{
	// If we are downloading an image, we first cancel 
	// the current download and wait for the download thread
	// to exit.

	if(m_hDownloadThread)
	{
		m_fCancelDownload = TRUE;
		::WaitForSingleObject(m_hDownloadThread, 60*1000);
		::CloseHandle(m_hDownloadThread);
		m_hDownloadThread = NULL;
		m_fCancelDownload = FALSE;
	}

	m_dci.DeleteImage();
	m_strImageURL = newVal;

	DWORD idThread;
	m_hDownloadThread = CreateThread(NULL, 0, DownloadImageThread, this, 0, &idThread);

	return S_OK;
}

STDMETHODIMP CImageUpload::get_CameraType(long *pVal)
{
	*pVal = m_fCameraType;
	return S_OK;
}

STDMETHODIMP CImageUpload::put_CameraType(long newVal)
{
	if(newVal >= CT_PDC640 && newVal <= CT_URL)
	{
		m_fCameraType = (CAMERA_TYPE)newVal;
		return S_OK;
	}
	else
	{
		return Error(_T("Unknown Camera Type"));
	}
}

HRESULT CImageUpload::OnDraw(ATL_DRAWINFO& di)
{
	RECT& rcDest = *(RECT*)di.prcBounds;
	HDC hdc = di.hdcDraw;
	CComBSTR strError;

	if(m_dci.IsEmpty())
	{
		if(m_strStatus.Length())
			strError = m_strStatus;
		else
			strError = _T("Image unavailable. Click Get Image to get image from camera.");
		goto ExitError;
	}
	if(!m_dci.DrawImage(hdc, rcDest))
	{
		strError = m_dci.GetErrorMessage();
		goto ExitError;
	}
	if(m_strStatus.Length())
	{
		USES_CONVERSION;
		RECT rcText = rcDest;
		SetBkMode(hdc, TRANSPARENT);
		int cyText = DrawText(hdc, OLE2T(m_strStatus), m_strStatus.Length(),
			&rcText, DT_CALCRECT | DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
		rcText = rcDest;
		rcText.top += (rcDest.bottom - rcDest.top) / 2 -  cyText / 2;
		DrawText(hdc, OLE2T(m_strStatus), m_strStatus.Length(),
			&rcText, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
	}


// ExitSuccess:

	return S_OK;

ExitError:
	USES_CONVERSION;

	if(strError.Length() == 0)
		strError = _T("Unknown error. Cannot display bitmap.");

	Rectangle(di.hdcDraw, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom);
	RECT rcText = rcDest;
	int cyText = DrawText(hdc, OLE2T(strError), strError.Length(),
		&rcText, DT_CALCRECT | DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
	rcText = rcDest;
	rcText.top += (rcDest.bottom - rcDest.top) / 2 -  cyText / 2;
	DrawText(hdc, OLE2T(strError), strError.Length(),
		&rcText, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

	return S_OK;
}

LRESULT CImageUpload::OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CStatusMessage* psm = (CStatusMessage*)wParam;
	m_strStatus = psm->m_bstrStatus;
	FireViewChange();
	delete psm;
	return TRUE;
}

LRESULT CImageUpload::OnDownloadComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam)
	{
		CheckDesignSize();
		void* pvImage = (void*)wParam;
		UINT cbImage = (UINT)lParam;

		m_dci.m_fResizeMode = m_fResizeMode;
		m_dci.m_fStretchMode = m_fStretchMode;
		m_dci.m_cxDesignWidth = m_cxDesignWidth;
		m_dci.m_cyDesignHeight = m_cyDesignHeight;
		if(m_dci.SetImage(pvImage, cbImage))
		{
			m_strStatus.Empty();
		}
		else
		{
			m_strStatus = m_dci.GetErrorMessage();
		}
	}

	m_fCancelDownload = FALSE;
	if(m_hDownloadThread)
	{
		::CloseHandle(m_hDownloadThread);
		m_hDownloadThread = NULL;
	}
	FireViewChange();

	return TRUE;
}

// DownloadImageThread --------------------------------------------------------

DWORD WINAPI DownloadImageThread(LPVOID pv)
{
	USES_CONVERSION;
	CImageUpload* pctl = (CImageUpload*)pv;
	HWND hwnd = pctl->m_hWnd;
	CHTTPClient hc;
	void* pvImage = NULL;
	UINT cbImageCur = 0;
	UINT cbImageTotal = 0;
	UINT cbImageRead = 0;
	char szBuffer[1024];

	CStatusMessage::SetStatus(hwnd, ST_INFORMATION, _T("Contacting server"));
	if(!hc.OpenFileDownloadURL(OLE2T(pctl->m_strImageURL)))
		goto ExitHTTPError;

	if(pctl->m_fCancelDownload)
		goto ExitCancel;
	CStatusMessage::SetStatus(hwnd, ST_INFORMATION, _T("Downloading headers"));
	// Content length is not always available...
	if(!hc.GetContentLength(cbImageTotal))
		cbImageTotal = 0;

	CStatusMessage::SetStatus(hwnd, ST_INFORMATION, _T("Downloading image"));
	for(;;)
	{
		if(pctl->m_fCancelDownload)
			goto ExitCancel;
		cbImageRead = sizeof(szBuffer);
		if(!hc.Read(szBuffer, cbImageRead))
			goto ExitHTTPError;
		if(cbImageRead == 0)
			break;

		pvImage = realloc(pvImage, cbImageCur + cbImageRead);
		memcpy((char*)pvImage + cbImageCur, szBuffer, cbImageRead);
		cbImageCur += cbImageRead;
		//if(cbImageRead != sizeof(szBuffer))
		//	break;

		if(cbImageTotal)
			wsprintf(szBuffer, _T("Downloading image: %d of %d"), cbImageCur, cbImageTotal);
		else
			wsprintf(szBuffer, _T("Downloading image: %d"), cbImageCur);
		CStatusMessage::SetStatus(hwnd, ST_INFORMATION, szBuffer);
	}

// ExitSuccess:
	::PostMessage(hwnd, WM_DCDOWNLOADCOMPLETE, (WPARAM)pvImage, (LPARAM)cbImageCur);
	return TRUE;

ExitCancel:
	{
		free(pvImage);
		::PostMessage(hwnd, WM_DCDOWNLOADCOMPLETE, NULL, 0);
		return FALSE;
	}

ExitHTTPError:
	{
		USES_CONVERSION;
		free(pvImage);
		CStatusMessage::SetStatus(hwnd, ST_WARNING, 
			OLE2T(hc.GetErrorMessage()));
		::PostMessage(hwnd, WM_DCDOWNLOADCOMPLETE, NULL, 0);
		return FALSE;
	}
}

