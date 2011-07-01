// GetImageDlg.cpp : Implementation of CGetImageDlg
#include "stdafx.h"
#include <commctrl.h>
#include "GetImageDlg.h"
#include "HTTPClient.h"
#include "DCSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CGetImageDlg

UINT WM_DCGETTHUMBNAILS = ::RegisterWindowMessage(_T("dcuploadGetThumbnails"));
UINT WM_DCGOTTHUMBNAIL = ::RegisterWindowMessage(_T("dcuploadGotThumbnail"));
UINT WM_DCGOTIMAGE = ::RegisterWindowMessage(_T("dcuploadGotImage"));
UINT WM_DCUPLOADCOMPLETE = ::RegisterWindowMessage(_T("dcuploadUploadComplete"));

const int cThumbnailBorderPixels = 5;

const TCHAR g_szAutomatic[] = _T("Automatic");

const char g_szOldKey[] = "Software\\Exemplar Software\\Digital Camera Upload";
const char g_szKey[] = "Software\\Digital Camera Upload";
const char g_szEmpty[] = _T("XXXemptyXXX");

BOOL GetRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, long& iValue);
BOOL GetRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, CComBSTR& strValue);
BOOL AddRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, long iValue);
BOOL AddRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, const TCHAR* szValue);
BOOL RemoveRegKeys(HKEY hkey, const TCHAR* szSubkey);

static void AddStrings(HWND hwnd, UINT idControl, const TCHAR* rgszStrings[])
{
	HWND hwndCtl = ::GetDlgItem(hwnd, idControl);
	for(int i = 0; rgszStrings[i]; i++)
	{
		::SendMessage(hwndCtl, CB_ADDSTRING, 0, (LPARAM)rgszStrings[i]);
	}
}

CGetImageDlg::CGetImageDlg()
{
	RegistryToSettings();

	// These settings will be set by the web server via DCUploadCtl
	m_fCameraType = CT_USB;
	m_fResizeMode = RM_DESIGN;
	m_fStretchMode = SM_STRETCH;
	m_cxDesignWidth = 320;
	m_cyDesignHeight = 240;
	m_cxThumbnail = 80;
	m_cyThumbnail = 60;
	m_cxImage = 320;
	m_cyImage = 240;

	m_iCurImage = -1;

	m_pdcc = NULL;
}

CGetImageDlg::~CGetImageDlg()
{
	DestroyCamera();
}

static const TCHAR* g_rgszCameraTypes[] = 
{ "Polaroid PDC 640", "Vivitar Vivicam 3315", "Generic USB Camera", "Computer Folder", "URL", NULL };

LRESULT CGetImageDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AddStrings(m_hWnd, IDC_CAMERA_TYPE, g_rgszCameraTypes);

	SettingsToScreen();

	EnableControls(FALSE);

	RECT rcDlg;
	GetClientRect(&rcDlg);

	HWND hwndThumbnails = GetDlgItem(IDC_THUMBNAILS);
	::GetWindowRect(hwndThumbnails, &m_rcThumbnails);
	ScreenToClient(&m_rcThumbnails);
	::GetClientRect(hwndThumbnails, &m_rcThumbnailsClient);
	HWND hwndPreview = GetDlgItem(IDC_PREVIEW);
	::GetWindowRect(hwndPreview, &m_rcPreview);
	ScreenToClient(&m_rcPreview);
	memcpy(&m_rcPreviewArea, &m_rcPreview, sizeof(m_rcPreviewArea));

	::DestroyWindow(hwndPreview);

	m_cxDlgMin = rcDlg.right  - rcDlg.left;
	m_cyDlgMin = rcDlg.bottom - rcDlg.top;
	m_cxPreviewAreaRightMargin  = rcDlg.right  - m_rcPreviewArea.right;
	m_cyPreviewAreaBottomMargin = rcDlg.bottom - m_rcPreviewArea.bottom;

	RecalculateLayout(m_cxThumbnail, m_cyThumbnail, m_cxImage, m_cyImage);

	PostMessage(WM_DCGETTHUMBNAILS);

	return FALSE;  // Let the system set the focus
}

LRESULT CGetImageDlg::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	DeleteThumbnails();
	return 0;
}

LRESULT CGetImageDlg::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(&ps);
	CComBSTR strError;

	if(m_dciPreview.IsEmpty())
	{
		strError = _T("Select Image on left, then click Preview Image.");
		goto ExitError;
	}
	if(!m_dciPreview.DrawImage(hdc, m_rcPreview))
	{
		strError = m_dciPreview.GetErrorMessage();
		goto ExitError;
	}

// ExitSuccess:

	EndPaint(&ps);
	return 0;

ExitError:
	USES_CONVERSION;

	if(strError.Length() == 0)
		strError = _T("Unknown error. Cannot display preview.");

	Rectangle(hdc, m_rcPreview.left, m_rcPreview.top, 
		m_rcPreview.right, m_rcPreview.bottom);

	RECT rcText = m_rcPreview;
	int cyText = DrawText(hdc, OLE2T(strError), strError.Length(),
		&rcText, DT_CALCRECT | DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);
	rcText = m_rcPreview;
	rcText.top += (m_rcPreview.bottom - m_rcPreview.top) / 2 - 
		cyText / 2;
	DrawText(hdc, OLE2T(strError), strError.Length(),
		&rcText, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

	EndPaint(&ps);
	return 0;
}

