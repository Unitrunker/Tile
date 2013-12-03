#include "Control.h"
#include "Delegate.h"

/*
Copyright © 2011-2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

// Tab is essentially a button specialized for use as a tab in a tab control.
// A plain Pane serves as the tab container.
// The only event supported is the click. No up/down state.
// Optional tool tip text.
struct Tab : public Control
{
	// Click delegate - notifies when tab clicked.
	sophia::delegate1<void, Tab *> Click;

	Tab(identity_t id, Theme& theme, Theme::Font &desc, const TCHAR *text, const TCHAR *tip = NULL);
	virtual ~Tab();

	// ITile implementation
	// instance type
	virtual const char* getType() const;
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);

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
	virtual void setHover(bool hover);
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

	bool getTip(string_t &tip) const;
	void setTip(const TCHAR *tip);

	const string_t &getText() const;
	void setText(const TCHAR *tip);

	void setColorIdle(const Theme::Color &fore, const Theme::Color &back);
	void setColorOver(const Theme::Color &fore, const Theme::Color &back);
	void setColorSelect(const Theme::Color &fore, const Theme::Color &back);

	void getColorIdle(Theme::Color &fore, Theme::Color &back) const;
	void getColorOver(Theme::Color &fore, Theme::Color &back) const;
	void getColorSelect(Theme::Color &fore, Theme::Color &back) const;

	static const char* type();

	string_t _tip;
	Theme::Color _colorIdle[2];
	Theme::Color _colorOver[2];
	Theme::Color _colorSelect[2];
	// text label
	string_t _text;

private:
	IWindow *_popup;

};

};