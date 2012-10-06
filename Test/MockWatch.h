#include "../Tile/IControl.h"

/*
Copyright 2011 Rick Parrish
*/

#pragma once

using namespace Tiles;

struct MockWatch : public IRedraw
{
	/// <param name="pDraw">control in need of redrawing.</param>
	virtual void Redraw(ITile *pDraw);
};
