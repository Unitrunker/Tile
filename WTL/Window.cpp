#include "stdafx.h"
#include "Window.h"
#include "Canvas.h"
#include "../Tile/Fill.h"
#include "Popup.h"

Window::Window(Theme &theme, bool bSelfDelete) :
	_theme(theme),
	_pCapture(NULL),
	_pFocus(NULL),
	_pHover(NULL),
	_pPane(NULL),
	_bInternal(false),
	_bBatch(true),
	_bSelfDelete(bSelfDelete),
	_themeChange(this, &Window::onChange)
{
	Pane *pPane = new Pane(0, _theme, eDown);
	pPane->setWeight(eDown, 1);
	pPane->watch(this);
	pPane->setDesktop(this);

	_theme.Change -= _themeChange;
	_theme.Change += _themeChange;

	Fill *pFill = new Fill(0, _theme);
	pPane->Add(pFill, 0, 2048, 1);
	setPane(pPane);
}

Window::Window(Window *pParent, const rect_t &rect, Pane *pContent, IControl *pOwner) :
	_theme(pParent->getTheme()),
	_pCapture(pContent),
	_pFocus(pContent),
	_pHover(NULL),
	_pPane(pContent),
	_bInternal(false),
	_bBatch(false),
	_bSelfDelete(false)
{
	pContent->watch(this);
	pContent->setDesktop(this);
	_theme.Change -= _themeChange;
	_theme.Change += _themeChange;
	// TODO: need these?
	rect; // ??
	pOwner; // ??
}

Window::~Window()
{
	_theme.Change -= _themeChange;
	_pFocus = NULL;
	delete _pPane;
}

ATL::CWndClassInfo& Window::GetWndClassInfo()
{
	static ATL::CWndClassInfo wc =
	{
		{ sizeof(WNDCLASSEX), CS_DBLCLKS, StartWindowProc,
		  0, 0, NULL, NULL, NULL, NULL, NULL, _T("TILE"), NULL },
		NULL, NULL, IDC_ARROW, TRUE, 0, _T("")
	};
	return wc;
}

BOOL Window::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

// Native paint event call root Flow's draw operation.
LRESULT Window::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	Canvas dc(m_hWnd);
	if (_pPane)
	{
		bool focus = GetFocus() == m_hWnd;
#if 0
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
#endif
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
LRESULT Window::OnKeyDown(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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
LRESULT Window::OnChar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = true;
	return 0;
}

// native keyup event triggers framework keypress.
LRESULT Window::OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
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
LRESULT Window::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
LRESULT Window::OnLButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
LRESULT Window::OnLButtonDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDoubleClick;
	mouse._button = MouseEvent::eLeft;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Window::OnMButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
LRESULT Window::OnMButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
LRESULT Window::OnMButtonDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDoubleClick;
	mouse._button = MouseEvent::eMiddle;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse button event triggers framework event.
LRESULT Window::OnRButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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
LRESULT Window::OnRButtonUp(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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

// native mouse button event triggers framework event.
LRESULT Window::OnRButtonDblClick(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
	MouseEvent mouse = {0};
	mouse._place.x = GET_X_LPARAM(lParam);
	mouse._place.y = GET_Y_LPARAM(lParam);
	mouse._what = MouseEvent::eDoubleClick;
	mouse._button = MouseEvent::eRight;
	if (_pCapture)
		bHandled = _pCapture->dispatch(mouse);
	return 0;
}

// native mouse move event triggers framework event.
LRESULT Window::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
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

void Window::Layout(int wide, int high)
{
	rect_t rect = {0};
	rect.wide = wide;
	rect.high = high;
	if (_pPane != NULL)
		_pPane->setRect(rect);
}

LRESULT Window::OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
	{
		Layout( GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) );
	}
	return 0;
}

LRESULT Window::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	CREATESTRUCT *p = reinterpret_cast<CREATESTRUCT*>(lParam);
	// not quite right - window may have a border so client area will be smaller.
	Layout(p->cx, p->cy);
	Invalidate();
	return 0;
}

LRESULT Window::OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// no flicker here.
	return 1;
}

