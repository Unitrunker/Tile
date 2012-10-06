#include "stdafx.h"
#include "MockWatch.h"
#include "../Tile/ITile.h"

/*
Copyright 2011 Rick Parrish
*/

/// <param name="pDraw">control in need of redrawing.</param>
void MockWatch::Redraw(ITile *pTile)
{
	printf("%ld called %s\n", pTile->identity(), __FUNCTION__);
}
