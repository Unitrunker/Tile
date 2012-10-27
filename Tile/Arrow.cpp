#include "stdafx.h"
#include "Arrow.h"
#include "Pane.h"
#include "ICanvas.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Arrow::Arrow(identity_t id, Theme& theme, orient_t flow) :
	Tile(id, theme),
	_flow(flow)
{
	_font.index = Theme::eArrow;
	_text = _T("?");
	Flow desc = {1, 1, 0, true};
	setFlow(eRight, desc);
	setFlow(eDown, desc);
	_fore.color = _back.color = 0;
	_fore.index = Theme::eCaptionFore;
	_back.index = Theme::eCaptionBack;
}

// instance type
const char* Arrow::getType() const
{
	return "Arrow";
}

const char* Arrow::type()
{
	return "Arrow";
}

static string_t& glyph(Theme &theme, orient_t flow)
{
	static string_t placebo = _T("?");
	switch (flow)
	{
		case eUp:
			return theme.ArrowUp;
			break;
		case eDown:
			return theme.ArrowDn;
			break;
		case eLeft:
			return theme.ArrowLt;
			break;
		case eRight:
			return theme.ArrowRt;
			break;
		default:
			break;
	}
	return placebo;
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Arrow::Draw(ICanvas *canvas, bool bFocus)
{
	align_t align = eLeft|eRight|eUp|eDown;

	bFocus; // ignored.
	_text = glyph(_theme, _flow);
	color_t fore = _theme.getColor(_fore);
	color_t back = _theme.getColor(_back);
	const Font& font = getFont(_font);
	canvas->DrawString(_rect, _scrollBox, fore, back, font, align, _text);
	_bChanged = false;
	return true;
}

// serialize
bool Arrow::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	Tile::save(writer);
	writer.writeNamedValue("orient", Pane::getName(_flow));
	writer.writeEndObject(true);
	return true;
}
