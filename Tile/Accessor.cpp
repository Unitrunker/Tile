#include "stdafx.h"
#include "Accessor.h"
#include <time.h>

/*
Copyright © 2011,2012 Rick Parrish
*/

using namespace Tiles;

// cursor - buffer pointer
// length - max number of digits.
// result - parsed integer value.
// returns false if unsuccessful, true if at least one digit extracted.
// TODO: support base other than 10.
static bool tcsntol(const TCHAR *&cursor, size_t length, long &result)
{
	bool once = false;
	long value = 0;

	while (length > 0)
	{
		int ch = *cursor;
		if ( isdigit(ch) )
		{
			value = value * 10 + ch - '0';
			length--;
			cursor++;
			once = true;
		}
		else break;
	}
	if (once)
		result = value;
	return once;
}

// TODO: support optional alpha channel someday.
ColorReference::ColorReference(IAccessor<color_t>& access) : _access(access)
{
}

// extract RGB text
const string_t& ColorReference::getValue() const
{
	color_t rgb = _access.getValue();
	TCHAR fill = _T(' ');
	_text.resize(32, fill);
	unsigned char red = static_cast<unsigned char>(rgb);
	rgb >>= 8;
	unsigned char grn = static_cast<unsigned char>(rgb);
	rgb >>= 8;
	unsigned char blu = static_cast<unsigned char>(rgb);
	int size = wsprintf(&_text[0], _T("%d %d %d"), red, grn, blu);
	_text.resize(size);
	return _text;
}

// compose RGB value from string.
bool ColorReference::setValue(const string_t &value)
{
	const TCHAR *cursor = value.c_str();
	long octet = 0;

	if ( tcsntol(cursor, 3, octet) )
	{
		unsigned char red = static_cast<unsigned char>(octet);
		if (*cursor == ' ') 
		{
			cursor++;
			if ( tcsntol(cursor, 3, octet) )
			{
				unsigned char grn = static_cast<unsigned char>(octet);
				if (*cursor == ' ') 
				{
					cursor++;
					if ( tcsntol(cursor, 3, octet) )
					{
						unsigned char blu = static_cast<unsigned char>(octet);
						color_t color = RGB(red, grn, blu);
						return _access.setValue(color);
					}
				}
			}
		}
	}
	return false;
}

// accessor adapter for string to long.
Integer::Integer(IAccessor<long>& wrap, int base) : 
	_wrap(wrap), _base(base)
{
}

const string_t &Integer::getValue() const
{
	int size = 0;
	_text.resize(32, _T(' '));
	switch (_base)
	{
		case 8:
			size = wsprintf(&_text[0], _T("%o"), _wrap.getValue());
			break;
		case 16:
			size = wsprintf(&_text[0], _T("%X"), _wrap.getValue());
			break;
		case 10:
		default:
			size = wsprintf(&_text[0], _T("%d"), _wrap.getValue());
			break;
	}
	_text.resize(size);
	return _text;
}

bool Integer::setValue(const string_t &value)
{
	TCHAR *cursor = NULL;
	long iValue = _tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}

Time::Time(IAccessor<time_t>& wrap, char iFormat) : _wrap(wrap), _iFormat(iFormat)
{
}

// short, date, or time.
void Time::setFormat(char iFormat)
{
	_iFormat = iFormat;
}

// retrieve value as a string.
const string_t &Time::getValue() const
{
	struct tm tmValue = {0};
	time_t iValue = _wrap.getValue();

	if ( localtime_s(&tmValue, &iValue) == 0 )
	{
		const TCHAR *strFormat = NULL;

		time_t iNow = 0L;
		time(&iNow);

		// date format or short format on value older than one day.
		if (_iFormat == eDate || _iFormat == eShort && iNow - iValue >= 24*60*60)
			strFormat = _T("%Y-%m-%d");
		// time format or short format less than a day old.
		else
			strFormat = _T("%H:%M:%S");

		_text.resize(16, _T(' '));
		int size = _tcsftime(&_text[0], _text.size(), strFormat,  &tmValue);
		_text.resize(size);
	}
	else if (_text.size() > 0)
	{
		_text.clear();
	}
	return _text;
}

