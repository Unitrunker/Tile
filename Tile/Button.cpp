#include "stdafx.h"
#include "Button.h"
#include "Pane.h"
#include "ICanvas.h"
#include "JSON.h"
#include "Text.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Button::Button(identity_t id, Theme& theme, Theme::Font &font, const TCHAR *text) :
	Control(id, theme, font),
	_down(false),
	_textUp(text),
	_textDown(text),
	_textOver(text),
	_iGlyphUp(0), // unused.
	_iGlyphDn(0), // unused.
	_iGlyphOver(0), // unused.
	_popup(NULL)
{
	_colorUp[0]._index = Theme::eToolFore;
	_colorUp[0]._color = RGB(0, 0, 0);
	_colorUp[1]._index = Theme::eToolBack;
	_colorUp[1]._color = RGB(192, 192, 192);

	_colorDn[0]._index = Theme::eToolFore;
	_colorDn[0]._color = RGB(128, 128, 128);
	_colorDn[1]._index = Theme::eToolBack;
	_colorDn[1]._color = RGB(255, 255, 255);

	_colorOver[0]._index = Theme::eToolFore;
	_colorOver[0]._color = RGB(0, 0, 0);
	_colorOver[1]._index = Theme::eToolOver;
	_colorOver[1]._color = RGB(255, 255, 255);

	_colorFocus[0]._index = Theme::eToolFore;
	_colorFocus[0]._color = RGB(0, 0, 0);
	_colorFocus[1]._index = Theme::eToolOver;
	_colorFocus[1]._color = RGB(255, 255, 255);
}

Button::Button(identity_t id, Theme& theme, Theme::Font &font, const TCHAR *textUp, const TCHAR *textDown, const TCHAR *textOver) :
	Control(id, theme, font),
	_down(false),
	_textUp(textUp),
	_textDown(textDown),
	_textOver(textOver),
	_iGlyphUp(0), // unused.
	_iGlyphDn(0), // unused.
	_iGlyphOver(0), // unused.
	_popup(NULL)
{
	_colorUp[0]._index = Theme::eToolFore;
	_colorUp[0]._color = RGB(0, 0, 0);
	_colorUp[1]._index = Theme::eToolBack;
	_colorUp[1]._color = RGB(192, 192, 192);

	_colorDn[0]._index = Theme::eToolFore;
	_colorDn[0]._color = RGB(128, 128, 128);
	_colorDn[1]._index = Theme::eToolBack;
	_colorDn[1]._color = RGB(255, 255, 255);

	_colorOver[0]._index = Theme::eToolFore;
	_colorOver[0]._color = RGB(0, 0, 0);
	_colorOver[1]._index = Theme::eToolOver;
	_colorOver[1]._color = RGB(255, 255, 255);

	_colorFocus[0]._index = Theme::eToolFore;
	_colorFocus[0]._color = RGB(0, 0, 0);
	_colorFocus[1]._index = Theme::eToolOver;
	_colorFocus[1]._color = RGB(255, 255, 255);
}

Button::Button(identity_t id, Theme& theme, Theme::Font &font, unsigned char iUp, unsigned char iDn, unsigned char iOver) :
	Control(id, theme, font),
	_down(false),
	_textUp(_T("?")),
	_textDown(_T("?")),
	_textOver(_T("?")),
	_iGlyphUp(iUp),
	_iGlyphDn(iDn),
	_iGlyphOver(iOver),
	_popup(NULL)
{
	_colorUp[0]._index = Theme::eToolFore;
	_colorUp[0]._color = RGB(0, 0, 0);
	_colorUp[1]._index = Theme::eToolBack;
	_colorUp[1]._color = RGB(192, 192, 192);

	_colorDn[0]._index = Theme::eToolFore;
	_colorDn[0]._color = RGB(128, 128, 128);
	_colorDn[1]._index = Theme::eToolBack;
	_colorDn[1]._color = RGB(255, 255, 255);

	_colorOver[0]._index = Theme::eToolFore;
	_colorOver[0]._color = RGB(0, 0, 0);
	_colorOver[1]._index = Theme::eToolOver;
	_colorOver[1]._color = RGB(255, 255, 255);

	_colorFocus[0]._index = Theme::eToolFore;
	_colorFocus[0]._color = RGB(0, 0, 0);
	_colorFocus[1]._index = Theme::eToolOver;
	_colorFocus[1]._color = RGB(255, 255, 255);
}

