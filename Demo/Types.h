/*
Copyright © 2012 Rick Parrish
*/

#pragma once

namespace Model
{

// PODs
typedef unsigned long address_t;
typedef unsigned short channel_t;
typedef unsigned short site_t;
typedef unsigned long frequency_t;
typedef unsigned long hit_t;

enum {Unknown = 0};

// service types for voice.
enum
{
	Analog = 1, Data = 2, Encrypted = 3, VSELP = 4, 
	ProVoice = 5, P25CAI = 6, P25X2 = 7, P25P2 = 8
};

typedef unsigned short service_t;

extern "C" const TCHAR *getServiceType(service_t value);

// trunking system types
enum
{
	APCOP25 = 1, EDACS48 = 2, EDACS96 = 3, Motorola = 4, 
	MPT1327 = 5, OpenSky = 6, DMR = 7, NXDN = 8
};

typedef unsigned short trunk_t;

extern "C" const TCHAR *getTrunkType(trunk_t value);

typedef struct Network
{
	trunk_t _type;
	unsigned long _network;

	bool operator == (const Network &right) const;
	bool operator < (const Network &right) const;
	static bool less(const Network &left, const Network &right);
}
network_t;

};