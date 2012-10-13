#include "stdafx.h"
#include "MainFrame.h"
#include "../Tile/List.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Gauge.h"
#include "../WTL/Snapshot.h"
#include <time.h>

MainFrame::MainFrame(Theme &theme) : 
	Frame(theme), m_editor(theme), m_set(theme), m_about(theme), m_adapter(theme)
{
	srand( clock() );
	// create a form with a toolbar and a list control.
	Flow *pFlow = new Flow(0, theme, eDown);
	Flow *pTools = new Flow(0, theme, eRight);
	List *pList = new List(0, theme);

	const TCHAR one[] = {0x203B, 0};
	const TCHAR two[] = {0x2298, 0};
	const TCHAR three[] = {0x2339, 0};
	const TCHAR five[] = {0x2702, 0};
	const TCHAR six[] = {0x2709, 0};
	const TCHAR seven[] = {0x2714, 0};
	const TCHAR eight[] = {0x2716, 0};

	const TCHAR up[] = {0x2639, 0};
	const TCHAR dn[] = {0x263A, 0};
	const TCHAR over[] = {0x263B, 0};

	Theme::Font font = {Theme::eDefault, theme.Arrow};
	font.font._height *= 2;

	Button *pButton = NULL;

	pFlow->setBorder(2);

	// construct toolbar.
	Fill *fill = NULL;
	FlowDesc tool = {0, 4096, 1, false};

	fill = new Fill(0, theme);
	fill->setFlow(eDown, tool);
	pTools->Add(fill, 0, 4096, 1);
	pButton = new Button(0, theme, font, one);
	pButton->Click.bind(this, &MainFrame::clickSnapshot);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, two);
	pButton->Click.bind(this, &MainFrame::clickAbout);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, three);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, up, dn, over);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, five);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, six);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, seven);
	pButton->Click.bind(this, &MainFrame::clickTick);
	pTools->Add(pButton, 1, 1, 0, true);
	pButton = new Button(0, theme, font, eight);
	pButton->Click.bind(this, &MainFrame::clickTheme);
	pTools->Add(pButton, 1, 1, 0, true );

	m_gauge = new Gauge(123, theme, Tiles::eRight);
	pTools->Add(m_gauge, 1, 1, 0, true);
	m_gauge->setValue(12);

	fill = new Fill(0, theme);
	fill->setFlow(eDown, tool);
	pTools->Add(fill, 0, 4096, 1);

	FlowDesc desc = {1, 1, 0, true};
	pTools->setFlow(eDown, desc);
	pTools->setFont(font);
	// add toolbar to form
	pFlow->Add(pTools, true);
	// add list control to form
	pFlow->Add(pList, 0, 4096, 1);
	// connect adapter to the data.
	m_adapter.setValue(&m_person);
	// populate list control.
	pList->setItems(&m_adapter);
	// connect the form to the frame window.
	setFlow(pFlow);
};

// user clicked the theme editor button
void MainFrame::clickTheme(Button *, bool bDown)
{
	if (!bDown)
	{
		if (m_editor.IsWindow())
		{
			m_editor.CenterWindow(m_hWnd);
			m_editor.ShowWindow(SW_SHOW);
		}
		else
		{
			RECT rect = {0, 0, 320, 480};
			List *pList = new List(0, getTheme());
			pList->setItems(&m_set);
			m_editor.setFlow(pList);
			m_editor.Create(m_hWnd, rect, _T("Theme Editor"), WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW);
			m_editor.CenterWindow(m_hWnd);
			m_editor.ShowWindow(SW_SHOW);
		}
	}
}

// user clicked the About botton.
void MainFrame::clickAbout(Button *, bool bDown)
{
	if (!bDown)
	{
		if (m_about.IsWindow())
		{
			m_about.CenterWindow(m_hWnd);
			m_about.ShowWindow(SW_SHOW);
		}
		else
		{
			Theme& theme = getTheme();
			Theme::Font textFont = {Theme::eText, theme.Text};
			Theme::Font dingFont = {Theme::eDefault, Font(_T("Wing dings"), theme.Text._height, 0)};
			RECT rect = {0, 0, theme.Text._height*24, theme.Text._height * 3};
			Fill* pFill = NULL;
			Flow *pFlow = new Flow(0, theme, eDown);
			Flow *pLine = new Flow(0, theme, eRight);
			pLine->setSpace(0);
			static const TCHAR frill1[] = {0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0};
			static const TCHAR frill2[] = {0x97, 0x97, 0x97, 0x97, 0x97, 0x97, 0};
			Text *pLeft = new Text(0, theme, dingFont, eLeft, frill1);
			Text *pText = new Text(0, theme, textFont, eLeft|eRight, _T("Copyright 2011"));
			Text *pRight = new Text(0, theme, dingFont, eRight, frill2);
			pLine->Add(pLeft, 0, 4096, 2);
			pLine->Add(pText, 0, 4096, 3);
			pLine->Add(pRight, 0, 4096, 2);
			Button *pOK = new Button(0, theme, textFont, _T("OK"));
			pOK->Click.bind(this, &MainFrame::clickOK);

			pFill = new Fill(0, theme);
			pFlow->Add(pFill, 0, 4096, 1, false);
			pFlow->Add(pLine, 1, 1, 0, true);
			pFlow->Add(pOK, 1, 1, 0, true);
			pFill = new Fill(0, theme);
			pFlow->Add(pFill, 0, 4096, 1, false);
			m_about.setFlow(pFlow);
			m_about.Create(m_hWnd, rect, _T("About this sample app"), WS_POPUP|WS_BORDER);
			m_about.CenterWindow(m_hWnd);
			m_about.ShowWindow(SW_SHOW);
		}
	}
}

void MainFrame::clickOK(Button *, bool)
{
	m_about.PostMessage(WM_CLOSE);
}

void MainFrame::clickSnapshot(Button *, bool)
{
	Snapshot camera(m_hWnd);
	getFlow()->Draw(&camera, true);
	camera.Save(_T(".\\Picture.bmp"));
}

// user clicked check
void MainFrame::clickTick(Button *, bool bDown)
{
	if (bDown)
	{
		rect_t rect;
		m_gauge->getRect(rect);
		m_gauge->setValue( (rand() * rect.wide) / 32768 );
	}
}
