// DCCamera.cpp: implementation of the CDCCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DCImage.h"
#include "DCCamera.h"
#include "HTTPClient.h"

extern "C" 
{
#include "../ppc/eph_io.h"
#include "../ppc/a12scan.h"
}

// CDCCamera ------------------------------------------------------------------

CDCCamera::CDCCamera()
{
	m_hwndStatus = NULL;
	m_cbImage = 0;
	m_pvJPEGBytes = NULL;
	m_cbJPEGBytes = 0;

	m_nSpeed = 0;
}

CDCCamera::~CDCCamera()
{
}

const CComBSTR& CDCCamera::GetErrorMessage()
{
	return m_strError;
}

const CComBSTR& CDCCamera::GetDevice()
{
	return m_strDevice;
}

long CDCCamera::GetSpeed()
{
	return m_nSpeed;
}

const CComBSTR& CDCCamera::GetImagesFolder()
{
	return m_strImagesFolder;
}

void CDCCamera::CloseCamera()
{
}

// CSerialCamera --------------------------------------------------------------

static CDCCamera* g_pdcc;
static eph_iob* g_piob;

extern "C" void DCReportError(int errcode, char *errstr) 
{
	if(errcode != ERR_TIMEOUT)
		g_pdcc->m_strError = errstr;
	else
		ATLTRACE(_T("CDCCamera - WARNING: %s\n"), errstr);
}

extern "C" void DCReportCount(off_t count) 
{
	if(g_pdcc->m_cbImage && g_pdcc->m_hwndStatus)
	{
		::PostMessage(g_pdcc->m_hwndStatus, WM_DCPROGRESS, 
			(WPARAM)new CProgressMessage(g_pdcc->m_cbImage, count), 0);
	}
}

extern "C" int DCStoreImageBytes(char *data, size_t len) 
{
	BYTE* pvBytes = (BYTE*)realloc(g_pdcc->m_pvJPEGBytes, 
		g_pdcc->m_cbJPEGBytes + len);
	if(!pvBytes)
	{
		g_pdcc->m_strError = _T("Cannot realloc memory");
		return -1;
	}

	memcpy(pvBytes + g_pdcc->m_cbJPEGBytes, data, len);

	g_pdcc->m_pvJPEGBytes = pvBytes;
	g_pdcc->m_cbJPEGBytes += len;

	return 0;
}

CSerialCamera::CSerialCamera(const char* szDevice, long nSpeed)
{
	m_strDevice = szDevice;
	m_nSpeed = nSpeed;
}

CSerialCamera::~CSerialCamera()
{
	CloseCamera();
}

BOOL CSerialCamera::IsOpen()
{
	return g_piob ? TRUE : FALSE;
}

BOOL CSerialCamera::OpenCamera()
{
	USES_CONVERSION;

#if 0
	int debug = 1;
#else
	int debug = 0;
#endif

	if(g_pdcc)
	{
		m_strError = _T("Camera object is already in use");
		return FALSE;
	}
	g_pdcc = this;
	g_piob = eph_new(DCReportError, NULL, DCReportCount, 
		NULL, debug);
	if(!g_piob)
	{
		m_strError = _T("Cannot create camera object");
		g_pdcc = NULL;
		return FALSE;
	}

	BOOL fAutoDevice = m_strDevice.Length() <= 0 ? TRUE : FALSE;
	char szAutoDevice[20];
	const char* device = OLE2CA(m_strDevice);

	BOOL fAutoSpeed = m_nSpeed <= 0 ? TRUE : FALSE;
	long int rgiAutoSpeeds[] = { 115200, 57600, 38400, 19200, 9600 };
	long speed = m_nSpeed;

	int fError = -1;

	for(int iComPort = 1; iComPort <= 4; iComPort++)
	{
		if(fAutoDevice)
		{
			wsprintf(szAutoDevice, "COM%d:", iComPort);
			device = szAutoDevice;
		}

		for(int iSpeed = 0; iSpeed < sizeof(rgiAutoSpeeds)/sizeof(rgiAutoSpeeds[0]); iSpeed++)
		{
			if(fAutoSpeed)
				speed = rgiAutoSpeeds[iSpeed];

			// Open camera
			fError = eph_open(g_piob, (char*)device, speed);
			switch(fError)
			{
				case -1:
					goto ExitDeviceLoop;
				case -2:
				case -3:
				case -4:
					goto TryNextDevice;
				case -5:
					goto TryNextSpeed;
					break;
			}
			// Probe camera
			long ret;
			if(eph_getint(g_piob, 1, &ret))
			{
				eph_close(g_piob, 0);
				fError = -1;
				m_strError = _T("Cannot probe camera");
			}
			else
				goto ExitDeviceLoop;

TryNextSpeed:
			if(!fAutoSpeed)
				goto TryNextDevice;
		}
TryNextDevice:
		if(!fAutoDevice)
			goto ExitDeviceLoop;
	}

ExitDeviceLoop:

	if(fError < 0)
	{
		m_strError = _T("Cannot open communications port or connect to camera");
		CloseCamera();
		g_pdcc = NULL;
		return FALSE;
	}

	m_strDevice = device;
	m_nSpeed = speed;

	return TRUE;	
}

