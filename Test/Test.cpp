#include "stdafx.h"
#include <windows.h>
#include "MockCanvas.h"
#include "MockWatch.h"
#include "MockWindow.h"
#include "../Tile/Fill.h"
#include "../Tile/Arrow.h"
#include "../Tile/Text.h"
#include "../Tile/Button.h"
#include "../Tile/Check.h"
#include "../Tile/Edit.h"
#include "../Tile/Scroll.h"
#include "../Tile/Flow.h"
#include "../Tile/Tree.h"
#include "../Tile/Grid.h"
#include "../Tile/Glyph.h"

/*
Copyright 2011 Rick Parrish
*/

using namespace Tiles;

static void testITile(ITile *pTile, MockWatch &watch)
{
	MockCanvas canvas;

	printf("Test %s starts on %ld\n", __FUNCTION__, pTile->identity());

	rect_t rect = {100, 100, 100, 100};
	pTile->setRect(rect);

	// notify test 1
	printf("%ld should *not* trigger Redraw\n", pTile->identity());
	pTile->setChanged(false);
	pTile->watch(&watch);
	// notify test 2
	printf("%ld should trigger Redraw\n", pTile->identity());
	pTile->setChanged(true);
	// notify test 3
	printf("%ld should trigger another Redraw\n", pTile->identity());
	pTile->watch(&watch);
	// setChanged test
	printf("%ld change state before draw %d\n", pTile->identity(), pTile->getChanged());
	pTile->Draw(&canvas, false);
	printf("%ld change state after draw %d\n", pTile->identity(), pTile->getChanged());
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

	printf("Test %s starts on %ld\n", __FUNCTION__, pControl->identity());

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
	Flow flow(0, theme);
	flow.Add(&fill);
	printf("%s\n", __FUNCTION__);
	testITile(&fill, watch);
	flow.clear();
}

static void testText()
{
	MockWatch watch;
	Theme theme;
	Theme::Font textFont = {Theme::eText, theme.Text};
	Text text(1, theme, textFont);
	Flow flow(0, theme);
	flow.Add(&text);
	printf("%s\n", __FUNCTION__);
	testITile(&text, watch);
	flow.clear();
}

static void testArrow()
{
	MockWatch watch;
	Theme theme;
	Arrow arrow(1, theme, eRight);
	Flow flow(0, theme);
	flow.Add(&arrow);
	printf("%s\n", __FUNCTION__);
	testITile(&arrow, watch);
	flow.clear();
}

static void testButton()
{
	MockWatch watch;
	Theme theme;
	Theme::Font font = {Theme::eDefault, Font(_T("Arial"), 18, 0)};
	Button button(1, theme, font, _T("Test"), _T("Test"), _T("Test"));
	Flow flow(0, theme);
	flow.Add(&button);
	printf("%s\n", __FUNCTION__);
	testIControl(&button, watch);
	flow.clear();
}

static void testCheck()
{
	MockWatch watch;
	Theme theme;
	bool toggle = false;
	Check check(1, theme, toggle);
	Flow flow(0, theme);
	flow.Add(&check);
	printf("%s\n", __FUNCTION__);
	testIControl(&check, watch);
	flow.clear();
}

static void testEdit()
{
	MockWatch watch;
	Theme theme;
	string_t string = _T("Test");
	Theme::Font textFont = {Theme::eText, theme.Text};
	Edit edit(1, theme, textFont, string);
	Flow flow(0, theme);
	flow.Add(&edit);
	printf("%s\n", __FUNCTION__);

	Tiles::KeyEvent action;
	action._mask = 0;
	action._what = KeyEvent::DOWN;
	action._code = VK_RIGHT;

	_tprintf(_T("Edit text [%s]\n"), string.c_str());

	//edit.setLocal(false);
	edit.setFocus(true);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	edit.dispatch(action);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	action._code = VK_BACK;
	edit.dispatch(action);
	_tprintf(_T("Edit index %d\n"), edit.getIndex());
	edit.setFocus(false);
	_tprintf(_T("Edit text [%s]\n"), string.c_str());

	testIControl(&edit, watch);

	flow.clear();
}

static void testScroll()
{
	MockWatch watch;
	Theme theme;
	long value = 0;
	Scroll scroll(1, theme, eRight, value);
	Flow flow(0, theme);
	flow.Add(&scroll);
	printf("%s\n", __FUNCTION__);
	testIControl(&scroll, watch);
	flow.clear();
}

static void testTree()
{
	MockWatch watch;
	Theme theme;
	Flow flow(0, theme);
	Tree tree(0, theme);
	flow.Add(&tree);
	printf("%s\n", __FUNCTION__);
	testIControl(&tree, watch);
	flow.clear();
}

static void testGrid()
{
	MockWatch watch;
	Theme theme;
	Flow flow(0, theme);
	Grid grid(0, theme);
	flow.Add(&grid);
	printf("%s\n", __FUNCTION__);
	testIControl(&grid, watch);
	flow.clear();
}

static void testGlyph()
{
	MockWatch watch;
	Theme theme;
	Theme::Font font = {Theme::eText, theme.Text};
	Flow flow(0, theme);
	Glyph glyph(0, theme, font);
	flow.Add(&glyph);
	printf("%s\n", __FUNCTION__);
	testITile(&glyph, watch);
	flow.clear();
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
	printf("line segment left of box: %s\n", lines.size() == 0 ? "passes" : "fails");
	lines.clear();
	line.clear();

	pt.x = 120;
	pt.y = 0;
	line.push_back(pt);
	pt.x = 120;
	pt.y = 120;
	line.push_back(pt);

	rect.clipPolyline(line, lines);
	printf("line segment right of box: %s\n", lines.size() == 0 ? "passes" : "fails");
	lines.clear();
	line.clear();


	pt.x = 0;
	pt.y = 55;
	line.push_back(pt);
	pt.x = 120;
	pt.y = 55;
	line.push_back(pt);

	rect.clipPolyline(line, lines);
	printf("penetrating horizontal line: %s\n", lines.size() == 1 ? "passes" : "fails");
	std::vector<point_t> &peek = lines[0];
	printf("segment has %d points\n", peek.size());
	for (size_t i = 0; i < peek.size(); i++)
		printf("(%d,%d)", peek[i].x, peek[i].y);
	printf("\n");

	lines.clear();
	line.clear();
}

static void testFlow()
{
	Theme theme;
	Flow flow(0, theme, eDown);
	Theme::Font textFont = {Theme::eText, theme.Text};

	Text a(0, theme, textFont, eLeft, _T("A"));
	Text b(0, theme, textFont, eLeft, _T("B"));
	Text c(0, theme, textFont, eLeft, _T("C"));

	flow.Add(&a);
	flow.Add(&b);
	flow.Add(&c);

	rect_t rect = {0, 0, 320, 40};
	flow.setRect(rect);

	rect_t box = {0}, view = {0};

	for (meter_t i = 0; i < 30; i++)
	{
		flow.getRect(view);
		flow.getScrollBox(box);
		fprintf(stderr, "Flow: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n",
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		a.getRect(view);
		a.getScrollBox(box);
		fprintf(stderr, "A: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n",
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		b.getRect(view);
		b.getScrollBox(box);
		fprintf(stderr, "B: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n",
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		c.getRect(view);
		c.getScrollBox(box);
		fprintf(stderr, "C: [%d, %d, %d, %d] scroll [%d, %d, %d, %d]\n",
			view.x, view.y, view.wide, view.high, box.x, box.y, box.wide, box.high);

		point_t pt = {0, i};
		flow.setScrollPoint(pt);
	}
	flow.clear();
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

	getchar();
	return 0;
}
