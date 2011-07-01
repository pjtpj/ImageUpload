// DCSettings.cpp : Implementation of CDCSettings
#include "stdafx.h"
#include "DCSettings.h"

/////////////////////////////////////////////////////////////////////////////
// CDCSettings

const TCHAR g_szAutomatic[] = _T("Automatic");


static CComBSTR GetModuleFilePath(HMODULE hModule = NULL);
static CComBSTR GetFileVersion(const TCHAR* szFilePath);

CDCSettings::CDCSettings()
{
	m_strDevice = g_szAutomatic;
	m_nSpeed = 0;
	m_fCameraType = CT_PDC640;
	m_fResizeMode = RM_DESIGN;
	m_fStretchMode = SM_STRETCH;
	m_cxDesignWidth = 320;
	m_cyDesignHeight = 240;
}

CDCSettings::~CDCSettings()
{
}

void CDCSettings::UpdateControls()
{
	m_fCameraType = (CAMERA_TYPE)(SendDlgItemMessage(IDC_CAMERA_TYPE, CB_GETCURSEL, 0, 0) + 1);
	BOOL fSerial = m_fCameraType == CT_PDC640;

	::EnableWindow(GetDlgItem(IDC_DEVICE), fSerial);
	::EnableWindow(GetDlgItem(IDC_SPEED),  fSerial);
}

const TCHAR* g_rgszCameraTypes[] = 
{ "Polaroid PDC 640", "Vivitar Vivicam 3315", "Generic USB Camera", "Computer Folder", "URL", NULL };
const TCHAR* g_rgszDevices[] = 
{ "Automatic", "COM1:", "COM2:", "COM3:", "COM4:", NULL };
const TCHAR* g_rgszSpeeds[] = 
{ "Automatic", "9600", "19200", "38400", "57600", "115200", NULL };
const TCHAR* g_rgszResizeModes[] = 
{ "Use size specified by designer", "Use picture size", NULL };
const TCHAR* g_rgszStretchModes[] = 
{ "Stretch", "Resample", "Crop", NULL };

static void AddStrings(HWND hwnd, UINT idControl, const TCHAR* rgszStrings[])
{
	HWND hwndCtl = ::GetDlgItem(hwnd, idControl);
	for(int i = 0; rgszStrings[i]; i++)
	{
		::SendMessage(hwndCtl, CB_ADDSTRING, 0, (LPARAM)rgszStrings[i]);
	}
}

LRESULT CDCSettings::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;

	AddStrings(m_hWnd, IDC_CAMERA_TYPE, g_rgszCameraTypes);
	AddStrings(m_hWnd, IDC_DEVICE, g_rgszDevices);
	AddStrings(m_hWnd, IDC_SPEED, g_rgszSpeeds);
	AddStrings(m_hWnd, IDC_RESIZE_MODE, g_rgszResizeModes);
	AddStrings(m_hWnd, IDC_STRETCH_MODE, g_rgszStretchModes);

	SendDlgItemMessage(IDC_CAMERA_TYPE, CB_SETCURSEL, 
		(WPARAM)m_fCameraType - 1, 0);

	CComBSTR strDevice = m_strDevice;
	if(!strDevice.Length())
		strDevice = g_szAutomatic;
	SetDlgItemText(IDC_DEVICE, OLE2T(strDevice));
	
	if(m_nSpeed == 0)
		SetDlgItemText(IDC_SPEED, g_szAutomatic);
	else
		SetDlgItemInt(IDC_SPEED, m_nSpeed);

	SendDlgItemMessage(IDC_RESIZE_MODE, CB_SETCURSEL, 
		(WPARAM)m_fResizeMode - 1, 0);
	SendDlgItemMessage(IDC_STRETCH_MODE, CB_SETCURSEL, 
		(WPARAM)m_fStretchMode - 1, 0);

	SetDlgItemInt(IDC_DESIGN_WIDTH, m_cxDesignWidth);
	SetDlgItemInt(IDC_DESIGN_HEIGHT, m_cyDesignHeight);

	TCHAR szBuffer[1024];
	// CComBSTR strModule  = GetModuleFilePath();
	// CComBSTR strVersion = GetFileVersion(OLE2CT(strModule));
	CComBSTR strVersion = VER_FILEVERSION_STR;
	wsprintf(szBuffer, "Image Upload Version %s", OLE2CT(strVersion));
	SetDlgItemText(IDC_VERSION, szBuffer);

	UpdateControls();

	return 1;  // Let the system set the focus
}

