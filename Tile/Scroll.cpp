#include "stdafx.h"
#include "Scroll.h"
#include "Pane.h"
#include "Identity.h"
#include "../JSON/Writer.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

// keep value in range of (min,max) inclusive.
static long range(long min, long max, long value)
{
	if (value < min)
		value = min;
	if (value > max)
		value = max;
	return value;
}

Scroll::Scroll(identity_t id, Theme& theme, orient_t flow, long &value) : 
	Pane(id, theme, flow),
	_theme(theme),
	_nearArrow(0, theme, Pane::opposite(flow) ), 
	_nearSpacer(0, theme),
	_thumb(0, theme),
	_farSpacer(0, theme),
	_farArrow(0, theme, flow),
	_min(0), 
	_max(0), 
	_value(value),
	_access(new Reference<long>(value)),
	_focus(false)
{
	Theme::Font font = {Theme::eArrow, theme.Arrow};
	_tile.setFont(font);
	_nearSpacer.setFont(font);
	_farSpacer.setFont(font);
	_thumb.getFont(font);
	Add(&_nearArrow, 1, 1, 0, true);
	_indexNear = Add(&_nearSpacer, 0, 2048, 0);
	_indexThumb = Add(&_thumb, theme.Text._height, 2048, 0);
	_indexFar = Add(&_farSpacer, 0, 2048, 0);
	Add(&_farArrow, theme.Text._height, theme.Text._height, 0);

	Theme::Color fill = {Theme::eCaptionBack, 0};
	Theme::Color thumb = {Theme::eDefault, RGB(192, 192, 192)};

	_nearSpacer._fill = fill;
	_farSpacer._fill = fill;
	_thumb._fill = thumb;
	_thick.local = true;
	_thick.thick = 0;

	Flow desc = {_theme.Text._height, _theme.Text._height, 0};
	setFlow(Pane::opposite(flow), desc);
}

Scroll::Scroll(identity_t id, Theme& theme, orient_t flow, IAccessor<long> *pAccess) : 
	Pane(id, theme, flow),
	_theme(theme),
	_nearArrow(0, theme, Pane::opposite(flow) ), 
	_nearSpacer(0, theme),
	_thumb(0, theme),
	_farSpacer(0, theme),
	_farArrow(0, theme, flow),
	_min(0), 
	_max(0), 
	_value(0),
	_access(pAccess),
	_focus(false)
{
	Theme::Font font = {Theme::eArrow, theme.Arrow};
	_tile.getFont(font);
	_nearSpacer.getFont(font);
	_farSpacer.getFont(font);
	_thumb.getFont(font);
	Add(&_nearArrow, 1, 1, 0, true);
	_indexNear = Add(&_nearSpacer, 0, 2048, 0);
	_indexThumb = Add(&_thumb, theme.Text._height, 2048, 0);
	_indexFar = Add(&_farSpacer, 0, 2048, 0);
	Add(&_farArrow, 1, 1, 0, true);

	Theme::Color fill = {Theme::eCaptionBack, 0};
	Theme::Color thumb = {Theme::eDefault, RGB(192, 192, 192)};

	_nearSpacer._fill = fill;
	_farSpacer._fill = fill;
	_thumb._fill = thumb;

	// TODO: verify font based height is same as near/far arrows
	Flow desc = {1, 1, 0, true};
	setFlow(Pane::opposite(flow), desc);
}

Scroll::~Scroll()
{
	_listTiles.clear();
}

// instance type
const char* Scroll::getType() const
{
	return Scroll::type();
}

// class type
const char* Scroll::type()
{
	return "Scroll";
}

// key event sink
bool Scroll::dispatch(KeyEvent &action)
{
	if (action._what == KeyEvent::DOWN &&
		action._mask == 0)
	{
		orient_t flow = getFlow();
		if (flow == eDown || flow == eUp)
		{
			switch(action._code)
			{
				case VK_UP:
					_value = _access->getValue();
					setValue(_value - 1);
					return true;
				case VK_DOWN:
					_value = _access->getValue();
					setValue(_value + 1);
					return true;
			}
		}
		else if (flow == eLeft || flow == eRight)
		{
			switch(action._code)
			{
				case VK_LEFT:
					_value = _access->getValue();
					setValue(_value - 1);
					return true;
				case VK_RIGHT:
					_value = _access->getValue();
					setValue(_value + 1);
					return true;
			}
		}
	}
	return false;
}

bool Scroll::onMove(MouseEvent &action)
{
	if (_thumb.contains(action._place) || _drag)
	{
		if (_drag)
		{
			meter_t span = 0;
			rect_t rect = {0};
			_nearSpacer.getRect(rect);
			meter_t move = action._place.x - rect.x - _offset;
			span += rect.wide;
			_farSpacer.getRect(rect);
			span += rect.wide;
			move = move * (_max - _min);
			move /= span;
			setValue(move);
		}
		else
		{
			rect_t rect = {0};
			_thumb.getRect(rect);
			_offset = action._place.x - rect.x;
			_pt = action._place;
			_drag = true;
			if (_pDesktop)
				_pDesktop->setCapture(this);
		}
	}
	return true;
}

