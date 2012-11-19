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

SiteFrame::SiteFrame(Factory& factory, Model::Site *site) : 
	Window(factory._theme), _factory(factory), _site(site), _set(factory._theme), _channels(factory._theme)
{
	Theme &theme = factory._theme;
	for (size_t i = 0; i < _channels.getHeader()->Columns.size(); i++)
		_channels.setContent(i, &site->Channels);

	_top = new Pane(0, theme, eDown);
	_tools = new Tab(0, theme);
	_tabset = new Tab(0, theme);

	sophia::delegate2<void, Button*, bool> click;

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	click.bind(this, &SiteFrame::clickHome);
	_tools->Add(_T("\x48"), font_webdings, click);	// home
	click.clear();
	_tools->Add(_T("\x270D"), font_segoe, click);	// edit
	_tools->Add(_T("+"), font_segoe, click);		// plus
	_tools->Add(_T("-"), font_segoe, click);		// minus
	_tools->Add(_T("\x4C"), font_webdings, click);	// inspect
	_tools->Add(_T("\x71"), font_webdings, click);	// refresh

	click.bind(this, &SiteFrame::activateInfo);
	_tabset->Add(_T("Info"), click);
	click.bind(this, &SiteFrame::activateChannels);
	_tabset->Add(_T("Channels"), click);

	_set.setValue(site);

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
	_factory.deactivate(_site);
}

void SiteFrame::clickHome(Button *, bool up)
{
	if (up)
		_factory.activate(_site->_system);
}

void SiteFrame::activateInfo(Button *, bool up)
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

void SiteFrame::activateChannels(Button *, bool up)
{
	if (up)
	{
		_top->clear();
		_grid->setTable(&_channels);
		_grid->DoubleClick.bind(this, &SiteFrame::activateChannelPopup);
		_top->Add(_tools);
		_top->Add(_grid);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void SiteFrame::activateChannelPopup(Grid *, size_t row, size_t)
{
	Model::Channel *channel = _site->Channels[row];
	_factory.activate(channel);
}

bool SiteFrame::Create(RECT rect)
{
	return Window::Create(NULL, rect, _site->_label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
