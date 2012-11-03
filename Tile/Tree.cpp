#include "stdafx.h"
#include "Tree.h"
#include "IWindow.h"
#include "JSON.h"
#include "Factory.h"

/*
Copyright © 2011-2012 Rick Parrish
*/

using namespace Tiles;

Tree::Tree(identity_t id, Theme &theme, const Theme::Font &font, const TCHAR *label) :
	Pane(id, theme, eDown), _checked(true),
	_header(0, theme, eRight), 
	_control(0, theme, this, eLeft),
	_label(0, theme, font, eLeft, label)
{
	Flow text = {0, 4096, 1, false};
	//Flow edit = {0, 4096, 3, false};
	Flow line = {1, 1, 0, true};
	Flow coil = {0, 4096, 0, false};

	setFlow(eDown, coil);
	setFlow(eRight, coil);

	_label.setFlow(eRight, text);
	_control.setGlyphs(theme.Collapse.c_str(), theme.Expand.c_str());
	_control.setAlign(eLeft|eRight);
	_header.Add(&_control);
	_header.Add(&_label);
	_header.setFlow(eDown, line);
	Pane::Add(&_header);

	_thick.local = true;
	_thick.thick = 0;
}

Tree::~Tree()
{
	_header.clear();
	// remove the first tile - which is a pane holding our checkbox and text label.
	_listTiles.erase(_listTiles.begin());
}

// instance type
const char* Tree::getType() const { return Tree::type(); }

const char* Tree::type()
{
	return "Tree";
}

const bool& Tree::getValue() const
{
	return _checked;
}

bool Tree::setValue(const bool &value)
{
	_checked = value;
	if (value)
	{
		for (size_t i = 0; i < _listHidden.size(); i++)
		{
			IControl *pControl = _listHidden[i];
			Add(pControl);
		}
		_listHidden.clear();
	}
	else
	{
		hideControls();
	}
	Pane *climb = this;
	while ( climb->getContainer() != NULL )
		climb = climb->getContainer();
	climb->reflow();
	IControl *null = NULL;
	// toggle focus to force ourselves into view.
	_pDesktop->setFocus(null);
	//_pDesktop->setFocus(this);
	_pDesktop->setFocus(&_control);
	return true;
}

void Tree::hideControls()
{
	if (_listControls.size() <= 1)
		return;

	std::vector<IControl*> list(_listControls.begin() + 1, _listControls.end());
	_listControls.resize(1);
	for (size_t i = 0; i < list.size(); i++)
	{
		_listHidden.push_back(list[i]);
		ITile *pTile = list[i];
		for (size_t j = 0; j < _listTiles.size(); j++)
		{
			if (_listTiles[j] == pTile)
			{
				_listTiles.erase(_listTiles.begin() + j, _listTiles.begin() + j + 1);
				break;
			}
		}
	}
}

size_t Tree::Add(IControl *pControl)
{
	if (_checked)
	{
		Pane::Add(pControl);
	}
	else
	{
		_listHidden.push_back(pControl);
	}
	return 0;
}

// serialize
bool Tree::save(JSON::Writer &writer)
{
	std::string name;
	orient(name, _flow);
	writer.writeStartObject();
	writer.writeNamedValue("type", type());
	//writer.writeNamedValue("orient", name.c_str()); // always same.
	_tile.save(writer);
	writer.writeStartNamedArray("Items");
	// Note: the tiles list includes both tiles and controls so no need to skim the controls list.
	std::vector<IControl *>::iterator it;
	std::vector<IControl *>::iterator end;

	// enumerate controls

	if (_checked)
	{
		// controls are visible.
		// skip the internal Checkbox.
		it = _listControls.begin() + 1;
		end = _listControls.end();
	}
	else
	{
		// controls are hidden.
		it = _listHidden.begin();	
		end = _listHidden.end();
	}

	while (it != end)
	{
		ITile *p = *it;
		p->save(writer);
		it++;
	}
	writer.writeEndArray();
	writer.writeEndObject(true);
	return true;
}

// de-serialize
bool Tree::load(JSON::Reader &reader, Theme &theme, const char *type, IControl *&pControl)
{
	bool bOK = false;
	if ( !strcmp(type, Tree::type()) )
	{
		string_t text;
		identity_t id = 0;
		Flow horz, vert;
		Theme::Font font = { Theme::eDefault, theme.Text };
		bool bOnce = false;

		do
		{
			bOK = reader.namedValue("orient", text) || // orientation - if given - is ignored.
				reader.namedValue("id", id) ||
				loadFlow(reader, "Horz", horz) ||
				loadFlow(reader, "Vert", vert) ||
				reader.namedValue("text", text) ||
				loadFont(reader, "Font", font);

			bOnce = bOK || bOnce;
		}
		while (bOK && reader.comma());

		if (bOnce)
		{
			Tree *pTree = new Tree(id, theme, font, text.c_str());
			pTree->setFlow(eRight, horz);
			pTree->setFlow(eDown, vert);
			if ( reader.beginNamedArray("Items") )
			{
				do
				{
					Pane *pPane = pTree;
					bOK = loadUnknown(reader, theme, pPane);
				}
				while ( bOK && reader.comma() );
				if (bOK)
					bOK = reader.endArray();
			}
			if (bOK)
				pControl = pTree;
		}
	}
	return bOK;
}
