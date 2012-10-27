#include "stdafx.h"
#include "JSON.h"
#include "Types.h"
#include "Arrow.h"
#include "Button.h"
#include "Check.h"
#include "Edit.h"
#include "Fill.h"
#include "Flow.h"
#include "Font.h"
#include "Glyph.h"
#include "Scroll.h"
#include "Text.h"
#include "Theme.h"
#include "../JSON/Reader.h"
#include "../Stream/FileInputStream.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

static bool loadFlowDesc(JSON::Reader &json, const char *name, FlowDesc &desc)
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

static bool loadFill(JSON::Reader &json, Theme& theme, ITile* &pDraw)
{
	bool bOK = false;
	identity_t id = 0;
	color_t fill = 0;
	FlowDesc horz, vert;
	do 
	{
		bOK = json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert) ||
			json.namedValue("fill", fill);
	}
	while (bOK && json.comma());
	if (bOK)
	{
		Fill *p = new Fill(id, theme);
		p->_fill.color = fill; // TODO
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pDraw = p;
	}
	return bOK;
}

static bool loadText(JSON::Reader &json, Theme& theme, ITile * &pDraw)
{
	bool bOK = false;
	FlowDesc horz, vert;
	std::string text, align;
	identity_t id = 0;
	do
	{
		bOK = json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert) ||
			json.namedValue("text", text) ||
			json.namedValue("align", align);
	}
	while (bOK && json.comma());
	if (bOK)
	{
		string_t wide;
		wide.resize(text.size() + 1, ' ');
		size_t used = 0;
		mbstowcs_s(&used, &wide[0], wide.size(), text.c_str(), text.size());
		wide.resize(used); 
		Theme::Font textFont = {Theme::eText, theme.Text};
		Text *p = new Text(id, theme, textFont, (align_t)orient(align), wide.c_str());
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pDraw = p;
	}
	return bOK;
}

static bool loadEdit(JSON::Reader &json, Theme& theme, IControl* &pControl)
{
	bool bOK = false;
	identity_t id = 0;
	FlowDesc horz, vert;
	do
	{
		bOK = json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert);
	}
	while (bOK && json.comma());

	if (bOK)
	{
		Theme::Font textFont = {Theme::eText, theme.Text};
		// accessor/reference binding to be filled in later.
		Edit *p = new Edit(id, theme, textFont, NULL);
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pControl = p;
	}
	return bOK;
}

static bool loadButton(JSON::Reader &json, Theme& theme, IControl* &pControl)
{
	bool bOK = false;
	identity_t id = 0;
	std::string text;
	FlowDesc horz, vert;
	Theme::Font desc = {Theme::eDefault, Font(_T("MS Shell Dlg"), 18, 0)};
	do
	{
		bOK = json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert) ||
			json.namedValue("text", text) ||
			Font::load(json, desc.font);
	}
	while (bOK && json.comma());
	if (bOK)
	{
		string_t wide;
		wide.resize(text.size() + 1, ' ');
		size_t used = 0;
		mbstowcs_s(&used, &wide[0], wide.size(), text.c_str(), text.size());
		wide.resize(used);
		Button *p = new Button(id, theme, desc, wide.c_str(), wide.c_str(), wide.c_str());
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pControl = p;
	}
	return bOK;
}

static bool loadArrow(JSON::Reader &json, Theme& theme, ITile* &pDraw)
{
	bool bOK = false;
	identity_t id = 0;
	std::string text;
	FlowDesc horz, vert;
	do
	{
		bOK = 
			json.namedValue("orient", text) ||
			json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert);
	}
	while (bOK && json.comma());

	if (bOK)
	{
		Arrow *p = new Arrow(id, theme, orient(text));
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pDraw = p;
	}
	return bOK;
}

static bool loadCheck(JSON::Reader &json, Theme& theme, IControl* &pControl)
{
	bool bOK = false;
	identity_t id = 0;
	std::string text;
	FlowDesc horz, vert;
	do
	{
		bOK = json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert);
	}
	while (bOK && json.comma());
	if (bOK)
	{
		Check *p = new Check(id, theme, NULL);
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pControl = p;
	}
	return bOK;
}