LRESULT CGetImageDlg::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam; 

	lpmis->itemWidth = 	m_cxThumbnail + cThumbnailBorderPixels * 2;
	lpmis->itemHeight = m_cyThumbnail + cThumbnailBorderPixels * 2;

	return TRUE; 
}

LRESULT CGetImageDlg::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam; 

	// If there are no list box items, skip this message. 
	if(lpdis->itemID == -1) 
		return FALSE;

	// Draw the bitmap and text for the list box item. Draw a 
	// rectangle around the bitmap if it is selected. 

	switch(lpdis->itemAction) 
	{ 
		case ODA_SELECT: 
		case ODA_DRAWENTIRE: 
		{
			if(lpdis->itemState & ODS_SELECTED)
				::FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));
			else
				::FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));

			int x = lpdis->rcItem.left + cThumbnailBorderPixels;
			int y = lpdis->rcItem.top + cThumbnailBorderPixels;
			RECT rcDest = { x, y, x + m_cxThumbnail, y + m_cyThumbnail };
			CDCImage* pdci = (CDCImage*)::SendMessage(lpdis->hwndItem, 
				LB_GETITEMDATA, lpdis->itemID, (LPARAM) 0);
			pdci->DrawImage(lpdis->hDC, rcDest);

			break; 
		}
		case ODA_FOCUS: 
			// Do not process focus changes. The focus caret 
			// (outline rectangle) indicates the selection. 
			// The IDOK button indicates the final 
			// selection. 

			break; 
	} 

	return TRUE; 
}

LRESULT CGetImageDlg::OnGetThumbnails(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	EnableControls(FALSE);

	DeleteThumbnails();

	::SendMessage(GetDlgItem(IDC_THUMBNAILS), LB_RESETCONTENT, 0, 0);

	DWORD idThread;
	CreateThread(NULL, 0, GetThumbnailsThread, this, 0, &idThread);
	return TRUE;
}

LRESULT CGetImageDlg::OnGotThumbnail(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL fLastThumbnail = (BOOL)wParam;
	CDCImage* pdci = (CDCImage*)lParam;
	if(pdci)
	{
		if(pdci->GetImageWidth() != m_cxThumbnail)
			RecalculateLayout(pdci->GetImageWidth(), pdci->GetImageHeight(),
				m_cxImage, m_cyImage);

		::SendMessage(GetDlgItem(IDC_THUMBNAILS), LB_ADDSTRING, 0, lParam);
	}

	if(fLastThumbnail)
		EnableControls(TRUE);

	return TRUE;
}

LRESULT CGetImageDlg::OnGotImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL fSuccess = (BOOL)wParam;

	if(fSuccess)
	{
		if(m_dciPreview.GetImageWidth() != m_cxImage || 
			m_dciPreview.GetImageHeight() != m_cyImage )
		{
			RecalculateLayout(m_cxThumbnail, m_cyThumbnail, 
				m_dciPreview.GetImageWidth(), m_dciPreview.GetImageHeight());
		}
		Invalidate();
	}
	else
	{
		m_iCurImage = -1;
	}

	EnableControls(TRUE);

	return TRUE;
}

LRESULT CGetImageDlg::OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;
	CStatusMessage* psm = (CStatusMessage*)wParam;
	if(psm->m_fType == ST_ERROR)
	{
		MessageBox(OLE2T(psm->m_bstrStatus));
		EndDialog(IDCANCEL);
	}
	else
	{
		SetStatus(OLE2T(psm->m_bstrStatus));
	}
	delete psm;
	return TRUE;
}

LRESULT CGetImageDlg::OnSetProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CProgressMessage* ppm = (CProgressMessage*)wParam;
	SetProgress(ppm->m_cTotalParts, ppm->m_cPartsCompleted);
	delete ppm;
	return TRUE;
}

LRESULT CGetImageDlg::OnUploadComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(wParam)
		EndDialog(IDOK);
	else
		EnableControls(TRUE);
	return TRUE;
}

LRESULT CGetImageDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ScreenToSettings();
	SettingsToRegistry();

	if(m_dciPreview.IsEmpty())
		return 0;

	EnableControls(FALSE);

	DWORD idThread;
	CreateThread(NULL, 0, UploadImageThread, this, 0, &idThread);

	return 0;
}

LRESULT CGetImageDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CGetImageDlg::OnClickedPreviewImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ScreenToSettings();
	SettingsToRegistry();

	LRESULT lres = ::SendMessage(GetDlgItem(IDC_THUMBNAILS), LB_GETCURSEL, 0, 0);
	if(lres == LB_ERR)
		return 0;

	EnableControls(FALSE);

	m_iCurImage = (int)lres;

	DWORD idThread;
	CreateThread(NULL, 0, GetImageThread, this, 0, &idThread);

	return 0;
}

