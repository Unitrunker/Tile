#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Button.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

// accessor adapter for string to address_t.
struct Address : public IAccessor<string_t>
{
	Address(IAccessor<Model::address_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<Model::address_t>& _wrap;
	mutable string_t _text;
	int _base;
	Address& operator = (const Address &never);
};