bool Scroll::onClick(MouseEvent &action)
{
	if ( _nearSpacer.contains(action._place) )
	{
		if ( Pane::horizontal(getFlow()) )
		{
			// TODO: math is close but not 100%
			meter_t span = 0;
			rect_t rect = {0};
			_thumb.getRect(rect);
			meter_t move = rect.wide / 2;
			_nearSpacer.getRect(rect);
			move += action._place.x - rect.x;
			span += rect.wide;
			_farSpacer.getRect(rect);
			span += rect.wide;
			move = move * (_max - _min);
			move /= span;
			setValue(move);
		}
		else if ( Pane::vertical(getFlow()) )
		{
			// TODO: math is close but not 100%
			meter_t span = 0;
			rect_t rect = {0};
			_thumb.getRect(rect);
			meter_t move = rect.high / 2;
			_nearSpacer.getRect(rect);
			move += action._place.y - rect.y;
			span += rect.high;
			_farSpacer.getRect(rect);
			span += rect.high;
			move = move * (_max - _min);
			move /= span;
			setValue(move);
		}
	}
	else if ( _farSpacer.contains(action._place) )
	{
		if ( Pane::horizontal(getFlow()) )
		{
			// TODO: math is close but not 100%
			meter_t span = 0;
			rect_t rect = {0};
			_thumb.getRect(rect);
			meter_t move = -rect.wide / 2;
			_nearSpacer.getRect(rect);
			move += action._place.x - rect.x;
			span += rect.wide;
			_farSpacer.getRect(rect);
			span += rect.wide;
			move = move * (_max - _min);
			move /= span;
			setValue(move);
		}
		else
		{
			// TODO: math is close but not 100%
			meter_t span = 0;
			rect_t rect = {0};
			_thumb.getRect(rect);
			meter_t move = -rect.high / 2;
			_nearSpacer.getRect(rect);
			move += action._place.y - rect.y;
			span += rect.high;
			_farSpacer.getRect(rect);
			span += rect.high;
			move = move * (_max - _min);
			move /= span;
			setValue(move);
		}
	}
	return true;
}

// mouse event sink
bool Scroll::dispatch(MouseEvent &action)
{
	if (action._button == MouseEvent::eLeft &&
		action._what == MouseEvent::eDownClick)
	{
		if ( _nearArrow.contains(action._place) )
		{
			_value = _access->getValue();
			setValue(_value - 1);
		}
		else if ( _farArrow.contains(action._place) )
		{
			_value = _access->getValue();
			setValue(_value + 1);
		}
		else
			onClick(action);

		if (!_focus)
			getContainer()->setFocus(this);
	}
	if (action._what == MouseEvent::eMove &&
		action._button == MouseEvent::eLeft)
	{
		return onMove(action);
	}
	if (action._what == MouseEvent::eUpClick &&
		action._button == MouseEvent::eLeft)
	{
		_drag = false;
		if (_pDesktop)
			_pDesktop->setCapture(NULL);
	}
	return true;
}

void Scroll::setHover(bool hover)
{
	hover; // ignored.
	// do nothing
	_drag = false;
}

bool Scroll::getFocus() const
{
	return _focus;
}

// control has input focus
void Scroll::setFocus(bool focus)
{
	bool change = _focus ^ focus;
	if (change)
	{
		_focus = focus;
		_thumb._fill.color = focus ? RGB(255,255,255) : RGB(192,192,192);
		if (focus)
			getContainer()->Shown(this);
		setChanged(change);
	}
}

// readonly
bool Scroll::getReadOnly() const
{
	return _readonly;
}

void Scroll::setReadOnly(bool readonly)
{
	_readonly = readonly;
}

// get/set accessors for scroll range
void Scroll::setRange(long min, long max)
{
	_min = min;
	_max = max;
	_value = _access->getValue();
	setValue(_value);
	setChanged(true);
}

void Scroll::getRange(long &min, long &max)
{
	min = _min;
	max = _max;
}

// get/set accessors for scroll value
void Scroll::getValue(long &value)
{
	if (_access != NULL)
		_value = _access->getValue();
	value = _value;
}

void Scroll::setValue(long value)
{
	value = range(_min, _max, value);
	if (_access != NULL)
		_value = _access->getValue();
	bool bChange = _value != value;
	if (bChange)
	{
		_value = value;
		if (_access != NULL)
			_access->setValue(_value);
		_thumb.setWeight(_flow, _min);
		_nearSpacer.setWeight(_flow, value - _min);
		_farSpacer.setWeight(_flow, _max - value);
		Pane::reflow();
		setChanged(true);
	}
}

/*
TODO
1. click and hold on near/far arrow should continuously scroll.
2. mouse capture while dragging scroll thumb.
*/

// serialize
bool Scroll::save(JSON::Writer &writer)
{
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	writer.writeNamedValue("orient", Pane::getName(_flow));
	_tile.save(writer);
	// don't save underlying Flow's tiles.
	// those are managed programmatically.
	writer.writeEndObject(true);
	return true;
}

bool Scroll::onNear(orient_t)
{
	if (!_focus)
	{
		getContainer()->setFocus(this);
		return true;
	}
	return false;
}

bool Scroll::onFar(orient_t)
{
	if (!_focus)
	{
		getContainer()->setFocus(this);
		return true;
	}
	return false;
}