LRESULT CGetImageDlg::OnClickedSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ScreenToSettings();

	CDCSettings dcsdlg;
	dcsdlg.m_fCameraType       = m_fCameraType;
	dcsdlg.m_strDevice         = m_strDevice;
	dcsdlg.m_nSpeed            = m_nSpeed;
	dcsdlg.m_fResizeMode       = m_fResizeMode;
	dcsdlg.m_fStretchMode      = m_fStretchMode;
	dcsdlg.m_cxDesignWidth     = m_cxDesignWidth;
	dcsdlg.m_cyDesignHeight    = m_cyDesignHeight;
	if(dcsdlg.DoModal() == IDOK)
	{
		USES_CONVERSION;
		BOOL fGetThumbnails = FALSE;

		if( m_fCameraType != dcsdlg.m_fCameraType ||
			lstrcmp(OLE2T(m_strDevice), OLE2T(dcsdlg.m_strDevice)) ||
			m_nSpeed != dcsdlg.m_nSpeed )
		{
			fGetThumbnails = TRUE;
		}

		m_fCameraType       = dcsdlg.m_fCameraType;
		m_strDevice         = dcsdlg.m_strDevice;
		m_nSpeed            = dcsdlg.m_nSpeed;
		m_fResizeMode       = dcsdlg.m_fResizeMode;
		m_fStretchMode      = dcsdlg.m_fStretchMode;
		m_cxDesignWidth     = dcsdlg.m_cxDesignWidth;
		m_cyDesignHeight    = dcsdlg.m_cyDesignHeight;

		SettingsToScreen();

		if(fGetThumbnails)
			PostMessage(WM_DCGETTHUMBNAILS);
	}

	return 0;
}

LRESULT CGetImageDlg::OnSelChangeCameraType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	USES_CONVERSION;

	// Pick up new camera type and path changes 
	ScreenToSettings();

	EnableControls(TRUE);

	// If we are changing camera types and there is a path
	// we can use, refresh the thumbnails

	if(m_fCameraType == CT_URL)
	{
		if(m_strURL.Length())
			PostMessage(WM_DCGETTHUMBNAILS);
	}
	else if(m_fCameraType == CT_COMPUTER_FOLDER)
	{
		if(m_strComputerFolder.Length())
			PostMessage(WM_DCGETTHUMBNAILS);
	}
	else
	{
		PostMessage(WM_DCGETTHUMBNAILS);
	}

	return 0;
}

//The call back is used to set the initial directory and
//display the selected path in thhe top of the box.
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData) 
{
	USES_CONVERSION;

    switch(uMsg) 
    {
	    case BFFM_INITIALIZED: 
			SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)OLE2T((BSTR)pData));
	        break;
    }
    return 0;
}

LRESULT CGetImageDlg::OnClickedBrowseFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	USES_CONVERSION;

	// Pick up any changes to paths
	ScreenToSettings();

	TCHAR szFolder[MAX_PATH];
	szFolder[0] = 0;
	BROWSEINFO bif;
	memset(&bif, 0, sizeof(bif));
	bif.hwndOwner = m_hWnd;
	bif.pszDisplayName = szFolder;
	if(m_fCameraType == CT_USB)
		bif.lpszTitle = "Find Camera Folder";
	else
		bif.lpszTitle = "Find Computer Folder";
	bif.ulFlags = BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS;
	bif.lpfn = BrowseCallbackProc;

	IMalloc* imalloc = 0;
	SHGetMalloc(&imalloc); // Yes, we will crash if this fails.

	if(m_fCameraType == CT_USB && lstrcmp(OLE2T(m_strImagesFolder.m_str), g_szAutomatic) != 0)
	{
		bif.lParam = (LPARAM)m_strImagesFolder.m_str;
	}
	if(m_fCameraType == CT_COMPUTER_FOLDER && !m_strComputerFolder.Length())
	{
		// If computer folder is empty, try and find My Pictures
		// If My Pictures doesn't exist, start with My Documents
		ITEMIDLIST* pidlMyDocuments = NULL;
		if(SHGetSpecialFolderLocation(m_hWnd, CSIDL_PERSONAL, &pidlMyDocuments) == NOERROR)
		{
			TCHAR szMyDocuments[MAX_PATH];
			if(SHGetPathFromIDList(pidlMyDocuments, szMyDocuments))
			{
				CComBSTR strMyDocuments = szMyDocuments;

				CComBSTR strMyPictures = ExsMakePath(szMyDocuments, _T("My Pictures"));

				if(ExsDirectoryExists(OLE2CT(strMyPictures)))
					bif.lParam = (LPARAM)strMyPictures.m_str;
				else
					bif.lParam = (LPARAM)strMyDocuments.m_str;
			}

			imalloc->Free(pidlMyDocuments);
		}
	}
	else if(m_fCameraType == CT_COMPUTER_FOLDER && m_strComputerFolder.Length())
	{
		bif.lParam = (LPARAM)m_strComputerFolder.m_str;
	}

	ITEMIDLIST* pidl = SHBrowseForFolder(&bif);
	if(pidl != NULL)
	{
		// get the name of the folder
		TCHAR path[MAX_PATH];
		if(SHGetPathFromIDList(pidl, path))
		{
			if(m_fCameraType == CT_USB)
				m_strImagesFolder = path;
			else
				m_strComputerFolder = path;

			SettingsToScreen();
			PostMessage(WM_DCGETTHUMBNAILS);
		}

		imalloc->Free(pidl);
	}

	imalloc->Release();

	return 0;
}

LRESULT CGetImageDlg::OnClickedGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	// Pick up any changes to paths
	ScreenToSettings();

	PostMessage(WM_DCGETTHUMBNAILS);
	return 0;
}

