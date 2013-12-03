#include "stdafx.h"
#include "Font.h"
#include "../JSON/Writer.h"
#include "../JSON/Reader.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Font::Font() :
	_face(_T("MS Shell Dlg")),
	_height(24),
	_style(0)
{
}

Font::Font(const Font &copy) :
	_face(copy._face),
	_height(copy._height),
	_style(copy._style)
{
}

Font::Font(const TCHAR *face, meter_t height, style_t style) :
	_face(face),
	_height(height),
	_style(style)
{
}

void Font::toString(string_t &s)
{
	TCHAR work[16] = {0};
	s.clear();
	s.append(_face);
	wsprintf(work, _T(" %ldpt"), _height);
	s.append(work);
}
