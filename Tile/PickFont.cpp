#include "stdafx.h"
#include "PickFont.h"
#include "Edit.h"
#include "Button.h"
#include <atldlgs.h>

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

PickFont::PickFont(identity_t id, Theme &theme, const TCHAR *prompt, IAccessor<Font>* access) :
	Flow(id, theme, eRight),
	_access(access),
	_prompt(prompt)
{
	Theme::Font textFont = {Theme::eText, theme.Text};
	Theme::Font arrowFont = {Theme::eArrow, theme.Arrow};
	FlowDesc desc = {1, 1, 0, true};

	_edit = new Edit(0, theme, textFont, this);

	_button = new Button(0, theme, arrowFont, Theme::eDn, Theme::eDn, Theme::eDn);

	_button->Click.bind(this, &PickFont::click);
	//pButton->setReadOnly(true);

	Add(_edit, 0, 4096, 1, false);
	Add(_button, 1, 1, 0, true);
	_edit->setFlow(eDown, desc);
	_button->setFlow(eDown, desc);
	setFlow(eDown, desc);
	_thick.local = true;
	_thick.thick = 0;
}

void PickFont::click(Button*, bool down)
{
	// 1. uses the WTL wrappers to the Windows Color Dialog.
	// 2. uses WTL to grab the window handle of the current message to use as the parent window.
	// Both mean this code really belongs in the WTL Tile library, not the generic Tile library.
	if (!down)
	{
		HWND hwnd = static_cast<HWND>( _pDesktop->getHandle() );
		LOGFONT logFont = {0};
		Font font = _access->getValue();
		_tcsncpy_s(logFont.lfFaceName, _countof(logFont.lfFaceName), font._face.c_str(), font._face.size());
		logFont.lfWeight = FW_NORMAL;
		logFont.lfCharSet = (font._style & 1) ? SYMBOL_CHARSET : DEFAULT_CHARSET;
		logFont.lfHeight = font._height;

		CFontDialog dlg(&logFont);
		if ( dlg.DoModal(hwnd) == IDOK)
		{
			if (dlg.m_cf.lpLogFont)
			{
				logFont = *dlg.m_cf.lpLogFont;
				font._face = logFont.lfFaceName;
				font._height = logFont.lfHeight;

				if (font._height < 0)
					font._height = 0 - font._height;

				font._style = logFont.lfCharSet == SYMBOL_CHARSET ? 1 : 0;
				_access->setValue(font);
				_tile.getTheme().setHeight(font._height);
			}
		}
	}
}

const string_t &PickFont::getValue() const
{
	TCHAR work[16] = {0};
	const Font &font = _access->getValue();
	_text = font._face;
	wsprintf(work, _T(" %d"), font._height);
	_text.append(work);
	return _text;
}

bool PickFont::setValue(const string_t &value)
{
	value;
	// TODO!
	return false;
}

// Modified getMin above causes standard Flow::setRect to break so use
// this greatly simplified logic for layout of the text and drop arrow.
void PickFont::setRect(const rect_t &rect)
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
