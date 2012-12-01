#include "stdafx.h"
#include "Popup.h"
#include "Canvas.h"
#include "../Tile/Fill.h"

Popup::Popup(Theme &theme) :
	_theme(theme), // init theme before anything else :)
	_pCapture(NULL),
	_pFocus(NULL),
	_pPane(NULL),
	_pHover(NULL),
	_bInternal(false)
{
	Pane *pPane = new Pane(0, _theme, eDown);
	pPane->setWeight(eDown, 1);
	//pPane->watch(this);
	//pPane->setDesktop(this);

	Fill *pFill = new Fill(0, _theme);
	pPane->Add(pFill, 0, 2048, 1);
	setPane(pPane);
}

Popup::Popup(Theme &theme, const rect_t &rect, Pane *pContent, IControl *pOwner) :
	_theme(theme), // init theme before anything else :)
	_pCapture(NULL),
	_pFocus(NULL),
	_pPane(NULL),
	_pHover(NULL),
	_bInternal(false)
{
	setPane(pContent);
	// TODO
	rect;	// needed?
	pOwner; // needed?
}

Popup::~Popup()
{
	delete _pPane;
}

ATL::CWndClassInfo& Popup::GetWndClassInfo()
{
	static ATL::CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), 0, StartWindowProc,
		  0, 0, NULL, NULL, NULL, NULL, NULL, _T("POPUP"), NULL },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}

BOOL Popup::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

// Native paint event call root Flow's draw operation.
LRESULT Popup::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Canvas dc(m_hWnd);
	if (_pPane)
	{
		bool focus = GetFocus() == m_hWnd;
		// internal paint operation?
		if (_bInternal)
		{
			std::list<ITile*>::iterator it = _internal.begin();
			while (it != _internal.end())
			{
				(*it)->Draw(&dc, focus);
				it++;
			}
		}
		else
			_pPane->Draw(&dc, focus);
	}
	_internal.clear();				
	return 0;
}

static int getKeyMask()
{
	int mask = 0;
	if ( GetKeyState(VK_SHIFT) < 0 )
		mask |= KeyEvent::SHIFT;
	if ( GetKeyState(VK_CONTROL) < 0 )
		mask |= KeyEvent::CONTROL;
	if ( GetKeyState(VK_MENU) < 0 )
		mask |= KeyEvent::ALT;
	int caps = GetKeyState(VK_CAPITAL);
	if (caps & 1)
		mask |= KeyEvent::CAPS;
	return mask;
}

// native keydown event triggers framework keypress.
LRESULT Popup::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	KeyEvent press = {0};
	press._what = KeyEvent::DOWN;
	press._mask = getKeyMask();
	press._code = wParam;
	if (_pPane)
		bHandled = _pPane->dispatch(press);
	return 0;
}

// native char event triggers framework keypress.
LRESULT Popup::OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = true;
	return 0;
}

// native keyup event triggers framework keypress.
LRESULT Popup::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	KeyEvent press = {0};
	press._what = KeyEvent::UP;
	press._mask = getKeyMask();
	press._code = wParam;
	if (_pPane)
		bHandled = _pPane->dispatch(press);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eLeft;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eLeft;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnMButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eMiddle;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eMiddle;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eRight;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eRight;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse move event triggers framework event.
LRESULT Popup::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eMove;
	if ( GetKeyState(VK_LBUTTON) < 0 )
		mouse._button += MouseEvent::eLeft;
	if ( GetKeyState(VK_RBUTTON) < 0 )
		mouse._button += MouseEvent::eRight;
	if ( GetKeyState(VK_MBUTTON) < 0 )
		mouse._button += MouseEvent::eMiddle;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

void Popup::Layout(int wide, int high)
{
	rect_t rect = {0};
	rect.wide = wide;
	rect.high = high;
	if (_pPane != NULL)
		_pPane->setRect(rect);
}

LRESULT Popup::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
	{
		Layout( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
	}
	return 0;
}

LRESULT Popup::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CREATESTRUCT *p = reinterpret_cast<CREATESTRUCT*>(lParam);
	// not quite right - window may have a border so client area will be smaller.
	Layout(p->cx, p->cy);
	if (p->style & WS_VISIBLE)
		Invalidate();
	return 0;
}

LRESULT Popup::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// no flicker here.
	return 1;
}

void Popup::Redraw(ITile *pDraw)
{
	if (m_hWnd)
	{
#if 0
		// hack!
		// low tech alternate to the internal paint mechanism below. :^<
		Invalidate();
#else
		// save the item
		_internal.push_back(pDraw);
		// internal paint mode
		_bInternal = true;
		RECT update = {0};
		rect_t tile = {0};
		pDraw->getRect(tile);
		update.left = tile.left();
		update.top = tile.top();
		update.right = tile.right();
		update.bottom = tile.bottom();
		// invalidate something to make windows happy.
		InvalidateRect(&update, FALSE);
		// force paint to occur
		UpdateWindow();
		// return to normal paint mode.
		_bInternal = false;
#endif
	}
}

// mouse cursor
void Popup::setCursor(int cursor)
{
	cursor;
}

// IWindow
void Popup::setCapture(IControl *p, int cursor)
{
	p; // ignore - don't allow popups to mouse capture.
	cursor;
}

// alter focus.
void Popup::setFocus(bool bFocus)
{
	if (_pFocus)
		_pFocus->setFocus(bFocus);
}

// focus
void Popup::setFocus(IControl *pFocus)
{
	if (_pFocus != pFocus)
	{
		if (_pFocus != NULL)
			_pFocus->setFocus(false);
		_pFocus = pFocus;
		if (_pFocus != NULL)
			_pFocus->setFocus(true);
	}
}

// commit focus control edits.
void Popup::apply()
{
	if (_pFocus != NULL)
		_pFocus->apply();
}

// hover
void Popup::setHover(IControl *pHover)
{
	if (pHover != _pHover)
	{
		if (_pHover)
			_pHover->setHover(false);

		_pHover = pHover;

		if (_pHover)
			_pHover->setHover(false);
	}
}

Pane *Popup::getPane()
{
	return _pPane;
}

void Popup::setPane(Pane *pPane)
{
	if (_pPane != NULL)
	{
		_pFocus = NULL;
		_pCapture = NULL;
		delete _pPane;
	}
	_pPane = pPane;
	_pPane->setDesktop(this);
	_pPane->watch(this);
	if (m_hWnd)
	{
		RECT client = {0};
		GetClientRect(&client);
		rect_t rect = {0};
		rect.x = client.left;
		rect.y = client.top;
		rect.wide = client.right - client.left;
		rect.high = client.bottom - client.top;
		_pPane->setRect(rect);
		Invalidate();
	}
	_pCapture = _pPane;
}

// popup window
IWindow* Popup::popup(const rect_t &rect, Pane *pContent, IControl *pOwner)
{
	rect;		// ignore
	pContent;	// ignore
	pOwner;		// ignore
	return NULL;
}

// close and destroy the window
void Popup::close()
{
	if (m_hWnd)
	{
		DestroyWindow();
		m_hWnd = NULL;
	}
	delete this;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

// native mouse button event triggers framework event.
LRESULT Popup::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	setHover(NULL);
	return 0;
}

// 
LRESULT Popup::OnActivate(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	HWND other = reinterpret_cast<HWND>(lParam);
	if (wParam == WA_ACTIVE && other != NULL)
	{
		::SetActiveWindow(other);
	}
	return 0;
}

// 
LRESULT Popup::OnMouseActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return MA_NOACTIVATE;
}

// get window handle
Tiles::handle_t Popup::getHandle()
{
	return m_hWnd;
}