void CGetImageDlg::DeleteThumbnails()
{
	HWND hwnd = GetDlgItem(IDC_THUMBNAILS);
	for(;;)
	{
		LRESULT lres = ::SendMessage(hwnd, LB_GETITEMDATA, 0, (LPARAM)0);
		if(lres == LB_ERR || lres == 0)
			break;

		CDCImage* pdci = (CDCImage*)lres;
		delete pdci;

		lres = SendMessage(hwnd, LB_DELETESTRING, 0, (LPARAM)0);
		if(lres == LB_ERR)
			break;
	}
}

UINT CGetImageDlg::GetCountThumbnails()
{
	LRESULT lres = SendDlgItemMessage(IDC_THUMBNAILS, LB_GETCOUNT, 0, (LPARAM)0);
	if(lres == LB_ERR)
		return 0;

	return (UINT)lres;
}

void CGetImageDlg::SettingsToRegistry()
{
	USES_CONVERSION;

	// Clean up old settings
	RemoveRegKeys(HKEY_LOCAL_MACHINE, g_szOldKey);

	// Overwrite all settings
	AddRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("Device"), OLE2T(m_strDevice));
	AddRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("Speed"), m_nSpeed);
	AddRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("ImagesFolder"), OLE2T(m_strImagesFolder));
	AddRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("ComputerFolder"), OLE2T(m_strComputerFolder));
}

void CGetImageDlg::RegistryToSettings()
{
	USES_CONVERSION;

	// These settings are never seen on the web server, so they need
	// to be saved locally
	m_strDevice = g_szEmpty;
	GetRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("Device"), m_strDevice);
	if(!lstrcmp(OLE2CT(m_strDevice), g_szEmpty))
	{
		m_strDevice.Empty();
		GetRegValue(HKEY_LOCAL_MACHINE, g_szOldKey, _T("Device"), m_strDevice);
	}
	m_nSpeed = -1;
	GetRegValue(HKEY_LOCAL_MACHINE, g_szOldKey, _T("Speed"), m_nSpeed);
	if(m_nSpeed < 0)
	{
		m_nSpeed = 0;
		GetRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("Speed"), m_nSpeed);
		if(m_nSpeed < 0)
			m_nSpeed = 0;
	}
	m_strImagesFolder = g_szEmpty;
	GetRegValue(HKEY_LOCAL_MACHINE, g_szOldKey, _T("ImagesFolder"), m_strImagesFolder);
	if(!lstrcmp(OLE2CT(m_strImagesFolder), g_szEmpty))
	{
		m_strImagesFolder.Empty();
		GetRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("ImagesFolder"), m_strImagesFolder);
	}
	GetRegValue(HKEY_LOCAL_MACHINE, g_szKey, _T("ComputerFolder"), m_strComputerFolder);
}

void CGetImageDlg::SettingsToScreen()
{
	USES_CONVERSION;

	SendDlgItemMessage(IDC_CAMERA_TYPE, CB_SETCURSEL, (WPARAM)m_fCameraType - 1, 0);

	CComBSTR strImagesFolder = m_strImagesFolder;
	if(!strImagesFolder.Length())
		strImagesFolder = g_szAutomatic;
	SetDlgItemText(IDC_IMAGES_FOLDER, OLE2T(strImagesFolder));

	SetDlgItemText(IDC_COMPUTER_FOLDER, OLE2T(m_strComputerFolder));

	SetDlgItemText(IDC_URL, OLE2T(m_strURL));
}

void CGetImageDlg::ScreenToSettings()
{
	USES_CONVERSION;

	m_fCameraType = (CAMERA_TYPE)(SendDlgItemMessage(IDC_CAMERA_TYPE, CB_GETCURSEL, 0, 0) + 1);

	GetDlgItemText(IDC_IMAGES_FOLDER, m_strImagesFolder.m_str);
	if(lstrcmpi(OLE2T(m_strImagesFolder), g_szAutomatic) == 0)
		m_strImagesFolder.Empty();
	
	GetDlgItemText(IDC_COMPUTER_FOLDER, m_strComputerFolder.m_str);

	GetDlgItemText(IDC_URL, m_strURL.m_str);
}

void CGetImageDlg::EnableControls(BOOL fEnable)
{
	UINT cThumbnails = GetCountThumbnails();

	::EnableWindow(GetDlgItem(IDC_THUMBNAILS), fEnable && cThumbnails);
	::EnableWindow(GetDlgItem(IDC_CAMERA_TYPE), fEnable);
	::EnableWindow(GetDlgItem(IDC_URL), fEnable && m_fCameraType == CT_URL);
	::EnableWindow(GetDlgItem(IDC_IMAGES_FOLDER), fEnable && m_fCameraType == CT_USB);
	::EnableWindow(GetDlgItem(IDC_COMPUTER_FOLDER), fEnable && m_fCameraType == CT_COMPUTER_FOLDER);
	::EnableWindow(GetDlgItem(IDC_BROWSE_FOLDER), fEnable && (m_fCameraType == CT_USB || m_fCameraType == CT_COMPUTER_FOLDER));
	::EnableWindow(GetDlgItem(IDC_GO), fEnable && (m_fCameraType == CT_USB || m_fCameraType == CT_COMPUTER_FOLDER || m_fCameraType == CT_URL));

	::ShowWindow(GetDlgItem(IDC_URL), m_fCameraType == CT_URL ? SW_SHOW : SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_IMAGES_FOLDER), m_fCameraType == CT_USB ? SW_SHOW : SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_COMPUTER_FOLDER), m_fCameraType == CT_COMPUTER_FOLDER ? SW_SHOW : SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_BROWSE_FOLDER), (m_fCameraType == CT_USB || m_fCameraType == CT_COMPUTER_FOLDER) ? SW_SHOW : SW_HIDE);
	::ShowWindow(GetDlgItem(IDC_GO), (m_fCameraType == CT_USB || m_fCameraType == CT_COMPUTER_FOLDER || m_fCameraType == CT_URL) ? SW_SHOW : SW_HIDE);

	::EnableWindow(GetDlgItem(IDC_PREVIEW_IMAGE), fEnable && cThumbnails);
	::EnableWindow(GetDlgItem(IDOK), fEnable && !m_dciPreview.IsEmpty());
	::EnableWindow(GetDlgItem(IDCANCEL), fEnable);
	::EnableWindow(GetDlgItem(IDC_SETTINGS), fEnable);
}

