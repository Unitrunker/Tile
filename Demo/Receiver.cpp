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

Receiver::~Receiver()
{
	onRemove(this);
}

void Receiver::broadcast(const char *)
{
	onChange(this);
}

Tiles::Set *::Control::getUI(Theme &theme)
{
	if (_set == NULL)
	{
		_set = new SerialSet(theme);
		_set->setObject(&_serial);
	}
	return _set;
}

Tiles::Set *SignalVCO::getUI(Theme &theme)
{
	if (_set == NULL)
	{
		_set = new SoundSet(theme);
		_set->setObject(&_sound);
	}
	return _set;
}

ReceiverSet::ReceiverSet(Theme& theme) : 
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

bool ReceiverSet::getCaption(string_t &label) const
{
	if (size() == 1)
	{
		Receiver *receiver = at(0);
		label = receiver->_label;
	}
	else
	{
		TCHAR caption[32] = {0};
		wsprintf(caption, _T("%d Receivers"), size());
		label = caption;
	}
	return true;
}

ReceiverFrame::ReceiverFrame(Factory &factory, Receiver *receiver) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObject(receiver);
	inside();
}

ReceiverFrame::ReceiverFrame(Factory &factory, std::vector<Receiver *> &list) : 
	Window(factory._theme), _factory(factory), _set(factory._theme)
{
	_set.setObjects(list);
	inside();
}

void ReceiverFrame::inside()
{
	Theme &theme = _factory._theme;
	theme.setHeight(14);
	Font webdings(_T("Webdings"), 24, 1);
	Theme::Font font_webdings = { Theme::eDefault, webdings };
	Theme::Font text = {Theme::eText, theme.Text};
	Button* button = NULL;
	Tab* tab = NULL;

	_set.Remove.bind(this, &ReceiverFrame::remove);

	_top = new Pane(0, theme, eDown);
	_tabset = new Pane(0, theme, eRight);
	_tools = new Pane(0, theme, eRight);
	_list = new List(0, theme);

	Theme::Color color(Theme::eToolOver, theme.ToolOver);
	_tabset->setLineColor(color);
	_tools->setLineColor(color);

	tab = new Tab(0, theme, text, _T("Info"));
	tab->Click.bind(this, &ReceiverFrame::clickInfo);
	_tabset->Add(tab);

	// receiver accepts some form of control?
	if (_set.size() == 1)
	{
		Receiver *receiver = _set.at(0);
		if (receiver->_control)
		{
			tab = new Tab(0, theme, text, _T("Serial"));
			tab->Click.bind(this, &ReceiverFrame::clickControl);
			_tabset->Add(tab);
		}

		for (size_t i = 0; i < receiver->_VCOs.size(); i++)
		{
			static const TCHAR * labels[] = {_T("VCO A"), _T("VCO B")};
			VCO *vco = receiver->_VCOs[i];
			if (vco->_signal)
			{
				// get index to last control
				size_t last = _tabset->getControlCount();
				tab = new Tab(0, theme, text, labels[i]);
				tab->Click.bind(this, &ReceiverFrame::clickVCO);
				_tabset->Add(tab);

				// get last control (just added).
				IControl *control = _tabset->getControl(last);
				// map control to vco. See clickVCO() below.
				_map[control] = vco;
			}
		}
	}

	//Font segoe(_T("Segoe UI Symbol"), 24, 0);
	//Theme::Font font_segoe = { Theme::eDefault, segoe };

	//_tools->Add(L"\x231A", font_segoe, click);	// watch
	//_tools->Add(L"\x231B", font_segoe, click);	// hourglass
	//_tools->Add(L"\x2328", font_segoe, click);	// keyboard
	//_tools->Add(L"\x2388", font_segoe, click);	// Helm
	//_tools->Add(L"\x2622", font_segoe, click);	// radioactive
	//_tools->Add(L"\x2623", font_segoe, click);	// biohazard

	button = new Button(0, theme, font_webdings, _T("\x48"));
	button->Click.bind(this, &ReceiverFrame::clickHome);
	button->setTip(_T("Home"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x34"));
	button->Click.bind(this, &ReceiverFrame::clickStart);
	button->setTip(_T("Start"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x3C"));
	button->Click.bind(this, &ReceiverFrame::clickStop);
	button->setTip(_T("Stop"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x71"));
	button->Click.bind(this, &ReceiverFrame::clickRefresh);
	button->setTip(_T("Start"));
	_tools->Add(button);

	button = new Button(0, theme, font_webdings, _T("\x72"));
	button->Click.bind(this, &ReceiverFrame::clickDelete);
	button->setTip(_T("Stop"));
	_tools->Add(button);

	_list->setItems(&_set);
	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	// connect the form to the frame window.
	setPane(_top);
}

ReceiverFrame::~ReceiverFrame()
{
	if (_set.size() == 1)
		_factory.deactivate(_set.at(0));
}

void ReceiverFrame::clickInfo(Tab*)
{
	_top->clear();
	_list->clear();
	_list->setItems(&_set);
	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	_top->reflow();
}

void ReceiverFrame::clickControl(Tab*)
{
	_top->clear();
	_list->clear();
	Set *set = _set.at(0)->getUI(_factory._theme);
	_list->setItems(set);
	_top->Add(_tools, 1, 1, 0, true);
	_top->Add(_list, 0, 4096, 1, false);
	_top->Add(_tabset, 1, 1, 0, true);
	_top->reflow();
}

void ReceiverFrame::clickVCO(Tab* button)
{
	_top->clear();
	std::map<IControl*, VCO*>::iterator it = _map.find(button);
	if (it != _map.end())
	{
		Set *set = it->second->getUI(_factory._theme);
		_list->setItems(set);
		_top->Add(_tools, 1, 1, 0, true);
		_top->Add(_list, 0, 4096, 1, false);
		_top->Add(_tabset, 1, 1, 0, true);
		_top->reflow();
	}
}

void ReceiverFrame::clickHome(Button*)
{
	_factory.activate(SW_SHOW);
}

void ReceiverFrame::clickRefresh(Button*)
{
	Invalidate(FALSE);
}

void ReceiverFrame::clickDelete(Button*)
{
	if (_set.size() == 1)
	{
		int id = MessageBox(_T("Do you want to delete this receiver?"), 
			_T("Confirm Delete Receiver"), MB_ICONEXCLAMATION|MB_YESNO);
		if (id == IDYES)
		{
			Receiver *kill = _set.at(0);
			delete kill;
		}
	}
}

void ReceiverFrame::clickStart(Button*)
{
	_set.Active.setValue(true);
}

void ReceiverFrame::clickStop(Button*)
{
	_set.Active.setValue(false);
}

void ReceiverFrame::remove()
{
	DestroyWindow();
}

bool ReceiverFrame::Create(RECT &rect)
{
	string_t label;
	_set.getCaption(label);
	return Window::Create(NULL, rect, label.c_str(), WS_OVERLAPPEDWINDOW|WS_VISIBLE, WS_EX_OVERLAPPEDWINDOW) != NULL;
}
