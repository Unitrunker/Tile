#include "stdafx.h"
#include "MockWindow.h"
#include "../Tile/IControl.h"

/*
Copyright 2011 Rick Parrish
*/

// mouse capture
void MockWindow::setCapture(IControl *pCapture)
{
	printf("%ld called %s\n", pCapture->identity(), __FUNCTION__);
}

// focus
void MockWindow::setFocus(IControl *pFocus)
{
	printf("%ld called %s\n", pFocus->identity(), __FUNCTION__);
}

// hover
void MockWindow::setHover(IControl *pHover)
{
	printf("%ld called %s\n", pHover->identity(), __FUNCTION__);
}

// popup window
IWindow* MockWindow::popup(const rect_t &rect, Flow *pContent, IControl *pOwner)
{
	printf("%ld called %s\n", pOwner->identity(), __FUNCTION__);
	return NULL;
}

// close and destroy the window
void MockWindow::close()
{
	printf("Called %s\n", __FUNCTION__);
}
