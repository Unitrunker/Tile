#include "stdafx.h"
#include "Serial.h"
#include "../Tile/Combo.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Tiles;

// Serial RS232 or RS485 communication device
SerialSet::SerialSet(Theme &theme) :
	SetT<Serial>(NULL),
	_port(*this, &Serial::_port),
	_rate(*this, &Serial::_rate),
	_data(*this, &Serial::_data),
	_stop(*this, &Serial::_stop),
	_parity(*this, &Serial::_parity),
	Rate(&_rate),
	Data(&_data),
	Stop(&_stop),
	Parity(&_parity)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	std::vector<Combo::Item> list;

	static Combo::Item listRates[] = 
	{
		{_T("115200"), 115200L},
		{_T("57600"), 57600},
		{_T("38400"), 38400},
		{_T("19200"), 19200},
		{_T("9600"), 9600},
		{_T("4800"), 4800}
	};

	static Combo::Item listData[] = 
	{
		{_T("7"), 7L},
		{_T("8"), 8L}
	};

	static Combo::Item listStop[] = 
	{
		{_T("1"), 1L},
		{_T("2"), 2L}
	};

	static Combo::Item listParity[] = 
	{
		{_T("None"), 0L},
		{_T("Even"), 2L},
		{_T("Odd"), 1L}
	};

	section = new Section(_T("Serial Port"), _T("Serial Communications Port"));

	for (size_t i = 0; i < _countof(listRates); i++)
		list.push_back(listRates[i]);
	prop = new Property(_T("Rate"), _T("Bit rate"), new Combo(0, theme, textFont, list, &Rate) );
	section->Items.push_back(prop);
	list.clear();

	for (size_t i = 0; i < _countof(listData); i++)
		list.push_back(listData[i]);
	prop = new Property(_T("Data"), _T("Data bits per character"), new Combo(0, theme, textFont, list, &Data) );
	section->Items.push_back(prop);
	list.clear();

	for (size_t i = 0; i < _countof(listStop); i++)
		list.push_back(listStop[i]);
	prop = new Property(_T("Stop"), _T("Stop bits"), new Combo(0, theme, textFont, list, &Stop) );
	section->Items.push_back(prop);
	list.clear();

	for (size_t i = 0; i < _countof(listParity); i++)
		list.push_back(listParity[i]);
	prop = new Property(_T("Parity"), _T("Parity bit"), new Combo(0, theme, textFont, list, &Parity) );
	section->Items.push_back(prop);
	list.clear();

	Add(section);
}
