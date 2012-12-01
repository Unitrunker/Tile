#include "stdafx.h"
#include "Site.h"
#include "../Tile/Edit.h"

/*
Copyright © 2012 Rick Parrish
*/

// accessor adapter for string to long.
Site::Site(IAccessor<Model::site_t>& wrap, int base) : 
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
	Model::site_t iValue = (Model::site_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}

SiteSet::SiteSet(Theme& theme) : 
#pragma warning(disable:4355)
	_first(*this, &Model::Site::_first),
	_last(*this, &Model::Site::_last),
	_site(*this, &Model::Site::_site),
	Label(*this, &Model::Site::_label),
#pragma warning(default:4355)
	Site(_site),
	First(_first, Time::eDate),
	Last(_last, Time::eDate),
	_add(false)
{
	Section *section = NULL;
	Property *prop = NULL;
	Edit *edit = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Site"), _T("Site"));

	edit = new Edit(0, theme, textFont, &Site);
	edit->setReadOnly(true);
	_primary = edit;
	prop = new Property(_T("Site"), _T("Site identity"), edit);
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

bool SiteSet::getCaption(string_t &label) const
{
	TCHAR caption[32] = {0};
	if ( getAdd() )
	{
		wsprintf(caption, _T("Site Unknown"));
	}
	else if (size() == 1)
	{
		Model::Site *site = at(0);
		wsprintf(caption, _T("%s %0X Site %0X"), Model::getTrunkType(site->_system->_network._type), site->_system->_network._network, site->_site);
	}
	else
	{
		wsprintf(caption, _T("%d Sites"), size());
	}
	label = caption;

	return true;
}

// allow or disallow editing of the primary key.
void SiteSet::setAdd(bool add)
{
	_add = add;
	_primary->setReadOnly(!add);
}

// allow or disallow editing of the primary key.
bool SiteSet::getAdd() const
{
	return _add;
}

SiteFrame::SiteFrame(Factory& factory, Model::System *system) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), _channels(factory._theme)
{
	Model::Site *site = new Model::Site(system, 0);
	_set.setObject(site);
	_set.setAdd(true);
	inside();
}

SiteFrame::SiteFrame(Factory& factory, Model::Site *site) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), _channels(factory._theme)
{
	_set.setObject(site);
	inside();
}

SiteFrame::SiteFrame(Factory& factory, std::vector<Model::Site *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme), _channels(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void SiteFrame::inside()
{
	Theme &theme = _factory._theme;
	if (_set.size() == 1)
		for (size_t i = 0; i < _channels.getHeader()->Columns.size(); i++)
			_channels.setContent(i, &_set.at(0)->Channels);

	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme, eRight);
	_tools = new Pane(0, theme, eRight);
	_set.Remove.bind(this, &SiteFrame::remove);

	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_tools->setLineColor(color);

	Tab* tab = NULL;
	Button* button = NULL;

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };
	Theme::Font text = { Theme::eDefault, theme.Text };

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &SiteFrame::clickHome);
	button->setTip(_T("Home"));
	_tools->Add(button);	// home

	button = new Button(0, theme, font_segoe, _T("\x270D"));
	button->Click.bind(this, &SiteFrame::clickHome);
	button->setTip(_T("Edit"));
	_tools->Add(button);

	button = new Button(0, theme, font_segoe, _T("+"));
	button->Click.bind(this, &SiteFrame::clickPlus);
	button->setTip(_T("Add"));
	_tools->Add(button);
	button = new Button(0, theme, font_segoe, _T("-"));
	button->Click.bind(this, &SiteFrame::clickMinus);
	button->setTip(_T("Remove"));
	_tools->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x4C"));
	button->Click.bind(this, &SiteFrame::clickHome);
	button->setTip(_T("Inspect"));
	_tools->Add(button);
	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &SiteFrame::clickHome);
	button->setTip(_T("Refresh"));
	_tools->Add(button);

	if (_set.getAdd() )
	{
		button = new Button(0, theme, font_segoe, _T("Commit"));
		button->Click.bind(this, &SiteFrame::clickMinus);
		_tools->Add(button);
	}

	tab = new Tab(0, theme, text, _T("Info"));
	tab->Click.bind(this, &SiteFrame::activateInfo);
	_tabset->Add(tab);

	// no history for multi-select or new additions.
	if ( _set.size() == 1 && !_set.getAdd() )
	{
		tab = new Tab(0, theme, text, _T("Channels"));
		tab->Click.bind(this, &SiteFrame::activateChannels);
		_tabset->Add(tab);
		tab = new Tab(0, theme, text, _T("History"));
		tab->Click.bind(this, &SiteFrame::activateHistory);
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

SiteFrame::~SiteFrame()
{
	if (_set.size() == 1)
	{
		Model::Site *site = _set.at(0);
		_factory.deactivate(site);
		_set.Remove.clear();
		if (_set.getAdd())
			delete site;
	}
}

void SiteFrame::clickHome(Button *)
{
	_factory.activate(_set.at(0)->_system);
}

void SiteFrame::clickEdit(Button *)
{
	bool enable = _grid->getEnable();
	_grid->setEnable(!enable);
}

void SiteFrame::clickPlus(Button *)
{
	_factory.activateChannel(_set.at(0));
}

void SiteFrame::clickMinus(Button *)
{
	std::vector<Model::Channel*> list;
	_set.at(0)->Channels.getSelected(list);
	_set.at(0)->Channels.clearSelect();
	for (size_t i = 0; i < list.size(); i++)
	{
		delete list[i];
	}
}

void SiteFrame::clickInspect(Button *)
{
	std::vector<Model::Channel*> list;
	_set.at(0)->Channels.getSelected(list);
	_factory.activate(list);
}

void SiteFrame::clickRefresh(Button *)
{
	Invalidate(FALSE);
}

void SiteFrame::clickClear(Button *)
{
	//
}

void SiteFrame::activateInfo(Tab*)
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

void SiteFrame::activateChannels(Tab*)
{
	_top->clear();
	_grid->setTable(&_channels);
	_grid->DoubleClick.bind(this, &SiteFrame::activateChannelPopup);
	_top->Add(_tools);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void SiteFrame::activateHistory(Tab*)
{
	_top->clear();
	_grid->setTable(NULL);
	_grid->DoubleClick.clear();
	_top->Add(_tools);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void SiteFrame::activateChannelPopup(Grid *, size_t row, size_t)
{
	Model::Channel *channel = _set.at(0)->Channels[row];
	_factory.activate(channel);
}

void SiteFrame::clickCommit(Button*)
{
	if ( _set.getAdd() )
	{
		_set.setAdd(false);
		Model::Site *site = _set.at(0);
		// TODO: check for duplicate.
		site->_system->Sites.insert(site->_site, site);
	}
}

void SiteFrame::remove()
{
	DestroyWindow();
}

bool SiteFrame::Create(RECT rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