LRESULT CDCSettings::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	USES_CONVERSION;

	m_fCameraType = (CAMERA_TYPE)(SendDlgItemMessage(IDC_CAMERA_TYPE, CB_GETCURSEL, 0, 0) + 1);

	GetDlgItemText(IDC_DEVICE, m_strDevice.m_str);
	if(lstrcmpi(OLE2T(m_strDevice), g_szAutomatic) == 0)
		m_strDevice.Empty();

	CComBSTR strSpeed;
	GetDlgItemText(IDC_SPEED, strSpeed.m_str);
	if(lstrcmpi(OLE2T(strSpeed), g_szAutomatic) == 0)
	{
		m_nSpeed = 0;
	}
	else
	{
		m_nSpeed = GetDlgItemInt(IDC_SPEED);
		if(m_nSpeed <= 0)
		{
			MessageBox(_T("Please choose a speed greater than 0"));
			return 0;
		}
	}

	m_fResizeMode = (RESIZE_MODE)(SendDlgItemMessage(IDC_RESIZE_MODE, CB_GETCURSEL, 0, 0) + 1);
	m_fStretchMode = (STRETCH_MODE)(SendDlgItemMessage(IDC_STRETCH_MODE, CB_GETCURSEL, 0, 0) + 1);

	m_cxDesignWidth = GetDlgItemInt(IDC_DESIGN_WIDTH);
	if(m_cxDesignWidth < 1 || m_cxDesignWidth > 5000)
	{
		MessageBox(_T("Please choose a Design Width between 1 and 5000"));
		return 0;
	}

	m_cyDesignHeight = GetDlgItemInt(IDC_DESIGN_HEIGHT);
	if(m_cyDesignHeight < 1 || m_cyDesignHeight > 5000)
	{
		MessageBox(_T("Please choose a Design Height between 1 and 5000"));
		return 0;
	}

	EndDialog(wID);
	return 0;
}

LRESULT CDCSettings::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(wID);
	return 0;
}

LRESULT CDCSettings::OnSelchangeCameraType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	UpdateControls();
	bHandled = TRUE;
	return 0;
}

// ----------------------------------------------------------------------------

static CComBSTR GetModuleFilePath(HMODULE hModule)
{
	TCHAR szFilePath[MAX_PATH];
	::GetModuleFileName(hModule, szFilePath, sizeof(szFilePath));
	CComBSTR strFilePath = szFilePath;
	return strFilePath;
}

static CComBSTR GetFileVersion(const TCHAR* cszFilePath)
{
	CComBSTR strVersion;

	TCHAR szFilePath[MAX_PATH]; _tcscpy(szFilePath, cszFilePath);
	DWORD hVersionInfo;
	DWORD cbVersionInfo = GetFileVersionInfoSize(szFilePath, &hVersionInfo);
	if(cbVersionInfo > 0)
	{
		char *rgchVersionInfo = new char[cbVersionInfo];
		if(GetFileVersionInfo(szFilePath, hVersionInfo, cbVersionInfo, rgchVersionInfo))
		{
			UINT cbFixedFileInfo;
			VS_FIXEDFILEINFO* pffiFixedFileInfo;

			if(VerQueryValue(rgchVersionInfo, "\\", (void**)&pffiFixedFileInfo, &cbFixedFileInfo))
			{
				int ver_major = HIWORD( pffiFixedFileInfo->dwFileVersionMS ) ;
				int ver_minor = LOWORD( pffiFixedFileInfo->dwFileVersionMS ) ;
				int ver_build = HIWORD( pffiFixedFileInfo->dwFileVersionLS ) ;
				int ver_year  = LOWORD( pffiFixedFileInfo->dwFileVersionLS ) ;

				char szVersion[12 * 4 + 3 + 1];
				wsprintf(szVersion, "%d.%d.%d.%d", ver_major, ver_minor, ver_build, ver_year);

				strVersion = szVersion;
			}
		}
		delete rgchVersionInfo;
	}

	return strVersion ;
}

