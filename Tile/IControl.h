#include "ITile.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

// mouse events
struct MouseEvent
{
	// place
	point_t _place;
	// move, down-click, up-click.
	enum {eMove = 1, eDownClick = 2, eUpClick = 3, eDoubleClick = 4};
	int _what;
	// left, middle, right.
	enum {eLeft = 1, eMiddle = 2, eRight = 4};
	int _button;
};

struct KeyEvent
{
	// event - up or down
	enum {UP = 1, DOWN = 2};
	int _what;
	// key code.
	int _code;
	// other keys (left/right shift, alt, control keys)
	enum {SHIFT=1, CONTROL=2, ALT=4, CAPS=8};
	int _mask;
};

struct IWindow;

struct IControl : public ITile
{
	// key event sink
	virtual bool dispatch(KeyEvent &action) = 0;
	// mouse event sink
	virtual bool dispatch(MouseEvent &action) = 0;
	// focus
	virtual bool getFocus() const = 0;
	virtual void setFocus(bool) = 0;
	// hover
	virtual void setHover(bool) = 0;

	virtual bool onNear(orient_t) = 0;
	virtual bool onFar(orient_t) = 0;
	virtual bool hasControls() const = 0;

	virtual void setDesktop(IWindow*) = 0;
	// readonly
	virtual bool getReadOnly() const = 0;
	virtual void setReadOnly(bool) = 0;
	// TODO: focus callback
};

};