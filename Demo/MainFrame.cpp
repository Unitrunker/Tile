#include "stdafx.h"
#include "MainFrame.h"
#include "../Tile/List.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Edit.h"
#include "../WTL/Snapshot.h"
#include <stdlib.h>

// accessor adapter for string to channel_t.
LCN::LCN(IAccessor<channel_t>& wrap, int base) : 
	_wrap(wrap), _base(base)
{
}

const string_t &LCN::getValue() const
{
	int size = 0;
	_text.resize(32, _T(' '));
	switch (_base)
	{
		case 8:
			size = wsprintf(&_text[0], _T("%o"), _wrap.getValue());
			break;
		case 16:
			size = wsprintf(&_text[0], _T("%lX"), _wrap.getValue());
			break;
		case 10:
		default:
			size = wsprintf(&_text[0], _T("%ld"), _wrap.getValue());
			break;
	}
	_text.resize(size);
	return _text;
}

bool LCN::setValue(const string_t &value)
{
	TCHAR *cursor = NULL;
	channel_t iValue = (channel_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}

// accessor adapter for string to long.
Site::Site(IAccessor<site_t>& wrap, int base) : 
	_wrap(wrap), _base(base)
{
}

const string_t &Site::getValue() const
{
	int size = 0;
	_text.resize(32, _T(' '));
	switch (_base)
	{
		case 8:
			size = wsprintf(&_text[0], _T("%o"), _wrap.getValue());
			break;
		case 16:
			size = wsprintf(&_text[0], _T("%lX"), _wrap.getValue());
			break;
		case 10:
		default:
			size = wsprintf(&_text[0], _T("%ld"), _wrap.getValue());
			break;
	}
	_text.resize(size);
	return _text;
}

bool Site::setValue(const string_t &value)
{
	TCHAR *cursor = NULL;
	site_t iValue = (site_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}

// accessor adapter for string to long.
Address::Address(IAccessor<address_t>& wrap, int base) : 
	_wrap(wrap), _base(base)
{
}

const string_t &Address::getValue() const
{
	int size = 0;
	_text.resize(32, _T(' '));
	switch (_base)
	{
		case 8:
			size = wsprintf(&_text[0], _T("%o"), _wrap.getValue());
			break;
		case 16:
			size = wsprintf(&_text[0], _T("%lX"), _wrap.getValue());
			break;
		case 10:
		default:
			size = wsprintf(&_text[0], _T("%ld"), _wrap.getValue());
			break;
	}
	_text.resize(size);
	return _text;
}

bool Address::setValue(const string_t &value)
{
	TCHAR *cursor = NULL;
	address_t iValue = (address_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}

MainFrame::MainFrame(Theme &theme) : 
	Frame(theme), _table(theme)
{
	theme.setHeight(14);

	size_t columns = _table.getHeader()->Columns.size();

	_table.setReadOnly(true);

	for (size_t i = 0; i < columns; i++)
	{
		_table.setContent(i, &_folder.Systems);
	}

	time_t now = 0;
	time(&now);

	size_t size = _folder.Systems.size();
	_ASSERT( 0 == size );
	srand( (unsigned)time(NULL) );
	for (size_t i = 0; i < 10; i++)
	{
		network_t network = 100 - i;
		Model::System *system = _folder.newSystem(network, true);
		size = _folder.Systems.size();
		_ASSERT( size == (i + 1) );
		system->_first = system->_last = now;
		for (size_t j = 0; j < 5; j++)
		{
			address_t address = rand();
			Model::Group *group = system->newGroup(address, true);
			group->_first = group->_first = now;
			address = rand();
			Model::User *user = system->newUser(address, true);
			user->_first = user->_first = now;
			site_t peer = (site_t)rand();
			Model::Site *site = system->newSite(peer, true);
			site->_first = site->_first = now;
			for (size_t k = 0; k < 5; k++)
			{
				channel_t lcn = (channel_t)rand();
				Model::Channel *channel = site->newChannel(lcn, true);
				channel->_first = channel->_first = now;
			}
		}
	}

	Grid *pGrid = new Grid(0, theme);
	pGrid->setTable(&_table);

	pGrid->DoubleClick.bind(this, &MainFrame::clickety);

	// connect the form to the frame window.
	setPane(pGrid);
}

void MainFrame::clickety(Grid *, size_t row, size_t)
{
	Model::System *system = _folder.Systems[row];
	Theme &theme = getTheme();
	RECT rect = {200, 200, 840, 680};
	SystemFrame *frame = new SystemFrame(theme, system);
	frame->Create(m_hWnd, rect, _T("System X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}

SystemSet::SystemSet(Theme& theme) : 
	SetT<Model::System>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::System::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::System::_last),
	Last(_last, Time::eDate),
	_network(*this, &Model::System::_network),
	Network(_network, 16)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("System"), _T("System"));
	prop = new Property(_T("Network"), _T("Network identity"), new Edit(0, theme, textFont, &Network) );
	section->Items.push_back(prop);
	prop = new Property(_T("First"), _T("First seen"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last seen"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);
}

SiteSet::SiteSet(Theme& theme) : 
	SetT<Model::Site>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::Site::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::Site::_last),
	Last(_last, Time::eDate),
	_site(*this, &Model::Site::_site),
	Site(_site)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Site"), _T("Site"));
	prop = new Property(_T("Site"), _T("Site identity"), new Edit(0, theme, textFont, &Site) );
	section->Items.push_back(prop);
	prop = new Property(_T("First"), _T("First seen"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last seen"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);
}

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

ChannelSet::ChannelSet(Theme& theme) : 
	SetT<Model::Channel>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::Channel::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::Channel::_last),
	Last(_last, Time::eDate),
	_lcn(*this, &Model::Channel::_LCN),
	LCN(_lcn, 10),
	_frequency(*this, &Model::Channel::_hz),
	Frequency(_frequency, 10)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Channel"), _T("Channel"));
	prop = new Property(_T("LCN"), _T("Channel number"), new Edit(0, theme, textFont, &LCN) );
	section->Items.push_back(prop);
	prop = new Property(_T("Frequency"), _T("Frequency"), new Edit(0, theme, textFont, &Frequency) );
	section->Items.push_back(prop);
	prop = new Property(_T("First"), _T("First seen"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last seen"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);
}

SystemFrame::SystemFrame(Theme &theme, Model::System *system) : 
	Window(theme), _system(system), _set(theme), _users(theme), _groups(theme), _sites(theme)
{
	for (size_t i = 0; i < _groups.getHeader()->Columns.size(); i++)
		_groups.setContent(i, &system->Groups);

	for (size_t i = 0; i < _users.getHeader()->Columns.size(); i++)
		_users.setContent(i, &system->Users);

	for (size_t i = 0; i < _sites.getHeader()->Columns.size(); i++)
		_sites.setContent(i, &system->Sites);

	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme);
	Button *button = NULL;

	Theme::Font font = {Theme::eText, theme.Text};

	Flow flow = {1, 1, 0, true};

	button = new Button(0, theme, font, _T("Info"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SystemFrame::activateInfo);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Sites"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SystemFrame::activateSites);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Groups"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SystemFrame::activateGroups);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Users"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SystemFrame::activateUsers);
	_tabset->Add(button, 0, 4096, 1, false);

	_set.setValue(system);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tabset);
	_top->Add(_list);
	setPane(_top);
}

void SystemFrame::activateInfo(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		_top->Add(_list);
		_top->reflow();
	}
}

void SystemFrame::activateGroups(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		_grid->setTable(&_groups);
		_grid->DoubleClick.bind(this, &SystemFrame::activateGroupPopup);
		_top->Add(_grid);
		_top->reflow();
	}
}

void SystemFrame::activateUsers(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		_grid->setTable(&_users);
		_grid->DoubleClick.bind(this, &SystemFrame::activateUserPopup);
		_top->Add(_grid);
		_top->reflow();
	}
}

