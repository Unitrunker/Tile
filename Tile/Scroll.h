#include "Arrow.h"
#include "Fill.h"
#include "Pane.h"
#include "Theme.h"
#include "IWindow.h"
#include "Accessor.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Scroll : public Pane
{
	Scroll(identity_t id, Theme &theme, orient_t flow, long &value);
	Scroll(identity_t id, Theme &theme, orient_t flow, IAccessor<long> *pAccess);
	virtual ~Scroll();

	// get/set accessors for changed flag.
	using Pane::getChanged;
	using Pane::setChanged;
	// set callback for change notification.
	using Pane::watch;
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
	using Pane::identity;
	// instance type
	virtual const char* getType() const;
	// get/set accessors for layout mimimums
	using Pane::getMin;
	using Pane::setMin;
	// get/set accessors for layout maximums
	using Pane::getMax;
	using Pane::setMax;
	// get/set accessors for layout weights
	using Pane::getWeight;
	using Pane::setWeight;
	// get/set accessors for layout descriptors
	using Pane::getFlow;
	using Pane::setFlow;
	// get/set accessor for bounding rectangle.
	using Pane::setRect;
	using Pane::getRect;

	using Pane::contains;
	using Pane::setDesktop;
	using Pane::Draw;
	using Pane::getContainer;
	using Pane::setContainer;
	// serialize
	virtual bool save(JSON::Writer &writer);
	static bool load(JSON::Reader &json, Theme &theme, const char *type, IControl* &pControl);
	static const char* type();
private:
	bool onMove(MouseEvent &action);
	bool onClick(MouseEvent &action);

	Arrow _nearArrow, _farArrow;
	Fill _nearSpacer;
	Fill _farSpacer;
	Fill _thumb;
	bool _focus;
	bool _readOnly;
	long _min, _max, _value;
	size_t _indexNear, _indexThumb, _indexFar;
	bool _drag;
	point_t _pt;
	meter_t _offset;
	IAccessor<long> *_access;
	Theme& _theme;
};

};