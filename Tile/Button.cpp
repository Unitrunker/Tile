#include "stdafx.h"
#include "Button.h"
#include "Flow.h"
#include "ICanvas.h"
#include "../JSON/Writer.h"

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
	_iGlyphOver(0) // unused.
{
	_colorUp[0].index = Theme::eToolFore;
	_colorUp[0].color = RGB(0, 0, 0);
	_colorUp[1].index = Theme::eToolBack;
	_colorUp[1].color = RGB(192, 192, 192);

	_colorDn[0].index = Theme::eToolFore;
	_colorDn[0].color = RGB(128, 128, 128);
	_colorDn[1].index = Theme::eToolBack;
	_colorDn[1].color = RGB(255, 255, 255);

	_colorOver[0].index = Theme::eToolFore;
	_colorOver[0].color = RGB(0, 0, 0);
	_colorOver[1].index = Theme::eToolOver;
	_colorOver[1].color = RGB(255, 255, 255);

	_colorFocus[0].index = Theme::eToolFore;
	_colorFocus[0].color = RGB(0, 0, 0);
	_colorFocus[1].index = Theme::eToolOver;
	_colorFocus[1].color = RGB(255, 255, 255);
}

Button::Button(identity_t id, Theme& theme, Theme::Font &font, const TCHAR *textUp, const TCHAR *textDown, const TCHAR *textOver) :
	Control(id, theme, font),
	_down(false),
	_textUp(textUp),
	_textDown(textDown),
	_textOver(textOver),
	_iGlyphUp(0), // unused.
	_iGlyphDn(0), // unused.
	_iGlyphOver(0) // unused.
{
	_colorUp[0].index = Theme::eToolFore;
	_colorUp[0].color = RGB(0, 0, 0);
	_colorUp[1].index = Theme::eToolBack;
	_colorUp[1].color = RGB(192, 192, 192);

	_colorDn[0].index = Theme::eToolFore;
	_colorDn[0].color = RGB(128, 128, 128);
	_colorDn[1].index = Theme::eToolBack;
	_colorDn[1].color = RGB(255, 255, 255);

	_colorOver[0].index = Theme::eToolFore;
	_colorOver[0].color = RGB(0, 0, 0);
	_colorOver[1].index = Theme::eToolOver;
	_colorOver[1].color = RGB(255, 255, 255);

	_colorFocus[0].index = Theme::eToolFore;
	_colorFocus[0].color = RGB(0, 0, 0);
	_colorFocus[1].index = Theme::eToolOver;
	_colorFocus[1].color = RGB(255, 255, 255);
}

Button::Button(identity_t id, Theme& theme, Theme::Font &font, unsigned char iUp, unsigned char iDn, unsigned char iOver) :
	Control(id, theme, font),
	_down(false),
	_textUp(_T("?")),
	_textDown(_T("?")),
	_textOver(_T("?")),
	_iGlyphUp(iUp),
	_iGlyphDn(iDn),
	_iGlyphOver(iOver)
{
	_colorUp[0].index = Theme::eToolFore;
	_colorUp[0].color = RGB(0, 0, 0);
	_colorUp[1].index = Theme::eToolBack;
	_colorUp[1].color = RGB(192, 192, 192);

	_colorDn[0].index = Theme::eToolFore;
	_colorDn[0].color = RGB(128, 128, 128);
	_colorDn[1].index = Theme::eToolBack;
	_colorDn[1].color = RGB(255, 255, 255);

	_colorOver[0].index = Theme::eToolFore;
	_colorOver[0].color = RGB(0, 0, 0);
	_colorOver[1].index = Theme::eToolOver;
	_colorOver[1].color = RGB(255, 255, 255);

	_colorFocus[0].index = Theme::eToolFore;
	_colorFocus[0].color = RGB(0, 0, 0);
	_colorFocus[1].index = Theme::eToolOver;
	_colorFocus[1].color = RGB(255, 255, 255);
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
	else if (_hover)
	{
		fore = _tile.getColor(_colorOver[0]);
		back = _tile.getColor(_colorOver[1]);
	}
	else
	{
		fore = _tile.getColor(_colorUp[0]);
		back = _tile.getColor(_colorUp[1]);
	}
	const Font& font = _tile.getFont(_tile.getFont());
	// get glyph text and draw it.
	const string_t &glyph = text(focus);
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
	if (action._code == VK_SPACE)
	{
		switch (action._what)
		{
			case KeyEvent::DOWN:
				_down = true;
				setChanged(true);
				if (!Click.empty())
					Click(this, _down);
				return true;

			case KeyEvent::UP:
				_down = false;
				setChanged(true);
				if (!Click.empty())
					Click(this, _down);
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
	Control::dispatch(action);
	if (action._button == MouseEvent::eLeft)
	{
		switch (action._what)
		{
			case MouseEvent::eDownClick:
				if (!_readonly)
				{
					_down = true;
					setChanged(true);
					if (!Click.empty())
						Click(this, _down);
				}
				if (!_focus)
					getContainer()->setFocus(this);
				break;

			case MouseEvent::eUpClick:
				if (!_readonly)
				{
					_down = false;
					setChanged(true);
					if (!Click.empty())
						Click(this, _down);
				}
				break;

			default:
				break;
		}
	}
	return false;
}

// serialize
bool Button::save(JSON::Writer &writer)
{
	std::string text;
	size_t used = 0;

	text.resize(_textUp.size() + 2, ' ');
	wcstombs_s(&used, &text[0], text.size(), _textUp.c_str(), _textUp.size());
	text.resize(used);

	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	_tile.save(writer);

	writer.writeNamedValue("text", text.c_str());
	Font::save(writer, _tile.getFont().font);
	writer.writeEndObject(true);
	return true;
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
