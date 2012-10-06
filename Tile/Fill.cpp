#include "stdafx.h"
#include "Fill.h"
#include "Flow.h"
#include "ICanvas.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Fill::Fill(identity_t id, Theme &theme) : 
	Tile(id, theme)
{
	_fill.color = 0;
	_fill.index = Theme::eCaptionBack;
}

Fill::Fill(identity_t id, Theme &theme, Theme::Font& desc) : 
	Tile(id, theme, desc) 
{
	_fill.color = 0;
	_fill.index = Theme::eCaptionBack;
}

/// <param name="copy">.</param>
Fill::Fill(const Fill &copy) : 
	Tile(copy),
	_fill(copy._fill)
{
}

// instance type
const char* Fill::getType() const
{
	return Fill::type();
}

const char* Fill::type()
{
	return "Fill";
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Fill::Draw(ICanvas *canvas, bool)
{
	color_t fill = _theme.getColor(_fill);
	canvas->FillRectangle(_rect, fill);
	_bChanged = false; // missing: found this via the ITile unit test.
	return true;
}

// serialize
bool Fill::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", "Fill");
	Tile::save(writer);
	writer.writeNamedValue("fill", _fill.color, 16); // TODO
	writer.writeEndObject(true);
	return true;
}
