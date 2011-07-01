// HTTPClient.cpp: implementation of the CHTTPClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HTTPClient.h"
#include <wininet.h>
#include <limits.h>

// CHTTPClientData ------------------------------------------------------------
/*
	CHTTPClientData is a simple structure used by CHTTPClient
	to store WinINet related data. By hiding WinINet specific 
	information, client applications do not have to include 
	or be aware of wininet.h to use CHTTPClient.
*/

struct CHTTPClientData
{
	HINTERNET m_hinetConnect;
	HINTERNET m_hinetURL;
	HINTERNET m_hinetHTTPConnect;
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHTTPClient::CHTTPClient()
{
	m_phcd = new CHTTPClientData;
	m_phcd->m_hinetConnect = NULL;
	m_phcd->m_hinetURL = NULL;
	m_phcd->m_hinetHTTPConnect = NULL;

	m_cbUploadFileCur = 0;
	m_cbUploadFileTotal = 0;
}

CHTTPClient::~CHTTPClient()
{
	CloseURL();
	CloseConnection();
	delete m_phcd;
}

BOOL CHTTPClient::IsConnected()
{
	return m_phcd->m_hinetConnect ? TRUE : FALSE;
}

BOOL CHTTPClient::IsURLOpen()
{
	return m_phcd->m_hinetURL ? TRUE : FALSE;
}

BOOL CHTTPClient::GetContentLength(UINT& cbContent)
{
	ATLASSERT(IsConnected());
	ATLASSERT(IsURLOpen());

	DWORD dwContent = 0;
	DWORD cbDwContent = sizeof(dwContent);
	if(!HttpQueryInfo(m_phcd->m_hinetURL, 
		HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER,
		&dwContent, &cbDwContent, NULL))
	{
		SetErrorMessage();
		return FALSE;
	}
	ATLASSERT(dwContent <= UINT_MAX);
	cbContent = (UINT)dwContent;
	return TRUE;
}

BOOL CHTTPClient::GetStatusCode(int& fStatusCode)
{
	ATLASSERT(IsConnected());
	ATLASSERT(IsURLOpen());

	DWORD dwStatus = 0;
	DWORD cbDwStatus = sizeof(dwStatus);
	if(!HttpQueryInfo(m_phcd->m_hinetURL, 
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&dwStatus, &cbDwStatus, NULL))
	{
		SetErrorMessage();
		return FALSE;
	}
	ATLASSERT(dwStatus <= INT_MAX);
	fStatusCode = (int)dwStatus;
	return TRUE;
}

const CComBSTR& CHTTPClient::GetErrorMessage()
{
	return m_strError;
}

BOOL CHTTPClient::Connect(const TCHAR* szAgent)
{
	CloseURL();
	CloseConnection();

	m_phcd->m_hinetConnect = InternetOpen(szAgent, INTERNET_OPEN_TYPE_PRECONFIG, 
		NULL, NULL, 0);
	if(m_phcd->m_hinetConnect == NULL)
	{
		SetErrorMessage();
		return FALSE;
	}

	return TRUE;
}

BOOL CHTTPClient::OpenFileDownloadURL(const TCHAR* szURL)
{
	ATLASSERT(!IsURLOpen());

	if(!IsConnected())
		if(!Connect())
			return FALSE;

	ATLASSERT(IsConnected());
	m_phcd->m_hinetURL = InternetOpenUrl(m_phcd->m_hinetConnect, szURL, NULL, 0, 
		INTERNET_FLAG_RESYNCHRONIZE, 0);
	if(!m_phcd->m_hinetURL)
	{
		SetErrorMessage();
		return FALSE;
	}

	return TRUE;
}

static char szBoundary[] = "SEPARATORSTRINGTEZTECHDOTCOM1";
static char szBoundary2[] = "\r\n--SEPARATORSTRINGTEZTECHDOTCOM1\r\n";
static char szBoundary3[] = "\r\n--SEPARATORSTRINGTEZTECHDOTCOM1--";

BOOL CHTTPClient::OpenFileUploadURL(const TCHAR* szURL, 
																		const TCHAR* szFileName, 
																		const TCHAR* szMIMEType, 
																		UINT cbFile)
{
	ATLASSERT(!IsURLOpen());

	if(!IsConnected())
		if(!Connect())
			return FALSE;

	URL_COMPONENTS urlcs;
	memset(&urlcs, 0, sizeof(urlcs));
	urlcs.dwStructSize = sizeof(urlcs);
	TCHAR szHostName[256];
	urlcs.lpszHostName = szHostName;
	urlcs.dwHostNameLength = sizeof(szHostName);
	TCHAR szUrlPath[1024];
	urlcs.lpszUrlPath = szUrlPath;
	urlcs.dwUrlPathLength = sizeof(szUrlPath);
	TCHAR szExtraInfo[1024];
	urlcs.lpszExtraInfo = szExtraInfo;
	urlcs.dwExtraInfoLength = sizeof(szExtraInfo);
	if(!InternetCrackUrl(szURL, 0, 0, &urlcs))
	{
		SetErrorMessage();
		return FALSE;
	}

	m_phcd->m_hinetHTTPConnect = InternetConnect(
		m_phcd->m_hinetConnect, szHostName, urlcs.nPort, NULL, 
		NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if(!m_phcd->m_hinetHTTPConnect)
	{
		SetErrorMessage();
		return FALSE;
	}

	TCHAR szObjectName[2048];
	strcpy(szObjectName, szUrlPath);
	strcat(szObjectName, szExtraInfo);
	m_phcd->m_hinetURL = HttpOpenRequest(m_phcd->m_hinetHTTPConnect, 
		_T("POST"), szObjectName, NULL, NULL, NULL, 0, 0);
	if(!m_phcd->m_hinetURL)
	{
		SetErrorMessage();
		InternetCloseHandle(m_phcd->m_hinetHTTPConnect);
		m_phcd->m_hinetHTTPConnect = NULL;
		return FALSE;
	}

	// Calculate upload data size

	// I don't think http will accept wide characters
	char szFileSizeHdr[] = 
		"Content-Disposition: form-data; name=\"MAX_FILE_SIZE\"\r\n\r\n";
	char szFileSizeData[50];
	wsprintf(szFileSizeData, "%d", cbFile + 50000);

	ATLASSERT(strlen(szFileName) < MAX_PATH);
	ATLASSERT(strlen(szMIMEType) < MAX_PATH);
	char szFileHdrFmt[] = 
		"Content-Disposition: form-data; name=\"userfile\"; filename=\"%s\"\r\n"
	  "Content-Type: %s\r\n\r\n";
	char szFileHdr[sizeof(szFileHdrFmt) + MAX_PATH + MAX_PATH];
	wsprintf(szFileHdr, szFileHdrFmt, szFileName, szMIMEType);
	
	UINT cbContent = 
		strlen(szBoundary2) +
		strlen(szFileSizeHdr) +
		strlen(szFileSizeData) +
		strlen(szBoundary2) +
		strlen(szFileHdr) +
		cbFile +
		strlen(szBoundary3);

	// Calculate and send extra http headers

	char szBuffer[2048];
	wsprintf(szBuffer, "Content-length: %d\r\n", cbContent);
	if(!HttpAddRequestHeaders(m_phcd->m_hinetURL, 
		szBuffer, strlen(szBuffer), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE))
	{
		SetErrorMessage();
		CloseURL();
		return FALSE;
	}

	wsprintf(szBuffer, "Content-type: multipart/form-data; boundary=%s\r\n", szBoundary);
	if(!HttpAddRequestHeaders(m_phcd->m_hinetURL, 
		szBuffer, strlen(szBuffer), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE))
	{
		SetErrorMessage();
		CloseURL();
		return FALSE;
	}

	// Start upload sequency

	if(!HttpSendRequestEx(m_phcd->m_hinetURL, NULL, NULL, HSR_INITIATE, 0))
	{
		SetErrorMessage();
		CloseURL();
		return FALSE;
	}

	// Write pre-data MIME headers

	DWORD cbWritten;
	if(
		!InternetWriteFile(m_phcd->m_hinetURL, szBoundary2, 
			strlen(szBoundary2), &cbWritten) ||
		!InternetWriteFile(m_phcd->m_hinetURL, szFileSizeHdr, 
			strlen(szFileSizeHdr), &cbWritten) || 
		!InternetWriteFile(m_phcd->m_hinetURL, szFileSizeData, 
			strlen(szFileSizeData), &cbWritten) ||
		!InternetWriteFile(m_phcd->m_hinetURL, szBoundary2, 
			strlen(szBoundary2), &cbWritten) ||
		!InternetWriteFile(m_phcd->m_hinetURL, szFileHdr, 
			strlen(szFileHdr), &cbWritten)
		)
	{
		SetErrorMessage();
		CloseURL();
		return FALSE;
	}

	// Set these so Write knowns when to "end" the request

	m_cbUploadFileCur = 0;
	m_cbUploadFileTotal = cbFile;

	return TRUE;
}

BOOL CHTTPClient::Read(void* pvData, UINT& cbRead)
{
	ATLASSERT(cbRead > 0);
	DWORD dwRead = 0;
	if(!InternetReadFile(m_phcd->m_hinetURL, pvData, cbRead, &dwRead))
	{
		SetErrorMessage();
		return FALSE;
	}
	ATLASSERT(dwRead < UINT_MAX);
	cbRead = (UINT)dwRead;
	return TRUE;
}

BOOL CHTTPClient::Write(void* pvData, UINT& cbWrite)
{
	ATLASSERT(cbWrite > 0);
	DWORD dwWrite = 0;
	if(!InternetWriteFile(m_phcd->m_hinetURL, pvData, cbWrite, &dwWrite))
	{
		SetErrorMessage();
		return FALSE;
	}
	ATLASSERT(dwWrite < UINT_MAX);
	cbWrite = (UINT)dwWrite;

	if(m_cbUploadFileTotal > 0 && m_cbUploadFileCur < m_cbUploadFileTotal)
	{
		m_cbUploadFileCur += cbWrite;
		if(m_cbUploadFileCur >= m_cbUploadFileTotal)
		{
			m_cbUploadFileCur = 0;
			m_cbUploadFileTotal = 0;

			DWORD cbWritten = 0;
			if(
				!InternetWriteFile(m_phcd->m_hinetURL, szBoundary3, 
					strlen(szBoundary3), &cbWritten) || 
				!HttpEndRequest(m_phcd->m_hinetURL, NULL, 0, 0)
				)
			{
				SetErrorMessage();
				return FALSE;
			}
		}
	}

	return TRUE;
}

void CHTTPClient::CloseURL()
{
	if(m_phcd->m_hinetURL)
	{
		InternetCloseHandle(m_phcd->m_hinetURL);
		m_phcd->m_hinetURL = NULL;
	}
	if(m_phcd->m_hinetHTTPConnect)
	{
		InternetCloseHandle(m_phcd->m_hinetHTTPConnect);
		m_phcd->m_hinetHTTPConnect = NULL;
	}
}

void CHTTPClient::CloseConnection()
{
	if(IsURLOpen())
		CloseURL();
	if(m_phcd->m_hinetConnect)
	{
		InternetCloseHandle(m_phcd->m_hinetConnect);
		m_phcd->m_hinetConnect = NULL;
	}
}

void CHTTPClient::SetErrorMessage()
{
	DWORD fError = GetLastError();
	m_strError.Empty();

	if(fError == ERROR_INTERNET_EXTENDED_ERROR)
	{
		DWORD fInetError = 0;
		DWORD cchMessage = 0;
		InternetGetLastResponseInfo(&fInetError, NULL, &cchMessage);
		if(cchMessage)
		{
			TCHAR* szMessage = new TCHAR[cchMessage + 1];
			if(InternetGetLastResponseInfo(&fInetError, szMessage, &cchMessage))
			{
				szMessage[cchMessage] = 0;
				m_strError = szMessage;
			}
			delete szMessage;
		}

		if(m_strError.Length() == 0)
			m_strError = _T("Unknown WinINet error");
	}
	else
	{
		TCHAR* szMessage;
		DWORD cbMessage = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_MAX_WIDTH_MASK, 
			NULL, fError, 0,
			(TCHAR*)&szMessage, 0, NULL);

		if(cbMessage == 0 || szMessage == NULL || (DWORD)szMessage == 0xFFFFFFFF)
		{
			// Load default error message
			m_strError = _T("Unknown system exception");
		}
		else
		{
			m_strError = szMessage;

			HLOCAL hMessage = LocalHandle(szMessage);
			LocalFree(hMessage);
		}
	}
}
