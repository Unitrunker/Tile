#include "Arrow.h"
#include "Fill.h"
#include "Flow.h"
#include "Theme.h"
#include "IWindow.h"
#include "Accessor.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Scroll : public Flow
{
	Scroll(identity_t id, Theme &theme, orient_t flow, long &value);
	Scroll(identity_t id, Theme &theme, orient_t flow, IAccessor<long> *pAccess);
	virtual ~Scroll();

	// get/set accessors for changed flag.
	using Flow::getChanged;
	using Flow::setChanged;
	// set callback for change notification.
	using Flow::watch;
	// IControl
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);
	// focus
	virtual bool getFocus() const;
	virtual void setFocus(bool);
	// readonly
	virtual bool getReadOnly() const;
	virtual void setReadOnly(bool);
	// hover
	virtual void setHover(bool hover);
	// navigation
	virtual bool onNear(orient_t);
	virtual bool onFar(orient_t);
	virtual bool hasControls() const { return true; }

	void setRange(long min, long max);
	void getRange(long &min, long &max);
	void setValue(long value);
	void getValue(long &value);
	// ITile implementation
	using Flow::identity;
	// instance type
	virtual const char* getType() const;
	// get/set accessors for layout mimimums
	using Flow::getMin;
	using Flow::setMin;
	// get/set accessors for layout maximums
	using Flow::getMax;
	using Flow::setMax;
	// get/set accessors for layout weights
	using Flow::getWeight;
	using Flow::setWeight;
	// get/set accessors for layout descriptors
	using Flow::getFlow;
	using Flow::setFlow;
	// get/set accessor for bounding rectangle.
	using Flow::setRect;
	using Flow::getRect;

	using Flow::contains;
	using Flow::setDesktop;
	using Flow::Draw;
	using Flow::getContainer;
	using Flow::setContainer;
	// serialize
	virtual bool save(JSON::Writer &writer);
	static const char* type();
private:
	bool onMove(MouseEvent &action);
	bool onClick(MouseEvent &action);

	Arrow _nearArrow, _farArrow;
	Fill _nearSpacer;
	Fill _farSpacer;
	Fill _thumb;
	bool _focus;
	bool _readonly;
	long _min, _max, _value;
	size_t _indexNear, _indexThumb, _indexFar;
	bool _drag;
	point_t _pt;
	meter_t _offset;
	IAccessor<long> *_access;
	Theme& _theme;
};

};