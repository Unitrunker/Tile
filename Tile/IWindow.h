#include "Types.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct IControl;
struct Flow;

// Soon to be renamed "IWindow"

struct IWindow
{
	// mouse capture
	virtual void setCapture(IControl *pControl) = 0;
	// alter focus.
	virtual void setFocus(bool bFocus) = 0;
	// set focus to control.
	virtual void setFocus(IControl *pFocus) = 0;
	// hover
	virtual void setHover(IControl *pFocus) = 0;
	// popup window associated with this window
	virtual IWindow* popup(const rect_t &rect, Flow *pContent, IControl *pOwner) = 0;
	// close and destroy the window
	virtual void close() = 0;
	// window handle
	virtual handle_t getHandle() = 0;
	// TODO: clipboard operations
};

};