void CSerialCamera::CloseCamera()
{
	if(g_piob)
	{
		eph_close(g_piob, 0);
		eph_free(g_piob);
		g_piob = NULL;
	}
	g_pdcc = NULL;
}

BOOL CSerialCamera::GetImageCount(int& cImages)
{
	m_strError = _T("Unknown error");

	long cImages2;
	if(eph_getint(g_piob, 0x0a, &cImages2))
		return FALSE;
	ATLASSERT(cImages2 < 5000);
	cImages = (int)cImages2;

	return TRUE;
}

BOOL CSerialCamera::GetImage(int iImage, BOOL fThumbnail, CDCImage& dci)
{
	m_hwndStatus = dci.m_hwndStatus;

	void* pvJPEGBits = NULL;
	UINT cbJPEGBits = 0;

	if(!GetImage(iImage, fThumbnail, pvJPEGBits, cbJPEGBits))
		return FALSE;

	if(!dci.SetImage(pvJPEGBits, cbJPEGBits))
	{
		m_strError = dci.GetErrorMessage();
		return FALSE;
	}

	return TRUE;
}

BOOL CSerialCamera::GetImage(int iImage, BOOL fThumbnail, 
                             void*& pvJPEGBits, UINT& cbJPEGBits)
{
	char *buffer,*buf8i;
	off_t bufsize,buf8isize;
	long length,ilength,tlength;
//long alength;
	off_t got;
	time_t pictime;
//	unsigned char *res;
//	off_t ressize;
//	char *p;
//	struct stat st;
	int dreg=0;
	int needfillbuffer = 0;

	m_strError = _T("Unknown camera error");

	buf8isize = 32;
	if((buf8i = (char*)malloc(buf8isize)) == NULL) 
	{
		m_strError = _T("Cannot allocate buf8i memory");
		return FALSE;
	}

	// Set frame number
	if(eph_setint(g_piob, 4, iImage + 1)) 
		return FALSE;
	ATLTRACE(_T("GetImage set image to %d\n"), iImage);

	// Get length of current thumbnail
	if(eph_getint(g_piob, 13, &tlength)) 
		return FALSE;
	ATLTRACE(_T("GetImage thumbnail size is %d\n"), tlength);
	
	m_cbImage = length = tlength;
	if(!fThumbnail) 
	{
		// Get length of current frame
		if(eph_getint(g_piob, 12, &ilength)) 
			return FALSE;
		ATLTRACE(_T("GetImage frame size is %d\n"), ilength);
		length += ilength;
		m_cbImage = ilength;
	}

	pictime = 0;
	buf8isize = 32;

	// Get picture summary data (32 bytes)
	if(eph_getvar(g_piob, 47, &buf8i, &buf8isize) == 0) 
	{
		pictime = (long)(buf8i[20]&0xff)+((long)(buf8i[21]&0xff)<<8)+
			((long)(buf8i[22]&0xff)<<16)+((long)(buf8i[23]&0xff)<<24);
	}
	free(buf8i);

	ATLTRACE(_T("GetImage 1st pictime is %d\n"), pictime);

/*
	What happens next

	MM  PT  DT | 1   2   3  4  5   6
	-----------+------------++-------
	N       T  | SA  RT  -- || --  WB
	o   !0  I  | BA  RI  -- || --  WB
	r       A  | BA  RA  -- || --  WB
	m  --------+------------++-------
	a       T  | SA  RT  AN || --  WB
	l    0  I  | BA  RI  AN || --  WB
	        A  | SA  RT  AN || RA  WB
	-----------+------------++-------
	L       T  | SA  RT  -- || --  WB
	o   !0  I  | --  --  -- || RW  --
	w       A  | --  --  -- || RW  --
	   --------+------------++-------
	N       T  | SA  RT  AN || --  WB
	/    0  I  | SA  RT  AN || RW  --
	A       A  | SA  RT  AN || RW  --
	-----------+------------++-------
*/

	/*=== Stage 1: allocate buffer ===*/

	bufsize = 0;
	buffer = NULL;

	if(fThumbnail)
		bufsize = ((tlength-1)/2048+2)*2048;
	else
		bufsize = ((length-1)/2048+2)*2048;

	/*				 ^ we should have had 1 here if
		we knew strict file size.  Although, we do not.  All we
		know is the size of image data chunk, and the size of
		thumbnail data chunk.  For now, we add extra 2048 in
		hope that extra JPEG data would not be more than that.
		If it is, the buffer will be automatically realloc()'ed
		anyway.
	*/
	buffer = (char*)malloc(bufsize);
	if(buffer == NULL)
	{
		m_strError = _T("Cannot allocate buffer memory");
		return FALSE;
	}

	/*=== Stage 2: read data into the buffer ===*/

	got = bufsize;
	if(fThumbnail)
		dreg=15; // Get thumbnail data
	else
		dreg=14; // Get frame data
	
	if(eph_getvar(g_piob, dreg, &buffer, &got)) 
		return FALSE;
	ATLTRACE(_T("GetImage got %d bytes of data\n"), got);

	/*=== Stage 3: analyze data in the buffer ===*/

#if 0
	if(pictime == 0) 
	{
		res = buffer;
		ressize = bufsize;
		a12scan(&res, &ressize);
		p=a12search("TimeDate", res, ressize);
		pictime= (time_t)-1;
		if(p) 
		{
			if(sscanf(p,"%lu",&pictime) != 1)
				pictime = (time_t)-1;
		}
		if (debug) fprintf(stderr,"got timestamp from file: %lu\n",
					pictime);
	}
	if (!quiet) {
		printf("taken %s\n",ctimetz(&pictime,usetimezone));
	}
#endif

	/*=== Stage 4: setup output file ===*/


	/*=== Stage 5: read data into the buffer or right out to file ===*/

#if 0
	if(needfillbuffer) 
	{
		/* we still need to read the data itself */
		if(eph_setint(iob, 4, frame)) 
			return -1;
		/* seems that some Agfa model needs setting reg4 again */
		dreg = 14; /* cannot be thumbnail here */
		got = bufsize;
		if(eph_getvar(iob, dreg, buffer ? &buffer : NULL, &got)) 
		{
			m_strError = _T("Error reading image from camera");
			free(buffer);
			return FALSE;
		}
	}
#endif

	/*=== Stage 6: write out the buffer contents */

	pvJPEGBits = buffer;
	cbJPEGBits = got;

	m_pvJPEGBytes = NULL;
	m_cbJPEGBytes = 0;
	m_cbImage = 0;

	return TRUE;
}

