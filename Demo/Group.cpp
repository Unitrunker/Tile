#include "stdafx.h"
#include "Group.h"
#include "../Tile/Combo.h"
#include "../Tile/Edit.h"
#include "../Tile/PickColor.h"

/*
Copyright © 2012 Rick Parrish
*/

GroupSet::GroupSet(Theme& theme) : 
#pragma warning(disable:4355)
	_first(*this, &Model::Group::_first),
	_last(*this, &Model::Group::_last),
	_address(*this, &Model::Group::_address),
	_color(*this, &Model::Group::_color),
	_rank(*this, &Model::Group::_rank),
	_hits(*this, &Model::Group::_hits),
	_alarm(*this, &Model::Group::_alarm),
	Label(*this, &Model::Group::_label),
#pragma warning(default:4355)
	_adaptRank(&_rank),
	_adaptHits(&_hits),
	_adaptAlarm(&_alarm),
	First(_first, Time::eDate),
	Last(_last, Time::eDate),
	Address(_address),
	Rank(_adaptRank),
	Hits(_adaptHits),
	_add(false)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	Edit *edit = NULL;

	section = new Section(_T("Group"), _T("Group"));

	// create a control
	edit = new Edit(0, theme, textFont, &Address);
	edit->setForeAccess(this);
	edit->setReadOnly(true);
	_primary = edit;
	prop = new Property(_T("Address"), _T("Group identity"), edit);
	section->Items.push_back(prop);

	// create a control
	edit = new Edit(0, theme, textFont, &Label);
	edit->setForeAccess(this);
	// create property and assign control to it.
	prop = new Property(_T("Label"), _T("Description"), edit);
	section->Items.push_back(prop);

	// create a control
	edit = new Edit(0, theme, textFont, &First);
	edit->setForeAccess(this);
	edit->setReadOnly(true);
	// create property and assign control to it.
	prop = new Property(_T("First"), _T("First seen"), edit);
	section->Items.push_back(prop);

	// create a control
	edit = new Edit(0, theme, textFont, &Last);
	edit->setForeAccess(this);
	edit->setReadOnly(true);
	// create property and assign control to it.
	prop = new Property(_T("Last"), _T("Last seen"), edit);
	section->Items.push_back(prop);

	// create a control
	PickColor *pick = new PickColor(0, theme, _color);
	// create property and assign control to it.
	prop = new Property(_T("Color"), _T("Text foreground color"), pick);
	section->Items.push_back(prop);

	// TODO: make this readonly.

	// create a control
	edit = new Edit(0, theme, textFont, &Hits);
	edit->setForeAccess(this);
	edit->setReadOnly(true);
	// create property and assign control to it.
	prop = new Property(_T("Hits"), _T("Call activity counter"), edit);
	section->Items.push_back(prop);

	// create a control
	edit = new Edit(0, theme, textFont, &Rank);
	edit->setForeAccess(this);
	// create property and assign control to it.
	prop = new Property(_T("Rank"), _T("Listening priority (1 = most important)"), edit);
	section->Items.push_back(prop);

	static Combo::Item list[] = 
	{
		{_T("None"), Model::None},
		{_T("Blink"), Model::Blink},
		{_T("Warble"), Model::Warble},
		{_T("Whoop"), Model::Whoop}
	};

	Combo *combo = new Combo(0, theme, textFont, list, _countof(list), &_adaptAlarm);
	// create property and assign control to it.
	prop = new Property(_T("Alarm"), _T("Annunciation type"), combo);
	section->Items.push_back(prop);

	Add(section);
}

// retrieve color.
const color_t GroupSet::getValue(const Theme &, bool) const
{
	return _color.getValue();
}

bool GroupSet::getCaption(string_t &label) const
{
	TCHAR caption[32] = {0};
	if ( getAdd() )
	{
		wsprintf(caption, _T("Group Unknown"));
	}
	else if (size() == 1)
	{
		Model::Group *group = at(0);
		wsprintf(caption, _T("Group %0X"), group->_address);
	}
	else
	{
		wsprintf(caption, _T("%d Groups"), size());
	}
	label = caption;
	return true;
}

