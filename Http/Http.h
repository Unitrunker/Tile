#include <tchar.h>

// Copyright © 2008 Rick Parrish

extern "C" bool HttpGetURL(
	const TCHAR *strURL, 
	const TCHAR *strUID, 
	const TCHAR *strPwd, 
	HANDLE hFile, 
	void *pContext, 
	bool (*progess)(void *, DWORD dwSofar, DWORD dwTotal));

extern "C" bool HttpHeadURL(const TCHAR *strURL, time_t &iStamp);

extern "C" bool HttpSOAP(
	const TCHAR *strURL,		// URL
	const TCHAR *strUID, 
	const TCHAR *strPwd,
	const TCHAR *strAction,		// SOAP action
	const char *strPost,		// SOAP request UTF-8
	size_t iPost,				// length of request
	HANDLE hFile,				// File handle to write response
	void *pContext, 
	bool (*progess)(void *, DWORD dwSofar, DWORD dwTotal));
