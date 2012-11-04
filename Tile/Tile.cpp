#include "stdafx.h"
#include "Tile.h"
#include "Pane.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Tile::Tile(identity_t id, Theme &theme) : 
	_id(id),
	_pNotify(NULL),
	_bChanged(true),
	_pSelf(this),
	_theme(theme),
	_pPane(NULL)
{
	_font.index = Theme::eText;
	_font.font = theme.Text;
	_rect.x = _rect.wide = _rect.y = _rect.high = 0;
	_scrollBox = _rect;
	Flow desc = {0, 4096, 1, false};
	_vert = desc;
	_horz = desc;
}

Tile::Tile(identity_t id, Theme &theme, const Theme::Font &font) : 
	_id(id),
	_pNotify(NULL),
	_bChanged(true),
	_pSelf(this),
	_theme(theme),
	_font(font),
	_pPane(NULL)
{
	_rect.x = _rect.wide = _rect.y = _rect.high = 0;
	_scrollBox = _rect;
	Flow desc = {0, 4096, 1, false};
	_vert = desc;
	_horz = desc;
}

Tile::Tile(const Tile &copy) : 
	_id(copy._id), 
	_pNotify(NULL),
	_rect(copy._rect), 
	_scrollBox(copy._scrollBox),
	_vert(copy._vert), 
	_horz(copy._horz),
	_pSelf(this),
	_font(copy._font),
	_theme(copy._theme),
	_pPane(NULL)
{
}

identity_t Tile::identity() const
{
	return _id;
}

// instance type
const char* Tile::getType() const
{
	if (_pSelf != this)
		return _pSelf->getType();
	_ASSERT(false);
	return "Rats";
}

// get/set accessor for bounding rectangle.
void Tile::getRect(rect_t &rect) const
{
	rect = _rect;
}

void Tile::setRect(const rect_t &rect)
{
	bool changed = !_rect.equals(rect);
	_rect = rect;

	_ASSERT(rect.wide >= 0);
	_ASSERT(rect.high >= 0);

	meter_t min = 0;

	rect_t box = _scrollBox;

	_pSelf->getMin(eRight, min);
	// did we meet minimum width?
	if (rect.wide >= min)
	{
		// yes: scroll box matches drawing rect at 0 offset.
		box.wide = rect.wide;		
		box.x = 0;
	}
	else 
	{
		// no: scroll box has minimum height and some offset.
 		box.wide = min;
		// adjust scroll X offset as needed.
		if ( box.x > (box.wide - rect.wide) )
			box.x = (box.wide - rect.wide);
	}

	_pSelf->getMin(eDown, min);
	// did we meet minimum height?
	if (rect.high >= min)
	{
		// yes: scroll box matches drawing rect at 0 offset.
		box.high = rect.high;		
		box.y = 0;
	}
	else
	{
		// no: scroll box has minimum height and some offset.
		box.high = min;
		// adjust scroll X offset as needed.
		if ( box.y > (box.high - rect.high) )
			box.y = (box.high - rect.high);
	}
	_pSelf->setScrollBox(box);
	if (changed)
		setChanged(true);
}

// get/set accessor for scroll box - width, height, and (x,y) offset
// where the rect above exposes a portion of the scroll box.
void Tile::getScrollBox(rect_t &box) const
{
	box = _scrollBox;
}

void Tile::setScrollBox(const rect_t &box) 
{
	bool changed = !_scrollBox.equals(box);
	_scrollBox = box;
	if (changed)
		setChanged(true);
}

void Tile::getScrollArea(area_t &area) const
{
	area = _scrollBox.getArea();
}

void Tile::setScrollArea(const area_t &area)
{
	_scrollBox.setArea(area);
}

void Tile::getScrollPoint(point_t &pt) const
{
	pt = _scrollBox.getPoint();
}

void Tile::setScrollPoint(const point_t &pt)
{
	point_t max = {0};
	point_t set = pt;
	max.x = _scrollBox.wide - _rect.wide;
	max.y = _scrollBox.high - _rect.high;
	if (set.x < 0) set.x = 0;
	if (set.y < 0) set.y = 0;
	if (max.x < 0) max.x = 0;
	if (max.y < 0) max.y = 0;
	if (set.x > max.x) set.x = max.x;
	if (set.y > max.y) set.y = max.y;
	_scrollBox.setPoint(set);
}

// get minimim required measure in the requested orientation.
void Tile::getMin(orient_t flow, meter_t &min)
{
	const Font &font = getFont(_font);
	switch (flow)
	{
		case eLeft:
		case eRight:
			min = _horz._min;
			if (_horz._fontScale)
				min *= font._height;
			break;

		case eUp:
		case eDown:
			min = _vert._min;
			if (_vert._fontScale)
				min *= font._height;
			break;
		default:
			break;
	}
}

