#include "stdafx.h"
#include "Window.h"
#include "Canvas.h"
#include "../Tile/Fill.h"

/*
Copyright 2011 Rick Parrish
*/

Window::Window(Theme& theme) :
	_theme(theme), // init theme before anything else :)
	_pCapture(NULL),
	_pFocus(NULL),
	_pFlow(NULL),
	_pHover(NULL),
	_bInternal(false),
	_pParent(NULL)
{
	Flow *pFlow = new Flow(0, _theme, eDown);
	pFlow->setWeight(eDown, 1);
	pFlow->watch(this);
	pFlow->setDesktop(this);

	Fill *pFill = new Fill(0, _theme);
	pFlow->Add(pFill, 0, 2048, 1);
	setFlow(pFlow);
}

Window::Window(Window *pParent, const rect_t &rect, Flow *pContent, IControl *pOwner) :
	_theme(pParent->_theme), // init theme before anything else :)
	_pCapture(pContent),
	_pFocus(pContent),
	_pHover(NULL),
	_pFlow(pContent),
	_bInternal(false),
	_pParent(pParent)
{
	rect; // TODO
	pOwner; // TODO
	pContent->watch(this);
	pContent->setDesktop(this);
}

Window::~Window()
{
	_pFlow->clear();
	delete _pFlow;
}

BEGIN_MESSAGE_MAP(Window, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL Window::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	HBRUSH hBrush = NULL; // we do NOT want a background brush

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), hBrush, NULL);

	return TRUE;
}

// Native paint event call root Flow's draw operation.
void Window::OnPaint()
{
	Canvas dc(this);
	if (_pFlow)
	{
		bool focus = ::GetFocus() == m_hWnd;
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
			_pFlow->Draw(&dc, focus);
	}
	_internal.clear();				
}

BOOL Window::OnEraseBkgnd(CDC*)
{
	// no flicker here.
	return 1;
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
void Window::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nFlags; // ignored
	nRepCnt; // ignored
	KeyEvent press = {0};
	press._what = KeyEvent::DOWN;
	press._mask = getKeyMask();
	press._code = nChar;
	if (_pFlow)
		_pFlow->dispatch(press);
}

// native char event triggers framework keypress.
void Window::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nChar; // ignored
	nFlags; // ignored
	nRepCnt; // ignored
}

// native keyup event triggers framework keypress.
void Window::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nFlags; // ignored
	nRepCnt; // ignored

	KeyEvent press = {0};
	press._what = KeyEvent::UP;
	press._mask = getKeyMask();
	press._code = nChar;
	if (_pFlow)
		_pFlow->dispatch(press);
}

// native mouse button event triggers framework event.
void Window::OnLButtonDown(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eLeft;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse button event triggers framework event.
void Window::OnLButtonUp(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eLeft;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse button event triggers framework event.
void Window::OnMButtonDown(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eMiddle;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse button event triggers framework event.
void Window::OnMButtonUp(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eMiddle;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse button event triggers framework event.
void Window::OnRButtonDown(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eRight;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse button event triggers framework event.
void Window::OnRButtonUp(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eUpClick;
	mouse._button = MouseEvent::eRight;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

// native mouse move event triggers framework event.
void Window::OnMouseMove(UINT, CPoint pt)
{
	MouseEvent mouse = {0};
	mouse._place.x = pt.x;
	mouse._place.y = pt.y;
	mouse._what = MouseEvent::eMove;
	if ( GetKeyState(VK_LBUTTON) < 0 )
		mouse._button += MouseEvent::eLeft;
	if ( GetKeyState(VK_RBUTTON) < 0 )
		mouse._button += MouseEvent::eRight;
	if ( GetKeyState(VK_MBUTTON) < 0 )
		mouse._button += MouseEvent::eMiddle;
	if (_pCapture)
		_pCapture->dispatch(mouse);
}

void Window::Layout(int wide, int high)
{
	rect_t rect = {0};
	rect.wide = wide;
	rect.high = high;
	if (_pFlow != NULL)
		_pFlow->setRect(rect);
}

void Window::OnSize(UINT code, int cx, int cy)
{
	if (code == SIZE_RESTORED || code == SIZE_MAXIMIZED)
	{
		Layout(cx, cy);
	}
}

int Window::OnCreate(LPCREATESTRUCT pcs)
{
	// TODO: get size from LPCREATESTRUCT
	Layout(pcs->cx, pcs->cy);
	Invalidate();
	return 0;
}

void Window::Redraw(ITile *pDraw)
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

// IWindow
void Window::setCapture(IControl *p)
{
	if (p == NULL)
	{
		_pCapture = _pFlow;
		ReleaseCapture();
	}
	else
	{
		_pCapture = p;
		SetCapture();
	}
}

// focus
void Window::setFocus(IControl *pFocus)
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

// hover
void Window::setHover(IControl *pHover)
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

Flow *Window::getFlow()
{
	return _pFlow;
}

void Window::setFlow(Flow *pFlow)
{
	if (_pFlow != NULL)
	{
		_pFocus = NULL;
		_pCapture = NULL;
		delete _pFlow;
	}
	_pFlow = pFlow;
	_pFlow->setDesktop(this);
	_pFlow->watch(this);
	if (m_hWnd)
	{
		RECT client = {0};
		GetClientRect(&client);
		rect_t rect = {0};
		rect.x = client.left;
		rect.y = client.top;
		rect.wide = client.right - client.left;
		rect.high = client.bottom - client.top;
		_pFlow->setRect(rect);
		Invalidate();
	}
	_pCapture = _pFlow;
}

// popup window
IWindow* Window::popup(const rect_t &rect, Flow *pContent, IControl *pOwner)
{
	Window *pPopup = new Window(this, rect, pContent, pOwner);
	RECT rc = {0};
	rc.left = rect.x;
	rc.top = rect.bottom();
	rc.right = rect.right();
	rc.bottom = rect.bottom() + 64;
	if ( pPopup->Create(NULL, NULL, WS_OVERLAPPED, rc, this, 0, NULL) )
	{
		pPopup->ShowWindow(SW_SHOW);
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
	DestroyWindow();
}

// get window handle
Tiles::handle_t Window::getHandle()
{
	return m_hWnd;
}

void Window::OnClose()
{
	// Important: need to revoke the focus'd control. Otherwise, when the window is shown again,
	// the control will be stuck in false focused state and tab navigation will skip over it.
	IControl *pNull = NULL;
	_pFlow->setFocus(pNull);
}
