// DCCamera.h: interface for the DCCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCCAMERA_H__11843C8A_A597_11D3_A61E_0020A631A360__INCLUDED_)
#define AFX_DCCAMERA_H__11843C8A_A597_11D3_A61E_0020A631A360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum CAMERA_TYPE { CT_NONE = -1, CT_PDC640 = 1, CT_DIGIMAX101 = 2, CT_USB = 3, CT_COMPUTER_FOLDER = 4, CT_URL = 5  };

class CDCCamera
{
public:
	CDCCamera();
	virtual ~CDCCamera();

	const CComBSTR& GetErrorMessage();
	const CComBSTR& GetDevice();
	long GetSpeed();
	const CComBSTR& GetImagesFolder();

	virtual BOOL IsOpen() = 0;
	virtual BOOL OpenCamera() = 0;
	virtual void CloseCamera() = 0;
	virtual BOOL GetImageCount(int& cImages) = 0;
	virtual BOOL GetImage(int iImage, BOOL fThumbnail, CDCImage& dci) = 0;

// Implementation - required to be public for ppclib callback functions
public:
	CComBSTR m_strError;
	HWND m_hwndStatus;
	UINT m_cbImage;
	void* m_pvJPEGBytes;
	UINT m_cbJPEGBytes; 

protected:
	CComBSTR m_strDevice;
	long m_nSpeed;
	CComBSTR m_strImagesFolder;
};

class CSerialCamera
	: public CDCCamera
{
public:
	CSerialCamera(const char* szDevice, long nSpeed);
	virtual ~CSerialCamera();

	virtual BOOL IsOpen();
	virtual BOOL OpenCamera();
	virtual void CloseCamera();
	virtual BOOL GetImageCount(int& cImages);
	virtual BOOL GetImage(int iImage, BOOL fThumbnail, CDCImage& dci);

	BOOL GetImage(int iImage, BOOL fThumbnail, 
	              void*& pvJPEGBits, UINT& cbJPEGBits);
};

class CUSBCamera  
	: public CDCCamera
{
public:
	CUSBCamera(const char* szImagesFolder = NULL, BOOL fScanSubFolders = TRUE);
	virtual ~CUSBCamera();

	virtual BOOL IsOpen();
	virtual BOOL OpenCamera();
	virtual void CloseCamera();
	virtual BOOL GetImageCount(int& cImages);
	virtual BOOL GetImage(int iImage, BOOL fThumbnail, CDCImage& dci);

protected:
	BOOL m_fScanSubFolders;
	std::vector<CComBSTR> m_rgstrImageFiles;

protected:
	void AddImagesFolder(const CComBSTR& strFolder);
};

class CURLCamera
	: public CDCCamera
{
public:
	CURLCamera(const char* szImageURL);
	virtual ~CURLCamera();

	virtual BOOL IsOpen();
	virtual BOOL OpenCamera();
	virtual void CloseCamera();
	virtual BOOL GetImageCount(int& cImages);
	virtual BOOL GetImage(int iImage, BOOL fThumbnail, CDCImage& dci);

protected:
	void* m_pvImage;
	UINT m_cbImage;
	CComBSTR m_strImageURL;

protected:
	void AddImagesFolder(const CComBSTR& strFolder);
};


CComBSTR ExsMakePath(const TCHAR* szLeft, const TCHAR* szRight);
BOOL ExsDirectoryExists(const TCHAR* szDirPath);

#endif // !defined(AFX_DCCAMERA_H__11843C8A_A597_11D3_A61E_0020A631A360__INCLUDED_)
