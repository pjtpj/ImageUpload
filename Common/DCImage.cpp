// DCImage.cpp: implementation of the CDCImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DCImage.h"
#include <setjmp.h>

#define WIDTHBYTES(i) (((i)+31)/32*4)

UINT WM_DCSTATUS = ::RegisterWindowMessage(_T("dcuploadStatus"));
UINT WM_DCPROGRESS = ::RegisterWindowMessage(_T("dcuploadProgress"));

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDCImage::CDCImage(RESIZE_MODE fResizeMode /* = RM_DESIGN */, STRETCH_MODE fStretchMode /* = SM_STRETCH */,
                   int cxDesignWidth /* = 320 */, int cyDesignHeight /* = 240 */)
{
	m_hwndStatus = NULL;
	m_fImageType = IT_JPG;
	m_fResizeMode = fResizeMode;
	m_fStretchMode = fStretchMode;
	m_cxDesignWidth = cxDesignWidth;
	m_cyDesignHeight = cyDesignHeight;
}

CDCImage::~CDCImage()
{
	DeleteImage();
}

BOOL CDCImage::SetImage(void* pvJPEGBits, UINT cbJPEGBits)
{
	DeleteImage();

	if(!m_image.Decode((BYTE*)pvJPEGBits, (DWORD)cbJPEGBits, 0))
	{
		m_strError = m_image.GetLastError();
		return FALSE;
	}

	// Adjust image size according to properties

	if(m_fResizeMode == RM_DESIGN && (
		m_image.GetWidth() != m_cxDesignWidth ||
		m_image.GetHeight() != m_cyDesignHeight))
	{
		if(m_fStretchMode == SM_STRETCH)
			ResizeImage(m_cxDesignWidth, m_cyDesignHeight);
		else if(m_fStretchMode == SM_RESAMPLE)
			ResampleImage(m_cxDesignWidth, m_cyDesignHeight);
		else
			CropImage(m_cxDesignWidth, m_cyDesignHeight);
	}

	return TRUE;
}

BOOL CDCImage::SetImage(const TCHAR* szFilePath)
{
	DeleteImage();

	if(!m_image.Load(szFilePath))
	{
		m_strError = m_image.GetLastError();
		return FALSE;
	}

	// Adjust image size according to properties

	if(m_fResizeMode == RM_DESIGN && (
		m_image.GetWidth() != m_cxDesignWidth ||
		m_image.GetHeight() != m_cyDesignHeight))
	{
		if(m_fStretchMode == SM_STRETCH)
			ResizeImage(m_cxDesignWidth, m_cyDesignHeight);
		else if(m_fStretchMode == SM_RESAMPLE)
			ResampleImage(m_cxDesignWidth, m_cyDesignHeight);
		else
			CropImage(m_cxDesignWidth, m_cyDesignHeight);
	}

	return TRUE;
}

BOOL CDCImage::SetThumbnailImage(const TCHAR* szFilePath)
{
	DeleteImage();

	if(!m_image.Load(szFilePath))
	{
		m_strError = m_image.GetLastError();
		return FALSE;
	}

	RGBQUAD white = {255, 255, 255, 0};
	if(!m_image.Thumbnail(m_cxDesignWidth, m_cyDesignHeight, white))
	{
		m_strError = m_image.GetLastError();
		return FALSE;
	}

	return TRUE;
}

BOOL CDCImage::GetImage(void*& pvJPEGBits, UINT& cbJPEGBits)
{
	BYTE* pvJPEGBitsTemp = NULL;
	long cbJPEGBitsTemp;

	// Currently, we always convert to jpg
	if(m_image.GetType() != CXIMAGE_FORMAT_JPG && m_image.GetBpp() < 24)
		m_image.IncreaseBpp(24);
	if(!m_image.Encode(pvJPEGBitsTemp, cbJPEGBitsTemp, CXIMAGE_FORMAT_JPG))
	{
		m_strError = m_image.GetLastError();
		return FALSE;
	}

	pvJPEGBits = (void*)pvJPEGBitsTemp;
	cbJPEGBits = (UINT)cbJPEGBitsTemp;
	return TRUE;
}

int CDCImage::GetImageHeight()
{
	return m_image.GetHeight();
}

int CDCImage::GetImageWidth()
{
	return m_image.GetWidth();
}

BOOL CDCImage::IsEmpty()
{
	return !m_image.IsValid();
}

BOOL CDCImage::DrawImage(HDC hdc, const RECT& rcDest)
{
	m_strError.Empty();

	if(IsEmpty())
	{
		m_strError = _T("No image available");
		return FALSE;
	}

	if(!m_image.Draw(hdc, rcDest))
	{
		m_strError = _T("Draw failed");
		return FALSE;
	}

	return TRUE;
}

const CComBSTR& CDCImage::GetErrorMessage()
{
	return m_strError;
}

void CDCImage::DeleteImage()
{
	m_image.Destroy();
}

BOOL CDCImage::ResizeImage(int w, int h)
{
	m_image.QIShrink(w, h);
	return TRUE;
}

BOOL CDCImage::ResampleImage(int w, int h)
{
	m_image.Resample2(w, h);
	return TRUE;
}

BOOL CDCImage::CropImage(int w, int h)
{
	int x, y, ox, oy, ow, oh;
	int x1, x2, y1, y2, ox1, oy1;

	ow = GetImageWidth();
	oh = GetImageHeight();

	if(ow > w)
	{
		// Orginal is larger than new: use center of original
		x1 = 0; 
		x2 = w; // 10
		ox1 = (ow - w) / 2; // (100 - 10) / 2 = 45
		// ox2 = ox1 + w; // 54 + 10 = 55;
	}
	else
	{
		// New is larger than original: copy original to center
		x1 = (w - ow) / 2;
		x2 = x1 + ow;
		ox1 = 0;
		// ox2 = ow;
	}
	if(oh > h)
	{
		// Orginal is larger than new: use center of original
		y1 = 0;
		y2 = h;
		oy1 = (oh - h) / 2;
		// oy2 = oy1 + h;
	}
	else
	{
		// New is larger than original: copy original to center
		y1 = (h - oh) / 2;
		y2 = y1 + oh;
		oy1 = 0;
		// oy2 = oh;
	}

	m_image.Crop(x1, y1, x2, y2);

	return TRUE;
}

