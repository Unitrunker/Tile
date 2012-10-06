#include "../Tile/Types.h"
#include "../Tile/Property.h"

using namespace Tiles;

#pragma once

struct Person
{
	string_t _first;
	string_t _last;
	string_t _address;
	string_t _city;
	string_t _state;
	string_t _postal;
	time_t _date;
	bool _toggle;
	long _rotor;
	long _value;

	Person();
	Person(const Person &);
	void broadcast(const char *);
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
	Reflect<Person, bool> Toggle;
	Reflect<Person, long> Rotor;
	Reflect<Person, long> _value;
	Reflect<Person, time_t> _date;
	Time Date;
	Integer Value;
};

