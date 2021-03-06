#include "stdafx.h"
#include "PickColor.h"
#include "Edit.h"
#include "Arrow.h"
#include "Fill.h"
#include <atldlgs.h>

/*
Copyright � 2011, 2012 Rick Parrish
*/

using namespace Tiles;

static size_t distance(color_t a, color_t b)
{
	size_t span = 0;
	unsigned char a1 = static_cast<unsigned char>(a);
	unsigned char b1 = static_cast<unsigned char>(b);
	if (a1 > b1)
		span += (a1 - b1);
	else
		span += (b1 - a1);
	a1 = static_cast<unsigned char>(a >> 8);
	b1 = static_cast<unsigned char>(b >> 8);
	if (a1 > b1)
		span += (a1 - b1);
	else
		span += (b1 - a1);
	a1 = static_cast<unsigned char>(a >> 16);
	b1 = static_cast<unsigned char>(b >> 16);
	if (a1 > b1)
		span += (a1 - b1);
	else
		span += (b1 - a1);
	return span;
}

PickColor::PickColor(identity_t id, Theme &theme, IAccessor<color_t> *access) :
	Pane(id, theme, eRight),
	_access(access),
	_local(*this)
{
	Theme::Font textFont = {Theme::eText, theme.Text};

	color_t black = 0;
	color_t white = RGB(255,255,255);
	_edit = new Edit(0, theme, textFont, &_local);
	_arrow = new Arrow(0, theme, eDown);
	_arrow->_back._index = Theme::eDefault;
	_arrow->_back._color = white;
	_arrow->_fore._index = Theme::eDefault;
	_arrow->_fore._color = black;
	Add(_edit, 0, 4096, 1, false);
	Add(_arrow, 1, 1, 0, true);
	Flow flow = {1, 1, 0, true};
	setFlow(eDown, flow);
	_arrow->setFlow(eDown, flow);
	//_space.local = true;
	//_space.thick = 0;
	_thick.local = true;
	_thick.thick = 0;
}

// mouse event sink
bool PickColor::dispatch(MouseEvent &action)
{
	// 1. uses the WTL wrappers to the Windows Color Dialog.
	// 2. uses WTL to grab the window handle of the current message to use as the parent window.
	// Both mean this code really belongs in the WTL Tile library, not the generic Tile library.
	if (action._what == MouseEvent::eDownClick && 
		action._button == MouseEvent::eLeft && 
		!_readOnly && _enable)
	{
		// clicked arrow?
		if ( _arrow->contains(action._place) && _access != NULL )
		{
			HWND hwnd = static_cast<HWND>( _pDesktop->getHandle() );
			color_t rgb = _access->getValue();
			// WTL wrapper to color picker dialog.
			CColorDialog dlg(rgb);
			if (dlg.DoModal(hwnd) == IDOK)
			{
				setValue(dlg.m_cc.rgbResult);
				// Note: if control used a button instead of an arrow,
				// the next few lines of code would not be needed.

				// force edit to briefly lose focus so
				// it can pick up the new color value.
				if (_edit->getFocus())
				{
					_edit->setFocus(false);
					_edit->setFocus(true);
				}
				_edit->setChanged(true);
				//getContainer()->setFocus(_edit);
			}
			return true;
		}
	}
	return Pane::dispatch(action);
}

const color_t& PickColor::getValue() const
{
	const color_t &value = _access->getValue();
	if (value != _arrow->_back._color)
		adjust(value);
	return value;
}

bool PickColor::setValue(const color_t &value)
{
	adjust(value);
	return _access->setValue(value);
}

// adjust arrow colors for contrast.
void PickColor::adjust(const color_t value) const
{
	color_t black = 0;
	color_t white = RGB(255,255,255);
	_arrow->_back._color = value;
	if ( distance(value, black) > distance(value, white) )
		_arrow->_fore._color = black;
	else
		_arrow->_fore._color = white;
	_arrow->setChanged(true);
}

// pre-empt normal Flow logic for calculating horizontal flow

// get/set accessors for layout mimimums
void PickColor::getMin(orient_t flow, meter_t &min)
{
	if ( horizontal(flow) )
		_tile.getMin(flow, min);
	else
		Pane::getMin(flow, min);
}

// get/set accessors for layout mimimums
void PickColor::getMax(orient_t flow, meter_t &max)
{
	if ( horizontal(flow) )
		_tile.getMax(flow, max);
	else
		Pane::getMax(flow, max);
}

// get/set accessors for layout mimimums
void PickColor::getWeight(orient_t flow, meter_t &weight)
{
	if ( horizontal(flow) )
		_tile.getWeight(flow, weight);
	else
		Pane::getWeight(flow, weight);
}

// Modified getMin above causes standard Pane::setRect to break so use
// this greatly simplified logic for layout of the text and drop arrow.
void PickColor::setRect(const rect_t &rect)
{
	rect_t arrow = rect;
	rect_t text = rect;
	_tile.setRect(rect);
	_arrow->getMin(eRight, arrow.wide);
	text.wide -= arrow.wide;
	arrow.x += text.wide;
	text.wide -= _tile.getThick(_space);
	_edit->setRect(text);
	_arrow->setRect(arrow);
}
