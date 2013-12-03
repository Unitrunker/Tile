#include "stdafx.h"
#include <windows.h>
#include "MockCanvas.h"
#include "MockWatch.h"
#include "MockWindow.h"
#include "../Tile/Fill.h"
#include "../Tile/Arrow.h"
#include "../Tile/Combo.h"
#include "../Tile/Text.h"
#include "../Tile/Button.h"
#include "../Tile/Check.h"
#include "../Tile/Edit.h"
#include "../Tile/Scroll.h"
#include "../Tile/Pane.h"
#include "../Tile/Tree.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Glyph.h"
#include "../Tile/Toggle.h"
#include "../Stream/FileOutputStream.h"
#include "../JSONTile/Factory.h"
#include "../JSON/Writer.h"
#include <io.h>

/*
Copyright 2011-2012 Rick Parrish
*/

using namespace Tiles;

#ifdef UNICODE
#define _tfprintf fwprintf
#else
#define _tfprintf fprintf
#endif

static void testITile(ITile *pTile, MockWatch &watch)
{
	MockCanvas canvas;

	_tfprintf(stdout, _T("Test %s starts on %ld\n"), _T(__FUNCTION__), pTile->identity());

	rect_t rect = {100, 100, 100, 100};
	pTile->setRect(rect);

	// notify test 1
	_tfprintf(stdout, _T("%ld should *not* trigger Redraw\n"), pTile->identity());
	pTile->setChanged(false);
	pTile->watch(&watch);
	// notify test 2
	_tfprintf(stdout, _T("%ld should trigger Redraw\n"), pTile->identity());
	pTile->setChanged(true);
	// notify test 3
	_tfprintf(stdout, _T("%ld should trigger another Redraw\n"), pTile->identity());
	pTile->watch(&watch);
	// setChanged test
	_tfprintf(stdout, _T("%ld change state before draw %d\n"), pTile->identity(), pTile->getChanged());
	pTile->Draw(&canvas, false);
	_tfprintf(stdout, _T("%ld change state after draw %d\n"), pTile->identity(), pTile->getChanged());
}

static void testIControl(IControl *pControl, MockWatch &watch)
{
	MockWindow desktop;
	KeyEvent key;
	MouseEvent mouse;

	key._what = KeyEvent::DOWN;
	key._code = VK_RETURN;
	key._mask = 0;

	mouse._what = MouseEvent::eDownClick;
	mouse._button = MouseEvent::eLeft;
	mouse._place.x = 1;
	mouse._place.y = 1;

	_tfprintf(stdout, _T("Test %s starts on %ld\n"), _T(__FUNCTION__), pControl->identity());

	pControl->setDesktop(&desktop);
	testITile(pControl, watch);
	pControl->dispatch(key);
	pControl->dispatch(mouse);
	pControl->setDesktop(NULL);
}

static void testFill()
{
	MockWatch watch;
	Theme theme;
	Fill fill(1, theme);
	Pane pane(0, theme);
	pane.Add(&fill);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testITile(&fill, watch);
	pane.clear();
}

static void testText()
{
	MockWatch watch;
	Theme theme;
	Theme::Font textFont = {Theme::eText, theme.Text};
	Text text(1, theme, textFont);
	Pane pane(0, theme);
	pane.Add(&text);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testITile(&text, watch);
	pane.clear();
}

static void testArrow()
{
	MockWatch watch;
	Theme theme;
	Arrow arrow(1, theme, eRight);
	Pane pane(0, theme);
	pane.Add(&arrow);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testITile(&arrow, watch);
	pane.clear();
}

static void testButton()
{
	MockWatch watch;
	Theme theme;
	Theme::Font font = {Theme::eDefault, Font(_T("Arial"), 18, 0)};
	Button button(1, theme, font, _T("Test"), _T("Test"), _T("Test"));
	Pane pane(0, theme);
	pane.Add(&button);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testIControl(&button, watch);
	pane.clear();
}

static void testCheck()
{
	MockWatch watch;
	Theme theme;
	bool toggle = false;
	Check check(1, theme, toggle);
	Pane pane(0, theme);
	pane.Add(&check);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testIControl(&check, watch);
	pane.clear();
}

