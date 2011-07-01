// HTTPClient.h: interface for the CHTTPClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTTPCLIENT_H__A594DF00_A683_11D3_A61E_0020A631A360__INCLUDED_)
#define AFX_HTTPCLIENT_H__A594DF00_A683_11D3_A61E_0020A631A360__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct CHTTPClientData;

class CHTTPClient  
{
public:
	CHTTPClient();
	virtual ~CHTTPClient();

public:
	BOOL IsConnected();
	BOOL IsURLOpen();
	BOOL GetContentLength(UINT& cbContent);
	BOOL GetStatusCode(int& fStatusCode);
	const CComBSTR& GetErrorMessage();

public:
	// Connect is optional - its only purpose is to set the Agent
	BOOL Connect(const TCHAR* szAgent = _T("ExemplarHTTPClient"));

	BOOL OpenFileDownloadURL(const TCHAR* szURL);
	BOOL OpenFileUploadURL(const TCHAR* szURL, const TCHAR* szFileName, 
		const TCHAR* szMIMEType, UINT cbFile);

	BOOL Read(void* pvData, UINT& cbRead);
	BOOL Write(void* pvData, UINT& cbWrite);

	void CloseURL();
	void CloseConnection();

protected:
	CHTTPClientData* m_phcd;
	CComBSTR m_strAgent;
	CComBSTR m_strError;
	UINT m_cbUploadFileCur;
	UINT m_cbUploadFileTotal;

	void SetErrorMessage();
};

#endif // !defined(AFX_HTTPCLIENT_H__A594DF00_A683_11D3_A61E_0020A631A360__INCLUDED_)