Button::~Button()
{
	if (_popup)
		_popup->close();
	_popup = NULL;
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Button::Draw(ICanvas *canvas, bool bFocus)
{
	align_t align = eLeft|eRight|eUp|eDown;

	rect_t rect = _tile.rect();
	rect_t box = _tile.scrollBox();

	bool focus = bFocus && _focus;

	color_t fore = 0;
	color_t back = 0;

	if (_down)
	{
		fore = _tile.getColor(_colorDn[0]);
		back = _tile.getColor(_colorDn[1]);
	}
	else if (focus)
	{
		fore = _tile.getColor(_colorFocus[0]);
		back = _tile.getColor(_colorFocus[1]);
	}
	//else if (_hover)
	//{
	//	fore = _tile.getColor(_colorOver[0]);
	//	back = _tile.getColor(_colorOver[1]);
	//}
	else
	{
		fore = _tile.getColor(_colorUp[0]);
		back = _tile.getColor(_colorUp[1]);
	}
	const Font& font = _tile.getFont(_tile.getFont());
	// get glyph text and draw it.
	const string_t &glyph = text(focus);
#if 0
	color_t fill = RGB(192,192,192);
	rect_t frame = rect;
	frame.high = 2;
	canvas->FillRectangle(frame, fill);
	frame.y = rect.bottom() - 2;
	canvas->FillRectangle(frame, fill);
	frame = rect;
	frame.wide = 2;
	canvas->FillRectangle(frame, fill);
	frame.x = rect.right() - 2;
	canvas->FillRectangle(frame, fill);
	rect.wide -= 4;
	rect.x += 2;
	rect.y += 2;
	rect.high -= 4;
#endif
	canvas->DrawString(rect, box, fore, back, font, align, glyph);

	_tile.setChanged(false);
	return true;
}

// instance type
const char* Button::getType() const
{
	return Button::type();
}

const char* Button::type()
{
	return "Button";
}

// key event sink
bool Button::dispatch(KeyEvent &action)
{
	if ( getEnable() && !getReadOnly() && action._code == VK_SPACE )
	{
		switch (action._what)
		{
			case KeyEvent::DOWN:
				_down = true;
				setChanged(true);
				if (!State.empty())
					State(this, _down);
				return true;

			case KeyEvent::UP:
				_down = false;
				setChanged(true);
				if (!Click.empty())
					Click(this);
				if (!State.empty())
					State(this, _down);
				return true;

			default:
				break;
		}
	}
	return false;
}

// mouse event sink
bool Button::dispatch(MouseEvent &action)
{
	if (action._button == MouseEvent::eLeft)
	{
		switch (action._what)
		{
			case MouseEvent::eDownClick:
				if ( getEnable() && !getReadOnly() )
				{
					_down = true;
					setChanged(true);
					if (!State.empty())
						State(this, _down);
				}
				// Don't let tool buttons take input focus on mouse click.
				// Allows other controls retain focus while user performs tool related action.
				// Key navigation focus still possible.
				//if (!_focus)
				//	getContainer()->setFocus(this);
				return true;

			case MouseEvent::eUpClick:
				if ( getEnable() && !getReadOnly() )
				{
					_down = false;
					setChanged(true);
					if (!Click.empty())
						Click(this);
					if (!State.empty())
						State(this, _down);
				}
				return true;

			default:
				break;
		}
	}
	return Control::dispatch(action);
}

/// <param name="focus">true if this control has focus</param>
void Button::setFocus(bool focus)
{
	Control::setFocus(focus);
	if (!focus) _down = false;
}

const string_t &Button::text(bool focus)
{
	// select text.
	if (_down)
		return _tile.getTheme().getGlyph(_iGlyphDn, _textDown);

	return focus ? 
		_tile.getTheme().getGlyph(_iGlyphOver, _textOver) :
		_tile.getTheme().getGlyph(_iGlyphUp, _textUp);
}

bool Button::getTip(string_t &tip) const
{
	if (_tip.size() > 0)
	{
		tip = _tip;
		return true;
	}
	return false;
}

void Button::setTip(const TCHAR *tip)
{
	_tip = tip;
}

// hover
void Button::setHover(bool hover)
{
	Control::setHover(hover);
	if (hover && _tile.getTheme().Tips && _tip.size())
	{
		size_t size = (5 * _tip.size() + 7) / 8;
		Flow vert = {1, 1, 0, true};
		Flow horz = { size, size, 0, true };
		Theme::Font font = { Theme::eText, _tile.getTheme().Text };
		Pane *pane = new Pane(0, _tile.getTheme(), eDown);
		Text *text = new Text(0, _tile.getTheme(), font, eLeft, _tip.c_str());
		text->setFlow(eRight, horz);
		text->setFlow(eDown, vert);
		rect_t rect;
		_ASSERT(_popup == NULL);
		pane->Add(text);

		// TODO: better placement.
		rect.wide = size * _tile.getTheme().Text._height + 2;
		rect.high = _tile.getTheme().Text._height + 2;

		pane->setBorder(1);

		rect_t here;
		_tile.getRect(here);

		rect.x = here.left();
		rect.y = here.bottom();

		_popup = _pDesktop->popup(rect, pane, this);
	}
	else if (_popup)
	{
		_popup->close();
		_popup = NULL;
	}
}

void Button::setColorUp(const Theme::Color &fore, const Theme::Color &back)
{
	_colorUp[0] = fore;
	_colorUp[1] = back;
}

void Button::setColorDn(const Theme::Color &fore, const Theme::Color &back)
{
	_colorDn[0] = fore;
	_colorDn[1] = back;
}

void Button::setColorOver(const Theme::Color &fore, const Theme::Color &back)
{
	_colorOver[0] = fore;
	_colorOver[1] = back;
}

void Button::setColorFocus(const Theme::Color &fore, const Theme::Color &back)
{
	_colorFocus[0] = fore;
	_colorFocus[1] = back;
}

void Button::getColorUp(Theme::Color &fore, Theme::Color &back) const
{
	fore = _colorUp[0];
	back = _colorUp[1];
}

void Button::getColorDn(Theme::Color &fore, Theme::Color &back) const
{
	fore = _colorDn[0];
	back = _colorDn[1];
}

void Button::getColorOver(Theme::Color &fore, Theme::Color &back) const
{
	fore = _colorOver[0];
	back = _colorOver[1];
}

void Button::getColorFocus(Theme::Color &fore, Theme::Color &back) const
{
	fore = _colorFocus[0];
	back = _colorFocus[1];
}

const string_t &Button::getTextUp() const
{
	return _textUp;
}

const string_t &Button::getTextOver() const
{
	return _textOver;
}

const string_t &Button::getTextDn() const
{
	return _textDown;
}
