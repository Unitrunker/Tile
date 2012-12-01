#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"
#include "Model.h"
#include "System.h"
#include "Receiver.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct FolderFrame : public Frame
{
	FolderFrame(Factory &factory);
	bool Create(RECT rect);
private:
	void clickSystem(Grid *, size_t row, size_t col);
	void clickReceiver(Grid *, size_t row, size_t col);
	void serial(Button*);
	void sound(Button*);
	void clickTheme(Tab*);
	void clickSystems(Tab*);
	void clickReceivers(Tab*);
	void clickHome(Button*);
	void clickAbout(Button*);
	void clickRefresh(Button*);
	void clickClear(Button*);
	void clickEdit(Button*);
	void clickSystemMinus(Button*);
	void clickSystemPlus(Button*);
	void clickSystemInspect(Button*);
	void clickReceiverMinus(Button*);
	void clickReceiverPlus(Button*);
	void clickReceiverInspect(Button*);

	Pane *_top;
	Pane *_tabset;
	Pane *_toolInfo;
	Pane *_toolSystem;
	Pane *_toolReceiver;
	Grid *_grid;
	List *_list;
	Receivers _receivers;
	Table<ReceiverSet, Receiver> _tableReceivers;
	Table<SystemSet, Model::System> _tableSystems;
	Factory& _factory;
	ThemeSet _themeSet;
};
