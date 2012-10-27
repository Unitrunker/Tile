#include "stdafx.h"
#include "resource.h"

#include "AboutDlg.h"
#include "MainFrm.h"
#include "../Tile/Pane.h"
#include "../Tile/Fill.h"
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

	::ShowWindow(this->m_hWndStatusBar, SW_HIDE);
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
	// TODO: add code to initialize document

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

void CMainFrame::updateControlFonts()
{
	Theme::Font font;
	font.index = Theme::eText;
	font.font = m_view.getTheme().Text;
	for (size_t row = 0; row < MAG; row++)
	{
		for (size_t col = 0; col < MAG; col++)
		{
			m_controls[row][col]->setFont(font);
		}
	}
	m_view.Invalidate();
}

LRESULT CMainFrame::OnViewArial(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Theme &theme = m_view.getTheme();
	theme.Arrow._face = _T("Arial Unicode MS");
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnViewDejavu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Theme &theme = m_view.getTheme();
	theme.Arrow._face = _T("DejaVu Sans");
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnViewGothic(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Theme &theme = m_view.getTheme();
	theme.Arrow._face = _T("MS UI Gothic");
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnViewMincho(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Theme &theme = m_view.getTheme();
	theme.Arrow._face = _T("MS PMincho");
	m_view.Invalidate();
	return 0;
}

LRESULT CMainFrame::OnViewSegoe(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	Theme &theme = m_view.getTheme();
	theme.Arrow._face = _T("Segoe UI Symbol");
	m_view.Invalidate();
	return 0;
}

void CMainFrame::createForm()
{
	// apparently the Unicode byte markers are required.
	static const TCHAR kingW[] = {0xFEFF, 0x2654, 0};
	static const TCHAR queenW[] = {0xFEFF, 0x2655, 0};
	static const TCHAR rookW[] = {0xFEFF, 0x2656, 0};
	static const TCHAR bishopW[] = {0xFEFF, 0x2657, 0};
	static const TCHAR knightW[] = {0xFEFF, 0x2658, 0};
	static const TCHAR pawnW[] = {0xFEFF, 0x2659, 0};

	static const TCHAR kingB[] = {0xFEFF, 0x2654+6, 0};
	static const TCHAR queenB[] = {0xFEFF, 0x2655+6, 0};
	static const TCHAR rookB[] = {0xFEFF, 0x2656+6, 0};
	static const TCHAR bishopB[] = {0xFEFF, 0x2657+6, 0};
	static const TCHAR knightB[] = {0xFEFF, 0x2658+6, 0};
	static const TCHAR pawnB[] = {0xFEFF, 0x2659+6, 0};

	Theme &theme = m_view.getTheme();
	//theme.Text._face = _T("Segoe UI Symbol");
	theme.Arrow._face = _T("DejaVu Sans");
	theme.setHeight(48);
	theme.Arrow._style = 0; // not symbol (oddly enough)

	Pane *pBox = new Pane(0, theme, eDown);
	pBox->setSpace(0);

	Flow rowDesc = {0, 4096, 0};
	Flow fillDesc = {0, 4096, 1};

	Fill *pFill = NULL;

	pFill = new Fill(0, theme);
	pBox->Add(pFill, 0, 4096, 1);
	pFill->setFlow(eRight, fillDesc);

	Pane *pCenter = new Pane(0, theme, eRight);
	pCenter->setSpace(0);
	pFill = new Fill(0, theme);
	pCenter->Add(pFill, 0, 4096, 1);
	pFill->setFlow(eDown, fillDesc);

	Pane *pPane = new Pane(0, theme, eDown);
	pPane->setSpace(0);
	for (size_t row = 0; row < MAG; row++)
	{
		Pane *pRow = new Pane(0, theme, eRight);
		pRow->setSpace(0);
		pRow->setFlow(eRight, rowDesc);
		for (size_t col = 0; col < MAG; col++)
		{
			m_map[row][col] = true;
			Check *pControl = new Check(0, theme, m_map[row][col], eLeft|eRight);
			//pControl->setAccessor(&m_map[row][col]);
			//pControl->setFont(theme.Text);
			m_controls[row][col] = pControl;
			pRow->Add(pControl, 1, 1, 0, true);
		}
		pPane->Add(pRow, 1, 1, 0, true);
	}

	pCenter->Add(pPane, 0, 4096, 0);
	pPane->setFlow(eDown, rowDesc);
	pPane->setFlow(eRight, rowDesc);
	pFill = new Fill(0, theme);
	pFill->setFlow(eDown, fillDesc);
	pFill->setFlow(eRight, fillDesc);
	pCenter->Add(pFill, 0, 4096, 1);
	pCenter->setFlow(eDown, rowDesc);
	pCenter->setFlow(eRight, rowDesc);

	pBox->Add(pCenter, 1, 1, 0, true);

	pFill = new Fill(0, theme);
	pFill->setFlow(eDown, fillDesc);
	pBox->Add(pFill, 0, 4096, 1);

	//TCHAR fake[] = {0xFEFF, 0x2650, 0};

	// outline
	m_controls[0][0]->setGlyphs(rookW, _T(" "));
	m_controls[0][1]->setGlyphs(knightW, _T(" "));
	m_controls[0][2]->setGlyphs(bishopW, _T(" "));
	m_controls[0][3]->setGlyphs(queenW, _T(" "));
	m_controls[0][4]->setGlyphs(kingW, _T(" "));
	m_controls[0][5]->setGlyphs(bishopW, _T(" "));
	m_controls[0][6]->setGlyphs(knightW, _T(" "));
	m_controls[0][7]->setGlyphs(rookW, _T(" "));

	// solid
	m_controls[7][0]->setGlyphs(rookB, _T(" "));
	m_controls[7][1]->setGlyphs(knightB, _T(" "));
	m_controls[7][2]->setGlyphs(bishopB, _T(" "));
	m_controls[7][3]->setGlyphs(kingB, _T(" "));
	m_controls[7][4]->setGlyphs(queenB, _T(" "));
	m_controls[7][5]->setGlyphs(bishopB, _T(" "));
	m_controls[7][6]->setGlyphs(knightB, _T(" "));
	m_controls[7][7]->setGlyphs(rookB, _T(" "));

	for (size_t i = 0; i < MAG; i++)
	{
		m_controls[1][i]->setGlyphs(pawnW, _T(" "));
		m_controls[2][i]->setGlyphs(_T(" "), pawnW);
		m_controls[3][i]->setGlyphs(_T(" "), pawnW);
		m_controls[4][i]->setGlyphs(_T(" "), pawnB);
		m_controls[5][i]->setGlyphs(_T(" "), pawnB);
		m_controls[6][i]->setGlyphs(pawnB, _T(" "));
	}

	Theme::Color focus = {Theme::eDefault, RGB(255, 0, 0)};
	Theme::Color normal = {Theme::eDefault, RGB(192, 0, 0)};

	for (size_t row = 0; row < MAG; row++)
	{
		for (size_t col = 0; col < MAG; col++)
		{
			if ( (row ^ col) & 1 )
				m_controls[row][col]->setBack(normal, focus);
		}
	}
	m_view.setPane(pBox);
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
