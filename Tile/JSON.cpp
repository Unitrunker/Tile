#include "stdafx.h"
#include "JSON.h"
#include "Types.h"
#include "Arrow.h"
#include "Button.h"
#include "Check.h"
#include "Edit.h"
#include "Fill.h"
#include "Pane.h"
#include "Font.h"
#include "Glyph.h"
#include "Scroll.h"
#include "Text.h"
#include "Theme.h"
#include "../JSON/Reader.h"
#include "../Stream/FileInputStream.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

bool loadThick(JSON::Reader &reader, const char *name, Tiles::Theme::Thick &thick)
{
	long iValue = 0;
	if ( reader.namedValue(name, iValue) )
	{
		thick.local = true;
		thick.thick = (meter_t)iValue;
		return true;
	}
	return false;
}

bool loadGlyph(JSON::Reader &reader, const char *name, Tiles::Theme::Glyph &glyph)
{
	bool bOK = false;
	if ( reader.beginNamedObject(name) )
	{
		long iValue = 0;
		if ( reader.namedValue("index", iValue) )
		{
			bOK = true;
			glyph.index = (unsigned char)iValue;
		}
		else bOK = reader.namedValue("value", glyph.glyph);
		if (bOK)
			bOK = reader.endObject();
	}
	return bOK;
}

bool loadColor(JSON::Reader &reader, const char *name, Tiles::Theme::Color &color)
{
	bool bOK = false;
	if ( reader.beginNamedObject(name) )
	{
		long iValue = 0;
		if ( reader.namedValue("index", iValue) )
		{
			bOK = true;
			color.index = (unsigned char)iValue;
		}
		else if (reader.namedValue("color", iValue) )
		{
			bOK = true;
			color.color = (color_t)iValue;
		}
		if (bOK)
			bOK = reader.endObject();
	}
	return bOK;
}

bool loadFont(JSON::Reader &reader, const char *name, Theme::Font &font)
{
	bool bOK = reader.beginNamedObject(name);
	if (bOK)
	{
		long iValue = 0;

		if ( reader.namedValue("index", iValue) )
		{
			font.index = (unsigned char)iValue;
		}
		else 
		{
			do
			{
				bOK = reader.namedValue("face", font.font._face) ||
					reader.namedValue("height", iValue);
			}
			while (bOK && reader.comma());
			if (bOK)
			{
				font.font._height = (meter_t)iValue;
			}
		}
		if (bOK)
			bOK = reader.endObject();
	}
	return bOK;
}

bool loadFlow(JSON::Reader &json, const char *name, Flow &desc)
{
	bool bOK = json.beginNamedObject(name);
	if (bOK) do
	{
		bOK = json.namedValue("min", desc._min) ||
			json.namedValue("max", desc._max) ||
			json.namedValue("weight", desc._weight);
	}
	while (bOK && json.comma());
	if (bOK)
		bOK = json.endObject();

	return bOK;
}

bool loadForm(const TCHAR *path, Theme &theme, Pane* &pPane)
{
	JSON::Reader json;
	FileInputStream stream;

	std::string type;
	return stream.Open(path) &&
		json.open(&stream) &&
		json.beginNamedObject("Pane") &&
		json.namedValue("type", type) && 
		Pane::load(json, theme, type.c_str(), pPane) &&
		json.endObject();
}

bool saveColor(JSON::Writer &writer, const char *label, const Theme::Color &color, bool eol)
{
	writer.writeStartNamedObject(label);
	if (color.index == Theme::eDefault)
		writer.writeNamedValue("color", color.color, 16);
	else
		writer.writeNamedValue("index", color.index, 10);
	writer.writeEndObject(eol);
	return true;
}

bool saveFont(JSON::Writer &writer, const char *label, const Theme::Font &font, bool eol)
{
	if (font.index == Theme::eDefault)
	{
		Font::save(writer, label, font.font);
	}
	else
	{
		writer.writeStartNamedObject(label);
		writer.writeNamedValue("index", font.index, 10);
		writer.writeEndObject(eol);
	}
	return true;
}

bool saveGlyph(JSON::Writer &writer, const char *label, const Theme::Glyph &glyph, bool eol)
{
	writer.writeStartNamedObject(label);
	if (glyph.index == Theme::eDefault)
	{
		writer.writeNamedValue("glyph", glyph.glyph.c_str());
	}
	else
	{
		writer.writeNamedValue("index", glyph.index, 10);
	}
	writer.writeEndObject(eol);
	return true;
}

bool saveThick(JSON::Writer &writer, const char *label, const Theme::Thick &thick)
{
	if (thick.local)
		return writer.writeNamedValue(label, thick.thick, 10);
	return true;
}
