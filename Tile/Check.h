#include "Control.h"
#include "Accessor.h"
#include "Delegate.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Check : public Control
{
	// Select delegate - notifies when selection changes.
	sophia::delegate2<void, Check *, bool> Select;

	Check(identity_t id, Theme &theme, IAccessor<bool> *, align_t align = eLeft);
	Check(identity_t id, Theme &theme, bool&, align_t align = eLeft);
	virtual ~Check() { };

	// ITile implementation
	// instance type
	virtual const char* getType() const;
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pControl);

	// IControl implementation
	// key event sink
	virtual bool dispatch(KeyEvent &action);
	// mouse event sink
	virtual bool dispatch(MouseEvent &action);

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
	// get/set font selector.
	using Control::getFont;
	using Control::setFont;

	static const char* type();
	// Check box specific methods
	void setAccessor(IAccessor<bool>*);
	void setGlyphs(const TCHAR *checked, const TCHAR *unchecked);
	void setFore(const Theme::Color &normal, const Theme::Color &focus);
	void setBack(const Theme::Color &normal, const Theme::Color &focus);
	void setAlign(align_t align);
private:
    void moveCursor(size_t i);
	bool onBackspace();
	bool onDelete();
	bool onChar(TCHAR ch);
	bool onCursorLeft();
	bool onCursorRight();

	IAccessor<bool>* _access;
	/// checked state
	bool _checked;
	size_t _cursor;
    /// <param name="orient">orientation</param>
	align_t _align;

	Theme::Color _fore[2], _back[2];
	Theme::Glyph _glyphs[2];
};

};