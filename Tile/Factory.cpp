#include "stdafx.h"
#include "Factory.h"
#include "Arrow.h"
#include "Button.h"
#include "Check.h"
#include "Edit.h"
#include "Fill.h"
#include "Grid.h"
#include "List.h"
#include "Scroll.h"
#include "Text.h"
#include "Tree.h"
#include "Toggle.h"
#include "JSON.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

bool loadUnknown(JSON::Reader &json, Theme &theme, Pane* &pPane)
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
			Pane *pChild = NULL;

			if ( Arrow::load(json, theme, type.c_str(), pDraw) )
			{
				pPane->Add(pDraw);
			}
			else if ( Button::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( Check::load(json, theme, type.c_str(), pControl ) )
			{
				pPane->Add(pControl);
			}
			else if ( Edit::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( Fill::load(json, theme, type.c_str(), pDraw) )
			{
				pPane->Add(pDraw);
			}
			else if ( Grid::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( List::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( Pane::load(json, theme, type.c_str(), pChild) )
			{
				pPane->Add(pChild);
			}
			else if ( Scroll::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( Text::load(json, theme, type.c_str(), pDraw) )
			{
				pPane->Add(pDraw);
			}
			else if ( Toggle::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else if ( Tree::load(json, theme, type.c_str(), pControl) )
			{
				pPane->Add(pControl);
			}
			else
				throw __LINE__;
		}
		json.endObject();
	}
	return bOK;
}

};