void Tile::setMin(orient_t flow, meter_t min)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			_horz._min = min;
			break;

		case eUp:
		case eDown:
			_vert._min = min;
			break;
		default:
			break;
	}
}

// get maximim allowed measure in the requested orientation.
void Tile::getMax(orient_t flow, meter_t &max)
{
	const Font &font = getFont(_font);
	switch (flow)
	{
		case eLeft:
		case eRight:
			max = _horz._max;
			if (_horz._fontScale)
				max *= font._height;
			break;

		case eUp:
		case eDown:
			max = _vert._max;
			if (_vert._fontScale)
				max *= font._height;
			break;
		default:
			break;
	}
}

void Tile::setMax(orient_t flow, meter_t max)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			_horz._max = max;
			break;

		case eUp:
		case eDown:
			_vert._max = max;
			break;
		default:
			break;
	}
}

// get weight value for computing allocation pixels in excess minimums.
void Tile::getWeight(orient_t flow, meter_t &weight)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			weight = _horz._weight;
			break;

		case eUp:
		case eDown:
			weight = _vert._weight;
			break;
		default:
			break;
	}
}

void Tile::setWeight(orient_t flow, meter_t weight)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			_horz._weight = weight;
			break;

		case eUp:
		case eDown:
			_vert._weight = weight;
			break;
		default:
			break;
	}
}

void Tile::getFlow(orient_t flow, Flow &desc)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			desc = _horz;
			break;

		case eUp:
		case eDown:
			desc = _vert;
			break;
		default:
			break;
	}
}

void Tile::setFlow(orient_t flow, const Flow &desc)
{
	switch (flow)
	{
		case eLeft:
		case eRight:
			_horz = desc;
			break;

		case eUp:
		case eDown:
			_vert = desc;
			break;
		default:
			break;
	}
}

// true if point lies in this tile.
bool Tile::contains(point_t pt) const
{
	if (pt.x >= _rect.x && pt.x < _rect.x + _rect.wide)
		if (pt.y >= _rect.y && pt.y < _rect.y + _rect.high)
			return true;
	return false;
}

const rect_t &Tile::rect() const
{
	return _rect;
}

const rect_t &Tile::scrollBox() const
{
	return _scrollBox;
}

static bool saveFlow(JSON::Writer &writer, const char *alias, const Flow &desc)
{
	writer.writeStartNamedObject(alias);
	writer.writeNamedValue("min", desc._min);
	writer.writeNamedValue("max", desc._max);
	writer.writeNamedValue("weight", desc._weight);
	writer.writeEndObject();
	return true;
}

// drawable
// get/set accessor: true if this has changed since last drawn.
// used to drive internal redrawing.
bool Tile::getChanged() const
{
	return _bChanged;
}

void Tile::setChanged(bool bChanged)
{
	bool bNotify = bChanged && !_bChanged;
	_bChanged = bChanged;
	if (bNotify && _pNotify != NULL)
		_pNotify->Redraw(_pSelf);
}

/// <param name="canvas">canvas where this object will be drawn</param>
bool Tile::Draw(ICanvas* canvas, bool focus)
{
	// abstract class really.
	_ASSERT(_pSelf != this);
	return _pSelf->Draw(canvas, focus);
}

// set callback for change notification.
void Tile::watch(IRedraw* pNotify)
{
	_pNotify = pNotify;
	if (pNotify && _bChanged)
		_pNotify->Redraw(_pSelf);
}

// serialize
bool Tile::save(JSON::Writer &writer)
{
	return writer.writeNamedValue("id", identity()) &&
		saveFlow(writer, "Horz", _horz) &&
		saveFlow(writer, "Vert", _vert);
}

// the tile's containing flow object
Pane *Tile::getContainer() const
{
	return _pPane;
}

void Tile::setContainer(Pane *pPane)
{
	_pPane = pPane;
}

color_t Tile::getColor(const Theme::Color &pick) const
{
	return _theme.getColor(pick);
}

Theme& Tile::getTheme()
{
	return _theme;
}

meter_t Tile::getThick(const Theme::Thick &thick) const
{
	return _theme.getThick(thick);
}

const Font &Tile::getFont(const Theme::Font &font) const
{
	return _theme.getFont(font);
}

void Tile::setFont(const Theme::Font &font)
{
	_font = font;
}

const Theme::Font& Tile::getFont() const
{
	return _font;
}
