#include "Property.h"
#include "Theme.h"
#include "Delegate.h"

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

struct ThemeSet : SetT<Theme>
{
	ThemeSet(Theme &theme);

private:

	struct ReflectFont : public Reflect<Theme, Font>
	{
		ReflectFont(IAccessor<Theme*> &base, Font Theme::*member) : Reflect<Theme, Font>(base, member) { };

		using Reflect<Theme, Font>::getValue;

		virtual bool setValue(const Font &value)
		{
			bool bOK = MemberAccessPtr<Theme, Font>::setValue(value);
			if (bOK)
			{
				// side effect - other fonts share height.
				_access.getValue()->Arrow._height = value._height;
				_access.getValue()->Stock._height = value._height;
				_access.getValue()->broadcast("Reflect");
			}
			return true;
		}
	};

	// updates all four arrow glyphs
	struct ArrowShape : public IAccessor<long>
	{
		ArrowShape(ThemeSet &base);
		virtual const long &getValue() const;
		virtual bool setValue(const long &value);
	private:
		ThemeSet& _base;
		mutable long _value;
	};

	// updates both checkbox glyphs
	struct CheckShape : public IAccessor<long>
	{
		CheckShape(ThemeSet &base);
		virtual const long &getValue() const;
		virtual bool setValue(const long &value);
	private:
		ThemeSet& _base;
		mutable long _value;
	};

	Reflect<Theme, color_t> _captionFore;
	Reflect<Theme, color_t> _captionBack;
	Reflect<Theme, color_t> _dataFore;
	Reflect<Theme, color_t> _dataBack;
	Reflect<Theme, color_t> _cellFore;
	Reflect<Theme, color_t> _cellBack;
	Reflect<Theme, color_t> _toolFore;
	Reflect<Theme, color_t> _toolBack;
	Reflect<Theme, color_t> _toolOver;
	Reflect<Theme, color_t> _gridLines;
	Reflect<Theme, meter_t> _gridThick;
	ArrowShape _arrowShape;
	CheckShape _checkShape;
	//Reflect<Theme, string_t> _arrowShape;
	Integer _gridThickText;
	ReflectFont _fontText;
	ReflectFont _fontArrow;
};

};
