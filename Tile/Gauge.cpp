#include "stdafx.h"
#include "Gauge.h"
#include "Pane.h"
#include "ICanvas.h"
#include "../JSON/Writer.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Tiles;

Gauge::Gauge(identity_t id, Theme& theme, orient_t flow) :
	Tile(id, theme),
	_flow(flow),
	_value(0),
	_thick(2),
	_fore(Theme::eCaptionFore, 0),
	_back(Theme::eCaptionBack, 0)
{
	Flow desc = {1, 1, 0, true};
	setFlow(eRight, desc);
	setFlow(eDown, desc);
}

// instance type
const char* Gauge::getType() const
{
	return "Gauge";
}

const char* Gauge::type()
{
	return "Gauge";
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Gauge::Draw(ICanvas *canvas, bool bFocus)
{
	bFocus; // ignored, will always be false.

	rect_t rect;
	getRect(rect);

	rect_t left, right, center;

	left = rect;
	right = rect;
	center = rect;

	if (_flow == eRight || _flow == eLeft)
	{
		left.wide = _value;
		if (left.wide > rect.wide)
			left.wide = rect.wide;
		center.x = left.right();
		center.wide = _thick;
		right.x = center.right();
		right.wide = rect.wide - left.wide - center.wide;
	}
	else
	{
		left.high = _value;
		if (left.high > rect.high)
			left.high = rect.high;
		center.y = left.bottom();
		center.high = _thick;
		right.y = center.bottom();
		right.high = rect.high - left.high - center.high;
	}

	color_t fore = _theme.getColor(_fore);
	color_t back = _theme.getColor(_back);
	canvas->FillRectangle(left, back);
	canvas->FillRectangle(center, fore);
	canvas->FillRectangle(right, back);
	_bChanged = false;
	return true;
}

// serialize
bool Gauge::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	Tile::save(writer);
	writer.writeNamedValue("orient", Pane::getName(_flow));
	writer.writeEndObject(true);
	return true;
}

const meter_t &Gauge::getValue() const
{
	return _value;
}

bool Gauge::setValue(const meter_t &value)
{
	bool bChange = _value != value;
	_value = value;
	setChanged(bChange);
	return true;
}
