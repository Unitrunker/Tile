#include "stdafx.h"
#include "User.h"
#include "../Tile/Edit.h"

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

UserFrame::UserFrame(Theme &theme, Model::User *user) : 
	Window(theme), _user(user), _set(theme)
{
	_top = new Pane(0, theme, eDown);
	_tools = new Tab(0, theme);
	_tabset = new Tab(0, theme);

	sophia::delegate2<void, Button*, bool> click;

	_tools->Add(_T("+"), click);
	_tools->Add(_T("-"), click);
	_tools->Add(_T("1"), click);
	_tools->Add(_T("2"), click);
	_tools->Add(_T("3"), click);
	_tools->Add(_T("X"), click);

	click.bind(this, &UserFrame::activateHome);
	_tabset->Add(_T("Home"), click);
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

void UserFrame::activateHome(Button *, bool up)
{
	if (up)
	{
		// TODO
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
