#include "../Tile/Flow.h"
#include "../Tile/Theme.h"
#include "../Tile/IWindow.h"
#include <list>

/*
Copyright 2011 Rick Parrish
*/

#pragma once

using namespace Tiles;

class Window : public CWnd,
	private IRedraw, private IWindow
{
	Theme& _theme;
	// all content goes here.
	Flow *_pFlow;
	// internal paint
	bool _bInternal;
	std::list<ITile*> _internal;
	// IWindow support
	IControl* _pCapture;
	IControl* _pFocus;
	IControl* _pHover;
	Window* _pParent;
// Construction
public:
	Window(Theme &);
	Window(Window *pParent, const rect_t &rect, Flow *pContent, IControl *pOwner);

// Attributes
public:

// Operations
public:
	// draw notification
	virtual void Redraw(ITile *);
	// IWindow
	// capture
	virtual void setCapture(IControl *);
	// focus
	virtual void setFocus(IControl *pFocus);
	// hover
	virtual void setHover(IControl *pHover);
	// popup window
	virtual IWindow* popup(const rect_t &rect, Flow *pContent, IControl *pOwner);
	// close and destroy the window
	virtual void close();
	// get window handle
	virtual Tiles::handle_t getHandle();

	Flow *getFlow();
	void setFlow(Flow *pFlow);
	Theme &getTheme() { return _theme; }

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~Window();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT, int cx, int cy);
	afx_msg void OnMouseMove(UINT, CPoint);
	afx_msg BOOL OnEraseBkgnd(CDC*);
	// create child controls (which aren't windows controls).
	afx_msg int OnCreate(LPCREATESTRUCT);
	// native keydown event triggers framework keypress.
	afx_msg void OnKeyDown(UINT, UINT, UINT);
	// native char event triggers framework keypress.
	afx_msg void OnChar(UINT, UINT, UINT);
	// native keyup event triggers framework keypress.
	afx_msg void OnKeyUp(UINT, UINT, UINT);
	// native mouse button event triggers framework event.
	afx_msg void OnLButtonDown(UINT, CPoint);
	// native mouse button event triggers framework event.
	afx_msg void OnLButtonUp(UINT, CPoint);
	// native mouse button event triggers framework event.
	afx_msg void OnMButtonDown(UINT, CPoint);
	// native mouse button event triggers framework event.
	afx_msg void OnMButtonUp(UINT, CPoint);
	// native mouse button event triggers framework event.
	afx_msg void OnRButtonDown(UINT, CPoint);
	// native mouse button event triggers framework event.
	afx_msg void OnRButtonUp(UINT, CPoint);

	DECLARE_MESSAGE_MAP()
	// compute layout of above controls for the area given.
	void Layout(int wide, int high);
};
