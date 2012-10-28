#include "Types.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace JSON
{
	struct Writer;
	struct Reader;
};

namespace Tiles
{

// Font
struct Font
{
	// font face name
	string_t _face;
	// font height
	meter_t _height;
	// style - italics, bold, etc.
	style_t _style;

	Font();
	Font(const Font &);
	Font(const TCHAR *face, meter_t height, style_t style);
	void toString(string_t &s);

	static bool save(JSON::Writer &writer, const char *name, const Font &desc);
	static bool load(JSON::Reader &json, const char *name, Font &desc);
};

};