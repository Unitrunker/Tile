#include "stdafx.h"
#include "User.h"
#include "../Tile/Combo.h"
#include "../Tile/Edit.h"
#include "../Tile/PickColor.h"

/*
Copyright © 2012 Rick Parrish
*/

UserSet::UserSet(Theme& theme) : 
#pragma warning(disable:4355)
	_first(*this, &Model::User::_first),
	_last(*this, &Model::User::_last),
	_address(*this, &Model::User::_address),
	_color(*this, &Model::User::_color),
	_rank(*this, &Model::User::_rank),
	_hits(*this, &Model::User::_hits),
	_alarm(*this, &Model::User::_alarm),
	Label(*this, &Model::User::_label),
#pragma warning(default:4355)
	First(_first, Time::eDate),
	Last(_last, Time::eDate),
	Address(_address),
	_adaptRank(&_rank),
	_adaptHits(&_hits),
	_adaptAlarm(&_alarm),
	Rank(_adaptRank),
	Hits(_adaptHits),
	_add(false)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	Edit *edit = NULL;

	// create a section
	section = new Section(_T("User"), _T("Subscriber radio information"));

	// create a control
	edit = new Edit(0, theme, textFont, &Address);
	edit->setForeAccess(this);
	edit->setReadOnly(true);
	_primary = edit;
	// create property and assign control to it.
	prop = new Property(_T("Address"), _T("Radio identity"), edit);
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
	PickColor *pick = new PickColor(0, theme, &_color);
	// create property and assign control to it.
	prop = new Property(_T("Color"), _T("Text foreground color"), pick);
	section->Items.push_back(prop);

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
const color_t UserSet::getValue(const Theme &, bool) const
{
	// TODO: sanity check to ensure background / foreground colors have contrast.
	return _color.getValue();
}

bool UserSet::getCaption(string_t &label) const
{
	TCHAR caption[32] = {0};
	if ( getAdd() )
	{
		wsprintf(caption, _T("User Unknown"));
	}
	else if ( size() == 1 )
	{
		Model::User *user = at(0);
		wsprintf(caption, _T("User %0X"), user->_address);
	}
	else if ( getAdd() )
	{
		wsprintf(caption, _T("User Unknown"));
	}
	else
	{
		wsprintf(caption, _T("%d Users"), size());
	}
	label = caption;
	return true;
}

// allow or disallow editing of the primary key.
void UserSet::setAdd(bool add)
{
	_add = add;
	_primary->setReadOnly(!add);
}

// allow or disallow editing of the primary key.
bool UserSet::getAdd() const
{
	return _add;
}

UserFrame::UserFrame(Factory& factory, Model::System *system) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	Model::User *user = new Model::User(system, 0);
	_set.setObject(user);
	_set.setAdd(true);
	inside();
}

UserFrame::UserFrame(Factory& factory, Model::User *user) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObject(user);
	inside();
}

UserFrame::UserFrame(Factory& factory, std::vector<Model::User *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void UserFrame::inside()
{
	Theme &theme = _factory._theme;
	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme, eRight);
	_tools = new Pane(0, theme, eRight);
	_set.Remove.bind(this, &UserFrame::remove);

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
	button->Click.bind(this, &UserFrame::clickHome);
	button->setTip(_T("Home"));
	_tools->Add(button);

	button = new Button(0, theme, font_segoe, _T("\x270D"));
	//button->Click.bind(this, &UserFrame::clickHome);
	button->setTip(_T("Edit"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &UserFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_tools->Add(button);

	if (_set.getAdd())
	{
		button = new Button(0, theme, font_segoe, _T("Commit"));
		button->Click.bind(this, &UserFrame::clickCommit);
		_tools->Add(button);
	}

	tab = new Tab(0, theme, text, _T("Info"));
	tab->Click.bind(this, &UserFrame::activateInfo);
	_tabset->Add(tab);
	if (_set.size() == 1 && !_set.getAdd() )
	{
		tab = new Tab(0, theme, text, _T("History"));
		tab->Click.bind(this, &UserFrame::activateHistory);
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

UserFrame::~UserFrame()
{
	if (_set.size() == 1)
		_factory.deactivate(_set.at(0));
	if ( _set.getAdd() )
		delete _set.at(0);
}

void UserFrame::clickHome(Button *)
{
	_factory.activate(_set.at(0)->_system);
}

void UserFrame::clickRefresh(Button *)
{
	Invalidate(FALSE);
}

void UserFrame::activateInfo(Tab*)
{
	_top->clear();
	_top->Add(_tools);
	_top->Add(_list);
	_top->Add(_tabset);
	_top->reflow();
}

void UserFrame::activateHistory(Tab*)
{
	_top->clear();
	_grid->setTable(NULL);
	_grid->DoubleClick.clear();
	_top->Add(_tools);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void UserFrame::clickCommit(Button *)
{
	if ( _set.getAdd() )
	{
		_set.setAdd(false);
		Model::User *user = _set.at(0);
		// TODO: check for duplicate.
		user->_system->Users.insert(user->_address, user);
	}
}

void UserFrame::remove()
{
	DestroyWindow();
}

bool UserFrame::Create(RECT rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
