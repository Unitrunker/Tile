#include "stdafx.h"
#include "Types.h"

using namespace Model;

// trunking system types
const TCHAR *getTrunkType(trunk_t value)
{
	static const TCHAR *list[] = 
	{
		_T("Unknown"),
		_T("APCOP25"),
		_T("DMR"),
		_T("EDACS48"),
		_T("EDACS96"),
		_T("Motorola"),
		_T("MPT1327"),
		_T("NXDN"),
		_T("OpenSky2"),
		_T("OpenSky4")
	};
	return list[value];
}

// service types for voice.
const TCHAR *getServiceType(service_t value)
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
