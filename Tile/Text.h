#include "Tile.h"
#include "Theme.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct Text : public Tile
{
    /// <param name="orient">orientation</param>
	align_t _align;
    /// <param name="text">text string to display in the designated font.</param>
	string_t _text;
	// color selection
	Theme::Color _fore, _back;

	Text(identity_t id, Theme &theme);
	Text(identity_t id, Theme &theme, Theme::Font& font);
	Text(identity_t id, Theme &theme, Theme::Font& font, align_t align, const TCHAR *text);
	Text(const Text &copy);
	virtual ~Text() { };
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

	// accessors for scrollable area.
	using Tile::getScrollBox;
	using Tile::setScrollBox;
	using Tile::getScrollArea;
	using Tile::setScrollArea;
	using Tile::getScrollPoint;
	using Tile::setScrollPoint;

	using Tile::contains;
	// serialize
	virtual bool save(JSON::Writer &writer);
	// de-serialize
	static bool load(JSON::Reader &reader, Theme &theme, const char *type, ITile *&pText);
	static const char* type();
};

};