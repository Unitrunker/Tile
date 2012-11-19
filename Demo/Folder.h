#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../WTL/Frame.h"
#include "Model.h"
#include "System.h"
#include "Serial.h"
#include "Sound.h"
#include "Receiver.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct FolderFrame : public Frame
{
	Serial _serial;
	Sound _sound;
	FolderFrame(Factory &factory);
	bool Create(RECT rect);
private:
	void clickSystem(Grid *, size_t row, size_t col);
	void clickReceiver(Grid *, size_t row, size_t col);
	void serial(Button*, bool);
	void sound(Button*, bool);
	void clickSystems(Button*, bool);
	void clickReceivers(Button*, bool);
	void clickHome(Button*, bool);
	void clickAbout(Button*, bool);
	void clickRefresh(Button*, bool);
	void clickClear(Button*, bool up);
	void clickTheme(Button*, bool up);

	Pane *_top;
	Tab *_tabset;
	Tab *_tools;
	Grid *_grid;
	List *_list;
	Receivers _receivers;
	Table<ReceiverSet, Receiver> _tableReceivers;
	Table<SystemSet, Model::System> _tableSystems;
	Factory& _factory;
	SerialSet _serialSet;
	SoundSet _soundSet;
	ThemeSet _themeSet;
};
