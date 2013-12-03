#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
#include "../Tile/Button.h"
#include "../WTL/Window.h"
#include "Model.h"
#include "System.h"
#include "Serial.h"
#include "Sound.h"
#include "Factory.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct Receiver;

struct VCO
{
	Receiver *_receiver;

	unsigned long _id;
	// voice vs. signal decoding role.
	unsigned char _role;
	// supports control tuning.
	bool _control;
	// supports signal decoding.
	bool _signal;

	virtual Set* getUI(Theme &) { return NULL; }

	VCO(unsigned long id, bool control = false, bool signal = false) : _id(id), _role(0), _control(control), _signal(signal) { }
};

// receiver bare essentials
struct Receiver : Follow<Receiver*>
{
	unsigned long _id;
	string_t _label;
	bool _active;
	bool _control;
	// most receivers only have one of these but a few have two.
	std::vector<VCO*> _VCOs;

	virtual Set* getUI(Theme &) { return NULL; };

	Receiver(unsigned long id, bool control = false) : _id(id), _control(control), _active(false) { }
	~Receiver();

	void broadcast(const char *);

	static unsigned long byID(Receiver *p) { return p->_id; }
	static bool less(const unsigned long &left, const unsigned long &right) { return left < right; }
}; 

// Control receiver
struct Control : public Receiver
{
	// control port config is per-receiver.
	Serial _serial;
	virtual Set* getUI(Theme &theme);

	Control(unsigned long id) : Receiver(id, true), _set(NULL) { }
private:
	SerialSet* _set;
};

struct SignalVCO : public VCO
{
	// discriminator audio config is per-VCO
	Sound _sound;

	virtual Set* getUI(Theme &theme);
	SignalVCO(unsigned long id, bool control) : VCO(id, control, true), _set(NULL) { }
private:
	SoundSet* _set;
};

struct InlineVCO : public VCO
{
	// Inline decoding happens over the serial port.
	// May be inline audio sampling in case of HP-1E.
	// This is empty because there is no additional hardware to configure.
	InlineVCO(unsigned long id) : VCO(id, true, true) { }
	using VCO::getUI;
};

struct ReceiverSet : public SetFollowT<Receiver>
{
	ReceiverSet(Theme&);
	bool getCaption(string_t &label) const;
	Reflect<Receiver, bool> Active;
	void setAdd(bool);
	bool getAdd() const;
private:
	Reflect<Receiver, string_t> Label;
	bool _add;
};

typedef AVL<unsigned long, Receiver *, Receiver *, Receiver::less, Receiver::byID> Receivers;

struct ReceiverFrame : public Window
{
	ReceiverFrame(Factory &factory);
	ReceiverFrame(Factory &factory, Receiver *receiver);
	ReceiverFrame(Factory &factory, std::vector<Receiver *> &list);
	virtual ~ReceiverFrame();
	bool Create(RECT &rect);

	ReceiverSet _set;
private:
	void clickety(Grid *, size_t row, size_t col);
	void clickControl(Tab*);
	void clickVCO(Tab*);
	void clickInfo(Tab*);
	void clickHome(Button*);
	void clickDelete(Button*);
	void clickRefresh(Button*);
	void clickStart(Button*);
	void clickStop(Button*);
	void inside();
	void remove();

	Pane *_top;
	Pane *_tabset;
	Pane *_tools;
	List *_list;
	Factory& _factory;
	std::map<IControl*, VCO*> _map;
};
