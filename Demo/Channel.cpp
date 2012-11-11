#include "stdafx.h"
#include "Channel.h"
#include "../Tile/Edit.h"

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

ChannelFrame::ChannelFrame(Theme &theme, Model::Channel *channel) : 
	Window(theme), _channel(channel), _set(theme)
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

	click.bind(this, &ChannelFrame::activateHome);
	_tabset->Add(_T("Home"), click);
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
