#include "stdafx.h"
#include "Identity.h"

/*
Copyright © 2011 Rick Parrish
*/

using namespace Tiles;

identity_t generate(void)
{
	static identity_t id = 0;
	return ++id;
}
