#include "stdafx.h"
#include "ThemeSet.h"
#include "Edit.h"
#include "PickColor.h"
#include "PickFont.h"
#include "Combo.h"

/*
Copyright © 2011 Rick Parrish
*/

namespace Tiles
{

static const TCHAR arrow1up[] = {0xFEFF, 0x25BC, 0};
static const TCHAR arrow1dn[] = {0xFEFF, 0x25BC, 0};
static const TCHAR arrow1lt[] = {0xFEFF, 0x25BC, 0};
static const TCHAR arrow1rt[] = {0xFEFF, 0x25BC, 0};

static const TCHAR arrow2up[] = {0xFEFF, 0x25BD, 0};
static const TCHAR arrow2dn[] = {0xFEFF, 0x25BD, 0};
static const TCHAR arrow2lt[] = {0xFEFF, 0x25BD, 0};
static const TCHAR arrow2rt[] = {0xFEFF, 0x25BD, 0};

static const TCHAR arrow3up[] = {0xFEFF, 0x25BE, 0};
static const TCHAR arrow3dn[] = {0xFEFF, 0x25BE, 0};
static const TCHAR arrow3lt[] = {0xFEFF, 0x25BE, 0};
static const TCHAR arrow3rt[] = {0xFEFF, 0x25BE, 0};

static const TCHAR arrow4up[] = {0xFEFF, 0x25BF, 0};
static const TCHAR arrow4dn[] = {0xFEFF, 0x25BF, 0};
static const TCHAR arrow4lt[] = {0xFEFF, 0x25BF, 0};
static const TCHAR arrow4rt[] = {0xFEFF, 0x25BF, 0};

static const TCHAR arrow5up[] = {0xFEFF, 0x2193, 0};
static const TCHAR arrow5dn[] = {0xFEFF, 0x2193, 0};
static const TCHAR arrow5lt[] = {0xFEFF, 0x2193, 0};
static const TCHAR arrow5rt[] = {0xFEFF, 0x2193, 0};

static const TCHAR arrow6up[] = {0xFEFF, 0x21A1, 0};
static const TCHAR arrow6dn[] = {0xFEFF, 0x21A1, 0};
static const TCHAR arrow6lt[] = {0xFEFF, 0x21A1, 0};
static const TCHAR arrow6rt[] = {0xFEFF, 0x21A1, 0};

static const TCHAR arrow7up[] = {0xFEFF, 0x21D3, 0};
static const TCHAR arrow7dn[] = {0xFEFF, 0x21D3, 0};
static const TCHAR arrow7lt[] = {0xFEFF, 0x21D3, 0};
static const TCHAR arrow7rt[] = {0xFEFF, 0x21D3, 0};

static const TCHAR arrow8up[] = {0xFEFF, 0x21E9, 0};
static const TCHAR arrow8dn[] = {0xFEFF, 0x21E9, 0};
static const TCHAR arrow8lt[] = {0xFEFF, 0x21E9, 0};
static const TCHAR arrow8rt[] = {0xFEFF, 0x21E9, 0};

static const TCHAR arrow9up[] = {0xFEFF, 0x261F, 0};
static const TCHAR arrow9dn[] = {0xFEFF, 0x261F, 0};
static const TCHAR arrow9lt[] = {0xFEFF, 0x261F, 0};
static const TCHAR arrow9rt[] = {0xFEFF, 0x261F, 0};

static const TCHAR *listArrows[][4] =
{
	{ arrow1up, arrow1dn, arrow1lt, arrow1rt },
	{ arrow2up, arrow2dn, arrow2lt, arrow2rt },
	{ arrow3up, arrow3dn, arrow3lt, arrow3rt },
	{ arrow4up, arrow4dn, arrow4lt, arrow4rt },
	{ arrow5up, arrow5dn, arrow5lt, arrow5rt },
	{ arrow6up, arrow6dn, arrow6lt, arrow6rt },
	{ arrow7up, arrow7dn, arrow7lt, arrow7rt },
	{ arrow8up, arrow8dn, arrow8lt, arrow8rt },
	{ arrow9up, arrow9dn, arrow9lt, arrow9rt }
};

ThemeSet::ArrowShape::ArrowShape(ThemeSet &base) : _base(base)
{
}

const long &ThemeSet::ArrowShape::getValue() const
{
	_value = _countof(listArrows);
	const Theme *theme =_base.getValue();
	for (size_t i = 0; i < _countof(listArrows); i++)
	{
		const TCHAR *glyph = listArrows[i][1];
		if (theme->ArrowDn.compare(glyph) == 0)
		{
			_value = i;
			break;
		}
	}
	return _value;
}

bool ThemeSet::ArrowShape::setValue(const long &value)
{
	_value = value;
	Theme *theme =_base.getValue();
	if (value >= 0 && value < _countof(listArrows) )
	{
		theme->ArrowUp = listArrows[value][0];
		theme->ArrowDn = listArrows[value][1];
		theme->ArrowLt = listArrows[value][2];
		theme->ArrowRt = listArrows[value][3];
		theme->broadcast("Reflect");
		return true;
	}
	return false;
}

static const TCHAR checked1[] = {0x2611, 0};
static const TCHAR uncheck1[] = {0x2610, 0};

static const TCHAR checked2[] = {0x2612, 0};
static const TCHAR uncheck2[] = {0x2610, 0};

static const TCHAR checked3[] = {0x2713, 0};
static const TCHAR uncheck3[] = {0x20, 0};

static const TCHAR checked4[] = {0x2714, 0};
static const TCHAR uncheck4[] = {0x20, 0};

static const TCHAR checked5[] = {0x2715, 0};
static const TCHAR uncheck5[] = {0x20, 0};

static const TCHAR checked6[] = {0x2716, 0};
static const TCHAR uncheck6[] = {0x20, 0};

static const TCHAR checked7[] = {0x2717, 0};
static const TCHAR uncheck7[] = {0x20, 0};

static const TCHAR checked8[] = {0x2718, 0};
static const TCHAR uncheck8[] = {0x20, 0};

static const TCHAR checked9[] = {0x2606, 0}; //{0x2729, 0};
static const TCHAR uncheck9[] = {0x2605, 0}; //{0x272D, 0};

static const TCHAR checked10[] = {0x25A0, 0};
static const TCHAR uncheck10[] = {0x25A1, 0};

static const TCHAR checked11[] = {0x25A3, 0};
static const TCHAR uncheck11[] = {0x25A1, 0};

static const TCHAR *listChecks[][2] =
{
	{ checked1, uncheck1 },
	{ checked2, uncheck2 },
	{ checked3, uncheck3 },
	{ checked4, uncheck4 },
	{ checked5, uncheck5 },
	{ checked6, uncheck6 },
	{ checked7, uncheck7 },
	{ checked8, uncheck8 },
	{ checked9, uncheck9 },
	{ checked10, uncheck10 },
	{ checked11, uncheck11 }
};

ThemeSet::CheckShape::CheckShape(ThemeSet &base) : _base(base)
{
}

const long &ThemeSet::CheckShape::getValue() const
{
	_value = _countof(listChecks);
	const Theme *theme =_base.getValue();
	for (size_t i = 0; i < _countof(listChecks); i++)
	{
		const TCHAR *glyph = listChecks[i][0];
		if (theme->Checked.compare(glyph) == 0)
		{
			_value = i;
			break;
		}
	}
	return _value;
}

bool ThemeSet::CheckShape::setValue(const long &value)
{
	_value = value;
	Theme *theme =_base.getValue();
	if (value >= 0 && value < _countof(listChecks) )
	{
		theme->Checked = listChecks[value][0];
		theme->Uncheck = listChecks[value][1];
		theme->broadcast("Reflect");
		return true;
	}
	return false;
}

ThemeSet::ThemeSet(Theme &theme) : 
	SetT<Theme>(&theme),
	_captionFore(*this, &Theme::CaptionFore),
	_captionBack(*this, &Theme::CaptionBack),
	_dataFore(*this, &Theme::DataFore),
	_dataBack(*this, &Theme::DataBack),
	_cellFore(*this, &Theme::CellFore),
	_cellBack(*this, &Theme::CellBack),
	_toolFore(*this, &Theme::ToolFore),
	_toolBack(*this, &Theme::ToolBack),
	_toolOver(*this, &Theme::ToolOver),
	_gridLines(*this, &Theme::GridLines),
	_gridThick(*this, &Theme::GridThick),
	_gridThickText(_gridThick),
	_fontText(*this, &Theme::Text),
	_fontArrow(*this, &Theme::Arrow),
	_arrowShape(*this),
	_checkShape(*this)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	Theme::Font arrowFont = {Theme::eArrow, theme.Arrow};

