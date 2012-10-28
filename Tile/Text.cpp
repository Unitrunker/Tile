#include "stdafx.h"
#include "Text.h"
#include "ICanvas.h"
#include "Pane.h"
#include "JSON.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

/// <param name="font">.</param>
Text::Text(identity_t id, Theme &theme) : 
	Tile(id, theme),
	_align(eLeft)
{
	_fore.index = Theme::eCaptionFore;
	_fore.color = 0;
	_back.index = Theme::eCaptionBack;
	_back.color = 0;
}

/// <param name="font">.</param>
Text::Text(identity_t id, Theme &theme, Theme::Font &font) : 
	Tile(id, theme, font),
	_align(eLeft)
{
	_fore.index = Theme::eCaptionFore;
	_fore.color = 0;
	_back.index = Theme::eCaptionBack;
	_back.color = 0;
}
/// <param name="theme">.</param>
/// <param name="align">text alignment</param>
/// <param name="text">displayed</param>
Text::Text(identity_t id, Theme &theme, Theme::Font& font, align_t align, const TCHAR *text) :
	Tile(id, theme, font),
	_align(align), 
	_text(text)
{
	// default to one line.
	//meter_t height = _theme.Text._height;
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
	_fore.index = Theme::eCaptionFore;
	_fore.color = 0;
	_back.index = Theme::eCaptionBack;
	_back.color = 0;
}

/// <param name="copy">.</param>
Text::Text(const Text &copy) : 
	Tile(copy), 
	_align(copy._align), 
	_text(copy._text)
{
	_fore.index = Theme::eCaptionFore;
	_fore.color = 0;
	_back.index = Theme::eCaptionBack;
	_back.color = 0;
}

// instance type
const char* Text::getType() const
{
	return Text::type();
}

const char* Text::type()
{
	return "Text";
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Text::Draw(ICanvas *canvas, bool bFocus)
{
	bFocus; // ignored.
	const Font &font = _theme.getFont(_font);
	canvas->DrawString(_rect, _scrollBox, 
		_theme.getColor(_fore), _theme.getColor(_back), 
		font, _align, _text);
	_bChanged = false;
	return true;
}

// serialize
bool Text::save(JSON::Writer &writer)
{
	std::string text, align;
	size_t used = 0;

	text.resize(_text.size() + 2, ' ');
	wcstombs_s(&used, &text[0], text.size(), _text.c_str(), _text.size());
	text.resize(used);

	orient(align, (orient_t)_align);

	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	Tile::save(writer);
	writer.writeNamedValue("text", text.c_str());
	writer.writeNamedValue("align", align.c_str());
	saveColor(writer, "Fore", _fore, false);
	saveColor(writer, "Back", _back, false);
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool Text::load(JSON::Reader &reader, Theme &theme, const char *type, ITile *&pText)
{
	bool bOK = false;
	if ( !strcmp(type, Text::type()) )
	{
		Flow horz, vert;
		std::string text, align;
		identity_t id = 0;
		do
		{
			bOK = reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert) ||
				reader.namedValue("text", text) ||
				reader.namedValue("align", align);
		}
		while (bOK && reader.comma());
		if (bOK)
		{
			string_t wide;
			wide.resize(text.size() + 1, ' ');
			size_t used = 0;
			mbstowcs_s(&used, &wide[0], wide.size(), text.c_str(), text.size());
			wide.resize(used); 
			Theme::Font textFont = {Theme::eText, theme.Text};
			Text *p = new Text(id, theme, textFont, (align_t)orient(align), wide.c_str());
			p->setFlow(eRight, horz);
			p->setFlow(eDown, vert);
			pText = p;
		}
	}
	return bOK;
}