bool Time::setValue(const string_t &value)
{
	struct tm tmValue = {0};
	time_t iValue = _wrap.getValue();

	localtime_s(&tmValue, &iValue);

	const TCHAR *cursor = value.c_str();
	// date format
	if (_iFormat == eDate)
	{
		// useful to see what these values were just before changing them.
		long year = tmValue.tm_year + 1900;
		long month = tmValue.tm_mon + 1;
		long day = tmValue.tm_mday;

		if ( tcsntol(cursor, 4, year) )
		{
			if (*cursor == '-' || *cursor == '/') cursor++;
			if ( tcsntol(cursor, 2, month) )
			{
				if (*cursor == '-' || *cursor == '/') cursor++;
				if ( tcsntol(cursor, 2, day) )
				{
					tmValue.tm_year = static_cast<int>(year - 1900);
					tmValue.tm_mon = static_cast<int>(month - 1);
					tmValue.tm_mday = static_cast<int>(day);
					iValue = mktime(&tmValue);
					return _wrap.setValue(iValue);
				}
			}
		}
		return false;
	}
	// time format
	else if (_iFormat == eTime)
	{
		// useful to see what these values were just before changing them.
		long hours = tmValue.tm_hour;
		long minutes = tmValue.tm_min;
		long seconds = tmValue.tm_sec;

		if ( tcsntol(cursor, 2, hours) )
		{
			if (*cursor == ':') cursor++;
			if ( tcsntol(cursor, 2, minutes) )
			{
				if (*cursor == ':') cursor++;
				if ( tcsntol(cursor, 2, seconds) )
				{
					tmValue.tm_hour = static_cast<int>(hours);
					tmValue.tm_min = static_cast<int>(minutes);
					tmValue.tm_sec = static_cast<int>(seconds);
					iValue = mktime(&tmValue);
					return _wrap.setValue(iValue);
				}
			}
		}
		return false;
	}
	// short format is display only.
	return false;
}

// Display IPv4 dotted address.
// Address is presumed to be in network byte order.
// No winsock dependencies.
IPv4::IPv4(IAccessor<long>& wrap) : _wrap(wrap)
{
}

// retrieve dotted IP address.
const string_t &IPv4::getValue() const
{
	long iIPv4 = _wrap.getValue();
	unsigned char *octets = (unsigned char *)&iIPv4;
	_text.resize(16, _T(' '));
	size_t size = wsprintf(&_text[0], _T("%d.%d.%d.%d"), 
		octets[0], octets[1], octets[2], octets[3]);
	_text.resize(size);
	return _text;
}

// parse dotted IP address.
bool IPv4::setValue(const string_t &value)
{
	const TCHAR *cursor = value.c_str();
	long octet = 0;
	long result = 0;
	unsigned char *octets = (unsigned char *)&result;
	// parse first octet
	if ( tcsntol(cursor, 3, octet) )
	{
		octets[0] = static_cast<unsigned char>(octet);
		if (*cursor == _T('.'))
		{
			cursor++;
			// parse second octet
			if ( tcsntol(cursor, 3, octet) )
			{
				octets[1] = static_cast<unsigned char>(octet);
				if (*cursor == _T('.'))
				{
					cursor++;
					// parse third octet
					if ( tcsntol(cursor, 3, octet) )
					{
						octets[2] = static_cast<unsigned char>(octet);
						if (*cursor == _T('.'))
						{
							cursor++;
							// parse last octet
							if ( tcsntol(cursor, 3, octet) )
							{
								octets[3] = static_cast<unsigned char>(octet);
								_wrap.setValue(result);
							}
						}
					}
				}
			}
		}
	}
	return false;
}