	section = new Section(_T("Display Font"), _T("Overall appearance"));
	prop = new Property(_T("Text"), _T("Font for text and edit controls"), new PickFont(0, theme, _T("Text Font"), &_fontText) );
	section->Items.push_back(prop);
	prop = new Property(_T("Arrow"), _T("Font for arrows and check boxes."), new PickFont(0, theme, _T("Arrow Font"), &_fontArrow) );
	section->Items.push_back(prop);
	Sections.push_back(section);

	section = new Section(_T("Caption"), _T("Captions and labels"));
	prop = new Property(_T("Fore"), _T("Caption text foreground color"), new PickColor(0, theme, _captionFore) );
	section->Items.push_back(prop);
	prop = new Property(_T("Back"), _T("Caption text background color"), new PickColor(0, theme, _captionBack) );
	section->Items.push_back(prop);
	Sections.push_back(section);
	
	section = new Section(_T("Data"), _T("Data display appearance") );
	prop = new Property( _T("Fore"), _T("Data foreground color"), new PickColor(0, theme, _dataFore) );
	section->Items.push_back(prop);
	prop = new Property( _T("Back"), _T("Data background color"), new PickColor(0, theme, _dataBack) );
	section->Items.push_back(prop);
	Sections.push_back(section);

	section = new Section(_T("Cell"), _T("Active cell") );
	prop = new Property( _T("Fore"), _T("Active cell's foreground color"), new PickColor(0, theme, _cellFore) );
	section->Items.push_back(prop);
	prop = new Property( _T("Back"), _T("Active cell's background color"), new PickColor(0, theme, _cellBack) );
	section->Items.push_back(prop);
	Sections.push_back(section);

