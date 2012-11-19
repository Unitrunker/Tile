#include "stdafx.h"
#include "User.h"
#include "../Tile/Edit.h"

/*
Copyright © 2012 Rick Parrish
*/

UserSet::UserSet(Theme& theme) : 
	SetT<Model::User>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::User::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::User::_last),
	Last(_last, Time::eDate),
	_address(*this, &Model::User::_address),
	Address(_address)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("User"), _T("User"));
	prop = new Property(_T("Address"), _T("Radio identity"), new Edit(0, theme, textFont, &Address) );
	section->Items.push_back(prop);
	prop = new Property(_T("First"), _T("First seen"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last seen"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);
}

UserFrame::UserFrame(Factory& factory, Model::User *user) : 
	Window(factory._theme), _factory(factory), _user(user), _set(factory._theme)
{
	Theme &theme = factory._theme;
	_top = new Pane(0, theme, eDown);
	_tools = new Tab(0, theme);
	_tabset = new Tab(0, theme);

	sophia::delegate2<void, Button*, bool> click;

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	click.bind(this, &UserFrame::clickHome);
	_tools->Add(_T("\x48"), font_webdings, click);	// home
	click.clear();
	_tools->Add(_T("\x270D"), font_segoe, click);	// edit
	_tools->Add(_T("+"), font_segoe, click);		// plus
	_tools->Add(_T("-"), font_segoe, click);		// minus
	_tools->Add(_T("\x4C"), font_webdings, click);	// inspect
	_tools->Add(_T("\x71"), font_webdings, click);	// refresh

	click.bind(this, &UserFrame::activateInfo);
	_tabset->Add(_T("Info"), click);
	click.bind(this, &UserFrame::activateHistory);
	_tabset->Add(_T("History"), click);

	_set.setValue(user);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	setPane(_top);
}

UserFrame::~UserFrame()
{
	_factory.deactivate(_user);
}

void UserFrame::clickHome(Button *, bool up)
{
	if (up)
	{
		_factory.activate(_user->_system);
	}
}

void UserFrame::activateInfo(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_tabset->watch(NULL);
		_grid->watch(NULL);
		_list->watch(NULL);

		_top->Add(_tools);
		_top->Add(_list);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void UserFrame::activateHistory(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_tabset->watch(NULL);
		_grid->watch(NULL);
		_list->watch(NULL);
		_top->Add(_tabset);
		//_grid->setTable(&_channels);
		_grid->DoubleClick.clear();
		_top->Add(_grid);
		_top->reflow();
	}
}


bool UserFrame::Create(RECT rect)
{
	return Window::Create(NULL, rect, _user->_label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
