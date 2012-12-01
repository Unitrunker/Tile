#include "stdafx.h"
#include "System.h"
#include "../Tile/Edit.h"

/*
Copyright © 2012 Rick Parrish
*/

Trunk::Trunk(IAccessor<Model::trunk_t>& wrap) : _wrap(wrap)
{
}

const string_t &Trunk::getValue() const
{
	Model::trunk_t value = _wrap.getValue();
	const TCHAR *text = Model::getTrunkType(value);
	_text = text;
	return _text;
}

bool Trunk::setValue(const string_t &)
{
	return false; // not supported.
}

SystemSet::SystemSet(Theme& theme) : 
#pragma warning(disable:4355)
	_first(*this, &Model::System::_first),
	_last(*this, &Model::System::_last),
	_identity(*this, &Model::System::_network, &Model::network_t::_network),
	_trunk(*this, &Model::System::_network, &Model::network_t::_type),
	Label(*this, &Model::System::_label),
#pragma warning(default:4355)
	First(_first, Time::eDate),
	Last(_last, Time::eDate),
	Network(_identity, 16),
	Trunk(_trunk),
	_add(false)
{
	Section *section = NULL;
	Property *prop = NULL;
	Edit *edit = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("System"), _T("System"));
	edit = new Edit(0, theme, textFont, &Trunk);
	edit->setReadOnly(true);
	_primary[0] = edit;
	prop = new Property(_T("Type"), _T("Trunking Type"), edit);
	section->Items.push_back(prop);

	edit = new Edit(0, theme, textFont, &Network);
	edit->setReadOnly(true);
	_primary[1] = edit;
	prop = new Property(_T("Network"), _T("Network identity"), edit);
	section->Items.push_back(prop);

	edit = new Edit(0, theme, textFont, &Label);
	prop = new Property(_T("Label"), _T("Description"), edit);
	section->Items.push_back(prop);

	edit = new Edit(0, theme, textFont, &First);
	edit->setReadOnly(true);
	prop = new Property(_T("First"), _T("First seen"), edit);
	section->Items.push_back(prop);

	edit = new Edit(0, theme, textFont, &Last);
	edit->setReadOnly(true);
	prop = new Property(_T("Last"), _T("Last seen"), edit);
	section->Items.push_back(prop);

	Add(section);
}

bool SystemSet::getCaption(string_t &label) const
{
	TCHAR caption[32] = {0};
	if (size() == 1)
	{
		Model::System *system = at(0);
		wsprintf(caption, _T("%s System %0X"), Model::getTrunkType(system->_network._type), system->_network._network);
	}
	else
	{
		wsprintf(caption, _T("%d Systems"), size());
	}
	label = caption;
	return true;
}


// allow or disallow editing of the primary key.
void SystemSet::setAdd(bool add)
{
	_add = add;
	_primary[0]->setReadOnly(!add);
	_primary[1]->setReadOnly(!add);
}

// allow or disallow editing of the primary key.
bool SystemSet::getAdd() const
{
	return _add;
}

SystemFrame::SystemFrame(Factory &factory) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), 
	_users(factory._theme), _groups(factory._theme), _sites(factory._theme)
{
	Model::network_t network;
	network._type = Model::Unknown;
	network._network = 0;
	Model::System* system = new Model::System(&factory._folder, network);
	_set.setObject(system);
	_set.setAdd(true);
	inside();
}

SystemFrame::SystemFrame(Factory &factory, Model::System *system) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), 
	_users(factory._theme), _groups(factory._theme), _sites(factory._theme)
{
	_set.setObject(system);
	inside();
}

