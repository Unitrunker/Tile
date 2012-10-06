#include "stdafx.h"
#include <map>
#include <functional>
#include "Cache.h"

/*
Copyright 2011 Rick Parrish
*/

static bool operator < (const Tiles::Font &left, const Tiles::Font &right)
{
	int compare = left._face.compare(right._face);
	if (compare == 0)
		compare = left._height - right._height;
	if (compare == 0)
		compare = left._style - right._style;
	return compare < 0;
}

struct less
{
	bool operator()(const Tiles::Font& _Left, const Tiles::Font& _Right) const
	{
		return _Left < _Right;
	}
};

// cached font factory.
HFONT makeFont(const Tiles::Font &desc)
{
	typedef std::map<Tiles::Font, HFONT, less> cache_t;
	static cache_t cache;

	cache_t::iterator it = cache.find(desc);
	if (it == cache.end())
	{
		LOGFONT log = {0};
		//HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		//GetObject(hFont, sizeof log, &log);
		log.lfHeight = desc._height;
		log.lfWeight = FW_NORMAL;
		log.lfCharSet = (desc._style & 1) ? SYMBOL_CHARSET : ANSI_CHARSET;
		_tcsncpy_s(log.lfFaceName, _countof(log.lfFaceName), desc._face.c_str(), desc._face.size());
		HFONT hFont = CreateFontIndirect(&log);
		cache[desc] = hFont;
		return hFont;
	}
	else
		return it->second;
}

// TODO: pen and brush caches for WTL canvas.