// Various Path Manipulation Functions ----------------------------------------

const TCHAR g_chExsPathSeparator = _T('\\');
const TCHAR g_szExsPathSeparator[] = _T("\\"); 
const TCHAR g_szExsAllFiles[] = _T("*.*");
const TCHAR g_szExsCurrentDirectory[] = _T(".");
const TCHAR g_szExsParentDirectory[] = _T("..");
const TCHAR g_szCameraFolder[] = _T("DCIM");

CComBSTR ExsMakePath(const TCHAR* szLeft, const TCHAR* szRight)
{
	USES_CONVERSION;

	CComBSTR strPath;
	
	if(szLeft[0] == 0)
		strPath = szRight;
	else if(szRight[0] == 0)
		strPath = szLeft;
	else if(szLeft[_tcslen(szLeft) - 1] == g_chExsPathSeparator && szRight[0] != g_chExsPathSeparator)
		strPath = CComBSTR(szLeft) += szRight;
	else if(szLeft[_tcslen(szLeft) - 1] != g_chExsPathSeparator && szRight[0] == g_chExsPathSeparator)
		strPath = CComBSTR(szLeft) += szRight;
	else if(szLeft[_tcslen(szLeft) - 1] == g_chExsPathSeparator && szRight[0] == g_chExsPathSeparator)
		strPath = CComBSTR(szLeft) += (szRight + 1);
	else
		strPath = CComBSTR(szLeft) += CComBSTR(g_szExsPathSeparator) += szRight;

	return strPath;
}

