#include "Control.h"
#include "Theme.h"
#include "IWindow.h"
#include "Accessor.h"
#include "Colorful.h"
#include <vector>

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Toggle : public Control, public Colorful
{
	struct Item
	{
		TCHAR *_label;
		long _value;
	};

	Toggle(identity_t id, Theme &theme, Theme::Font& desc, std::vector<Item> & list, long& value);
	Toggle(identity_t id, Theme &theme, Theme::Font& desc, std::vector<Item> & list, IAccessor<long>* pAccess);
	virtual ~Toggle() { };
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);

	// IControl
	// Import these methods from the Control base class
	// get/set accessors for changed flag.
	using Control::getChanged;
	using Control::setChanged;
	// set callback for change notification.
	using Control::watch;
	// hover
	using Control::setHover;
	// focus
	using Control::getFocus;
	using Control::setFocus;
	// readonly
	using Control::getReadOnly;
	using Control::setReadOnly;
	// enable
	using Control::getEnable;
	using Control::setEnable;
	// force an update of any edits in-progress.
	using Control::apply;

	// navigation
	using Control::onNear;
	using Control::onFar;
	using Control::hasControls;
	// desktop
	using Control::setDesktop;
	// identity
	using Control::identity;
	// get/set accessors for layout mimimums
	using Control::getMin;
	using Control::setMin;
	// get/set accessors for layout maximums
	using Control::getMax;
	using Control::setMax;
	// get/set accessors for layout weights
	using Control::getWeight;
	using Control::setWeight;
	// get/set accessors for layout descriptors
	using Control::getFlow;
	using Control::setFlow;
	// get/set accessor for bounding rectangle.
	using Control::setRect;
	using Control::getRect;
	// get/set accessor for scroll box - width, height, and (x,y) offset
	// where the rect above exposes a portion of the scroll box.
	using Control::getScrollBox;
	using Control::setScrollBox;
    // true if point lies in this tile.
	using Control::contains;
	// the tile's containing flow object
	using Control::getContainer;
	using Control::setContainer;
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

	// instance type
	virtual const char* getType() const;
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pControl);
	static const char* type();
	void setAccessor(IAccessor<long> *);
private:
	void step();

	std::vector<Item>& _list;
	IAccessor<long>* _access;
	Reference<long> _inside;
	size_t _cursor;
    /// <param name="orient">orientation</param>
	align_t _align;
	long _ignore;
};

};