SystemFrame::SystemFrame(Factory &factory, std::vector<Model::System *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), 
	_users(factory._theme), _groups(factory._theme), _sites(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void SystemFrame::inside()
{
	Theme &theme = _factory._theme;

	if (_set.size() == 1)
	{
		Model::System *system = _set.at(0);
		for (size_t i = 0; i < _groups.getHeader()->Columns.size(); i++)
			_groups.setContent(i, &system->Groups);

		for (size_t i = 0; i < _users.getHeader()->Columns.size(); i++)
			_users.setContent(i, &system->Users);

		for (size_t i = 0; i < _sites.getHeader()->Columns.size(); i++)
			_sites.setContent(i, &system->Sites);
	}

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };
	Theme::Font text = { Theme::eDefault, theme.Text };

	_top = new Pane(0, theme, eDown);
	_toolInfo = new Pane(0, theme, eRight);
	_toolSite = new Pane(0, theme, eRight);
	_toolGroup = new Pane(0, theme, eRight);
	_toolUser = new Pane(0, theme, eRight);
	_tabset = new Pane(0, theme, eRight);
	_set.Remove.bind(this, &SystemFrame::remove);

	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_toolInfo->setLineColor(color);
	_toolSite->setLineColor(color);
	_toolGroup->setLineColor(color);
	_toolUser->setLineColor(color);

	Tab* tab = NULL;
	Button* button = NULL;

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	tab = new Tab(0, theme, text, _T("Info"));
	tab->Click.bind(this, &SystemFrame::activateInfo);
	_tabset->Add(tab);
	if (_set.size() == 1)
	{
		tab = new Tab(0, theme, text, _T("Sites"));
		tab->Click.bind(this, &SystemFrame::activateSites);
		_tabset->Add(tab);
		tab = new Tab(0, theme, text, _T("Groups"));
		tab->Click.bind(this, &SystemFrame::activateGroups);
		_tabset->Add(tab);
		tab = new Tab(0, theme, text, _T("Users"));
		tab->Click.bind(this, &SystemFrame::activateUsers);
		_tabset->Add(tab);
	}

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &SystemFrame::clickHome);
	button->setTip(_T("Home"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &SystemFrame::clickHome);
	button->setTip(_T("Home"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &SystemFrame::clickHome);
	button->setTip(_T("Home"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &SystemFrame::clickHome);
	button->setTip(_T("Home"));
	_toolUser->Add(button);

	button = new Button(0, theme, font_segoe, _T("\x270D"));
	button->Click.bind(this, &SystemFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_segoe, _T("\x270D"));
	button->Click.bind(this, &SystemFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_segoe, _T("\x270D"));
	button->Click.bind(this, &SystemFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_segoe, _T("\x270D"));
	button->Click.bind(this, &SystemFrame::clickEdit);
	button->setTip(_T("Edit"));
	_toolUser->Add(button);

	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &SystemFrame::clickSitePlus);
	button->setTip(_T("Add"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &SystemFrame::clickGroupPlus);
	button->setTip(_T("Add"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &SystemFrame::clickUserPlus);
	button->setTip(_T("Add"));
	_toolUser->Add(button);

	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &SystemFrame::clickSiteMinus);
	button->setTip(_T("Remove"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &SystemFrame::clickGroupMinus);
	button->setTip(_T("Remove"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &SystemFrame::clickUserMinus);
	button->setTip(_T("Remove"));
	_toolUser->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &SystemFrame::clickSiteInspect);
	button->setTip(_T("Inspect"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &SystemFrame::clickGroupInspect);
	button->setTip(_T("Inspect"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &SystemFrame::clickUserInspect);
	button->setTip(_T("Inspect"));
	_toolUser->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &SystemFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolInfo->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &SystemFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolSite->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &SystemFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolGroup->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &SystemFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_toolUser->Add(button);

	_top->Add(_toolInfo, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);

	setPane(_top);
}

SystemFrame::~SystemFrame()
{
	if (_set.size() == 1)
		_factory.deactivate(_set.at(0));
}

void SystemFrame::clickEdit(Button *)
{
	bool enable = _grid->getEnable();
	_grid->setEnable(!enable);
}

void SystemFrame::clickGroupPlus(Button *)
{
	_factory.activateGroup(_set.at(0));
}

void SystemFrame::clickGroupMinus(Button *)
{
	std::vector<Model::Group*> list;
	_set.at(0)->Groups.getSelected(list);
	_set.at(0)->Groups.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void SystemFrame::clickGroupInspect(Button *)
{
	std::vector<Model::Group*> list;
	_set.at(0)->Groups.getSelected(list);
	_factory.activate(list);
}

void SystemFrame::clickUserPlus(Button *)
{
	_factory.activateUser(_set.at(0));
}

void SystemFrame::clickUserMinus(Button *)
{
	std::vector<Model::User*> list;
	_set.at(0)->Users.getSelected(list);
	_set.at(0)->Users.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void SystemFrame::clickUserInspect(Button *)
{
	std::vector<Model::User*> list;
	_set.at(0)->Users.getSelected(list);
	_factory.activate(list);
}

void SystemFrame::clickSitePlus(Button *)
{
	_factory.activateSite(_set.at(0));
}

void SystemFrame::clickSiteMinus(Button *)
{
	std::vector<Model::Site*> list;
	_set.at(0)->Sites.getSelected(list);
	_set.at(0)->Sites.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void SystemFrame::clickSiteInspect(Button *)
{
	std::vector<Model::Site*> list;
	_set.at(0)-> Sites.getSelected(list);
	_factory.activate(list);
}

void SystemFrame::clickRefresh(Button *)
{
	Invalidate(FALSE);
}

void SystemFrame::clickHome(Button *)
{
	_factory.activate(SW_SHOW);
}

void SystemFrame::activateInfo(Tab*)
{
	_top->clear();
	_top->Add(_toolInfo);
	_top->Add(_list);
	_top->Add(_tabset);
	_top->reflow();
}

void SystemFrame::activateGroups(Tab*)
{
	_top->clear();
	_grid->setTable(&_groups);
	_grid->DoubleClick.bind(this, &SystemFrame::activateGroupPopup);
	_top->Add(_toolGroup);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void SystemFrame::activateUsers(Tab*)
{
	_top->clear();
	_grid->setTable(&_users);
	_grid->DoubleClick.bind(this, &SystemFrame::activateUserPopup);
	_top->Add(_toolUser);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void SystemFrame::activateSites(Tab*)
{
	_top->clear();
	_grid->setTable(&_sites);
	_grid->DoubleClick.bind(this, &SystemFrame::activateSitePopup);
	_top->Add(_toolSite);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void SystemFrame::activateGroupPopup(Grid *, size_t row, size_t)
{
	Model::Group *group = _set.at(0)->Groups[row];
	_factory.activate(group);
}

void SystemFrame::activateUserPopup(Grid *, size_t row, size_t)
{
	Model::User *user = _set.at(0)->Users[row];
	_factory.activate(user);
}

void SystemFrame::activateSitePopup(Grid *, size_t row, size_t)
{
	Model::Site *site = _set.at(0)->Sites[row];
	_factory.activate(site);
}

void SystemFrame::remove()
{
	DestroyWindow();
}

bool SystemFrame::Create(RECT rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
