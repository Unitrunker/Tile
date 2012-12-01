#include "stdafx.h"
#include "Serial.h"
#include "../Tile/Combo.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Tiles;

Serial::Serial(const Serial &copy) : 
	_port(copy._port), _rate(copy._rate), _data(copy._data), _stop(copy._stop), _parity(copy._parity)
{
}

Serial::Serial(unsigned long rate, unsigned char data, unsigned char stop, unsigned char parity) : 
	_port(0), _rate(rate), _data(data), _stop(stop), _parity(parity)
{
}

Serial::Serial() : _port(0), _rate(115200), _data(8), _stop(1), _parity(0)
{
}

// Serial RS232 or RS485 communication device
SerialSet::SerialSet(Theme &theme) :
	_port(*this, &Serial::_port),
	_rate(*this, &Serial::_rate),
	_data(*this, &Serial::_data),
	_stop(*this, &Serial::_stop),
	_parity(*this, &Serial::_parity),
	Port(&_port),
	Rate(&_rate),
	Data(&_data),
	Stop(&_stop),
	Parity(&_parity)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	std::vector<Combo::Item> list;

	// TODO: replace this with real list of actual serial ports.
	static Combo::Item listPorts[] = 
	{
		{_T("COM1"), 1},
		{_T("COM2"), 2},
		{_T("COM4"), 4}
	};

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

	prop = new Property(_T("Port"), _T("Communication port"), new Combo(0, theme, textFont, listPorts, _countof(listPorts), &Port) );
	section->Items.push_back(prop);
	list.clear();

	prop = new Property(_T("Rate"), _T("Bit rate"), new Combo(0, theme, textFont, listRates, _countof(listRates), &Rate) );
	section->Items.push_back(prop);
	list.clear();

	prop = new Property(_T("Data"), _T("Data bits per character"), new Combo(0, theme, textFont, listData, _countof(listData), &Data) );
	section->Items.push_back(prop);
	list.clear();

	prop = new Property(_T("Stop"), _T("Stop bits"), new Combo(0, theme, textFont, listStop, _countof(listStop), &Stop) );
	section->Items.push_back(prop);
	list.clear();

	prop = new Property(_T("Parity"), _T("Parity bit"), new Combo(0, theme, textFont, listParity, _countof(listParity), &Parity) );
	section->Items.push_back(prop);
	list.clear();

	Add(section);
}
