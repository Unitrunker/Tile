#include "Types.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct IControl;
struct Pane;

// Window interface
struct IWindow
{
	enum cursor
	{
		ARROW = 32512, IBEAM = 32513, WAIT = 32514, CROSS = 32515,
		UPARROW = 32516, SIZE = 32640, ICON = 32641, SIZENWSE = 32642,
		SIZENESW = 32643, SIZEWE = 32644, SIZENS = 32645, SIZEALL = 32646, 
		NO = 32648, HAND = 32649, APPSTARTING = 32650, HELP = 32651
	};

	// mouse cursor
	virtual void setCursor(int cursor = ARROW) = 0;
	// mouse capture
	virtual void setCapture(IControl *pControl, int cursor = ARROW) = 0;
	// alter focus.
	virtual void setFocus(bool bFocus) = 0;
	// set focus to control.
	virtual void setFocus(IControl *pFocus) = 0;
	// hover
	virtual void setHover(IControl *pFocus) = 0;
	// popup window associated with this window
	virtual IWindow* popup(const rect_t &rect, Pane *pContent, IControl *pOwner) = 0;
	// close and destroy the window
	virtual void close() = 0;
	// window handle
	virtual handle_t getHandle() = 0;
	// TODO: clipboard operations
};

};