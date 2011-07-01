// DCSettings.h : Declaration of the CDCSettings

#ifndef __DCSETTINGS_H_
#define __DCSETTINGS_H_

#include "GetImageDlg.hrc"
#include <atlhost.h>
#include "DCImage.h"
#include "DCCamera.h"

/////////////////////////////////////////////////////////////////////////////
// CDCSettings
class CDCSettings : 
	public CAxDialogImpl<CDCSettings>
{
public:
	CComBSTR m_strDevice;
	long m_nSpeed;
	CAMERA_TYPE m_fCameraType;
	RESIZE_MODE m_fResizeMode;
	STRETCH_MODE m_fStretchMode;
	long m_cxDesignWidth;
	long m_cyDesignHeight;

public:
	CDCSettings();
	~CDCSettings();

	enum { IDD = IDD_DCSETTINGS };

protected:
	void UpdateControls();

public:

BEGIN_MSG_MAP(CDCSettings)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	COMMAND_HANDLER(IDC_CAMERA_TYPE, CBN_SELCHANGE, OnSelchangeCameraType)
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelchangeCameraType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
};

#endif //__DCSETTINGS_H_
