#include "../Tile/IWindow.h"

/*
Copyright 2011, 2012 Rick Parrish
*/

#pragma once

using namespace Tiles;

struct MockWindow : public IWindow
{
	// mouse cursor
	void setCursor(int cursor);
	// mouse capture
	virtual void setCapture(IControl *pControl, int cursor);
	// alter focus.
	virtual void setFocus(bool bFocus);
	// focus
	virtual void setFocus(IControl *pFocus);
	// commit focus control edits.
	virtual void apply();
	// hover
	virtual void setHover(IControl *pFocus);
	// popup window
	virtual IWindow* popup(const rect_t &rect, Pane *pContent, IControl *pOwner);
	// close and destroy the window
	virtual void close();
	// window handle
	virtual Tiles::handle_t getHandle() { return 0; }
};