#include "../Tile/Types.h"
#include "../Tile/Property.h"
#include "../Tile/Follow.h"

using namespace Tiles;

#pragma once

struct Person : public Follow<Person*>
{
	string_t _first;
	string_t _last;
	string_t _address;
	string_t _city;
	string_t _state;
	string_t _postal;
	time_t _date;
	bool _check;
	long _rotor;
	long _value;
	long _iIPv4;

	Person();
	Person(const Person &);
	void broadcast(const char *);

	static bool less(const string_t &left, const string_t &right) { return left.compare(right) < 0; }
	static string_t extract(Person * const p) { return p->_last; }
};

struct PersonSet : public SetT<Person>
{
	PersonSet(Theme&);

private:
	Reflect<Person, string_t> First;
	Reflect<Person, string_t> Last;
	Reflect<Person, string_t> Address;
	Reflect<Person, string_t> City;
	Reflect<Person, string_t> State;
	Reflect<Person, string_t> Postal;
	Reflect<Person, bool> Check;
	Reflect<Person, long> Rotor;
	Reflect<Person, long> _value;
	Reflect<Person, time_t> _date;
	Reflect<Person, long> _ipv4;
	Time Date;
	Time Time;
	Integer Value;
	IPv4 IPv4;
};