static void testEdit()
{
	MockWatch watch;
	Theme theme;
	string_t string = _T("Test");
	Theme::Font textFont = {Theme::eText, theme.Text};
	Edit edit(1, theme, textFont, string);
	Pane pane(0, theme);
	pane.Add(&edit);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));

	Tiles::KeyEvent action;
	action._mask = 0;
	action._what = KeyEvent::DOWN;
	action._code = VK_RIGHT;

	_tfprintf(stdout, _T("Edit text [%s]\n"), string.c_str());

	//edit.setLocal(false);
	edit.setFocus(true);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	action._code = VK_BACK;
	edit.dispatch(action);
	_tfprintf(stdout, _T("Edit index %d\n"), edit.getIndex());
	edit.setFocus(false);
	_tfprintf(stdout, _T("Edit text [%s]\n"), string.c_str());

	testIControl(&edit, watch);

	pane.clear();
}

static void testScroll()
{
	MockWatch watch;
	Theme theme;
	long value = 0;
	Scroll scroll(1, theme, eRight, value);
	Pane pane(0, theme);
	pane.Add(&scroll);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testIControl(&scroll, watch);
	pane.clear();
}

static void testTree()
{
	MockWatch watch;
	Theme theme;
	Theme::Font font = {Theme::eDefault, theme.Text};
	Pane pane(0, theme);
	Tree tree(0, theme, font);
	pane.Add(&tree);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testIControl(&tree, watch);
	pane.clear();
}

static void testGrid()
{
	MockWatch watch;
	Theme theme;
	Pane pane(0, theme);
	Grid grid(0, theme);
	pane.Add(&grid);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testIControl(&grid, watch);
	pane.clear();
}

static void testGlyph()
{
	MockWatch watch;
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Pane pane(0, theme);
	Glyph glyph(0, theme, font);
	pane.Add(&glyph);
	_tfprintf(stdout, _T("%s\n"), _T(__FUNCTION__));
	testITile(&glyph, watch);
	pane.clear();
}

static void testRectangle()
{
	std::vector<point_t> line;
	std::vector< std::vector<point_t> > lines;
	rect_t rect = {10, 10, 100, 100};
	point_t pt = {0};

	pt.x = 0;
	pt.y = 0;
	line.push_back(pt);
	pt.x = 0;
	pt.y = 120;
	line.push_back(pt);

	rect.clipPolyline(line, lines);
	_tfprintf(stdout, _T("line segment left of box: %s\n"), lines.size() == 0 ? _T("passes") : _T("fails"));
	lines.clear();
	line.clear();

	pt.x = 120;
	pt.y = 0;
	line.push_back(pt);
	pt.x = 120;
	pt.y = 120;
	line.push_back(pt);

	rect.clipPolyline(line, lines);
	_tfprintf(stdout, _T("line segment right of box: %s\n"), lines.size() == 0 ? _T("passes") : _T("fails"));
	lines.clear();
	line.clear();


	pt.x = 0;
	pt.y = 55;
	line.push_back(pt);
	pt.x = 120;
	pt.y = 55;
	line.push_back(pt);

	rect.clipPolyline(line, lines);
	_tfprintf(stdout, _T("penetrating horizontal line: %s\n"), lines.size() == 1 ? _T("passes") : _T("fails"));
	std::vector<point_t> &peek = lines[0];
	_tfprintf(stdout, _T("segment has %d points\n"), peek.size());
	for (size_t i = 0; i < peek.size(); i++)
		_tfprintf(stdout, _T("(%d,%d)"), peek[i].x, peek[i].y);
	_tfprintf(stdout, _T("\n"));

	lines.clear();
	line.clear();
}

