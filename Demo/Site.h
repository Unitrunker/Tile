#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"
#include "Channel.h"

/*
Copyright © 2012 Rick Parrish
*/

// accessor adapter for string to site_t.
struct Site : public IAccessor<string_t>
{
	Site(IAccessor<Model::site_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<Model::site_t>& _wrap;
	mutable string_t _text;
	int _base;
	Site& operator = (const Site &never);
};

struct SiteSet : public SetT<Model::Site>
{
	SiteSet(Theme&);
private:
	MemberAccessPtr<Model::Site, Model::site_t> _site;
	MemberAccessPtr<Model::Site, time_t> _first;
	MemberAccessPtr<Model::Site, time_t> _last;
	Site Site;
	Time First;
	Time Last;
};

struct SiteFrame : public Window
{
	SiteFrame(Theme &theme, Model::Site *site);
private:
	Pane *_top;
	Tab *_tools;
	Tab *_tabset;
	Grid *_grid;
	List *_list;
	Model::Site *_site;
	SiteSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateChannels(Button *, bool up);

	void activateChannelPopup(Grid *, size_t row, size_t col);

	Table<ChannelSet, Model::Channel> _channels;
};

