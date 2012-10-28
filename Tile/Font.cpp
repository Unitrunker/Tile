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

bool Font::save(JSON::Writer &writer, const char *name, const Font &desc)
{
	std::string face;
	size_t used = 0;

	face.resize(desc._face.size() + 2, ' ');
	wcstombs_s(&used, &face[0], face.size(), desc._face.c_str(), desc._face.size());
	face.resize(used);

	writer.writeStartNamedObject(name);
	writer.writeNamedValue("face", face.c_str());
	writer.writeNamedValue("height", desc._height);
	writer.writeEndObject();
	return true;
}

bool Font::load(JSON::Reader &json, const char *name, Font &desc)
{
	bool bOK = false;
	if ( json.beginNamedObject(name) )
	{
		std::string face;
		bOK = json.namedValue("face", face) &&
			json.comma() &&
			json.namedValue("height", desc._height) &&
			json.endObject();
		if (bOK)
		{
			string_t wide;
			wide.resize(face.size() + 1, ' ');
			size_t used = 0;
			mbstowcs_s(&used, &wide[0], wide.size(), face.c_str(), face.size());
			wide.resize(used); 
			desc._face = wide;
		}
	}
	return bOK;
}

void Font::toString(string_t &s)
{
	TCHAR work[16] = {0};
	s.clear();
	s.append(_face);
	wsprintf(work, _T(" %ldpt"), _height);
	s.append(work);
}
