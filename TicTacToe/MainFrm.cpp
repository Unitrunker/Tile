#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
#include "MainFrm.h"
#include "../Tile/Fill.h"
#include "../Tile/Pane.h"
#include "../Tile/Check.h"

CMainFrame::CMainFrame() : m_view(_theme)
{
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	m_CmdBar.AttachMenu(GetMenu());
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);

	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

	CreateSimpleStatusBar();

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 0);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	createForm();

	::ShowWindow(m_hWndStatusBar, SW_HIDE);
	OnViewToolBar(0, 0, m_hWnd, bHandled);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}

LRESULT CMainFrame::OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	reset(true);
	return 0;
}

LRESULT CMainFrame::OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static BOOL bVisible = TRUE;	// initially visible
	bVisible = !bVisible;
	CReBarCtrl rebar = m_hWndToolBar;
	int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
	rebar.ShowBand(nBandIndex, bVisible);
	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
	UpdateLayout();
	return 0;
}

LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

void CMainFrame::createForm()
{
	m_bTurn = true;

	for (size_t row = 0; row < 3; row++)
		for (size_t col = 0; col < 3; col++)
			m_map[row][col].setFrame(this);

	Theme &theme = m_view.getTheme();
	theme.setHeight(144);

	Pane *pBox = new Pane(0, theme, eDown);
	pBox->setSpace(0);

	Fill *pFill = NULL;

	pFill = new Fill(0, theme);
	pBox->Add(pFill, 0, 4096, 1);

	Pane *pCenter = new Pane(0, theme, eRight);
	pCenter->setSpace(0);
	pFill = new Fill(0, theme);
	pCenter->Add(pFill, 0, 4096, 1);

	Pane *pPane = new Pane(0, theme, eDown);
	for (size_t row = 0; row < 3; row++)
	{
		Pane *pRow = new Pane(0, theme, eRight);
		for (size_t col = 0; col < 3; col++)
		{
			Check *pControl = new Check(0, theme, &m_map[row][col], eLeft|eRight);
			pControl->setAccessor(&m_map[row][col]);
			//pControl->setFont(theme.Text);
			m_controls[row][col] = pControl;
			pRow->Add(pControl, 1, 1, 0, true);
		}
		pRow->setSpace(8);
		pPane->Add(pRow, 1, 1, 0, true);
	}
	pPane->setSpace(8);

	pCenter->Add(pPane, 0, 4096, 0);
	pFill = new Fill(0, theme);
	pFill->setWeight(eDown, 1);
	pCenter->Add(pFill, 0, 4096, 1);

	pBox->Add(pCenter, 1, 1, 0, true);

	pFill = new Fill(0, theme);
	pFill->setWeight(eDown, 1);
	pBox->Add(pFill, 0, 4096, 1);

	m_view.setPane(pBox);
	cycle();
}

void CMainFrame::cycle()
{
	for (size_t row = 0; row < 3; row++)
	{
		for (size_t col = 0; col < 3; col++)
		{
			Check *pControl = m_controls[row][col];
			bool checked = m_map[row][col].getValue();
			if (checked)
				pControl->setReadOnly(true);
			else
				pControl->setGlyphs(m_bTurn ? _T("X") : _T("O"), _T(" ") );
		}
	}
	m_bTurn = !m_bTurn;
}

void CMainFrame::reset(bool turn)
{
	m_bTurn = turn;
	bool wipe = false;
	for (size_t row = 0; row < 3; row++)
	{
		for (size_t col = 0; col < 3; col++)
		{
			Check *pControl = m_controls[row][col];
			pControl->setGlyphs(m_bTurn ? _T("X") : _T("O"), _T(" ") );
			pControl->setReadOnly(false);
			m_map[row][col].setValue(wipe);
			pControl->setChanged(true);
		}
	}
}

LRESULT CMainFrame::OnSizing(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	enum {eLeft = 1, eTop = 2, eRight = 4, eBottom = 8};
	RECT *p = reinterpret_cast<RECT *>(lParam);
	SIZE sz = {p->right - p->left, p->bottom - p->top};
	char mask = 0;

	switch (wParam)
	{
		case WMSZ_TOP:
			mask |= eTop;
			break;

		case WMSZ_BOTTOM:
			mask |= eBottom;
			break;

		case WMSZ_LEFT:
			mask |= eLeft;
			break;

		case WMSZ_RIGHT:
			mask |= eRight;
			break;

		case WMSZ_TOPLEFT:
			mask |= eTop|eLeft;
			break;

		case WMSZ_TOPRIGHT:
			mask |= eTop|eRight;
			break;

		case WMSZ_BOTTOMLEFT:
			mask |= eBottom|eLeft;
			break;

		case WMSZ_BOTTOMRIGHT:
			mask |= eBottom|eRight;
			break;

		default:
			break;
	}

	if (mask & eTop)
		if (sz.cy > m_max.cy)
			p->bottom = p->top + m_max.cy;
		else if (sz.cy < m_min.cy)
			p->bottom = p->top + m_min.cy;

	if (mask & eBottom)
		if (sz.cy > m_max.cy)
			p->top = p->bottom - m_max.cy;
		else if (sz.cy < m_min.cy)
			p->top = p->bottom - m_min.cy;

	if (mask & eLeft)
		if (sz.cx > m_max.cx)
			p->right = p->left + m_max.cx;
		else if (sz.cx < m_min.cx)
			p->right = p->left + m_min.cx;

	if (mask & eRight)
		if (sz.cx > m_max.cx)
			p->left = p->right - m_max.cx;
		else if (sz.cx < m_min.cx)
			p->left = p->right - m_min.cx;

	return TRUE;
}
