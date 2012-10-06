
// MFCTileHost.h : main header file for the MFCTileHost application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCTileHostApp:
// See MFCTileHost.cpp for the implementation of this class
//

class CMFCTileHostApp : public CWinAppEx
{
public:
	CMFCTileHostApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCTileHostApp theApp;