void Window::Redraw(ITile *pDraw)
{
	if (m_hWnd)
	{
		rect_t tile = {0};
		pDraw->getRect(tile);
		if (tile.wide > 0 && tile.high > 0)
		{
			RECT update = {0};
			update.left = tile.left();
			update.top = tile.top();
			update.right = tile.right();
			update.bottom = tile.bottom();
			InvalidateRect(&update, FALSE);
			// using normal batched painting?
			if (!_bBatch)
			{
				// no: save the item
				_internal.push_back(pDraw);
				// internal paint mode
				_bInternal = true;
				// force paint to occur
				UpdateWindow();
				// return to normal paint mode.
				_bInternal = false;
			}
		}
	}
}

// IWindow

// mouse cursor
void Window::setCursor(int cursor)
{
	HCURSOR hCursor = LoadCursor(NULL, MAKEINTRESOURCE(cursor));
	SetCursor(hCursor);
}

// p=null to release mouse capture
// p=control to accept captured mouse events
void Window::setCapture(IControl *p, int cursor)
{
	if (p == NULL)
	{
		HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
		SetCursor(hCursor);
		_pCapture = _pPane;
		ReleaseCapture();
	}
	else
	{
		_pCapture = p;
		SetCapture();
		HCURSOR hCursor = LoadCursor(NULL, MAKEINTRESOURCE(cursor));
		SetCursor(hCursor);
	}
}

// alter focus.
void Window::setFocus(bool bFocus)
{
	if (_pFocus)
		_pFocus->setFocus(bFocus);
}

// focus
void Window::setFocus(IControl *pFocus)
{
	if (_pFocus != pFocus)
	{
		//IRedraw *self = this;
		if (_pFocus != NULL)
			_pFocus->setFocus(false);
		_pFocus = pFocus;
		if (_pFocus != NULL)
			_pFocus->setFocus(true);
	}
}

void Window::apply()
{
	if (_pFocus != NULL)
	{
		_pFocus->apply();
	}
}

// hover
void Window::setHover(IControl *pHover)
{
	if (pHover != _pHover)
	{
		if (_pHover)
			_pHover->setHover(false);

		_pHover = pHover;

		if (_pHover)
			_pHover->setHover(true);
	}
}

Pane *Window::getPane()
{
	return _pPane;
}

void Window::setPane(Pane *pPane)
{
	if (_pPane != NULL)
	{
		_pFocus = NULL;
		_pCapture = NULL;
		_pHover = NULL;
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
IWindow* Window::popup(const rect_t &rect, Pane *pContent, IControl *pOwner)
{
	Popup *pPopup = new Popup(_theme, rect, pContent, pOwner);
	RECT rc = {0};
	rc.left = rect.x;
	rc.top = rect.top();
	rc.right = rect.right();
	rc.bottom = rect.bottom();
	ClientToScreen(&rc);
	if (pPopup->Create(m_hWnd, rc, NULL, WS_POPUP, 0, (HMENU)0, NULL) != NULL)
	{
		pPopup->ShowWindow(SW_SHOWNA);
	}
	else
	{
		delete pPopup;
		pPopup = NULL;
	}
	return pPopup;
}

// close and destroy the window
void Window::close()
{
	if (m_hWnd)
		DestroyWindow();
}

// native mouse button event triggers framework event.
LRESULT Window::OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 1;
}

// native mouse button event triggers framework event.
LRESULT Window::OnKillFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	setHover(NULL);
	return 0;
}

void Window::onChange(const Theme *)
{
	if (_pPane && IsWindow())
	{
		Invalidate();
		_pPane->reflow();
	}
}

// get window handle
Tiles::handle_t Window::getHandle()
{
	return m_hWnd;
}

LRESULT Window::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// Important: need to revoke the focus'd control. Otherwise, when the window is shown again,
	// the control will be stuck in false focused state and tab navigation will skip over it.
	IControl *pNull = NULL;
	_pPane->setFocus(pNull);
	bHandled = false;
	return 0;
}

LRESULT Window::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled = false;
	// WTL hack:
	// CWindowImpl<T> complains Window object destroyed before HWND.
	// We've received the WM_DESTROY message so clearly it was. CWindowImpl<T> clears 
	// m_hWnd on WM_NCDESTROY. Works great for frame windows. This window won't get 
	// a WM_NCDESTORY if it's a child window.
	// Fix: clear the m_hWnd here IF it is a child window.
	if (GetStyle() & WS_CHILD)
		m_hWnd = NULL;
	return 0;
}

void Window::OnFinalMessage(HWND /*hWnd*/)
{
	if (_bSelfDelete)
		delete this;
}
