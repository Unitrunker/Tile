#include "stdafx.h"
#include "Tab.h"
#include "Button.h"
#include "Fill.h"
#include "IWindow.h"
#include "JSON.h"

/*
Copyright © 2011, 2012 Rick Parrish
*/

using namespace Tiles;

Tab::Tab(identity_t id, Theme &theme) : Pane(id, theme, eRight)
{
}

bool Tab::Add(const TCHAR *label, sophia::delegate2<void, Button*, bool> &click)
{
	Theme &theme = _tile.getTheme();
	Theme::Font font = { Theme::eText, theme.Text };
	return Add(label, font, click);
}

bool Tab::Add(const TCHAR *label, Theme::Font &font, sophia::delegate2<void, Button*, bool> &click)
{
	Theme &theme = _tile.getTheme();
	Flow flow = {1, 1, 0, true};
	Button *button = new Button(0, theme, font, label);
	button->setFlow(eDown, flow);
	button->Click = click;
	Pane::Add(button, 0, 4096, 1, false);
	return true;
}

bool Tab::Populate(std::vector<TabItem> &list)
{
	for(std::vector<TabItem>::iterator it = list.begin(); it != list.end(); it++)
	{
		Add(it->_label, it->_click);
	}
	return true;
}