void CGetImageDlg::MoveControl(int idcControl, int dxLeft, int dxRight)
{
	HWND hwndControl = GetDlgItem(idcControl);

	RECT rc;
	::GetWindowRect(hwndControl, &rc);
	ScreenToClient(&rc);
	rc.left  += dxLeft;
	rc.right += dxRight;

	::MoveWindow(hwndControl, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
}

void CGetImageDlg::RecalculateLayout(int cxThumbnail, int cyThumbnail,
	int cxImage, int cyImage)
{
	// Calculate new thumbnail width
	int cxSelThumbnail = cxThumbnail + cThumbnailBorderPixels * 2;
	int dxThumbnails = cxSelThumbnail - (m_rcThumbnailsClient.right - m_rcThumbnailsClient.left);

	// Adjust for new thumbnail width
	m_rcThumbnails.right += dxThumbnails;
	m_rcThumbnailsClient.right += dxThumbnails;

	// Adjust for new thumbnail width
	m_rcPreviewArea.left += dxThumbnails;

	// Calculate minimum and maximum preview deltas: The dialog can't be smaller it's original size
	// or larger than the screen. If necessary, we force the fit by adjusting the size of the 
	// preview area. The actual preview must be smaller or equal to the preview area
	int cxPreviewAreaMin = max(0, m_cxDlgMin - m_rcPreviewArea.left - m_cxPreviewAreaRightMargin);
	int cyPreviewAreaMin = max(0, m_cyDlgMin - m_rcPreviewArea.top  - m_cyPreviewAreaBottomMargin);
	int cxPreviewAreaMax = max(0, ((GetSystemMetrics(SM_CXSCREEN) * 80) / 100) - m_rcPreviewArea.left - m_cxPreviewAreaRightMargin);
	int cyPreviewAreaMax = max(0, ((GetSystemMetrics(SM_CYSCREEN) * 80) / 100) - m_rcPreviewArea.top  - m_cyPreviewAreaBottomMargin);
	int cxPreviewArea    = max(cxPreviewAreaMin, min(cxPreviewAreaMax, cxImage));
	int cyPreviewArea    = max(cyPreviewAreaMin, min(cyPreviewAreaMax, cyImage));
	int dxPreviewArea    = cxPreviewArea - (m_rcPreviewArea.right - m_rcPreviewArea.left);
	int dyPreviewArea    = cyPreviewArea - (m_rcPreviewArea.bottom - m_rcPreviewArea.top);

	if(cxImage > cxPreviewArea || cyImage > cyPreviewArea)
	{
		// Image is bigger than image area - scale image proportionally to fit image area
		if(cxImage - cxPreviewArea > cyImage - cyPreviewArea)
		{
			cyImage = (cxPreviewArea * cyImage) / cxImage; // NewX / OldX = NewY / OldY => NewY = NewX * OldY / OldX
			cxImage = cxPreviewArea;
		}
		else
		{
			cxImage = (cyPreviewArea * cxImage) / cyImage; // NewY / OldY = NewX / OldX => NewX = NewY * OldX / OldY
			cyImage = cyPreviewArea;
		}
	}

	// Adjust for new preview area height
	m_rcThumbnails.bottom += dyPreviewArea;
	m_rcThumbnailsClient.bottom += dyPreviewArea;

	// Adjust for new preview area hight and width
	m_rcPreviewArea.bottom += dyPreviewArea;
	m_rcPreviewArea.right  += dxThumbnails + dxPreviewArea;
	m_rcPreview.top    = m_rcPreviewArea.top + (cyPreviewArea - cyImage) / 2;
	m_rcPreview.left   = m_rcPreviewArea.left + (cxPreviewArea - cxImage) / 2;
	m_rcPreview.bottom = m_rcPreview.top + cyImage;
	m_rcPreview.right  = m_rcPreview.left + cxImage;

	// Resize dialog
	RECT rcDialog;
	GetWindowRect(&rcDialog);
	rcDialog.right += dxThumbnails + dxPreviewArea; // dxDialog
	rcDialog.bottom += dyPreviewArea; // dyDialog
	MoveWindow(&rcDialog, TRUE);

	::MoveWindow(GetDlgItem(IDC_THUMBNAILS), m_rcThumbnails.left,
		m_rcThumbnails.top, m_rcThumbnails.right - m_rcThumbnails.left,
		m_rcThumbnails.bottom - m_rcThumbnails.top, TRUE);


	// Move controls under thumbnails and preview
	for(HWND hwnd = GetDlgItem(IDC_MESSAGE); hwnd; 
		hwnd = ::GetWindow(hwnd, GW_HWNDNEXT))
	{
		RECT rc;
		::GetWindowRect(hwnd, &rc);
		ScreenToClient(&rc);
		rc.top += dyPreviewArea;
		rc.bottom += dyPreviewArea;
		::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, TRUE);
	}

	// Stretch IDC_MESSAGE
	MoveControl(IDC_MESSAGE, 0, dxThumbnails + dxPreviewArea);

	// Move IDC_PROGRESS right
	MoveControl(IDC_PROGRESS, dxThumbnails + dxPreviewArea, dxThumbnails + dxPreviewArea);

	// Stretch IDC_IMAGES_FOLDER
	MoveControl(IDC_IMAGES_FOLDER, 0, dxThumbnails + dxPreviewArea);

	// Stretch IDC_COMPUTER_FOLDER
	MoveControl(IDC_COMPUTER_FOLDER, 0, dxThumbnails + dxPreviewArea);

	// Stretch IDC_URL
	MoveControl(IDC_URL, 0, dxThumbnails + dxPreviewArea);

	// Move IDC_BROWSE_FOLDER right
	MoveControl(IDC_BROWSE_FOLDER, dxThumbnails + dxPreviewArea, dxThumbnails + dxPreviewArea);

	// Move IDC_GO right
	MoveControl(IDC_GO, dxThumbnails + dxPreviewArea, dxThumbnails + dxPreviewArea);

	// Save for later
	m_cxThumbnail = cxThumbnail;
	m_cyThumbnail = cyThumbnail;
	m_cxImage = cxImage;
	m_cyImage = cyImage;

	InvalidateRect(NULL, TRUE);
}

