#include "Control.h"
#include "Delegate.h"

/*
Copyright © 2011-2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Button : public Control
{
	// Click delegate - notifies when button clicked.
	sophia::delegate1<void, Button *> Click;
	// Click delegate - notifies when button is up or down.
	sophia::delegate2<void, Button *, bool> State;

	Button(identity_t id, Theme& theme, Theme::Font &desc, const TCHAR *text);
	Button(identity_t id, Theme& theme, Theme::Font &desc, const TCHAR *textUp, const TCHAR *textDown, const TCHAR *textOver);
	Button(identity_t id, Theme& theme, Theme::Font &desc, unsigned char iUp, unsigned char iDn, unsigned char iOver);
	virtual ~Button();

	// ITile implementation
	// instance type
	virtual const char* getType() const;
    /// <param name="canvas">canvas where this object will be drawn</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pButton);

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

	bool getTip(string_t &tip) const;
	void setTip(const TCHAR *tip);

	void setColorUp(const Theme::Color &fore, const Theme::Color &back);
	void setColorDn(const Theme::Color &fore, const Theme::Color &back);
	void setColorOver(const Theme::Color &fore, const Theme::Color &back);
	void setColorFocus(const Theme::Color &fore, const Theme::Color &back);

	static const char* type();

private:
	const string_t & text(bool focus);

	string_t _tip;
	IWindow *_popup;

	bool _down;
	Theme::Color _colorUp[2];
	Theme::Color _colorOver[2];
	Theme::Color _colorDn[2];
	Theme::Color _colorFocus[2];
	// glyphs for up, down, and hover
	string_t _textUp, _textOver, _textDown;
	unsigned char _iGlyphOver;
	unsigned char _iGlyphDn;
	unsigned char _iGlyphUp;
};

};