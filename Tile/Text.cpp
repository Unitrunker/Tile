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
	_align(eLeft),
	_fore(Theme::eCaptionFore, 0),
	_back(Theme::eCaptionBack, 0)
{
}

/// <param name="font">.</param>
Text::Text(identity_t id, Theme &theme, const Theme::Font &font) : 
	Tile(id, theme, font),
	_align(eLeft),
	_fore(Theme::eCaptionFore, 0),
	_back(Theme::eCaptionBack, 0)
{
}
/// <param name="theme">.</param>
/// <param name="align">text alignment</param>
/// <param name="text">displayed</param>
Text::Text(identity_t id, Theme &theme, const Theme::Font& font, align_t align, const TCHAR *text) :
	Tile(id, theme, font),
	_align(align), 
	_text(text),
	_fore(Theme::eCaptionFore, 0),
	_back(Theme::eCaptionBack, 0)
{
	// default to one line.
	//meter_t height = _theme.Text._height;
	Flow desc = {1, 1, 0, true};
	setFlow(eDown, desc);
}

/// <param name="copy">.</param>
Text::Text(const Text &copy) : 
	Tile(copy), 
	_align(copy._align), 
	_text(copy._text),
	_fore(Theme::eCaptionFore, 0),
	_back(Theme::eCaptionBack, 0)
{
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
