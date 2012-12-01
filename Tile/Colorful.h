#include "Theme.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

// similar to an accessor but read-only.
struct IColorAccess
{
	// retrieve color.
	virtual const color_t getValue(const Theme &theme, bool focus) const = 0;
};

// normal vs. focus color selection logic.
struct ColorAccess : public IColorAccess
{
	ColorAccess(Theme::Color &normal, Theme::Color& focus) : 
		_normal(normal), _focus(focus) { };

	// retrieve color.
	const color_t getValue(const Theme &theme, bool focus) const
	{
		return theme.getColor(focus ? _focus : _normal);
	}

private:
	ColorAccess & operator = (const ColorAccess &never);
	Theme::Color &_normal;
	Theme::Color &_focus;
};

// Feature class - adds foreground and background color.
// Retrieval of colors can be diverted elsewhere via ColorAccess.
// Some controls may need more dynamic control of color. 
// Examples: flash / fade effects; property sets where the underlying data specifies color.
struct Colorful
{
	Colorful();

	void setForeAccess(IColorAccess *fore);
	void setBackAccess(IColorAccess *back);

protected:
	// color selections
	Theme::Color _foreView;
	Theme::Color _foreEdit;
	Theme::Color _backView;
	Theme::Color _backEdit;

	ColorAccess _foreLocal;
	ColorAccess _backLocal;

	IColorAccess* _foreAccess;
	IColorAccess* _backAccess;
};

};