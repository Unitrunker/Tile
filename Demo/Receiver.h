#include "../Tile/Theme.h"
#include "../Tile/ThemeSet.h"
#include "../Tile/Tab.h"
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

	Receiver(unsigned long id, bool control = false) : _id(id), _control(control), _active(false) { }

	static unsigned long byID(Receiver *p) { return p->_id; }
	static bool less(const unsigned long &left, const unsigned long &right) { return left < right; }
}; 

// control receiver
struct Control : public Receiver
{
	// control port config is per-receiver.
	Serial _serial;

	Control(unsigned long id) : Receiver(id, true) { }
};

struct SignalVCO : public VCO
{
	// discriminator audio config is per-VCO
	Sound _sound;

	SignalVCO(unsigned long id, bool control) : VCO(id, control, true) { }
};

struct InlineVCO : public VCO
{
	// Inline decoding happens over the serial port.
	// May be inline audio sampling in case of HP-1E.
	// This is empty because there is no additional hardware to configure.
	InlineVCO(unsigned long id) : VCO(id, true, true) { }
};

typedef AVL<unsigned long, Receiver *, Receiver *, Receiver::less, Receiver::byID> Receivers;

struct ReceiverSet : public SetT<Receiver>
{
	ReceiverSet(Theme&);
private:
	MemberAccessPtr<Receiver, string_t> Label;
	MemberAccessPtr<Receiver, bool> Active;
};

struct ReceiverFrame : public Window
{
	ReceiverFrame(Factory &factory, Receiver *receiver);
	virtual ~ReceiverFrame();
	bool Create(RECT &rect);
private:
	void clickety(Grid *, size_t row, size_t col);
	void serial(Button*, bool);
	void sound(Button*, bool);
	void clickHome(Button*, bool);
	void clickAbout(Button*, bool);
	void clickRefresh(Button*, bool);

	Serial _serial;
	Sound _sound;
	Pane *_top;
	Tab *_tabset;
	Tab *_tools;
	List *_list;
	Factory& _factory;
	Receiver *_receiver;
	SerialSet _serialSet;
	SoundSet _soundSet;
};