// allow or disallow editing of the primary key.
void GroupSet::setAdd(bool add)
{
	_add = add;
	_primary->setReadOnly(!add);
}

// allow or disallow editing of the primary key.
bool GroupSet::getAdd() const
{
	return _add;
}

GroupFrame::GroupFrame(Factory& factory, Model::System *system) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	Model::Group *group = new Model::Group(system, 0);
	_set.setObject(group);
	_set.setAdd(true);
	inside();
}

GroupFrame::GroupFrame(Factory& factory, Model::Group *group) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObject(group);
	inside();
}

GroupFrame::GroupFrame(Factory& factory, std::vector<Model::Group *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void GroupFrame::inside()
{
	Theme &theme = _factory._theme;
	_top = new Pane(0, theme, eDown);
	_tools = new Pane(0, theme, eRight);
	_tabset = new Pane(0, theme, eRight);

	_set.Remove.bind(this, &GroupFrame::remove);
	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_tools->setLineColor(color);

	Button* button = NULL;
	Tab* tab = NULL;

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };
	Theme::Font text = {Theme::eText, theme.Text};

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &GroupFrame::clickHome);
	button->setTip(_T("Home"));
	_tools->Add(button);

	button = new Button(0, theme, font_segoe, _T("\x270D"));
	//button->Click.bind(this, &GroupFrame::????);
	button->setTip(_T("Edit"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &GroupFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_tools->Add(button);

	if ( _set.getAdd() )
	{
		button = new Button(0, theme, font_segoe, _T("Commit"));
		button->Click.bind(this, &GroupFrame::clickCommit);
		_tools->Add(button);
	}

	tab = new Tab(0, theme, text, _T("Info"));
	tab->Click.bind(this, &GroupFrame::activateInfo);
	_tabset->Add(tab);

	// no history for multi-select or new additions.
	if ( _set.size() == 1 && !_set.getAdd() )
	{
		tab = new Tab(0, theme, text, _T("History"));
		tab->Click.bind(this, &GroupFrame::activateHistory);
		_tabset->Add(tab);
	}

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	setPane(_top);
}

GroupFrame::~GroupFrame()
{
	if (_set.size() == 1)
	{
		Model::Group *group = _set.at(0);
		_factory.deactivate(group);
		_set.Remove.clear();
		if (_set.getAdd())
			delete group;
	}
}

void GroupFrame::clickHome(Button *)
{
	_factory.activate(_set.at(0)->_system);
}

void GroupFrame::clickRefresh(Button *)
{
	Invalidate(FALSE);
}

void GroupFrame::clickCommit(Button*)
{
	if ( _set.getAdd() )
	{
		apply();

		Model::Group *group = _set.at(0);
		// check for duplicate.
		Model::Group *origin = group->_system->newGroup(group->_address, false);
		if (origin == NULL)
		{
			_set.setAdd(false);
			group->_system->Groups.insert(group->_address, group);
		}
		else
		{
			int res = MessageBox(_T("Click YES to merge and edit\nclick NO to discard and edit\nor click CANCEL"), 
				_T("Group ID Exists!"), MB_ICONEXCLAMATION|MB_YESNOCANCEL);
			switch (res)
			{
				case IDYES:
					if (group->_label.size() > 0)
						origin->_label = group->_label;
					// this will implicitly destroy this window.
					delete group;
					_factory.deactivate(group);
					_factory.activate(origin);
					break;

				case IDNO:
					// this will implicitly destroy this window.
					delete group;
					_factory.deactivate(group);
					_factory.activate(origin);
					break;

				case IDCANCEL:
					break;
			}
		}
	}
}

void GroupFrame::activateInfo(Tab*)
{
	_top->clear();
	_top->Add(_tools);
	_top->Add(_list);
	_top->Add(_tabset);
	_top->reflow();
}

void GroupFrame::activateHistory(Tab*)
{
	_top->clear();
	_grid->DoubleClick.clear();
	_top->Add(_tools);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void GroupFrame::remove()
{
	DestroyWindow();
}

bool GroupFrame::Create(RECT rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
