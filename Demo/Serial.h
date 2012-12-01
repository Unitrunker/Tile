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

	Serial(const Serial &copy);
	Serial(unsigned long rate, unsigned char data, unsigned char stop, unsigned char parity);
	Serial();
};

struct SerialSet : public Tiles::SetT<Serial>
{
	SerialSet(Tiles::Theme&);
private:
	Tiles::MemberVectorAccess<Serial, unsigned char> _port;
	Tiles::MemberVectorAccess<Serial, unsigned long> _rate;
	Tiles::MemberVectorAccess<Serial, unsigned char> _data;
	Tiles::MemberVectorAccess<Serial, unsigned char> _stop;
	Tiles::MemberVectorAccess<Serial, unsigned char> _parity;

	Tiles::Adapter<long, unsigned char> Port;
	Tiles::Adapter<long, unsigned long> Rate;
	Tiles::Adapter<long, unsigned char> Data;
	Tiles::Adapter<long, unsigned char> Stop;
	Tiles::Adapter<long, unsigned char> Parity;
};
