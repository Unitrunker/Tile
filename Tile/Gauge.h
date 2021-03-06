#include "Tile.h"
#include "Theme.h"
#include "Accessor.h"

/*
Copyright � 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Gauge : public Tile, public IAccessor<meter_t>
{
  	Gauge(identity_t id, Theme& theme, orient_t flow);
	virtual ~Gauge() { };
    /// <param name="canvas">Canvas where text will appear.</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// get/set accessors for changed flag.
	using Tile::getChanged;
	using Tile::setChanged;
	// set callback for change notification.
	using Tile::watch;

	using Tile::identity;
	// instance type
	virtual const char* getType() const;
	using Tile::getMin;
	using Tile::setMin;
	// get/set accessors for layout maximums
	using Tile::getMax;
	using Tile::setMax;
	// get/set accessors for layout weights
	using Tile::getWeight;
	using Tile::setWeight;
	// get/set accessors for layout descriptors
	using Tile::getFlow;
	using Tile::setFlow;
	// get/set accessor for bounding rectangle.
	using Tile::setRect;
	using Tile::getRect;
	// accessors for scrollable area.
	using Tile::getScrollBox;
	using Tile::setScrollBox;
	using Tile::getScrollArea;
	using Tile::setScrollArea;
	using Tile::getScrollPoint;
	using Tile::setScrollPoint;

	using Tile::contains;
	// serialize
	virtual bool save(JSON::Writer &writer);
	static const char* type();

	virtual const meter_t &getValue() const;
	virtual bool setValue(const meter_t &value);

	Theme::Color _fore;
	Theme::Color _back;
private:
	// orientation of gauge
	orient_t _flow;
	meter_t _value;
	meter_t _thick;
};

};