void CGetImageDlg::SetStatus(const TCHAR* szStatus)
{
	SetDlgItemText(IDC_MESSAGE, szStatus);
}

void CGetImageDlg::SetProgress(int cTotalParts, int cPartsCompleted)
{
	HWND hwnd = GetDlgItem(IDC_PROGRESS);
	int cMax = (int)::SendMessage(hwnd, PBM_GETRANGE, FALSE, NULL);
	if(cMax != cTotalParts)
		::SendMessage(hwnd, PBM_SETRANGE, 0, MAKELPARAM(0, cTotalParts));
	::SendMessage(hwnd, PBM_SETPOS, cPartsCompleted, 0);
}

void CGetImageDlg::CreateCamera()
{
	USES_CONVERSION;

	DestroyCamera();

	switch(m_fCameraType)
	{
		case CT_PDC640:
			m_pdcc = new CSerialCamera(OLE2T(m_strDevice), m_nSpeed);
			break;

		default:
			ATLASSERT(0);
		case CT_DIGIMAX101:
		case CT_USB:
			m_pdcc = new CUSBCamera(OLE2T(m_strImagesFolder), TRUE);
			break;

		case CT_COMPUTER_FOLDER:
			m_pdcc = new CUSBCamera(OLE2T(m_strComputerFolder), FALSE);
			break;

		case CT_URL:
		{
			CComBSTR url;
			GetDlgItemText(IDC_URL, url.m_str);
			m_pdcc = new CURLCamera(OLE2T(url));
			break;
		}
	}
}

void CGetImageDlg::DestroyCamera()
{
	delete m_pdcc;
	m_pdcc = NULL;
}

// GetThumbnailsThread --------------------------------------------------------

DWORD WINAPI GetThumbnailsThread(LPVOID pv)
{
	CGetImageDlg* pdlg = (CGetImageDlg*)pv;
	HWND hdlg = pdlg->m_hWnd;
	int cImages = 0;
	const TCHAR szFormat[] = _T("Retrieving thumbnail %d of %d");
	TCHAR szBuffer[sizeof(szFormat) + 50];
	int iImage = 1;
	CDCImage* pdci = NULL;
	USES_CONVERSION;

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Connecting to camera..."));
	pdlg->CreateCamera();
	if(!pdlg->m_pdcc->OpenCamera())
		goto ExitCameraError;
	pdlg->m_strDevice = pdlg->m_pdcc->GetDevice();
	pdlg->m_nSpeed = pdlg->m_pdcc->GetSpeed();
	pdlg->m_strImagesFolder = pdlg->m_pdcc->GetImagesFolder();

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Retrieving number of images..."));
	if(!pdlg->m_pdcc->GetImageCount(cImages))
		goto ExitCameraError;
	if(cImages == 0)
	{
		CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("No images in camera."));
		return FALSE;
	}

	for(iImage = 0; iImage < cImages; iImage++)
	{
		wsprintf(szBuffer, szFormat, iImage, cImages);
		CStatusMessage::SetStatus(hdlg, ST_INFORMATION, szBuffer);

		pdci = new CDCImage(RM_IMAGE, SM_STRETCH, 1, 1);
		if(!pdlg->m_pdcc->GetImage(iImage, TRUE, *pdci))
			goto ExitCameraError;

		::PostMessage(hdlg, WM_DCGOTTHUMBNAIL, iImage + 1 >= cImages, (LPARAM)pdci);

		CProgressMessage::SetProgress(hdlg, cImages, iImage + 1);
	}

// ExitSuccess:
	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Ready"));
	pdlg->DestroyCamera();
	return TRUE;

