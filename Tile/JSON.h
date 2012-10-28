#include <string>
#include "../Tile/Theme.h"
#include "../Tile/Pane.h"
#include "../JSON/Writer.h"
#include "../JSON/Reader.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

bool loadThick(JSON::Reader &reader, const char *name, Tiles::Theme::Thick &thick);
bool loadGlyph(JSON::Reader &reader, const char *name, Tiles::Theme::Glyph &glyph);
bool loadColor(JSON::Reader &reader, const char *name, Tiles::Theme::Color &color);
bool loadFont(JSON::Reader &reader, const char *name, Tiles::Theme::Font &font);
bool loadForm(const TCHAR *path, Tiles::Theme &theme, Tiles::Pane* &pPane);
bool loadFlow(JSON::Reader &json, const char *name, Tiles::Flow &desc);

bool saveColor(JSON::Writer &writer, const char *label, const Tiles::Theme::Color &color, bool eol = false);
bool saveGlyph(JSON::Writer &writer, const char *label, const Tiles::Theme::Glyph &glyph, bool eol = false);
bool saveFont(JSON::Writer &writer, const char *label, const Tiles::Theme::Font &font, bool eol = false);
bool saveThick(JSON::Writer &writer, const char *label, const Tiles::Theme::Thick &thick);
