#include "stdafx.h"
#include "Address.h"

/*
Copyright © 2012 Rick Parrish
*/

// accessor adapter for string to long.
Address::Address(IAccessor<Model::address_t>& wrap, int base) : 
	_wrap(wrap), _base(base)
{
}

const string_t &Address::getValue() const
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

bool Address::setValue(const string_t &value)
{
	TCHAR *cursor = NULL;
	Model::address_t iValue = (Model::address_t)_tcstol(value.c_str(), &cursor, _base);
	return cursor != value.c_str() && _wrap.setValue(iValue);
}
