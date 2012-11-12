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

// Trunking system types (16 bit codes are radix-50 encoded mneumonic).
// These are not voice types.
enum
{
	DMR = 0x1B1A,		// DMR
	EDACS48 = 0x24B6,	// E48
	EDACS96 = 0x257C,	// E96
	LTR = 0x4E32,		// LTR
	Motorola = 0x53AC,	// MOT
	MPT1327 = 0x53D4,	// MPT
	NXDN = 0x5B44,		// NXD
	OpenSky2 = 0x60D8,	// OS2
	OpenSky4 = 0x60DA,	// OS4
	Passport = 0x6694,	// PPT
	P25 = 0x6923		// P25
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