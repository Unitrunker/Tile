#include "stdafx.h"
#include "Channel.h"
#include "../Tile/Edit.h"
#include "../Tile/Button.h"
#include "../Tile/Tab.h"

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
#pragma warning(disable:4355)
	_first(*this, &Model::Channel::_first),
	_last(*this, &Model::Channel::_last),
	_lcn(*this, &Model::Channel::_LCN),
	_frequency(*this, &Model::Channel::_hz),
	Label(*this, &Model::Channel::_label),
	Control(*this, &Model::Channel::_control),
#pragma warning(default:4355)
	First(_first, Time::eDate),
	Last(_last, Time::eDate),
	LCN(_lcn, 10),
	Frequency(_frequency, 10),
	_add(false)
{
	Section *section = NULL;
	Property *prop = NULL;
	Edit *edit = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Channel"), _T("Channel"));

	edit = new Edit(0, theme, textFont, &LCN);
	edit->setReadOnly(true);
	_primary = edit;
	prop = new Property(_T("LCN"), _T("Channel number"), edit);
	section->Items.push_back(prop);

	edit = new Edit(0, theme, textFont, &Frequency);
	prop = new Property(_T("Frequency"), _T("Frequency"), edit);
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

bool ChannelSet::getCaption(string_t &label) const
{
	TCHAR caption[32] = {0};
	Model::Channel *channel = at(0);
	wsprintf(caption, _T("Channel %0X"), channel->_LCN);
	label = caption;
	return true;
}

// allow or disallow editing of the primary key.
void ChannelSet::setAdd(bool add)
{
	_add = add;
	_primary->setReadOnly(!add);
}

// allow or disallow editing of the primary key.
bool ChannelSet::getAdd() const
{
	return _add;
}

ChannelFrame::ChannelFrame(Factory& factory, Model::Site *site) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	Model::Channel *channel = new Model::Channel(site, 0);
	_set.setObject(channel);
	_set.setAdd(true);
	inside();
}

ChannelFrame::ChannelFrame(Factory& factory, Model::Channel *channel) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObject(channel);
	inside();
}

ChannelFrame::ChannelFrame(Factory& factory, std::vector<Model::Channel *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void ChannelFrame::inside()
{
	Theme &theme = _factory._theme;
	_top = new Pane(0, theme, eDown);
	_tools = new Pane(0, theme, eRight);
	_tabset = new Pane(0, theme, eRight);
	Tab* tab = NULL;
	Button* button = NULL;

	_set.Remove.bind(this, &ChannelFrame::remove);
	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_tools->setLineColor(color);

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &ChannelFrame::clickHome);
	button->setTip(_T("Home"));
	_tools->Add(tab);

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &ChannelFrame::clickRefresh);
	button->setTip(_T("Refresh"));
	_tools->Add(button);

	if ( _set.getAdd() )
	{
		button = new Button(0, theme, font_segoe, _T("Commit"));
		button->Click.bind(this, &ChannelFrame::clickCommit);
		_tools->Add(button);
	}

	tab = new Tab(0, theme, font_segoe, _T("Info") );
	tab->Click.bind(this, &ChannelFrame::activateInfo);
	_tabset->Add(tab);

	if (_set.size() == 1 && !_set.getAdd() )
	{
		tab = new Tab(0, theme, font_segoe, _T("History") );
		tab->Click.bind(this, &ChannelFrame::activateHistory);
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

ChannelFrame::~ChannelFrame()
{
	if (_set.size() == 1)
		_factory.deactivate(_set.at(0));
}

void ChannelFrame::clickHome(Button *)
{
	_factory.activate(_set.at(0)->_site);
}

void ChannelFrame::clickRefresh(Button *)
{
	Invalidate(FALSE);
}

void ChannelFrame::clickCommit(Button *)
{
	if ( _set.getAdd() )
	{
		apply();

		Model::Channel *channel = _set.at(0);
		// check for duplicate.
		Model::Channel *origin = channel->_site->newChannel(channel->_LCN, false);
		if (origin == NULL)
		{
			_set.setAdd(false);
			channel->_site->Channels.insert(channel->_LCN, channel);
		}
		else
		{
			int res = MessageBox(_T("Click YES to merge and edit\nclick NO to discard and edit\nor click CANCEL"), 
				_T("Group ID Exists!"), MB_ICONEXCLAMATION|MB_YESNOCANCEL);
			switch (res)
			{
				case IDYES:
					if (channel->_label.size() > 0)
						origin->_label = channel->_label;
					// this will implicitly destroy this window.
					delete channel;
					_factory.deactivate(channel);
					_factory.activate(origin);
					break;

				case IDNO:
					// this will implicitly destroy this window.
					delete channel;
					_factory.deactivate(channel);
					_factory.activate(origin);
					break;

				case IDCANCEL:
					break;
			}
		}
	}
}

void ChannelFrame::activateInfo(Tiles::Tab *)
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

void ChannelFrame::activateHistory(Tiles::Tab *)
{
	_top->clear();
	_grid->DoubleClick.clear();
	_top->Add(_tools);
	_top->Add(_grid);
	_top->Add(_tabset);
	_top->reflow();
}

void ChannelFrame::remove()
{
	DestroyWindow();
}

bool ChannelFrame::Create(RECT rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
