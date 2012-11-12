#include "stdafx.h"
#include "System.h"
#include "../Tile/Edit.h"

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
	SetT<Model::System>(NULL),
#pragma warning(disable:4355)
	_first(*this, &Model::System::_first),
	First(_first, Time::eDate),
	_last(*this, &Model::System::_last),
	Last(_last, Time::eDate),
	_network(NULL),
	_identity(_network, &Model::network_t::_network),
	_trunk(_network, &Model::network_t::_type),
	Network(_identity, 16),
	Trunk(_trunk)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("System"), _T("System"));
	prop = new Property(_T("Type"), _T("Trunking Type"), new Edit(0, theme, textFont, &Trunk) );
	section->Items.push_back(prop);
	prop = new Property(_T("Network"), _T("Network identity"), new Edit(0, theme, textFont, &Network) );
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
	_tools = new Tab(0, theme);
	_tabset = new Tab(0, theme);

	sophia::delegate2<void, Button*, bool> click;

	_tools->Add(_T("+"), click);
	_tools->Add(_T("-"), click);
	_tools->Add(_T("1"), click);
	_tools->Add(_T("2"), click);
	_tools->Add(_T("3"), click);
	_tools->Add(_T("X"), click);

	click.bind(this, &SystemFrame::activateInfo);
	_tabset->Add(_T("Info"), click);
	click.bind(this, &SystemFrame::activateSites);
	_tabset->Add(_T("Sites"), click);
	click.bind(this, &SystemFrame::activateGroups);
	_tabset->Add(_T("Groups"), click);
	click.bind(this, &SystemFrame::activateUsers);
	_tabset->Add(_T("Users"), click);

	_set.setValue(system);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	setPane(_top);
}

void SystemFrame::activateInfo(Button *, bool up)
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

void SystemFrame::activateGroups(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->setTable(&_groups);
		_grid->DoubleClick.bind(this, &SystemFrame::activateGroupPopup);
		_top->Add(_tools);
		_top->Add(_grid);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void SystemFrame::activateUsers(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->setTable(&_users);
		_grid->DoubleClick.bind(this, &SystemFrame::activateUserPopup);
		_top->Add(_tools);
		_top->Add(_grid);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void SystemFrame::activateSites(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->setTable(&_sites);
		_grid->DoubleClick.bind(this, &SystemFrame::activateSitePopup);
		_top->Add(_tools);
		_top->Add(_grid);
		_top->Add(_tabset);
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
