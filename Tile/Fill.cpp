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
