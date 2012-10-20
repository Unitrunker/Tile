#include "../Tile/IWindow.h"

/*
Copyright 2011 Rick Parrish
*/

#pragma once

using namespace Tiles;

struct MockWindow : public IWindow
{
	// mouse capture
	virtual void setCapture(IControl *pControl);
	// alter focus.
	virtual void setFocus(bool bFocus);
	// focus
	virtual void setFocus(IControl *pFocus);
	// hover
	virtual void setHover(IControl *pFocus);
	// popup window
	virtual IWindow* popup(const rect_t &rect, Flow *pContent, IControl *pOwner);
	// close and destroy the window
	virtual void close();
	// window handle
	virtual Tiles::handle_t getHandle() { return 0; }
};