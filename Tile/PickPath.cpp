#include "stdafx.h"
#include "PickPath.h"
#include <atldlgs.h>

/*
Copyright © 2011-2012 Rick Parrish
*/

using namespace Tiles;

static const Theme::Font placebo = {0};

static const TCHAR text[] = {0xFEFF, 0x2B5C, 0};

PickPath::PickPath(identity_t id, Theme &theme, const TCHAR *prompt, IAccessor<string_t>* access) :
	Pane(id, theme, eRight),
	_access(access),
	_prompt(prompt),
	_edit(NULL),
	_button(NULL)
{
	static Theme::Font textFont = {Theme::eText, theme.Text};
	static Theme::Font arrowFont = {Theme::eArrow, theme.Arrow};

	_edit = new Edit(0, theme, textFont, _access);
	_button = new Button(0, theme, arrowFont, text, text, text);

	_button->Click.bind(this, &PickPath::click);

	Add(_edit, 0, 4096, 1, false);
	Add(_button, 1, 1, 0, true);
	_thick.local = true;
	_thick.thick = 0;
}

PickPath::~PickPath()
{
}

void PickPath::click(Button*, bool down)
{
	// 1. this code uses the WTL wrappers to the Windows Color Dialog.
	// 2. this code uses WTL to grab the window handle of the current message to use as the parent window.
	// Both mean this code really belongs in the WTL Tile library, not the generic Tile library.
	if (!down)
	{
		CFolderDialog dlg(NULL, _prompt, BIF_RETURNONLYFSDIRS);

		memset(dlg.m_szFolderPath, 0, sizeof dlg.m_szFolderPath);

		const string_t &path = _access->getValue();

		GetFullPathName(path.c_str(), _countof(dlg.m_szFolderPath), dlg.m_szFolderPath, NULL);

		string_t work = dlg.m_szFolderPath;

		dlg.m_lpstrInitialFolder = work.c_str();

		dlg.m_bExpandInitialSelection = false;
		_tcsncpy_s(dlg.m_szFolderPath, MAX_PATH, work.c_str(), work.size());

		HWND hwnd = static_cast<HWND>( _pDesktop->getHandle() );
		if (dlg.DoModal(hwnd) == IDOK)
		{
			_access->setValue(dlg.m_szFolderPath);
		}
	}
}

// Modified getMin above causes standard Pane::setRect to break so use
// this greatly simplified logic for layout of the text and drop arrow.
void PickPath::setRect(const rect_t &rect)
{
	rect_t arrow = rect;
	rect_t text = rect;
	_tile.setRect(rect);
	_button->getMin(eRight, arrow.wide);
	text.wide -= arrow.wide;
	arrow.x += text.wide;
	text.wide -= _tile.getThick(_space);
	_edit->setRect(text);
	_button->setRect(arrow);
}


void PickPath::setReadOnly(bool bSet)
{
	Pane::setReadOnly(bSet);
	_edit->setReadOnly(_readOnly);
	_button->setReadOnly(_readOnly);
}