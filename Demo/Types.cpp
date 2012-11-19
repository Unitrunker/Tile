#include "stdafx.h"
#include "Types.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Model;

// trunking system types
const TCHAR *Model::getTrunkType(trunk_t value)
{
	static const struct 
	{
		trunk_t _code;
		TCHAR *_label;
	}
	list[] = 
	{
		{DMR, _T("DMR")},
		{EDACS48, _T("EDACS48")},
		{EDACS96, _T("EDACS96")},
		{LTR, _T("LTR")},
		{Motorola, _T("Motorola")},
		{MPT1327, _T("MPT1327")},
		{NXDN, _T("NXDN")},
		{OpenSky2, _T("OpenSky2")},
		{OpenSky4, _T("OpenSky4")},
		{Passport, _T("Passport")},
		{P25, _T("Project 25")}
	};
	for (size_t i = 0; i < _countof(list); i++)
	{
		if (list[i]._code == value)
			return list[i]._label;
	}
	return _T("Unknown");
}

// service types for voice.
const TCHAR *Model::getServiceType(service_t value)
{
	static const TCHAR *list[] = 
	{
		_T("Unknown"),
		_T("Analog"),
		_T("Data"),
		_T("Encrypted"),
		_T("VSELP"),
		_T("ProVoice"),
		_T("P25CAI"),
		_T("P25X2"),
		_T("P25P2")
	};
	return list[value];
}

// compare two networks.
bool Network::less(const Network &left, const Network &right)
{
	return left < right;
}

// less-than
bool Network::operator < (const Network &right) const
{
	if (_type < right._type)
		return true;
	if (_type == right._type)
		return _network < right._network;
	return false;
}

// equal
bool Network::operator == (const Network &right) const
{
	return _type == right._type && _network == right._network;
}
