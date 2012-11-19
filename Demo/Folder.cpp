#include "stdafx.h"
#include "Folder.h"
#include "../Tile/Grid.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Edit.h"
#include "../WTL/Snapshot.h"
#include <stdlib.h>

/*
Copyright © 2012 Rick Parrish
*/

FolderFrame::FolderFrame(Factory &factory) : 
	Frame(factory._theme), _factory(factory), 
	_tableSystems(factory._theme), _tableReceivers(factory._theme), 
	_serialSet(factory._theme), _soundSet(factory._theme),
	_themeSet(factory._theme)
{
	Theme &theme = factory._theme;
	theme.setHeight(14);

	_serial._data = 8;
	_serial._parity = 0;
	_serial._port = 1;
	_serial._rate = 115200;
	_serial._stop = 1;

	_serialSet.setValue(&_serial);


	_sound._bits = 16;
	_sound._channel = 0;
	_sound._channels = 1;
	_sound._port = 1;
	_sound._rate = 96000;

	_soundSet.setValue(&_sound);

	size_t columns = _tableSystems.getHeader()->Columns.size();

	_tableSystems.setReadOnly(false);

	for (size_t i = 0; i < columns; i++)
	{
		_tableSystems.setContent(i, &_factory._folder.Systems);
	}

	columns = _tableReceivers.getHeader()->Columns.size();

	_tableReceivers.setReadOnly(true);

	VCO *vco = new SignalVCO(0, false);

	Receiver *receiver = new Receiver(1);
	receiver->_label = _T("Signal");
	receiver->_VCOs.push_back(vco);
	vco->_receiver = receiver;
	_receivers.insert(receiver);

	vco = new VCO(0, true, false);
	
	receiver = new ::Control(2);
	receiver->_label = _T("Control");
	receiver->_VCOs.push_back(vco);
	vco->_receiver = receiver;
	_receivers.insert(receiver);

	vco = new SignalVCO(0, true);
	
	receiver = new ::Control(3);
	receiver->_label = _T("Both");
	receiver->_VCOs.push_back(vco);
	vco->_receiver = receiver;
	_receivers.insert(receiver);

	for (size_t i = 0; i < columns; i++)
	{
		_tableReceivers.setContent(i, &_receivers);
	}

	sophia::delegate2<void, Button*, bool> click;

	_top = new Pane(0, theme, eDown);
	_tabset = new Tab(0, theme);
	_tools = new Tab(0, theme);
	_list = new List(0, theme);

	click.bind(this, &FolderFrame::clickTheme);
	_tabset->Add(_T("Theme"), click);
	click.bind(this, &FolderFrame::serial);
	_tabset->Add(_T("Serial"), click);
	click.bind(this, &FolderFrame::sound);
	_tabset->Add(_T("Sound"), click);
	click.bind(this, &FolderFrame::clickSystems);
	_tabset->Add(_T("Systems"), click);
	click.bind(this, &FolderFrame::clickReceivers);
	_tabset->Add(_T("Receivers"), click);

	click.clear();

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	_tools->Add(L"\x231A", font_segoe, click, _T("Wrist watch")); // watch
	_tools->Add(L"\x231B", font_segoe, click, _T("Hourglass")); // hourglass
	_tools->Add(L"\x2328", font_segoe, click, _T("Keyboard")); // keyboard
	_tools->Add(L"\x2388", font_segoe, click, _T("Helm")); // Helm
	_tools->Add(L"\x2622", font_segoe, click, _T("Radioactive")); // radioactive
	_tools->Add(L"\x2623", font_segoe, click, _T("Biohazard")); // biohazard

	click.bind(this, &FolderFrame::clickHome);
	_tools->Add(L"\x48", font_webdings, click, _T("Home")); // house
	//_tools->Add(L"\x22", font_webdings, click); // web
	//_tools->Add(L"\x40", font_webdings, click); // tools
	click.bind(this, &FolderFrame::clickRefresh);
	_tools->Add(L"\x71", font_webdings, click, _T("Refresh")); // refresh
	click.bind(this, &FolderFrame::clickClear);
	_tools->Add(L"\x78", font_webdings, click, _T("Clear")); // X
	click.bind(this, &FolderFrame::clickAbout);
	_tools->Add(L"\x73", font_webdings, click, _T("About")); // ?

	_grid = new Grid(0, theme);
	_grid->setTable(&_tableSystems);

	_grid->DoubleClick.bind(this, &FolderFrame::clickSystem);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_grid, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	// connect the form to the frame window.
	setPane(_top);
}

void FolderFrame::clickSystem(Grid *, size_t row, size_t)
{
	Model::System *system = _factory._folder.Systems[row];
	_factory.activate(system);
}

void FolderFrame::clickReceiver(Grid *, size_t row, size_t)
{
	Receiver *receiver = _receivers[row];
	_factory.activate(receiver);
}

void FolderFrame::serial(Button*, bool up)
{
	if (up)
	{
		_top->clear();
		_list->clear();
		_list->setItems(&_serialSet);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void FolderFrame::sound(Button*, bool up)
{
	if (up)
	{
		_top->clear();
		_list->setItems(&_soundSet);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void FolderFrame::clickSystems(Button*, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->DoubleClick.bind(this, &FolderFrame::clickSystem);
		_grid->setTable(&_tableSystems);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_grid, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void FolderFrame::clickReceivers(Button*, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->DoubleClick.bind(this, &FolderFrame::clickReceiver);
		_grid->setTable(&_tableReceivers);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_grid, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void FolderFrame::clickHome(Button*, bool up)
{
	if (up)
		MessageBox(_T("Go Home!"), _T("Home"), MB_ICONINFORMATION|MB_OK);
}

void FolderFrame::clickAbout(Button*, bool up)
{
	if (up)
		MessageBox(_T("Copyright 2012"), _T("About Demo"), MB_ICONQUESTION|MB_OK);
}

void FolderFrame::clickRefresh(Button*, bool up)
{
	if (up)
		Invalidate(FALSE);
}

void FolderFrame::clickClear(Button*, bool up)
{
	if (up)
		_grid->clearSelect();
}

void FolderFrame::clickTheme(Button*, bool up)
{
	if (up)
	{
		_top->clear();
		_list->setItems(&_themeSet);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

bool FolderFrame::Create(RECT rect)
{
	return Window::Create(NULL, rect, _T("Demo"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
