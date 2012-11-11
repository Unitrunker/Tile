#include "Pane.h"
#include "Accessor.h"

/*
Copyright © 2011-2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Arrow;
struct Edit;

struct PickColor : public Pane, private IAccessor<color_t>
{
	PickColor(identity_t id, Theme &theme, IAccessor<color_t>& access);
	virtual void setRect(const rect_t &rect);
	// get/set accessors for layout mimimums
	virtual void getMin(orient_t flow, meter_t &min);
	// get/set accessors for layout maximums
	virtual void getMax(orient_t flow, meter_t &max);
	// get/set accessors for layout weight
	virtual void getWeight(orient_t flow, meter_t &weight);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

	// readonly
	using Pane::getReadOnly;
	virtual void setReadOnly(bool);

private:
	//void click(Button*, bool down);
	virtual const color_t& getValue() const;
	virtual bool setValue(const color_t &value);

	ColorReference _local;
	IAccessor<color_t> &_access;
	Arrow *_arrow;
	Edit *_edit;
};

};