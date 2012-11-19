#include "../Tile/Property.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct Serial
{
	unsigned char _port;
	unsigned long _rate;
	unsigned char _data;
	unsigned char _stop;
	unsigned char _parity;
};

struct SerialSet : public Tiles::SetT<Serial>
{
	SerialSet(Tiles::Theme&);
private:
	Tiles::MemberAccessPtr<Serial, unsigned char> _port;
	Tiles::MemberAccessPtr<Serial, unsigned long> _rate;
	Tiles::MemberAccessPtr<Serial, unsigned char> _data;
	Tiles::MemberAccessPtr<Serial, unsigned char> _stop;
	Tiles::MemberAccessPtr<Serial, unsigned char> _parity;

	Tiles::Adapter<long, unsigned long> Rate;
	Tiles::Adapter<long, unsigned char> Data;
	Tiles::Adapter<long, unsigned char> Stop;
	Tiles::Adapter<long, unsigned char> Parity;
};