	section = new Section(_T("Tool"), _T("Tool button display") );
	prop = new Property( _T("Fore"), _T("Tool's foreground color"), new PickColor(0, theme, _toolFore) );
	section->Items.push_back(prop);
	prop = new Property( _T("Back"), _T("Tool's background color"), new PickColor(0, theme, _toolBack) );
	section->Items.push_back(prop);
	prop = new Property( _T("Over"), _T("Color when mouse passes over"), new PickColor(0, theme, _toolOver) );
	section->Items.push_back(prop);
	Sections.push_back(section);

	section = new Section(_T("Grid"), _T("Grid appearance") );
	prop = new Property( _T("Color"), _T("Grid line color"), new PickColor(0, theme, _gridLines) );
	section->Items.push_back(prop);
	prop = new Property( _T("Thick"), _T("Grid line thickness"), new Edit(0, theme, textFont, &_gridThickText) );
	section->Items.push_back(prop);
	Sections.push_back(section);

	section = new Section(_T("Shapes"), _T("Arrow, Checkbox, List & Tree appearance") );
	std::vector<Combo::Item> items;
	for (size_t i = 0; i < _countof(listArrows); i++)
	{
		Combo::Item item = {listArrows[i][1], i};
		items.push_back(item);
	}
	Combo *comboArrow = new Combo(0, theme, arrowFont, items, &_arrowShape);
	prop = new Property( _T("Arrow"), _T("Arrow shape"), comboArrow);
	section->Items.push_back(prop);

	items.clear();
	for (size_t i = 0; i < _countof(listChecks); i++)
	{
		Combo::Item item = {listChecks[i][0], i};
		items.push_back(item);
	}
	Combo *comboCheck = new Combo(0, theme, arrowFont, items, &_checkShape);
	prop = new Property( _T("Check"), _T("Check box shape"), comboCheck);
	section->Items.push_back(prop);

	Sections.push_back(section);
}


};