// GetImageDlg.h : Declaration of the CGetImageDlg

#ifndef __GETIMAGEDLG_H_
#define __GETIMAGEDLG_H_

#include "GetImageDlg.hrc"
#include "DCImage.h"
#include "DCCamera.h"
#include <atlhost.h>

extern UINT WM_DCGETTHUMBNAILS;
extern UINT WM_DCGOTTHUMBNAIL;
extern UINT WM_DCGOTIMAGE;
extern UINT WM_DCUPLOADCOMPLETE;

/////////////////////////////////////////////////////////////////////////////
// CGetImageDlg
class CGetImageDlg
	: public CAxDialogImpl<CGetImageDlg>
{
public:
	CComBSTR m_strUploadImageURL;
	CComBSTR m_strDevice;
	long m_nSpeed;
	CAMERA_TYPE m_fCameraType;
	CComBSTR m_strImagesFolder;
	CComBSTR m_strComputerFolder;
	CComBSTR m_strURL;
	RESIZE_MODE m_fResizeMode;
	STRETCH_MODE m_fStretchMode;
	long m_cxDesignWidth;
	long m_cyDesignHeight;
	long m_cxThumbnail;
	long m_cyThumbnail;
	long m_cxImage;
	long m_cyImage;

public:
	CGetImageDlg();
	~CGetImageDlg();

	enum { IDD = IDD_GETIMAGEDLG };

BEGIN_MSG_MAP(CGetImageDlg)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_ID_HANDLER(IDOK, OnOK)
	COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_DCGETTHUMBNAILS, OnGetThumbnails)
	MESSAGE_HANDLER(WM_DCGOTTHUMBNAIL, OnGotThumbnail)
	MESSAGE_HANDLER(WM_DCGOTIMAGE, OnGotImage)
	MESSAGE_HANDLER(WM_DCSTATUS, OnSetStatus)
	MESSAGE_HANDLER(WM_DCPROGRESS, OnSetProgress)
	MESSAGE_HANDLER(WM_DCUPLOADCOMPLETE, OnUploadComplete)
	MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	MESSAGE_HANDLER(WM_MEASUREITEM, OnMeasureItem)
	MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
	COMMAND_HANDLER(IDC_PREVIEW_IMAGE, BN_CLICKED, OnClickedPreviewImage)
	COMMAND_HANDLER(IDC_SETTINGS, BN_CLICKED, OnClickedSettings)
	COMMAND_HANDLER(IDC_CAMERA_TYPE, LBN_SELCHANGE, OnSelChangeCameraType)
	COMMAND_HANDLER(IDC_BROWSE_FOLDER, BN_CLICKED, OnClickedBrowseFolder)
	COMMAND_HANDLER(IDC_GO, BN_CLICKED, OnClickedGo)

END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnGetThumbnails(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGotThumbnail(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGotImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetStatus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSetProgress(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUploadComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedPreviewImage(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedSettings(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelChangeCameraType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedBrowseFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnClickedGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

protected:
	int m_cxDlgMin;
	int m_cyDlgMin;
	int m_cxPreviewAreaRightMargin;
	int m_cyPreviewAreaBottomMargin;
	RECT m_rcPreview;
	RECT m_rcPreviewArea;
	RECT m_rcThumbnails;
	RECT m_rcThumbnailsClient;
	CDCImage m_dciPreview;
	CDCCamera* m_pdcc;
	int m_iCurImage;

protected:
	void DeleteThumbnails();
	UINT GetCountThumbnails();
	void SettingsToRegistry();
	void RegistryToSettings();
	void SettingsToScreen();
	void ScreenToSettings();
	void EnableControls(BOOL fEnable);
	void MoveControl(int idcControl, int dxLeft, int dxRight);
	void RecalculateLayout(int cxThumbnail, int cyThumbnail, int cxImage, int cyImage);
	void SetStatus(const TCHAR* szStatus);
	void SetProgress(int cTotalParts, int cPartsCompleted);
	void CreateCamera();
	void DestroyCamera();

friend DWORD WINAPI GetThumbnailsThread(LPVOID pv);
friend DWORD WINAPI GetImageThread(LPVOID pv);
friend DWORD WINAPI UploadImageThread(LPVOID pv);
};

#endif //__GETIMAGEDLG_H_
