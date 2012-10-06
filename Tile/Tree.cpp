#include "stdafx.h"
#include "Tree.h"
#include "IWindow.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

Tree::Tree(identity_t id, Theme &theme) :
	Flow(id, theme, eDown), _checked(true)
{
	_thick.local = true;
	_thick.thick = 0;
}

Tree::~Tree()
{
	// Remove property set controls from tile list so that they are not destroyed in Flow's 
	// destructor. The property set controls are owned by the property set, not this control.
	hideControls();
	_listHidden.clear();
}

// instance type
const char* Tree::getType() const { return "Tree"; }

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
	Flow *climb = this;
	while ( climb->getContainer() != NULL )
		climb = climb->getContainer();
	climb->reflow();
	// toggle focus to force ourselves into view.
	_pDesktop->setFocus(NULL);
	//_pDesktop->setFocus(this);
	_pDesktop->setFocus(_control);
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
		Flow::Add(pControl);
	}
	else
	{
		_listHidden.push_back(pControl);
	}
	return 0;
}

