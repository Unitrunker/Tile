#include "Pane.h"
#include "Button.h"
#include <vector>

/*
Copyright © 2011, 2012 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct TabItem
{
	TCHAR *_label;
	sophia::delegate2<void, Button*, bool> _click;
};

struct Tab : public Pane
{
	// Create a grid control.
	Tab(identity_t id, Theme &theme);

	bool Add(const TCHAR *label, sophia::delegate2<void, Button*, bool> &click);
	bool Populate(std::vector<TabItem> &list);

private:
	std::vector<Button*> _items;
};

};