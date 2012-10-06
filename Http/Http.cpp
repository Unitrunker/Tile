// Copyright © 2008 Rick Parrish

#include <windows.h>
#include <wininet.h>
#include <time.h>
#include "Http.h"
#include <string>

#pragma comment(lib, "wininet")

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

static const TCHAR strAgent[] = _T("Bath Tile SOAP");

static void crackURL(const TCHAR *url, tstring &strHost, tstring &strRelative)
{
	const tstring strURL(url);
	size_t iChop = strURL.find(_T("://"));
	if (iChop < strURL.size())
		strHost = strURL.substr(iChop + 3);
	else
		strHost = strURL;

	iChop = strHost.find(_T('/'));
	if (iChop >= strHost.size())
		strRelative = _T("/");
	else
	{
		strRelative = strHost.substr(iChop);
		strHost = strHost.substr(0, iChop);
	}
}

// http://xyz/
bool HttpGetURL(
	const TCHAR *strURL, 
	const TCHAR *strUID, 
	const TCHAR *strPwd, 
	HANDLE hFile, 
	void *pContext, 
	bool (*progress)(void *, DWORD dwSofar, DWORD dwTotal) )
{
    HINTERNET hSession = 0;
	DWORD dwError = 0;
    bool bOK = false;

	tstring strHost, strRelative;
	crackURL(strURL, strHost, strRelative);

    hSession = InternetOpen(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    dwError = GetLastError();
	if (hSession && hSession != INVALID_HANDLE_VALUE)
	{
		HINTERNET hConnect = InternetConnect(hSession, strHost.c_str(), INTERNET_DEFAULT_HTTP_PORT, strUID, strPwd, INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect && hConnect != INVALID_HANDLE_VALUE)
		{
			const TCHAR *listAcceptable[] = {_T("*/*"), NULL};
			const TCHAR *httpGet = _T("GET");
			HINTERNET hRequest= HttpOpenRequest(hConnect, httpGet, strRelative.c_str(), HTTP_VERSION, NULL, listAcceptable, INTERNET_FLAG_NO_CACHE_WRITE, NULL);
			if(hRequest && hRequest != INVALID_HANDLE_VALUE)
			{
				const TCHAR *httpAccept = _T("Accept: */*\r\n");
				HttpAddRequestHeaders(hRequest, httpAccept, (DWORD)_tcslen(httpAccept), HTTP_ADDREQ_FLAG_REPLACE);
				HttpSendRequest(hRequest, NULL, 0, NULL, 0);
				TCHAR strLength[32] = {0};
				DWORD dwLength = _countof(strLength);
				BOOL bQuery = HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, strLength, &dwLength, NULL);
				//if(bQuery)
				{
					unsigned char octetChunk[4096] = {0};
					DWORD dwTotal = 0;
					DWORD dwRemains = _tcstoul(strLength, NULL, 10);
					while (dwRemains > 0 || !bQuery)
					{
						DWORD dwRead = 0;
						DWORD dwXfer = sizeof octetChunk;
						BOOL bRead = InternetReadFile(hRequest, octetChunk, dwXfer, &dwRead);
						if (bRead && dwRead > 0)
						{
							DWORD dwWrote = 0;
							WriteFile(hFile, octetChunk, dwRead, &dwWrote, NULL);
							dwTotal += dwRead;
							if (bQuery)
								dwRemains -= dwRead;
							if (progress != NULL)
								progress(pContext, dwRemains, dwTotal);
						}
						else
							break;
					}
					bOK = dwRemains == 0;
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hSession);
	}
	return bOK;
}

static bool parseSkip(TCHAR *&strCursor, int iSkip)
{
	while ( *strCursor != 0 && iSkip > 0)
	{
		strCursor++;
		iSkip--;
	}
	return iSkip == 0;
}

static bool parseSpace(TCHAR *&strCursor)
{
	while ( *strCursor != 0 && *strCursor == ' ' )
	{
		strCursor++;
	}
	return true;
}

static bool parseMatch(TCHAR *&strCursor, TCHAR chMatch)
{
	if (*strCursor == chMatch)
	{
		strCursor++;
		return true;
	}
	return false;
}

static bool parseMatch(TCHAR *&strCursor, const TCHAR *strMatch)
{
	size_t iLen = _tcslen(strMatch);
	if ( !_tcsncmp(strCursor, strMatch, iLen) )
	{
		strCursor += iLen;
		return true;
	}
	return false;
}

static bool parseInt(TCHAR *&strCursor, int &iValue)
{
	TCHAR *strSave = strCursor;
	iValue = (int)_tcstol(strCursor, &strCursor, 10);
	return strSave != strCursor;
}

static bool parseMonth(TCHAR *&strCursor, int &iMonth)
{
	static const TCHAR *strMonths[] = 
	{
		_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")
	};
	for (size_t i = 0; i < _countof(strMonths); ++i)
	{
		if ( parseMatch(strCursor, strMonths[i]) )
		{
			iMonth = (int)i;
			return true;
		}
	}
	return false;
}

// http://xyz/
bool HttpHeadURL(const TCHAR *strURL, time_t &iStamp)
{
    HINTERNET hSession = 0;
	DWORD dwError = 0;
    bool bOK = false;

	tstring strHost, strRelative;
	crackURL(strURL, strHost, strRelative);

    hSession = InternetOpen(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    dwError = GetLastError();
	if (hSession && hSession != INVALID_HANDLE_VALUE)
	{
		HINTERNET hConnect = InternetConnect(hSession, strHost.c_str(), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect && hConnect != INVALID_HANDLE_VALUE)
		{
			const TCHAR *listAcceptable[] = {_T("*/*"), NULL};
			const TCHAR *httpGet = _T("HEAD");
			HINTERNET hRequest= HttpOpenRequest(hConnect, httpGet, strRelative.c_str(), HTTP_VERSION, NULL, listAcceptable, INTERNET_FLAG_NO_CACHE_WRITE, NULL);
			if(hRequest && hRequest != INVALID_HANDLE_VALUE)
			{
				const TCHAR *httpAccept = _T("Accept: */*\r\n");
				HttpAddRequestHeaders(hRequest, httpAccept, (DWORD)_tcslen(httpAccept), HTTP_ADDREQ_FLAG_REPLACE);
				HttpSendRequest(hRequest, NULL, 0, NULL, 0);
				TCHAR strLength[0x40] = {0};
				DWORD dwLength = _countof(strLength);
				BOOL bQuery = HttpQueryInfo(hRequest, HTTP_QUERY_LAST_MODIFIED, strLength, &dwLength, NULL);
				if (bQuery)
				{
					TCHAR *strCursor = strLength;
					if ( parseSkip(strCursor, 3) && 
						parseMatch(strCursor, ',') &&
						parseSpace(strCursor) )
					{
						struct tm stamp = {0};
						bOK = parseInt(strCursor, stamp.tm_mday) &&
							parseSpace(strCursor) &&
							parseMonth(strCursor, stamp.tm_mon) &&
							parseSpace(strCursor) &&
							parseInt(strCursor, stamp.tm_year) &&
							parseSpace(strCursor) &&
							parseInt(strCursor, stamp.tm_hour) &&
							parseMatch(strCursor, ':') &&
							parseInt(strCursor, stamp.tm_min) &&
							parseMatch(strCursor, ':') &&
							parseInt(strCursor, stamp.tm_sec);

						if (bOK)
						{
							stamp.tm_year -= 1900;
							iStamp = mktime(&stamp);
						}
					}
				}
				else
					bOK = false;
				// Mon, 26 May 2008 21:49:31 GMT
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hSession);
	}
	return bOK;
}

bool HttpSOAP(
	const TCHAR *strURL,	// URL
	const TCHAR *strUID, 
	const TCHAR *strPwd,
	const TCHAR *strAction,	// SOAP action
	const char *strPost,		// SOAP request UTF-8
	size_t iPost,				// length of request
	HANDLE hFile,				// File handle to write response
	void *pContext, 
	bool (*progress)(void *, DWORD dwSofar, DWORD dwTotal))
{
    HINTERNET hSession = 0;
	DWORD dwError = 0;
    bool bOK = false;

	tstring strHost, strRelative;
	crackURL(strURL, strHost, strRelative);

    hSession = InternetOpen(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    dwError = GetLastError();
	if (hSession && hSession != INVALID_HANDLE_VALUE)
	{
		HINTERNET hConnect = InternetConnect(hSession, strHost.c_str(), INTERNET_DEFAULT_HTTP_PORT, strUID, strPwd, INTERNET_SERVICE_HTTP, 0, NULL);
		if (hConnect && hConnect != INVALID_HANDLE_VALUE)
		{
			const TCHAR *listAcceptable[] = {_T("*/*"), NULL};
			const TCHAR *httpPost = _T("POST");
			HINTERNET hRequest= HttpOpenRequest(hConnect, httpPost, strRelative.c_str(), HTTP_VERSION, NULL, listAcceptable, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES, NULL);
			if(hRequest && hRequest != INVALID_HANDLE_VALUE)
			{
				tstring strHeader(_T("Soapaction: ") );
				strHeader += strAction;
				const TCHAR *strContent = _T("Content-Type: text/xml; charset=utf-8");

				if ( !HttpAddRequestHeaders(hRequest, strHeader.c_str(), (DWORD)strHeader.size(), HTTP_ADDREQ_FLAG_ADD) )
					_ASSERT(false);
				if ( !HttpAddRequestHeaders(hRequest, strContent, (DWORD)_tcslen(strContent), HTTP_ADDREQ_FLAG_ADD) )
					_ASSERT(false);

				//char strBuffer[512] = {0};
				//DWORD dwIndex = 0;
				//DWORD dwSize = sizeof strBuffer;
				HttpSendRequest(hRequest, NULL, NULL, (void *)strPost, (DWORD)iPost);

				//HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS|HTTP_QUERY_FLAG_REQUEST_HEADERS, strBuffer, &dwSize, &dwIndex);
				TCHAR strLength[32] = {0};
				DWORD dwTotal = 0;
				DWORD dwLength = _countof(strLength);
				BOOL bQuery = HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, strLength, &dwLength, NULL);
				//if(bQuery)
				{
					unsigned char octetChunk[4096] = {0};
					DWORD dwRemains = _tcstoul(strLength, NULL, 10);
					while (dwRemains > 0 || !bQuery)
					{
						DWORD dwRead = 0;
						DWORD dwXfer = sizeof octetChunk;
						BOOL bRead = InternetReadFile(hRequest, octetChunk, dwXfer, &dwRead);
						if (bRead && dwRead > 0)
						{
							DWORD dwWrote = 0;
							WriteFile(hFile, octetChunk, dwRead, &dwWrote, NULL);
							dwTotal += dwRead;
							if (bQuery)
								dwRemains -= dwRead;
							if (progress != NULL)
								progress(pContext, dwRemains, dwTotal);
						}
						else
							break;
					}
					bOK = dwRemains == 0;
				}
				InternetCloseHandle(hRequest);
			}
			InternetCloseHandle(hConnect);
		}
		InternetCloseHandle(hSession);
	}
	return bOK;
}
