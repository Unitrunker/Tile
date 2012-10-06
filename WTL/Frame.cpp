#include "stdafx.h"
#include "Frame.h"

Frame::Frame(Theme &theme) : Window(theme)
{
}

Frame::~Frame()
{
}

LRESULT Frame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DWORD dwMask = WS_CHILD|WS_POPUP;
	DWORD dwStyle = GetStyle();
	if ( (dwStyle & dwMask) == 0 )
		::PostQuitMessage(1);
	bHandled = FALSE;
	return 0;
}
