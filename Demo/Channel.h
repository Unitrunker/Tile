#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "../Tile/Button.h"
#include "../Tile/Tab.h"
#include "Model.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

// accessor adapter for string to channel_t.
struct LCN : public IAccessor<string_t>
{
	LCN(IAccessor<Model::channel_t>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<Model::channel_t>& _wrap;
	mutable string_t _text;
	int _base;
	LCN& operator = (const LCN &never);
};

struct ChannelSet : public SetFollowT<Model::Channel>
{
	ChannelSet(Theme&);
	bool getCaption(string_t &label) const;
	void setAdd(bool);
	bool getAdd() const;
private:
	Reflect<Model::Channel, Model::channel_t> _lcn;
	Reflect<Model::Channel, Model::frequency_t> _frequency;
	Reflect<Model::Channel, time_t> _first;
	Reflect<Model::Channel, time_t> _last;
	Reflect<Model::Channel, string_t> Label;
	Reflect<Model::Channel, bool> Control;
	LCN LCN;
	UInteger Frequency;
	Time First;
	Time Last;
	IControl* _primary;
	bool _add;
};

struct ChannelFrame : public Window
{
	ChannelFrame(Factory& factory, Model::Site *site);
	ChannelFrame(Factory& factory, Model::Channel *channel);
	ChannelFrame(Factory& factory, std::vector<Model::Channel *> &list);
	virtual ~ChannelFrame();
	bool Create(RECT rect);
	ChannelSet _set;
private:
	Tiles::Pane *_top;
	Tiles::Pane *_tools;
	Tiles::Pane *_tabset;
	Tiles::Grid *_grid;
	Tiles::List *_list;
	Factory& _factory;

	void inside();
	void remove();
	void clickHome(Tiles::Button *);
	void clickRefresh(Tiles::Button *);
	void clickCommit(Tiles::Button *);
	void activateInfo(Tiles::Tab *);
	void activateHistory(Tiles::Tab *);
};
