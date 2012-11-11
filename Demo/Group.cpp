#include "stdafx.h"
#include "Group.h"
#include "../Tile/Edit.h"

GroupSet::GroupSet(Theme& theme) : 
	SetT<Model::Group>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::Group::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::Group::_last),
	Last(_last, Time::eDate),
	_address(*this, &Model::Group::_address),
	Address(_address)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Group"), _T("Group"));
	prop = new Property(_T("Address"), _T("Group identity"), new Edit(0, theme, textFont, &Address) );
	section->Items.push_back(prop);
	prop = new Property(_T("First"), _T("First seen"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last seen"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);
}

GroupFrame::GroupFrame(Theme &theme, Model::Group *group) : 
	Window(theme), _group(group), _set(theme)
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

	click.bind(this, &GroupFrame::activateHome);
	_tabset->Add(_T("Home"), click);
	click.bind(this, &GroupFrame::activateInfo);
	_tabset->Add(_T("Info"), click);
	click.bind(this, &GroupFrame::activateHistory);
	_tabset->Add(_T("History"), click);

	_set.setValue(group);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	setPane(_top);
}

void GroupFrame::activateHome(Button *, bool up)
{
	if (up)
	{
		// TODO
	}
}

void GroupFrame::activateInfo(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tools);
		_top->Add(_list);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void GroupFrame::activateHistory(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->DoubleClick.clear();
		_top->Add(_tools);
		_top->Add(_grid);
		_top->Add(_tabset);
		_top->reflow();
	}
}
