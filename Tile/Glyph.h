#include "Tile.h"
#include "Theme.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Glyph : public Tile
{
	Glyph(identity_t id, Theme& theme, Theme::Font& desc, const TCHAR *text = _T("?"));
	virtual ~Glyph() { };
    /// <param name="canvas">Canvas where text will appear.</param>
	virtual bool Draw(ICanvas *canvas, bool bFocus);
	// get/set accessors for changed flag.
	using Tile::getChanged;
	using Tile::setChanged;
	// set callback for change notification.
	using Tile::watch;

	using Tile::identity;
	// instance type
	virtual const char* getType() const;
	using Tile::getMin;
	using Tile::setMin;
	// get/set accessors for layout maximums
	using Tile::getMax;
	using Tile::setMax;
	// get/set accessors for layout weights
	using Tile::getWeight;
	using Tile::setWeight;
	// get/set accessors for layout descriptors
	using Tile::getFlow;
	using Tile::setFlow;
	// get/set accessor for bounding rectangle.
	using Tile::setRect;
	using Tile::getRect;

	using Tile::contains;
	// serialize
	virtual bool save(JSON::Writer &writer);
	static const char* type();
private:
	// glyph index
	string_t _text;
};

};