static bool loadScroll(JSON::Reader &json, Theme &theme, IControl* &pControl)
{
	bool bOK = false;
	identity_t id = 0;
	std::string text;
	FlowDesc horz, vert;
	do
	{
		bOK = json.namedValue("orient", text) ||
			json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert);
	}
	while (bOK && json.comma());
	if (bOK)
	{
		Scroll *p = new Scroll(id, theme, orient(text), NULL);
		p->setFlow(eRight, horz);
		p->setFlow(eDown, vert);
		pControl = p;
	}
	return bOK;
}

static bool loadUnknown(JSON::Reader &json, Theme &theme, Flow* &pFlow);

static bool loadFlow(JSON::Reader &json, Theme &theme, Flow* &pFlow)
{
	std::string text;
	identity_t id = 0;
	FlowDesc horz, vert;
	bool bOK = false;
	bool bOnce = false;

	do
	{
		bOK = json.namedValue("orient", text) ||
			json.namedValue("id", id) ||
			loadFlowDesc(json, "Horz", horz) ||
			loadFlowDesc(json, "Vert", vert);
		bOnce = bOK || bOnce;
	}
	while (bOK && json.comma());

	if (bOnce)
	{
		pFlow = new Flow(id, theme, orient(text));
		pFlow->setFlow(eRight, horz);
		pFlow->setFlow(eDown, vert);
		if ( json.beginNamedArray("Items") )
		{
			do
			{
				bOK = loadUnknown(json, theme, pFlow);
			}
			while ( bOK && json.comma() );
			if (bOK)
				bOK = json.endArray();
		}
		return bOK;
	}
	return false;
}

static bool loadUnknown(JSON::Reader &json, Theme &theme, Flow* &pFlow)
{
	bool bOK = false;
	if (json.beginObject() )
	{
		std::string type;
		if ( json.namedValue("type", type) &&
			json.comma() )
		{
			ITile* pDraw = NULL;
			IControl* pControl = NULL;
			if ( type.compare(Arrow::type()) == 0 )
			{
				bOK = loadArrow(json, theme, pDraw);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pDraw);
				}
			}
			else if ( type.compare(Button::type()) == 0 )
			{
				bOK = loadButton(json, theme, pControl);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pControl);
				}
			}
			else if ( type.compare(Check::type()) == 0 )
			{
				bOK = loadCheck(json, theme, pControl);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pControl);
				}
			}
			else if ( type.compare(Edit::type()) == 0 )
			{
				bOK = loadEdit(json, theme, pControl);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pControl);
				}
			}
			else if ( type.compare(Fill::type()) == 0 )
			{
				bOK = loadFill(json, theme, pDraw);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pDraw);
				}
			}
			else if ( type.compare(Flow::type()) == 0 )
			{
				Flow *pChild = NULL;
				bOK = loadFlow(json, theme, pChild);
				if (bOK)
				{
					pFlow->Add(pChild);
					//pChild->setIndex(pFlow->getShared()); // TODO
				}
			}
			else if ( type.compare(Scroll::type()) == 0 )
			{
				bOK = loadScroll(json, theme, pControl);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pControl);
				}
			}
			else if ( type.compare(Text::type()) == 0 )
			{
				bOK = loadText(json, theme, pDraw);
				_ASSERT(bOK);
				if (bOK)
				{
					pFlow->Add(pDraw);
				}
			}
		}
		json.endObject();
	}
	return bOK;
}

bool loadForm(const TCHAR *path, Theme &theme, Flow* &pFlow)
{
	JSON::Reader json;
	FileInputStream stream;

	std::string type;
	return stream.Open(path) &&
		json.open(&stream) &&
		json.beginNamedObject("Flow") &&
		json.namedValue("type", type) &&
		type.compare(Flow::type()) == 0 &&
		json.comma() &&
		loadFlow(json, theme, pFlow) &&
		json.endObject();
}
