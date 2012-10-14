#include "../Tile/Flow.h"
#include "../Tile/Theme.h"
#include "../Tile/IWindow.h"
#include <list>

#pragma once

using namespace Tiles;

class Window : public CWindowImpl<Window>,
	private IRedraw, public IWindow
{
	Theme& _theme;
	// all content goes here.
	Flow *_pFlow;
	// internal paint
	bool _bInternal;
	// use simpler batched painting
	bool _bBatch;
	std::list<ITile*> _internal;
	// IWindow support
	IControl* _pCapture;
	IControl* _pFocus;
	IControl* _pHover;

	sophia::delegate1<void, const Theme*> _themeChange;

	void onChange(const Theme *);

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

	// compute layout of above controls for the area given.
	void Layout(int wide, int high);
	// create child controls (which aren't windows controls).
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// calls Layout above.
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// suppress background erase flicker.
	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native paint event triggers framework Draw.
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native keydown event triggers framework keypress.
	LRESULT OnKeyDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native char event triggers framework keypress.
	LRESULT OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native keyup event triggers framework keypress.
	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnMButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse button event triggers framework event.
	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// native mouse move event triggers framework event.
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCommand(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

public:
	Window(Theme &theme);
	Window(Window *pParent, const rect_t &rect, Flow *pContent, IControl *pOwner);
	~Window();

	static ATL::CWndClassInfo& GetWndClassInfo();

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(Window)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMButtonDown)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
	END_MSG_MAP()

	Flow *getFlow();
	void setFlow(Flow *pFlow);
	Theme &getTheme() { return _theme; }
};
