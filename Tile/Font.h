#include "Types.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

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
};

};