ExitCameraError:
	{
		USES_CONVERSION;
		CStatusMessage::SetStatus(hdlg, ST_WARNING, 
			OLE2T(pdlg->m_pdcc->GetErrorMessage()));
		::PostMessage(hdlg, WM_DCGOTTHUMBNAIL, TRUE, 0);
		pdlg->DestroyCamera();
		return FALSE;
	}
}

// GetImageThread -------------------------------------------------------------

DWORD WINAPI GetImageThread(LPVOID pv)
{
	CGetImageDlg* pdlg = (CGetImageDlg*)pv;
	HWND hdlg = pdlg->m_hWnd;

	int iImage = pdlg->m_iCurImage;
	void* pvImage = NULL;
	UINT cbImage = 0;
	const TCHAR szFormat[] = _T("Retrieving image %d from camera");
	TCHAR szBuffer[sizeof(szFormat) + 25];
	USES_CONVERSION;

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Connecting to camera..."));
	pdlg->CreateCamera();
	if(!pdlg->m_pdcc->OpenCamera())
		goto ExitCameraError;
	pdlg->m_strDevice = pdlg->m_pdcc->GetDevice();
	pdlg->m_nSpeed = pdlg->m_pdcc->GetSpeed();
	pdlg->m_strImagesFolder = pdlg->m_pdcc->GetImagesFolder();

	wsprintf(szBuffer, szFormat, iImage + 1);
	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, szBuffer);

	pdlg->m_dciPreview.m_fResizeMode = pdlg->m_fResizeMode;
	pdlg->m_dciPreview.m_fStretchMode = pdlg->m_fStretchMode;
	pdlg->m_dciPreview.m_cxDesignWidth = pdlg->m_cxDesignWidth;
	pdlg->m_dciPreview.m_cyDesignHeight = pdlg->m_cyDesignHeight;
	if(!pdlg->m_pdcc->GetImage(iImage, FALSE, pdlg->m_dciPreview))
		goto ExitCameraError;

// ExitSuccess:
	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Ready"));
	::PostMessage(hdlg, WM_DCGOTIMAGE, TRUE, 0);
	pdlg->DestroyCamera();
	return TRUE;

ExitCameraError:
	{
		USES_CONVERSION;
		CStatusMessage::SetStatus(hdlg, ST_WARNING, 
			OLE2T(pdlg->m_pdcc->GetErrorMessage()));
		::PostMessage(hdlg, WM_DCGOTIMAGE, FALSE, 0);
		pdlg->DestroyCamera();
		return FALSE;
	}
}

DWORD WINAPI UploadImageThread(LPVOID pv)
{
	USES_CONVERSION;
	CGetImageDlg* pdlg = (CGetImageDlg*)pv;
	HWND hdlg = pdlg->m_hWnd;

	CHTTPClient hc;
	void* pvImage = NULL;
	UINT cbImageCur = 0;
	UINT cbImageTotal = 0;
	UINT cbImageWrite = 0;
	char* pchBuffer = NULL;
	UINT cbStatus = 0;
	char szBuffer[1024];

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Converting image"));
	if(!pdlg->m_dciPreview.GetImage(pvImage, cbImageTotal))
		goto ExitImageError;

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Contacting server"));
	if(!hc.OpenFileUploadURL(OLE2T(pdlg->m_strUploadImageURL), _T("image.jpg"),
		_T("image/pjpeg"), cbImageTotal))
		goto ExitHTTPError;

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Uploading image"));
	pchBuffer = (char*)pvImage;
	while(cbImageCur < cbImageTotal)
	{
		cbImageWrite = min(1024, cbImageTotal - cbImageCur);
		if(!hc.Write(pchBuffer, cbImageWrite))
			goto ExitHTTPError;
		if(cbImageWrite == 0)
			continue;

		pchBuffer += cbImageWrite;
		cbImageCur += cbImageWrite;

		CProgressMessage::SetProgress(hdlg, cbImageCur, cbImageTotal);
	}

	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, _T("Upload complete - retrieving status"));
	cbStatus = sizeof(szBuffer) - 1;
	if(!hc.Read(szBuffer, cbStatus))
		goto ExitHTTPError;
	szBuffer[cbStatus] = 0;
	if(strncmp(szBuffer, "200", 3) != 0)
	{
		free(pvImage);
		CStatusMessage::SetStatus(hdlg, ST_WARNING, szBuffer);
		::PostMessage(hdlg, WM_DCUPLOADCOMPLETE, FALSE, 0);
		return FALSE;
	}
	CStatusMessage::SetStatus(hdlg, ST_INFORMATION, A2T(szBuffer));

// ExitSuccess:
	free(pvImage);
	::PostMessage(hdlg, WM_DCUPLOADCOMPLETE, TRUE, 0);
	return TRUE;

ExitImageError:
	{
		USES_CONVERSION;
		free(pvImage);
		CStatusMessage::SetStatus(hdlg, ST_WARNING, 
			OLE2T(pdlg->m_dciPreview.GetErrorMessage()));
		::PostMessage(hdlg, WM_DCUPLOADCOMPLETE, FALSE, 0);
		return FALSE;
	}

ExitHTTPError:
	{
		USES_CONVERSION;
		free(pvImage);
		CStatusMessage::SetStatus(hdlg, ST_WARNING, 
			OLE2T(hc.GetErrorMessage()));
		::PostMessage(hdlg, WM_DCUPLOADCOMPLETE, FALSE, 0);
		return FALSE;
	}
}

