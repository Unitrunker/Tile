#include "stdafx.h"
#include "Control.h"
#include "Flow.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Control::Control(identity_t id, Theme &theme, const Theme::Font &desc) : 
	_tile(id, theme, desc),
	_focus(false),
	_hover(false),
	_readonly(false),
	_pDesktop(NULL)
{
	FlowDesc flow = {1, 1, 0, true};
	_tile.setFlow(eDown, flow);
	_tile._pSelf = this;
}

Control::Control(identity_t id, Theme &theme) : 
	_tile(id, theme),
	_focus(false),
	_hover(false),
	_readonly(false),
	_pDesktop(NULL)
{
	FlowDesc flow = {1, 1, 0, true};
	_tile.setFlow(eDown, flow);
	_tile._pSelf = this;
}

Control::~Control()
{
	watch(NULL);
	if (_pDesktop)
	{
		if (_focus)
			_pDesktop->setFocus(NULL);
		if (_hover)
			_pDesktop->setHover(NULL);
	}
}

bool Control::getChanged() const
{
	return _tile.getChanged();
}

void Control::setChanged(bool bChanged)
{
	_tile.setChanged(bChanged);
}

void Control::watch(IRedraw* pNotify)
{
	_tile.watch(pNotify);
}

bool Control::getFocus() const
{
	return _focus;
}

/// <param name="focus">true if this control has focus</param>
void Control::setFocus(bool focus)
{
	bool change = _focus ^ focus;
	_focus = focus;
	if (change)
	{
		if (focus)
			getContainer()->Shown(this);
		setChanged(change);
	}
}

// readonly
bool Control::getReadOnly() const
{
	return _readonly;
}

void Control::setReadOnly(bool readonly)
{
	_readonly = readonly;
}

identity_t Control::identity() const
{
	return _tile.identity();
}

// get/set accessor for bounding rectangle.
void Control::getRect(rect_t &rect) const
{
	_tile.getRect(rect);
}

void Control::setRect(const rect_t &rect)
{
//	std::string log;
//	log.resize(128, _T(' '));
//	int size = sprintf(&log[0], "%s [%d,%d,%d,%d]\n", getType(), rect.x, rect.y, rect.wide, rect.high);
//	log.resize(size);
//	OutputDebugStringA(log.c_str());
	_tile.setRect(rect);
}

// get/set accessor for scroll box - width, height, and (x,y) offset
// where the rect above exposes a portion of the scroll box.
void Control::getScrollBox(rect_t &box) const
{
	_tile.getScrollBox(box);
}

void Control::setScrollBox(const rect_t &box)
{
	_tile.setScrollBox(box);
}

void Control::getScrollArea(area_t &area) const
{
	_tile.getScrollArea(area);
}

void Control::setScrollArea(const area_t &area)
{
	_tile.setScrollArea(area);
}

void Control::getScrollPoint(point_t &pt) const
{
	_tile.getScrollPoint(pt);
}

void Control::setScrollPoint(const point_t &pt)
{
	_tile.setScrollPoint(pt);
}

void Control::getMin(orient_t flow, meter_t &min)
{
	_tile.getMin(flow, min);
}

void Control::setMin(orient_t flow, meter_t min)
{
	_tile.setMin(flow, min);
}

void Control::getMax(orient_t flow, meter_t &max)
{
	_tile.getMax(flow, max);
}

void Control::setMax(orient_t flow, meter_t max)
{
	_tile.setMax(flow, max);
}

void Control::getWeight(orient_t flow, meter_t &weight)
{
	_tile.getWeight(flow, weight);
}

void Control::setWeight(orient_t flow, meter_t weight)
{
	_tile.setWeight(flow, weight);
}

void Control::getFlow(orient_t flow, FlowDesc &desc)
{
	_tile.getFlow(flow, desc);
}

void Control::setFlow(orient_t flow, const FlowDesc &desc)
{
	_tile.setFlow(flow, desc);
}

// true if point lies in this tile.
bool Control::contains(point_t pt)
{
	return _tile.contains(pt);
}

void Control::setDesktop(IWindow* pDesktop)
{
	_pDesktop = pDesktop;
}

bool Control::onNear(orient_t)
{
	if (!_focus)
	{
		getContainer()->setFocus(this);
		return true;
	}
	return false;
}

bool Control::onFar(orient_t)
{
	if (!_focus)
	{
		getContainer()->setFocus(this);
		return true;
	}
	return false;
}

// the tile's containing flow object
Flow *Control::getContainer() const
{
	return _tile.getContainer();
}

void Control::setContainer(Flow *pFlow)
{
	_tile.setContainer(pFlow);
}

// hover
void Control::setHover(bool hover)
{
	bool changed = _hover ^ hover;
	_hover = hover;
	if (changed)
		setChanged(true);
}

// sink all mouse click/move events.
bool Control::dispatch(MouseEvent &action)
{
	if (action._what == MouseEvent::eMove && !_hover)
	{
		_pDesktop->setHover(this);
		return true;
	}
	return false;
}

// get font selector
const Theme::Font &Control::getFont() const
{
	return _tile.getFont();
}

// set font selector
void Control::setFont(const Theme::Font &font)
{
	_tile.setFont(font);
}
