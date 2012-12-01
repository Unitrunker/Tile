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

	writer.writeStartNamedObject("Up");
	writer.writeNamedValue("glyph", _iGlyphUp, 10);
	 saveColor(writer, "Fore", _colorUp[true], false);
	 saveColor(writer, "Back", _colorUp[false], false);
	writer.writeEndObject(false);

	writer.writeStartNamedObject("Down");
	 writer.writeNamedValue("glyph", _iGlyphDn, 10);
	 saveColor(writer, "Fore", _colorDn[true], false);
	 saveColor(writer, "Back", _colorDn[false], false);
	writer.writeEndObject(false);

	writer.writeStartNamedObject("Over");
	 writer.writeNamedValue("glyph", _iGlyphOver, 10);
	 saveColor(writer, "Fore", _colorOver[true], false);
	 saveColor(writer, "Back", _colorOver[false], false);
	writer.writeEndObject(false);

	writer.writeStartNamedObject("Focus");
	 // no glyph needed here.
	 saveColor(writer, "Fore", _colorFocus[true], false);
	 saveColor(writer, "Back", _colorFocus[false], false);
	writer.writeEndObject(false);

	writer.writeNamedValue("text", text.c_str());
	saveFont(writer, "Font", _tile.getFont(), false);
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool Button::load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pButton)
{
	bool bOK = false;
	if ( !strcmp(type, Button::type()) )
	{
		identity_t id = 0;
		std::string text;
		Flow horz, vert;
		Theme::Color up[2];
		Theme::Color dn[2];
		Theme::Color over[2];
		Theme::Color focus[2];
		long iUp = 0, iDn = 0, iOver = 0;
		Theme::Font desc = {Theme::eDefault, Font(_T("MS Shell Dlg"), 18, 0)};
		do
		{
			bOK = reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert) ||
				reader.namedValue("text", text) ||
				loadFont(reader, "Font", desc);

			if (!bOK)
			{
				if ( reader.beginNamedObject("Up") )
				{
					do
					{
						bOK = reader.namedValue("glyph", iUp) ||
							loadColor(reader, "Fore", up[true]) ||
							loadColor(reader, "Back", up[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
				else if ( reader.beginNamedObject("Down") )
				{
					do
					{
						bOK = reader.namedValue("glyph", iDn) ||
							loadColor(reader, "Fore", dn[true]) ||
							loadColor(reader, "Back", dn[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
				else if ( reader.beginNamedObject("Over") )
				{
					do
					{
						bOK = reader.namedValue("glyph", iOver) ||
							loadColor(reader, "Fore", over[true]) ||
							loadColor(reader, "Back", over[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
				else if ( reader.beginNamedObject("Focus") )
				{
					do
					{
						bOK = loadColor(reader, "Fore", focus[true]) ||
							loadColor(reader, "Back", focus[false]);
					}
					while (bOK && reader.comma());
					bOK = reader.endObject();
				}
			}
		}
		while (bOK && reader.comma());
		if (bOK)
		{
			string_t wide;
			wide.resize(text.size() + 1, ' ');
			size_t used = 0;
			mbstowcs_s(&used, &wide[0], wide.size(), text.c_str(), text.size());
			wide.resize(used);
			Button *p = new Button(id, theme, desc, wide.c_str(), wide.c_str(), wide.c_str());
			p->setFlow(eRight, horz);
			p->setFlow(eDown, vert);
			p->_colorDn[false] = dn[false];
			p->_colorDn[true] = dn[true];
			p->_iGlyphDn = (unsigned char)iDn;
			p->_colorUp[false] = up[false];
			p->_colorUp[true] = up[true];
			p->_iGlyphUp = (unsigned char)iUp;
			p->_colorOver[false] = over[false];
			p->_colorOver[true] = over[true];
			p->_iGlyphOver = (unsigned char)iOver;
			p->_colorFocus[false] = focus[false];
			p->_colorFocus[true] = focus[true];
			pButton = p;
		}
	}
	return bOK;
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