BOOL GetRegValue(HKEY hkeyParent, const TCHAR* szKey, const TCHAR* szValueName, long& iValue)
{
	DWORD dwValue;
	HKEY hkey = NULL;
	LONG fErrorCode = ERROR_SUCCESS;

	fErrorCode = ::RegOpenKeyEx(hkeyParent, szKey, 0, KEY_QUERY_VALUE, &hkey);
	if(fErrorCode != ERROR_SUCCESS)
		return FALSE;
		
	DWORD cbValue = sizeof(dwValue);
	DWORD fValueType = REG_DWORD;
	fErrorCode = ::RegQueryValueEx(hkey, szValueName, NULL, &fValueType, 
		(BYTE*)&dwValue, &cbValue);
	if(fErrorCode != ERROR_SUCCESS || fValueType != REG_DWORD)
	{
		RegCloseKey(hkey);
		return FALSE;
	}

	iValue = (long)dwValue;

	RegCloseKey(hkey);
	return TRUE;
}

BOOL GetRegValue(HKEY hkeyParent, const TCHAR* szKey, const TCHAR* szValueName, CComBSTR& strValue)
{
	HKEY hkey = NULL;
	LONG fErrorCode = ERROR_SUCCESS;

	fErrorCode = ::RegOpenKeyEx(hkeyParent, szKey, 0, KEY_QUERY_VALUE, &hkey);
	if(fErrorCode != ERROR_SUCCESS)
		return FALSE;
		
	DWORD cbValue = 0;
	DWORD fValueType = REG_SZ;
	fErrorCode = ::RegQueryValueEx(hkey, szValueName, NULL, &fValueType, NULL, &cbValue);
	if(fErrorCode != ERROR_SUCCESS || fValueType != REG_SZ)
	{
		RegCloseKey(hkey);
		return FALSE;
	}

	TCHAR* szBuffer = new TCHAR[cbValue + 1];
	fErrorCode = ::RegQueryValueEx(hkey, szValueName, NULL, NULL, 
			(BYTE*)szBuffer, &cbValue);
	if(fErrorCode != ERROR_SUCCESS)
	{
		delete szBuffer;
		RegCloseKey(hkey);
		return FALSE;
	}
	strValue = szBuffer;
	delete szBuffer;

	RegCloseKey(hkey);
	return TRUE;
}

BOOL AddRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, long iValue)
{ 
	DWORD dwValue = (DWORD)iValue;
	HKEY hkeySubkey = NULL;
	LONG fErrorCode = ERROR_SUCCESS;
	DWORD dwDisposition = 0;

	fErrorCode = RegCreateKeyEx(hkey, szSubkey, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkeySubkey, &dwDisposition);
	if(fErrorCode != ERROR_SUCCESS)
		return FALSE;

	fErrorCode = RegSetValueEx(hkeySubkey, szValueName, 0, 
		REG_DWORD, (BYTE*)&dwValue, sizeof(dwValue));
	if(fErrorCode != ERROR_SUCCESS)
	{
		RegCloseKey(hkeySubkey);
		return FALSE;
	}

	RegCloseKey(hkeySubkey);
	return TRUE;
}

BOOL AddRegValue(HKEY hkey, const TCHAR* szSubkey, const TCHAR* szValueName, const TCHAR* szValue)
{ 
	HKEY hkeySubkey = NULL;
	LONG fErrorCode = ERROR_SUCCESS;
	DWORD dwDisposition = 0;

	fErrorCode = RegCreateKeyEx(hkey, szSubkey, 0, NULL, 
		REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkeySubkey, &dwDisposition);
	if(fErrorCode != ERROR_SUCCESS)
		return FALSE;

	const TCHAR szEmpty[] = "";
	if(!szValue) szValue = szEmpty;

	fErrorCode = RegSetValueEx(hkeySubkey, szValueName, 0, 
		REG_SZ, (BYTE*)szValue, (_tcslen(szValue) + 1) * sizeof(TCHAR));
	if(fErrorCode != ERROR_SUCCESS)
	{
		RegCloseKey(hkeySubkey);
		return FALSE;
	}

	return TRUE;
}

BOOL RemoveRegKeys(HKEY hkey, const TCHAR* szSubkey)
{
	HKEY hkeySubkey = NULL;
	LONG fErrorCode = RegOpenKeyEx(hkey, szSubkey, 0, 
		KEY_ENUMERATE_SUB_KEYS | DELETE, &hkeySubkey);

	if(fErrorCode != ERROR_SUCCESS)
		return FALSE;

	while(fErrorCode == ERROR_SUCCESS)
	{
		TCHAR szSubkey[MAX_PATH];
		DWORD cbSubkey = sizeof(szSubkey);
		LONG fErrorCode = RegEnumKeyEx(hkeySubkey, 0, szSubkey, &cbSubkey, 0, 0, 0, 0);

		if(fErrorCode == ERROR_NO_MORE_ITEMS)
			break;

		if(fErrorCode != ERROR_SUCCESS)
		{
			RegCloseKey(hkeySubkey);
			return FALSE;
		}

		if(!RemoveRegKeys(hkeySubkey, szSubkey))
		{
			RegCloseKey(hkeySubkey);
			return FALSE;
		}
	}

	RegCloseKey(hkeySubkey);
	fErrorCode = RegDeleteKey(hkey, szSubkey);

	return fErrorCode == ERROR_SUCCESS;
}
