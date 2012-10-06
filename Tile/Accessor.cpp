#include "stdafx.h"
#include "Accessor.h"
#include <time.h>

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

ColorReference::ColorReference(IAccessor<color_t>& access) : _access(access)
{
}

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

bool ColorReference::setValue(const string_t &value)
{
	bool bOK = false;
	TCHAR *p = const_cast<TCHAR *>( value.c_str() );
	TCHAR *n = NULL;
	unsigned char red = static_cast<unsigned char>( _tcstol(p, &n, 10) );
	bOK = n != p;
	p = n;
	unsigned char grn = static_cast<unsigned char>( _tcstol(p, &n, 10) );
	bOK = bOK && n != p;
	p = n;
	unsigned char blu = static_cast<unsigned char>( _tcstol(p, &n, 10) );
	bOK = bOK && n != p;
	p = n;
	_access.setValue( RGB(red, grn, blu) );
	return bOK;
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

Time::Time(IAccessor<time_t>& wrap) : _wrap(wrap)
{
}

const string_t &Time::getValue() const
{
	struct tm tmValue = {0};
	time_t iValue = _wrap.getValue();

	if ( localtime_s(&tmValue, &iValue) == 0 )
	{
		const TCHAR *strFormat = NULL;

		time_t iNow = 0L;
		time(&iNow);
		if (iNow - iValue >= 24*60*60)
			strFormat = _T("%Y-%m-%d");
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

bool Time::setValue(const string_t &)
{
	// not implemented :)
	return false;
}
