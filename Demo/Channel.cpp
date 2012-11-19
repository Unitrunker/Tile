#include "stdafx.h"
#include "Channel.h"
#include "../Tile/Edit.h"

/*
Copyright © 2012 Rick Parrish
*/

// accessor adapter for string to channel_t.
LCN::LCN(IAccessor<Model::channel_t>& wrap, int base) : 
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
	Model::channel_t iValue = (Model::channel_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
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

ChannelFrame::ChannelFrame(Factory& factory, Model::Channel *channel) : 
	Window(factory._theme), _factory(factory), _channel(channel), _set(factory._theme)
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

	click.bind(this, &ChannelFrame::clickHome);
	_tools->Add(_T("\x48"), font_webdings, click);	// home
	click.clear();
	_tools->Add(_T("\x270D"), font_segoe, click);	// edit
	_tools->Add(_T("+"), font_segoe, click);		// plus
	_tools->Add(_T("-"), font_segoe, click);		// minus
	_tools->Add(_T("\x4C"), font_webdings, click);	// inspect
	_tools->Add(_T("\x71"), font_webdings, click);	// refresh

	click.bind(this, &ChannelFrame::activateInfo);
	_tabset->Add(_T("Info"), click);
	click.bind(this, &ChannelFrame::activateHistory);
	_tabset->Add(_T("History"), click);

	_set.setValue(channel);

	_grid = new Grid(0, theme);
	_list = new List(0, theme);
	_list->setItems(&_set);

	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	setPane(_top);
}

ChannelFrame::~ChannelFrame()
{
	_factory.deactivate(_channel);
}

void ChannelFrame::clickHome(Button *, bool up)
{
	if (up)
	{
		_factory.activate(_channel->_site);
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
		_top->Add(_tools);
		_top->Add(_list);
		_top->Add(_tabset);
		_top->reflow();
	}
}

void ChannelFrame::activateHistory(Button *, bool up)
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

bool ChannelFrame::Create(RECT rect)
{
	return Window::Create(NULL, rect, _channel->_label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
