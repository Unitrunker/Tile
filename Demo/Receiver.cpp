#include "stdafx.h"
#include "Receiver.h"
#include "../Tile/Grid.h"
#include "../Tile/Fill.h"
#include "../Tile/Text.h"
#include "../Tile/Edit.h"
#include "../Tile/Check.h"
#include <stdlib.h>

/*
Copyright © 2012 Rick Parrish
*/

ReceiverSet::ReceiverSet(Theme& theme) : 
	SetT<Receiver>(NULL),
#pragma warning(disable:4355)
	Label(*this, &Receiver::_label),
	Active(*this, &Receiver::_active)
#pragma warning(default:4355)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Receiver"), _T("Receiver"));
	prop = new Property(_T("Label"), _T("Receiver label"), new Edit(0, theme, textFont, &Label) );
	section->Items.push_back(prop);
	prop = new Property(_T("Active"), _T("Operating state"), new Check(0, theme, &Active) );
	prop->Control->setReadOnly(true);
	section->Items.push_back(prop);
	Add(section);
}

ReceiverFrame::ReceiverFrame(Factory &factory, Receiver *receiver) : 
	Window(factory._theme), _factory(factory), _receiver(receiver), _serialSet(factory._theme), _soundSet(factory._theme)
{
	Theme &theme = factory._theme;
	theme.setHeight(14);

	_serial._data = 8;
	_serial._parity = 0;
	_serial._port = 1;
	_serial._rate = 115200;
	_serial._stop = 1;

	_sound._channel = 0;
	_sound._bits = 16;
	_sound._channels = 1;
	_sound._port = 0;
	_sound._rate = 96000;

	_serialSet.setValue(&_serial);
	_soundSet.setValue(&_sound);

	sophia::delegate2<void, Button*, bool> click;

	_top = new Pane(0, theme, eDown);
	_tabset = new Tab(0, theme);
	_tools = new Tab(0, theme);
	_list = new List(0, theme);

	// receiver accepts some form of control?
	if (_receiver->_control)
	{
		// assume connection is via serial port.
		click.bind(this, &ReceiverFrame::serial);
		_tabset->Add(_T("Serial"), click);
	}

	for (size_t i = 0; i < _receiver->_VCOs.size(); i++)
	{
		static const TCHAR * labels[] = {_T("VCO A"), _T("VCO B")};
		VCO *vco = _receiver->_VCOs[i];
		if (vco->_signal)
		{
			click.bind(this, &ReceiverFrame::sound);
			_tabset->Add(labels[i], click);
		}
	}

	click.clear();

	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Font segoe(_T("Segoe UI Symbol"), 24, 0);
	Theme::Font font_segoe = { Theme::eDefault, segoe };

	_tools->Add(L"\x231A", font_segoe, click); // watch
	_tools->Add(L"\x231B", font_segoe, click); // hourglass
	_tools->Add(L"\x2328", font_segoe, click); // keyboard
	_tools->Add(L"\x2388", font_segoe, click); // Helm
	_tools->Add(L"\x2622", font_segoe, click); // radioactive
	_tools->Add(L"\x2623", font_segoe, click); // biohazard

	click.bind(this, &ReceiverFrame::clickHome);
	_tools->Add(L"\x48", font_webdings, click); // house
	click.clear();
	_tools->Add(L"\x22", font_webdings, click); // web
	_tools->Add(L"\x40", font_webdings, click); // tools
	click.bind(this, &ReceiverFrame::clickRefresh);
	_tools->Add(L"\x71", font_webdings, click); // refresh
	click.clear();
	_tools->Add(L"\x72", font_webdings, click); // X
	click.bind(this, &ReceiverFrame::clickAbout);
	_tools->Add(L"\x73", font_webdings, click); // ?
	click.clear();

	_list->setItems(&_serialSet);
	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	// connect the form to the frame window.
	setPane(_top);
}

ReceiverFrame::~ReceiverFrame()
{
	_factory.deactivate(_receiver);
}

void ReceiverFrame::serial(Button*, bool bDown)
{
	if (!bDown)
	{
		_top->clear();
		_list->clear();
		_list->setItems(&_serialSet);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void ReceiverFrame::sound(Button*, bool bDown)
{
	if (!bDown)
	{
		_top->clear();
		_list->setItems(&_soundSet);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void ReceiverFrame::clickHome(Button*, bool up)
{
	if (up)
		_factory.activate(SW_SHOW);
}

void ReceiverFrame::clickRefresh(Button*, bool up)
{
	if (up)
		Invalidate(FALSE);
}

void ReceiverFrame::clickAbout(Button*, bool)
{
	// TODO
}

bool ReceiverFrame::Create(RECT &rect)
{
	return Window::Create(NULL, rect, _receiver->_label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
