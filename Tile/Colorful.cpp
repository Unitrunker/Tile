#include "stdafx.h"
#include "Colorful.h"

using namespace Tiles;

/*
Copyright © 2011, 2012 Rick Parrish
*/

Colorful::Colorful() :
	// color selections
	_foreEdit(Theme::eCellFore, RGB(0,0,0)),
	_foreView(Theme::eDataFore, RGB(0,0,0)),
	_backEdit(Theme::eCellBack, RGB(255,255,255)),
	_backView(Theme::eDataBack, RGB(192,192,192)),
	_foreLocal(_foreView, _foreEdit),
	_backLocal(_backView, _backEdit)
{
	_foreAccess = &_foreLocal;
	_backAccess = &_backLocal;
}

void Colorful::setForeAccess(IColorAccess *fore)
{
	if (fore)
		_foreAccess = fore;
	else
		_foreAccess = &_foreLocal;
}

void Colorful::setBackAccess(IColorAccess *back)
{
	if (back)
		_backAccess = back;
	else
		_backAccess = &_backLocal;
}