void SystemFrame::activateSites(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		_grid->setTable(&_sites);
		_grid->DoubleClick.bind(this, &SystemFrame::activateSitePopup);
		_top->Add(_grid);
		_top->reflow();
	}
}

void SystemFrame::activateGroupPopup(Grid *, size_t row, size_t)
{
	Model::Group *group = _system->Groups[row];
	RECT rect = {200, 200, 840, 680};
	GroupFrame *frame = new GroupFrame(getTheme(), group);
	frame->Create(m_hWnd, rect, _T("Group X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}

void SystemFrame::activateUserPopup(Grid *, size_t row, size_t)
{
	Model::User *group = _system->Users[row];
	RECT rect = {200, 200, 840, 680};
	UserFrame *frame = new UserFrame(getTheme(), group);
	frame->Create(m_hWnd, rect, _T("User X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}

void SystemFrame::activateSitePopup(Grid *, size_t row, size_t)
{
	Model::Site *site = _system->Sites[row];
	RECT rect = {200, 200, 840, 680};
	SiteFrame* frame = new SiteFrame(getTheme(), site);
	frame->Create(m_hWnd, rect, _T("Site X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}

SiteFrame::SiteFrame(Theme &theme, Model::Site *site) : 
	Window(theme), _site(site), _set(theme), _channels(theme)
{
	for (size_t i = 0; i < _channels.getHeader()->Columns.size(); i++)
		_channels.setContent(i, &site->Channels);

	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme);
	Button *button = NULL;

	Theme::Font font = {Theme::eText, theme.Text};

	Flow flow = {1, 1, 0, true};

	button = new Button(0, theme, font, _T("Home"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SiteFrame::activateHome);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Info"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SiteFrame::activateInfo);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Channels"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &SiteFrame::activateChannels);
	_tabset->Add(button, 0, 4096, 1, false);

	_set.setValue(site);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tabset);
	_top->Add(_list);
	setPane(_top);
}

void SiteFrame::activateHome(Button *, bool up)
{
	if (up)
	{
		// TODO
	}
}

void SiteFrame::activateInfo(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_tabset->watch(NULL);
		_grid->watch(NULL);
		_list->watch(NULL);
		_top->Add(_tabset);
		_top->Add(_list);
		_top->reflow();
	}
}

void SiteFrame::activateChannels(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		_grid->setTable(&_channels);
		_grid->DoubleClick.bind(this, &SiteFrame::activateChannelPopup);
		_top->Add(_grid);
		_top->reflow();
	}
}

void SiteFrame::activateChannelPopup(Grid *, size_t row, size_t)
{
	Model::Channel *channel = _site->Channels[row];
	RECT rect = {200, 200, 840, 680};
	ChannelFrame *frame = new ChannelFrame(getTheme(), channel);
	frame->Create(m_hWnd, rect, _T("Channel X"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW);
}

ChannelFrame::ChannelFrame(Theme &theme, Model::Channel *channel) : 
	Window(theme), _channel(channel), _set(theme)
{
	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme);
	Button *button = NULL;

	Theme::Font font = {Theme::eText, theme.Text};

	Flow flow = {1, 1, 0, true};

	button = new Button(0, theme, font, _T("Home"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &ChannelFrame::activateHome);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Info"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &ChannelFrame::activateInfo);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("History"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &ChannelFrame::activateHistory);
	_tabset->Add(button, 0, 4096, 1, false);

	_set.setValue(channel);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tabset);
	_top->Add(_list);
	setPane(_top);
}

void ChannelFrame::activateHome(Button *, bool up)
{
	if (up)
	{
		// TODO
	}
}

void ChannelFrame::activateInfo(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_tabset->watch(NULL);
		_grid->watch(NULL);
		_list->watch(NULL);
		_top->Add(_tabset);
		_top->Add(_list);
		_top->reflow();
	}
}

void ChannelFrame::activateHistory(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		//_grid->setTable(&_channels);
		_grid->DoubleClick.clear();
		_top->Add(_grid);
		_top->reflow();
	}
}

GroupFrame::GroupFrame(Theme &theme, Model::Group *group) : 
	Window(theme), _group(group), _set(theme)
{
	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme);
	Button *button = NULL;

	Theme::Font font = {Theme::eText, theme.Text};

	Flow flow = {1, 1, 0, true};

	button = new Button(0, theme, font, _T("Home"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &GroupFrame::activateHome);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Info"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &GroupFrame::activateInfo);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("History"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &GroupFrame::activateHistory);
	_tabset->Add(button, 0, 4096, 1, false);

	_set.setValue(group);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tabset);
	_top->Add(_list);
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
		_top->Add(_tabset);
		_top->Add(_list);
		_top->reflow();
	}
}

void GroupFrame::activateHistory(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_top->Add(_tabset);
		//_grid->setTable(&_channels);
		_grid->DoubleClick.clear();
		_top->Add(_grid);
		_top->reflow();
	}
}

UserFrame::UserFrame(Theme &theme, Model::User *user) : 
	Window(theme), _user(user), _set(theme)
{
	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme);
	Button *button = NULL;

	Theme::Font font = {Theme::eText, theme.Text};

	Flow flow = {1, 1, 0, true};

	button = new Button(0, theme, font, _T("Home"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &UserFrame::activateHome);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("Info"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &UserFrame::activateInfo);
	_tabset->Add(button, 0, 4096, 1, false);

	button = new Button(0, theme, font, _T("History"));
	button->setFlow(eDown, flow);
	button->Click.bind(this, &UserFrame::activateHistory);
	_tabset->Add(button, 0, 4096, 1, false);

	_set.setValue(user);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tabset);
	_top->Add(_list);
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

		_top->Add(_tabset);
		_top->Add(_list);
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
