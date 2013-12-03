// Calculator.cpp : main source file for Calculator.exe
//

#include "stdafx.h"

#include "Resource.h"

#include "AboutDlg.h"
#include "MainFrame.h"
#include "../JSONTile/Factory.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	Theme theme;
	MainFrame dlgMain(theme);
	RECT rect = {0, 0, 240, 380};

	Factory factory(theme);

	factory.load(_T("./Theme.json"));

	if(dlgMain.Create(NULL, rect, _T("Calculator"), WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW) == NULL)
	{
		TCHAR log[64] = {0};
		DWORD last = GetLastError();
		wsprintf(log, _T("Main dialog creation failed, error %d!\n"), last);
		ATLTRACE(log);
		return 0;
	}
	dlgMain.CenterWindow();
	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
