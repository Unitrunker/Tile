#include "stdafx.h"
#include "Glyph.h"
#include "Flow.h"
#include "ICanvas.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Glyph::Glyph(identity_t id, Theme& theme, Theme::Font& font, const TCHAR *text) :
	Tile(id, theme, font)
{
	_text = text;
	FlowDesc desc = {1, 1, 0, true};
	setFlow(eRight, desc);
	setFlow(eDown, desc);
}

// instance type
const char* Glyph::getType() const
{
	return type();
}

const char* Glyph::type()
{
	return "Glyph";
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Glyph::Draw(ICanvas *canvas, bool bFocus)
{
	align_t align = eLeft|eRight|eUp|eDown;
	const Font& font = getFont(_font);
	canvas->DrawString(_rect, _scrollBox, _theme.CaptionFore, _theme.CaptionBack, font, align, _text);
	_bChanged = false;
	return true;
}

// serialize
bool Glyph::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	Tile::save(writer);
	writer.writeEndObject(true);
	return true;
}
