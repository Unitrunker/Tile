#include "Control.h"
#include "IWindow.h"
#include "Accessor.h"
#include "Delegate.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

#pragma once

struct Edit : public Control
{
	// Select delegate - notifies when input changes.
	sophia::delegate2<void, Edit *, const string_t &> Select;

	Edit(identity_t id, Theme &theme, const Theme::Font &font, IAccessor<string_t> *);
	Edit(identity_t id, Theme &theme, const Theme::Font &font, string_t &text);
	virtual ~Edit() { };
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// Import these methods from the Control base class
	// get/set accessors for changed flag.
	using Control::getChanged;
	using Control::setChanged;
	// set callback for change notification.
	using Control::watch;
	// hover
	virtual void setHover(bool hover);
	// focus
	using Control::getFocus;
	virtual void setFocus(bool focus);
	// readonly
	using Control::getReadOnly;
	using Control::setReadOnly;
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
	static const char* type();

	void setAccessor(IAccessor<string_t> *pAccess);
	void setLocal(bool);

protected:
	bool onBackspace();
	bool onDelete();
	bool onChar(TCHAR ch);
	bool onCursorLeft();
	bool onCursorRight();
	void setIndex(size_t);
	size_t getIndex() const;

	// working copy of text
	string_t _text;
    /// <param name="text">get/set accessor for text content</param>
	IAccessor<string_t> *_access;
	// cursor column position
	size_t _cursor;
	bool _local; 
	/// <param name="orient">orientation</param>
	align_t _align;
	/// border width
	meter_t _border;
	// color selections
	Theme::Color _fore[2], _back[2];
};

};