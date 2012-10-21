#include "stdafx.h"
#include "MainFrame.h"
#include "../Tile/List.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Edit.h"
#include "../WTL/Snapshot.h"

MainFrame::MainFrame(Theme &theme) : 
	Frame(theme), m_editor(theme), m_about(theme), m_set(theme)
{
	// create a form with a toolbar and a list control.
	Flow *pFlow = new Flow(0, theme, eDown);
	Flow *pRow = NULL;
	Button *pButton = NULL;

	theme.setHeight(48);
	theme.GridThick = 8;

#if 0
	Theme::Font font = {Theme::eDefault, Font(_T("MS Reference Sans Serif"), 48, 0)};
#else
	Theme::Font font = {Theme::eDefault, Font(_T("OpenSymbol"), 48, 0)};
#endif
	static const TCHAR plus[] = {0xE109, 0};
	static const TCHAR minus[] = {0xE10B, 0};
	static const TCHAR times[] = {0xE129, 0};
	static const TCHAR divide[] = {0xE12D, 0};
	static const TCHAR clear[] = {0xE14F, 0};
	static const TCHAR zero[] = {0x30, 0};
	static const TCHAR one[] = {0x31, 0};
	static const TCHAR two[] = {0x32, 0};
	static const TCHAR three[] = {0x33, 0};
	static const TCHAR four[] = {0x34, 0};
	static const TCHAR five[] = {0x35, 0};
	static const TCHAR six[] = {0x36, 0};
	static const TCHAR seven[] = {0x37, 0};
	static const TCHAR eight[] = {0x38, 0};
	static const TCHAR nine[] = {0x39, 0};
	static const TCHAR about[] = {0xE020, 0};
	static const TCHAR equals[] = {0xE111, 0};
	static const TCHAR negate[] = {0xE127, 0};
	static const TCHAR exit[] = {0xE20C, 0};

	static const TCHAR* digits[] = {zero, one, two, three, four, five, six, seven, eight, nine};

	// construct calculator keypad
	Edit *pEdit = new Edit(0, theme, font, &m_value);
	pFlow->Add(pEdit, 1, 1, 0, true);

	pRow = new Flow(0, theme, eRight);
	pButton = new Button(0, theme, font, about);
	pButton->Click.bind(this, &MainFrame::clickAbout);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, negate);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, equals);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, exit);
	pRow->Add(pButton, 0, 4096, 1);
	pFlow->Add(pRow, 0, 4096, 1);

	pRow = new Flow(0, theme, eRight);
	pButton = new Button(0, theme, font, digits[7]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[8]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[9]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, divide);
	pRow->Add(pButton, 0, 4096, 1);
	pFlow->Add(pRow, 0, 4096, 1);

	pRow = new Flow(0, theme, eRight);
	pButton = new Button(0, theme, font, digits[4]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[5]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[6]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, times); // 0x2716
	pRow->Add(pButton, 0, 4096, 1);
	pFlow->Add(pRow, 0, 4096, 1);

	pRow = new Flow(0, theme, eRight);
	pButton = new Button(0, theme, font, digits[1]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[2]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[3]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, minus);
	pRow->Add(pButton, 0, 4096, 1);
	pFlow->Add(pRow, 0, 4096, 1);

	pRow = new Flow(0, theme, eRight);
	pButton = new Button(0, theme, font, clear);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, digits[0]);
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, _T("."));
	pRow->Add(pButton, 0, 4096, 1);
	pButton = new Button(0, theme, font, plus); // 0x271A
	pRow->Add(pButton, 0, 4096, 1);
	pFlow->Add(pRow, 0, 4096, 1);

	pFlow->setBorder(8);

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
