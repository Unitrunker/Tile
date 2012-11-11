#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../WTL/Frame.h"
#include "../Tile/Grid.h"
#include "../Tile/List.h"
#include "Model.h"

/*
Copyright © 2012 Rick Parrish
*/

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

struct ChannelSet : public SetT<Model::Channel>
{
	ChannelSet(Theme&);
private:
	MemberAccessPtr<Model::Channel, Model::channel_t> _lcn;
	MemberAccessPtr<Model::Channel, Model::frequency_t> _frequency;
	MemberAccessPtr<Model::Channel, time_t> _first;
	MemberAccessPtr<Model::Channel, time_t> _last;
	LCN LCN;
	UInteger Frequency;
	Time First;
	Time Last;
};

struct ChannelFrame : public Window
{
	ChannelFrame(Theme &theme, Model::Channel *channel);
private:
	Pane *_top;
	Tab *_tools;
	Tab *_tabset;
	Grid *_grid;
	List *_list;
	Model::Channel *_channel;
	ChannelSet _set;

	void activateHome(Button *, bool up);
	void activateInfo(Button *, bool up);
	void activateHistory(Button *, bool up);
};

