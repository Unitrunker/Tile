#include "../Tile/ICanvas.h"
#include "../Tile/ITile.h"

/*
Copyright 2012 Rick Parrish
*/

using namespace Tiles;

class Printer : public ICanvas, public CDC
{
	rect_t _clip;
	CBitmap _bmp;
	CDC _dc;
	DOCINFO _di;
	int _iPage;
	int _iJob;

	void privateDrawString(const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text);
public:
	Printer(HDC hDC);

	// accessors for bounding canvas rectangle.
	virtual bool setPlace(const rect_t &);
	virtual bool getPlace(rect_t &) const;
    /// <param name="rect">bounding rectangle</param>
    /// <param name="fore">foreground text color</param>
    /// <param name="back">background color</param>
    /// <param name="font">font including face, point size, and style.</param>
    /// <param name="orient">orientation</param>
    /// <param name="text">text string to display in the designated font.</param>
	virtual void DrawString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text);
	/// <param name="rect">bounding rectangle</param>
	/// <param name="fore">foreground text color</param>
	/// <param name="back">background color</param>
	/// <param name="font">font including face, point size, and style.</param>
	/// <param name="text">text string to display in the designated font.</param>
	/// <param name="cursor">cursor reticle.</param>
	virtual void DrawEditString(const rect_t &rect, const rect_t &box, color_t fore, color_t back, const Font& font, align_t align, const string_t &text, size_t cursor);
    /// <param name="rect">bounding rectangle</param>
    /// <param name="fill">polygon fill color</param>
	virtual void FillRectangle(const rect_t &rect, color_t fill);
    /// <param name="list">list of points</param>
    /// <param name="count">number of points</param>
    /// <param name="width">pen width for drawing line</param>
    /// <param name="color">pen color</param>
	virtual void DrawPolyline(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color);
    /// <param name="list">list of points</param>
    /// <param name="count">number of points</param>
    /// <param name="width">pen width for drawing line</param>
    /// <param name="color">pen color</param>
    /// <param name="fill">polygon fill color</param>
	virtual void DrawPolygon(const rect_t &rect, const rect_t &box, point_t list[], size_t count, meter_t width, color_t color, color_t fill);

	virtual bool overlaps(rect_t &control);

	bool StartDoc();
	bool DrawPage(ITile *pPage);
	bool EndDoc();
	// cancel print job.
    bool Cancel();
};