static void testFlow()
{
	Theme theme;
	Pane pane(0, theme, eDown);
	Theme::Font textFont = {Theme::eText, theme.Text};

	Text a(0, theme, textFont, eLeft, _T("A"));
	Text b(0, theme, textFont, eLeft, _T("B"));
	Text c(0, theme, textFont, eLeft, _T("C"));

	pane.Add(&a);
	pane.Add(&b);
	pane.Add(&c);

	rect_t rect = {0, 0, 320, 40};
	pane.setRect(rect);

	rect_t box = {0}, view = {0};

	for (meter_t i = 0; i < 30; i++)
	{
		pane.getRect(view);
		pane.getScrollBox(box);
		_tfprintf(stderr, _T("Pane: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n"),
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		a.getRect(view);
		a.getScrollBox(box);
		_tfprintf(stderr, _T("A: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n"),
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		b.getRect(view);
		b.getScrollBox(box);
		_tfprintf(stderr, _T("B: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n"),
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		c.getRect(view);
		c.getScrollBox(box);
		_tfprintf(stderr, _T("C: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n"),
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		point_t pt = {0, i};
		pane.setScrollPoint(pt);
	}
	pane.clear();
}

static bool testJSONControl(IControl *pControl, const TCHAR *reference)
{
	//JSON::Writer reader;
	JSON::Writer writer;

	const TCHAR test[] = _T("test.json");
	bool bOK = false;

	if ( writer.Open(test) )
	{
		TCHAR shell[32] = {0};

		Factory::save(writer, pControl);

		writer.Close();

		// read access?
		if ( _taccess(reference, 04) != 0 ) 
		{
			BOOL okay = CopyFile(test, reference, FALSE);

			_tfprintf(stdout, _T("Creating reference copy %s.\n"), okay ? _T("succeeds") : _T("fails"));
		}

		wsprintf(shell, _T("FC %s %s"), test, reference);
		bOK = _tsystem(shell) == 0;
		_tunlink(test);
	}
	return bOK;
}

static bool testJSONButton()
{
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Button button(0, theme, font, _T("Test"));
	return testJSONControl(&button, _T("button.json"));
}

static bool testJSONCheck()
{
	bool ignore = false;
	Theme theme;
	Check control(0, theme, ignore);
	return testJSONControl(&control, _T("check.json"));
}

static bool testJSONCombo()
{
	string_t value;
	Reference<string_t> accessor(value);
	std::vector<Combo::Item> list;
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Combo control(0, theme, font, list, &accessor);
	return testJSONControl(&control, _T("combo.json"));
}

static bool testJSONEdit()
{
	string_t value;
	Reference<string_t> accessor(value);
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Edit control(0, theme, font,&accessor);
	return testJSONControl(&control, _T("edit.json"));
}

static bool testJSONGrid()
{
	Theme theme;
	Grid control(0, theme);
	return testJSONControl(&control, _T("grid.json"));
}

static bool testJSONList()
{
	Theme theme;
	List control(0, theme);
	return testJSONControl(&control, _T("list.json"));
}

static bool testJSONScroll()
{
	long value = 0;
	Theme theme;
	Scroll control(0, theme, eRight, value);
	return testJSONControl(&control, _T("scroll.json"));
}

static bool testJSONToggle()
{
	long value = 0;
	std::vector<Toggle::Item> list;
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Toggle control(0, theme, font, list, value);
	return testJSONControl(&control, _T("toggle.json"));
}

static bool testJSONTree()
{
	Theme theme;
	Theme::Font font = {Theme::eDefault, theme.Text};
	Tree control(0, theme, font, _T("Test"));
	return testJSONControl(&control, _T("tree.json"));
}

/*
Navigation tests:
1. keyboard input: given active control "X", expect next focus to be "Y"; inject keypress.
a. tab across controls
b. arrow across controls
2. mouse input: given active control "X", inject mouse click on control "Y".
Layout tests:
1. reflow
a. border
b. spacing
2. scrolling
a. no-scroll (scroll area <= screen area).
b. simple scroll (horz and vert)
c. nested scroll - vert flow inside vert flow scrolling vertically
d. nested scroll - horz flow inside horz flow scrolling horizontally
e. nested scroll - vert flow inside horz flow inside vert flow scrolling vertically
f. nested scroll - horz flow inside vert flow inside horz flow scrolling horizontally
3. auto-scroll - setFocus triggers scrolling.
Raster tests:
1. generate raster image.
2. compare to known good image.
a. create layout, change font size, render image & validate.
*/

int _tmain(int argc, _TCHAR* argv[])
{
	argc;
	argv;

#if 0
	testRectangle();
	testArrow();
	testFill();
	testText();

	testButton();
	testCheck();
	testEdit();
	testScroll();
	testFlow();
	testTree();
	testGrid();
	testGlyph();
#endif
	testJSONButton();
	testJSONCheck();
	testJSONCombo();
	testJSONEdit();
	testJSONGrid();
	testJSONList();
	testJSONScroll();
	testJSONToggle();
	testJSONTree(); // tree not ready.
	getchar();
	return 0;
}
