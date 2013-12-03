#include "Types.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace JSON
{
struct Writer;
};

namespace Tiles
{

struct Flow
{
	// minimum distance
	meter_t _min;
	// maximum distance
	meter_t _max;
	// weight for growth relative to other layout items.
	meter_t _weight;
	// true if min/max scale by font height
	bool _fontScale;
};

struct ICanvas;
struct IRedraw;
struct Pane;

struct ITile
{
	virtual ~ITile() { };
	virtual identity_t identity() const = 0;
	virtual const char* getType() const = 0;
	// get/set accessors for layout mimimums
	virtual void getMin(orient_t flow, meter_t &min) const = 0;
	virtual void setMin(orient_t flow, meter_t min) = 0;
	// get/set accessors for layout maximums
	virtual void getMax(orient_t flow, meter_t &max) const = 0;
	virtual void setMax(orient_t flow, meter_t max) = 0;
	// get/set accessors for layout weights
	virtual void getWeight(orient_t flow, meter_t &weight) const = 0;
	virtual void setWeight(orient_t flow, meter_t weight) = 0;
	// get/set accessors for layout descriptors
	virtual void getFlow(orient_t flow, Flow &desc) const = 0;
	virtual void setFlow(orient_t flow, const Flow &desc) = 0;
	// get/set accessor for bounding rectangle.
	virtual void getRect(rect_t &rect) const = 0;
	virtual void setRect(const rect_t &rect) = 0;
	// get/set accessor for scroll box - width, height, and (x,y) offset
	// where the rect above exposes a portion of the scroll box.
	virtual void getScrollBox(rect_t &box) const = 0;
	virtual void setScrollBox(const rect_t &box) = 0;
	virtual void getScrollArea(area_t &area) const = 0;
	virtual void setScrollArea(const area_t &area) = 0;
	virtual void getScrollPoint(point_t &pt) const = 0;
	virtual void setScrollPoint(const point_t &pt) = 0;
	// returns true if point lies within layout.
	virtual bool contains(point_t pt) const = 0;
	// the tile's containing flow object
	virtual Pane *getContainer() const = 0;
	virtual void setContainer(Pane *) = 0;

	// drawable
	// get/set accessor: true if this has changed since last drawn.
	// used to drive internal redrawing.
	virtual bool getChanged() const = 0;
	virtual void setChanged(bool) = 0;
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus) = 0;

	// set callback for change notification.
	virtual void watch(IRedraw*) = 0;
	// TODO: get/set Parent(Flow *);
};

struct IRedraw
{
	/// <param name="pDraw">control in need of redrawing.</param>
	virtual void Redraw(ITile *pDraw) = 0;
};

};