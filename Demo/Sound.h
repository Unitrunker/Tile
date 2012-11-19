#include "../Tile/Property.h"

/*
Copyright © 2012 Rick Parrish
*/

#pragma once

struct Sound
{
	unsigned short _port;		// wave input device (-1 = default).
	unsigned long _rate;		// samples per second.
	unsigned char _channels;	// number of channels eg. 1 = monom 2 = stereo.
	unsigned char _channel;		// selected audio channel (0=left)
	unsigned char _bits;		// bits per sample eg. 8/16/32
};

struct SoundSet : public Tiles::SetT<Sound>
{
	SoundSet(Tiles::Theme&);
private:
	Tiles::MemberAccessPtr<Sound, unsigned short> _port;
	Tiles::MemberAccessPtr<Sound, unsigned long> _rate;
	Tiles::MemberAccessPtr<Sound, unsigned char> _channels;
	Tiles::MemberAccessPtr<Sound, unsigned char> _channel;
	Tiles::MemberAccessPtr<Sound, unsigned char> _bits;

	Tiles::Adapter<long, unsigned long> Rate;
	Tiles::Adapter<long, unsigned char> Channels;
	Tiles::Adapter<long, unsigned char> Bits;

};