BOOL ExsDirectoryExists(const TCHAR* szDirPath)
{
	DWORD dwAttributes = ::GetFileAttributes(szDirPath);

	if(dwAttributes != 0xFFFFFFFF && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;

	return FALSE;
}

// CUSBCamera -----------------------------------------------------------------

CUSBCamera::CUSBCamera(const char* szImagesFolder /* = NULL */, BOOL fScanSubFolders /* = TRUE */)
{
	m_strImagesFolder = szImagesFolder;
	m_fScanSubFolders = fScanSubFolders;
}

CUSBCamera::~CUSBCamera()
{
	CloseCamera();
}

BOOL CUSBCamera::IsOpen()
{
	return m_rgstrImageFiles.empty() ? FALSE : TRUE;
}

/*
	Search for USB camera's images folder. Here is the search order:

	1) Last used images folder, if non-empty
	2) Search removeable drives, from highest letter to lowest letter, for a 
	   folders beginning with the name X:\DCIM\*

*/
BOOL CUSBCamera::OpenCamera()
{
	USES_CONVERSION;

	if(m_strImagesFolder.Length() > 0 && ExsDirectoryExists(OLE2CT(m_strImagesFolder)))
	{
		AddImagesFolder(m_strImagesFolder);
		if(!m_rgstrImageFiles.empty())
			return TRUE;
	}

	// Put all removeable drives into a vector so we can search the vector
	// from beginning to end

	TCHAR szDriveStrings[_MAX_PATH];
	DWORD cbDriveStrings = ::GetLogicalDriveStrings(sizeof(szDriveStrings), szDriveStrings);
	if(cbDriveStrings == 0 || cbDriveStrings >= sizeof(szDriveStrings))
	{
		m_strError = _T("Error listing drives");
		return FALSE;
	}

	std::vector<CComBSTR> rgstrDriveStrings;
	for(TCHAR* szDrive = szDriveStrings; 
		szDrive[0] != 0; 
		szDrive = _tcschr(szDrive, 0) + 1)
	{
		DWORD fType = GetDriveType(szDrive);
		if(fType == DRIVE_REMOVABLE)
			rgstrDriveStrings.push_back(szDrive);
	}

	for(std::vector<CComBSTR>::reverse_iterator i = rgstrDriveStrings.rbegin();
	    i != rgstrDriveStrings.rend();
		i++)
	{
		CComBSTR strImagesFolder = ExsMakePath(OLE2CT(*i), g_szCameraFolder);

		if(ExsDirectoryExists(OLE2CT(strImagesFolder)))
		{
			AddImagesFolder(strImagesFolder);
			if(!m_rgstrImageFiles.empty())
			{
				m_strImagesFolder = strImagesFolder;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CUSBCamera::CloseCamera()
{
	m_rgstrImageFiles.clear();
}

BOOL CUSBCamera::GetImageCount(int& cImages)
{
	cImages = m_rgstrImageFiles.size();
	return TRUE;
}

BOOL CUSBCamera::GetImage(int iImage, BOOL fThumbnail, CDCImage& dci)
{
	USES_CONVERSION;

	m_hwndStatus = dci.m_hwndStatus;

	if(iImage >= m_rgstrImageFiles.size())
	{
		m_strError = _T("Image index out of range");
		return FALSE;
	}

	if(fThumbnail)
	{
		dci.m_fResizeMode = RM_DESIGN;
		dci.m_fStretchMode = SM_RESAMPLE;
		// SM_CXICON does not seem big enough
		dci.m_cxDesignWidth = 100;
		dci.m_cyDesignHeight = 80;
		if(!dci.SetThumbnailImage(OLE2CT(m_rgstrImageFiles[iImage])))
		{
			m_strError = dci.GetErrorMessage();
			return FALSE;
		}
	}
	else
	{
		if(!dci.SetImage(OLE2CT(m_rgstrImageFiles[iImage])))
		{
			m_strError = dci.GetErrorMessage();
			return FALSE;
		}
	}

	return TRUE;
}

void CUSBCamera::AddImagesFolder(const CComBSTR& strFolder)
{
	USES_CONVERSION;

	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA fdFindFile; memset(&fdFindFile, 0, sizeof(fdFindFile));

	CComBSTR strImages = ExsMakePath(OLE2CT(strFolder), g_szExsAllFiles);

	hFindFile = ::FindFirstFile(OLE2CT(strImages), &fdFindFile);
	while(hFindFile != INVALID_HANDLE_VALUE)
	{	
		if(lstrcmp(fdFindFile.cFileName, g_szExsCurrentDirectory) != 0 &&
		   lstrcmp(fdFindFile.cFileName, g_szExsParentDirectory) != 0)
		{
			if(fdFindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(m_fScanSubFolders)
				{
					CComBSTR strSubFolder = ExsMakePath(OLE2CT(strFolder), fdFindFile.cFileName);
					AddImagesFolder(strSubFolder);
				}
			}
			else
			{
				// 0123456789  10 - 4 = 6
				// ABCDEF.JPG
				TCHAR* pchExt = _tcsrchr(fdFindFile.cFileName, _T('.'));
				if( pchExt && 
					(
						lstrcmpi(pchExt, _T(".JPG")) == 0 ||
						lstrcmpi(pchExt, _T(".JPEG")) == 0 ||
						lstrcmpi(pchExt, _T(".BMP")) == 0 ||
						lstrcmpi(pchExt, _T(".GIF")) == 0 ||
						lstrcmpi(pchExt, _T(".PNG")) == 0
					)
				)
				{
					m_rgstrImageFiles.push_back(ExsMakePath(OLE2CT(strFolder), fdFindFile.cFileName));
				}
			}
		}

		if(!FindNextFile(hFindFile, &fdFindFile))
			break;
	}

	if(hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
}

// CURLCamera -----------------------------------------------------------------

CURLCamera::CURLCamera(const char* szImageURL)
{
	m_pvImage = NULL;
	m_cbImage = 0;;
	m_strImageURL = szImageURL;
}

CURLCamera::~CURLCamera()
{
	CloseCamera();
	if(m_pvImage)
		free(m_pvImage);
}

BOOL CURLCamera::IsOpen()
{
	return TRUE;
}

BOOL CURLCamera::OpenCamera()
{
	return TRUE;
}

void CURLCamera::CloseCamera()
{
}

BOOL CURLCamera::GetImageCount(int& cImages)
{
	cImages = 1;
	return TRUE;
}

BOOL CURLCamera::GetImage(int iImage, BOOL fThumbnail, CDCImage& dci)
{
	USES_CONVERSION;

	m_hwndStatus = dci.m_hwndStatus;

	if(!m_pvImage)
	{
		CHTTPClient hc;
		UINT cbImageCur = 0;
		UINT cbImageTotal = 0;
		UINT cbImageRead = 0;
		char szBuffer[1024];

		CStatusMessage::SetStatus(m_hwndStatus, ST_INFORMATION, _T("Contacting server"));
		if(!hc.OpenFileDownloadURL(OLE2T(m_strImageURL)))
		{
			m_strError = hc.GetErrorMessage();
			return FALSE;
		}

		CStatusMessage::SetStatus(m_hwndStatus, ST_INFORMATION, _T("Downloading headers"));
		// Content length is not always available...
		if(!hc.GetContentLength(cbImageTotal))
			cbImageTotal = 0;

		CStatusMessage::SetStatus(m_hwndStatus, ST_INFORMATION, _T("Downloading image"));
		for(;;)
		{
			cbImageRead = sizeof(szBuffer);
			if(!hc.Read(szBuffer, cbImageRead))
			{
				m_strError = hc.GetErrorMessage();
				return FALSE;
			}

			if(cbImageRead == 0)
				break;

			m_pvImage = realloc(m_pvImage, cbImageCur + cbImageRead);
			memcpy((char*)m_pvImage + cbImageCur, szBuffer, cbImageRead);
			cbImageCur += cbImageRead;
			//if(cbImageRead != sizeof(szBuffer))
			//	break;

			if(cbImageTotal)
				wsprintf(szBuffer, _T("Downloading image: %d of %d"), cbImageCur, cbImageTotal);
			else
				wsprintf(szBuffer, _T("Downloading image: %d"), cbImageCur);
			CStatusMessage::SetStatus(m_hwndStatus, ST_INFORMATION, szBuffer);
		}

		m_cbImage = cbImageCur;
	}

	if(fThumbnail)
	{
		dci.m_fResizeMode = RM_DESIGN;
		dci.m_fStretchMode = SM_RESAMPLE;
		// SM_CXICON does not seem big enough
		dci.m_cxDesignWidth = 100;
		dci.m_cyDesignHeight = 80;
	}

	if(!dci.SetImage(m_pvImage, m_cbImage))
	{
		m_strError = dci.GetErrorMessage();
		return FALSE;
	}

	return TRUE;
}

