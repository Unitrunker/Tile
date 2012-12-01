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
	_themeSet(factory._theme)
{
	Theme &theme = factory._theme;
	theme.setHeight(14);

	size_t columns = _tableSystems.getHeader()->Columns.size();

	_tableSystems.setEnable(true);

	for (size_t i = 0; i < columns; i++)
	{
		_tableSystems.setContent(i, &_factory._folder.Systems);
	}

	columns = _tableReceivers.getHeader()->Columns.size();

	_tableReceivers.setEnable(true);

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

	Tab* tab = NULL;
	Button* button = NULL;

	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme, eRight);
	_toolInfo = new Pane(0, theme, eRight);
	_toolSystem = new Pane(0, theme, eRight);
	_toolReceiver = new Pane(0, theme, eRight);
	_list = new List(0, theme);
	_list->setItems(&_themeSet);

	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_toolInfo->setLineColor(color);
	_toolSystem->setLineColor(color);
	_toolReceiver->setLineColor(color);

	Theme::Font text = {Theme::eText, theme.Text};

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font wingdings(_T("Wingdings"), 24, 1);
	Theme::Font font_wingdings = { Theme::eDefault, wingdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	tab = new Tab(0, theme, text, _T("Theme"));
	tab->Click.bind(this, &FolderFrame::clickTheme);
	_tabset->Add(tab);

	tab = new Tab(0, theme, text, _T("Systems"));
	tab->Click.bind(this, &FolderFrame::clickSystems);
	_tabset->Add(tab);
	_tabset->setIndex(1);

	tab = new Tab(0, theme, text, _T("Receivers"));
	tab->Click.bind(this, &FolderFrame::clickReceivers);
	_tabset->Add(tab);

	//_tools->Add(L"\x231A", font_segoe, click, _T("Wrist watch")); // watch
	//_tools->Add(L"\x231B", font_segoe, click, _T("Hourglass")); // hourglass
	//_tools->Add(L"\x2328", font_segoe, click, _T("Keyboard")); // keyboard
	//_tools->Add(L"\x2388", font_segoe, click, _T("Helm")); // Helm
	//_tools->Add(L"\x2622", font_segoe, click, _T("Radioactive")); // radioactive
	//_tools->Add(L"\x2623", font_segoe, click, _T("Biohazard")); // biohazard

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &FolderFrame::clickHome);
	button->setTip(_T("Home"));
	_toolInfo->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &FolderFrame::clickHome);
	button->setTip(_T("Home"));
	_toolSystem->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &FolderFrame::clickHome);
	button->setTip(_T("Home"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_wingdings, _T("\x3F"));
	button->Click.bind(this, &FolderFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_wingdings, _T("\x3F"));
	button->Click.bind(this, &FolderFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_wingdings, _T("\x3F"));
	button->Click.bind(this, &FolderFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &FolderFrame::clickSystemPlus);
	button->setTip(_T("Add"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &FolderFrame::clickReceiverPlus);
	button->setTip(_T("Add"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &FolderFrame::clickSystemMinus);
	button->setTip(_T("Remove"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &FolderFrame::clickReceiverMinus);
	button->setTip(_T("Remove"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &FolderFrame::clickSystemInspect);
	button->setTip(_T("Inspect"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &FolderFrame::clickReceiverInspect);
	button->setTip(_T("Inspect"));
	_toolReceiver->Add(button);

	//_tools->Add(L"\x22", font_webdings, click); // web
	//_tools->Add(L"\x40", font_webdings, click); // tools

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &FolderFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &FolderFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &FolderFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x78"));
	button->Click.bind(this, &FolderFrame::clickClear);
	button->setTip(_T("Clear"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x78"));
	button->Click.bind(this, &FolderFrame::clickClear);
	button->setTip(_T("Clear"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x78"));
	button->Click.bind(this, &FolderFrame::clickClear);
	button->setTip(_T("Clear"));
	_toolReceiver->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x73"));
	button->Click.bind(this, &FolderFrame::clickAbout);
	button->setTip(_T("About"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x73"));
	button->Click.bind(this, &FolderFrame::clickAbout);
	button->setTip(_T("About"));
	_toolSystem->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x73"));
	button->Click.bind(this, &FolderFrame::clickAbout);
	button->setTip(_T("About"));
	_toolReceiver->Add(button);

	_grid = new Grid(0, theme);
	_grid->setTable(&_tableSystems);

	_grid->DoubleClick.bind(this, &FolderFrame::clickSystem);

	_top->Add(_toolSystem, 1, 1, 0, true);
	_top->Add(_grid, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	// connect the form to the frame window.
	setPane(_top);
}

void FolderFrame::clickSystem(Grid *, size_t row, size_t)
{
	// TODO: is this a hack?
	row = _tableSystems.getAscendingIndex(row);
	Model::System *system = _factory._folder.Systems[row];
	_factory.activate(system);
}

void FolderFrame::clickReceiver(Grid *, size_t row, size_t)
{
	// TODO: is this a hack?
	row = _tableReceivers.getAscendingIndex(row);
	Receiver *receiver = _receivers[row];
	_factory.activate(receiver);
}

void FolderFrame::clickTheme(Tab*)
{
	_top->clear();
	_top->Add(_toolInfo, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	_top->reflow();
}

void FolderFrame::clickSystems(Tab*)
{
	_top->clear();
	_grid->DoubleClick.bind(this, &FolderFrame::clickSystem);
	_grid->setTable(&_tableSystems);
	_top->Add(_toolSystem, 1, 1, 0, true);
	_top->Add(_grid, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	_top->reflow();
}

void FolderFrame::clickReceivers(Tab*)
{
	_top->clear();
	_grid->DoubleClick.bind(this, &FolderFrame::clickReceiver);
	_grid->setTable(&_tableReceivers);
	_top->Add(_toolReceiver, 1, 1, 0, true);
	_top->Add(_grid, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	_top->reflow();
}

void FolderFrame::clickHome(Button*)
{
	MessageBox(_T("Go Home!"), _T("Home"), MB_ICONINFORMATION|MB_OK);
}

void FolderFrame::clickSystemMinus(Button*)
{
	std::vector<Model::System*> list;
	_factory._folder.Systems.getSelected(list);
	_factory._folder.Systems.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void FolderFrame::clickSystemPlus(Button*)
{
	_factory.activateSystem();
}

void FolderFrame::clickSystemInspect(Button*)
{
	std::vector<Model::System*> list;
	_factory._folder.Systems.getSelected(list);
	_factory.activate(list);
}

void FolderFrame::clickReceiverMinus(Button*)
{
	std::vector<Receiver*> list;
	_receivers.getSelected(list);
	_receivers.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void FolderFrame::clickReceiverPlus(Button*)
{
}

void FolderFrame::clickReceiverInspect(Button*)
{
	std::vector<Receiver*> list;
	//_set.at(0)->Receivers.getSelected(list);
	//_factory.activate(list);
}

void FolderFrame::clickAbout(Button*)
{
	MessageBox(_T("Copyright 2012"), _T("About Demo"), MB_ICONQUESTION|MB_OK);
}

void FolderFrame::clickRefresh(Button*)
{
	Invalidate(FALSE);
}

void FolderFrame::clickClear(Button*)
{
	_grid->clearSelect();
}

void FolderFrame::clickEdit(Button*)
{
	bool toggle = _grid->getEnable();
	_grid->setEnable(!toggle);
}

bool FolderFrame::Create(RECT rect)
{
	return Window::Create(NULL, rect, _T("Demo"), WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
