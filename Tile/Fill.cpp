#include "stdafx.h"
#include "Fill.h"
#include "Pane.h"
#include "ICanvas.h"
#include "JSON.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Fill::Fill(identity_t id, Theme &theme) : 
	Tile(id, theme),
	_fill(Theme::eCaptionBack, 0)
{
}

Fill::Fill(identity_t id, Theme &theme, Theme::Font& desc) : 
	Tile(id, theme, desc),
	_fill(Theme::eCaptionBack, 0)
{
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
	saveColor(writer, "Fill", _fill, false);
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool Fill::load(JSON::Reader &reader, Theme &theme, const char *type, ITile *&pDraw)
{
	bool bOK = false;
	if ( !strcmp(type, Fill::type()) )
	{
		identity_t id = 0;
		color_t fill = 0;
		Flow horz, vert;
		do 
		{
			bOK = reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert) ||
				reader.namedValue("fill", fill);
		}
		while (bOK && reader.comma());
		if (bOK)
		{
			Fill *p = new Fill(id, theme);
			p->_fill._color = fill; // TODO
			p->setFlow(eRight, horz);
			p->setFlow(eDown, vert);
			pDraw = p;
		}
	}
	return bOK;
}

