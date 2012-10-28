#include "stdafx.h"
#include "Image.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Tiles;

Image::Image(identity_t id, Theme& theme) : Tile(id, theme)
{
}

Image::Image(const Image &copy) : Tile(copy)
{
}

/// <param name="canvas">Canvas where text will appear.</param>
bool Image::Draw(ICanvas *canvas, bool)
{
	canvas;
	return false;
}
