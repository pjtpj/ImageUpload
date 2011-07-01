// DCImage.h: interface for the CDCImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCIMAGE_H__61ACF8C4_A1FD_11D3_A61E_0020A631A360__INCLUDED_)
#define AFX_DCIMAGE_H__61ACF8C4_A1FD_11D3_A61E_0020A631A360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum IMAGE_TYPE { IT_JPG = 1, IT_GIF = 2 };
enum RESIZE_MODE { RM_DESIGN = 1, RM_IMAGE = 2 };
enum STRETCH_MODE { SM_STRETCH = 1, SM_RESAMPLE = 2, SM_CROP = 3 };
enum STATUS_TYPE { ST_DEBUG = 1, ST_INFORMATION = 2, ST_WARNING = 3, ST_ERROR = 4 };

extern UINT WM_DCSTATUS;
struct CStatusMessage
{
	STATUS_TYPE m_fType;
	CComBSTR m_bstrStatus;
	CStatusMessage(STATUS_TYPE fType, const TCHAR* szStatus)
		: m_fType(fType), m_bstrStatus(szStatus) {}
	static void SetStatus(HWND hwnd, STATUS_TYPE fType, const TCHAR* szStatus)
	{
		CStatusMessage* psm = new CStatusMessage(fType, szStatus);
		::PostMessage(hwnd, WM_DCSTATUS, (WPARAM)psm, 0);
	}
};

extern UINT WM_DCPROGRESS;
struct CProgressMessage
{
	int m_cTotalParts;
	int m_cPartsCompleted;
	CProgressMessage(int cTotalParts, int cPartsCompleted)
		: m_cTotalParts(cTotalParts), m_cPartsCompleted(cPartsCompleted) {}
	static void SetProgress(HWND hwnd, int cTotalParts, int cPartsCompleted)
	{
		CProgressMessage* ppm = new CProgressMessage(cTotalParts, cPartsCompleted);
		::PostMessage(hwnd, WM_DCPROGRESS, (WPARAM)ppm, 0);
	}
};

class CDCImage  
{
public:
	CDCImage(RESIZE_MODE fResizeMode = RM_DESIGN, STRETCH_MODE fStretchMode = SM_STRETCH,
	         int cxDesignWidth = 320, int cyDesignHeight = 240);
	virtual ~CDCImage();

public:
	// These properties apply to the next call to SetImage
	HWND m_hwndStatus;
	IMAGE_TYPE m_fImageType;
	RESIZE_MODE m_fResizeMode;
	STRETCH_MODE m_fStretchMode;
	int m_cxDesignWidth;
	int m_cyDesignHeight;

public:
	BOOL IsEmpty();
	BOOL GetImage(void*& pvJPEGBits, UINT& cbJPEGBits);
	int GetImageHeight();
	int GetImageWidth();
	const CComBSTR& GetErrorMessage();

	BOOL SetImage(void* pvJPEGBits, UINT cbJPEGBits);
	BOOL SetImage(const TCHAR* szFilePath);
	BOOL SetThumbnailImage(const TCHAR* szFilePath);
	void DeleteImage();
	BOOL DrawImage(HDC hdc, const RECT& rcDest);
	BOOL ResizeImage(int w, int h);
	BOOL ResampleImage(int w, int h);
	BOOL CropImage(int w, int h);

protected:
	CComBSTR m_strError;
	CxImage m_image;

protected:
	BOOL SetImage(void* pjpeg_source_mgr, HWND hwndStatus,
		RESIZE_MODE fResizeMode, STRETCH_MODE fStretchMode,
		int cxDesignWidth, int cyDesignHeight);
};

#endif // !defined(AFX_DCIMAGE_H__61ACF8C4_A1FD_11D3_A61E_0020A631A360__INCLUDED_)
