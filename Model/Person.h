#include "../Tile/Types.h"
#include "../Tile/Property.h"
#include "../Tile/Follow.h"
#include "../Tile/Table.h"

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
	static bool less(const time_t &left, const time_t &right) { return left < right; }
	static string_t bySurname(Person * const p) { return p->_last; }
	static string_t byFirst(Person * const p) { return p->_first; }
	static string_t byCity(Person * const p) { return p->_city; }
	static string_t byState(Person * const p) { return p->_state; }
	static time_t byDate(Person * const p) { return p->_date; }
};

typedef AVL< string_t, Person *, Person *, Person::less, Person::byFirst > Firsts;
typedef AVL<string_t, Person *, Person *, Person::less, Person::bySurname> Surnames;
typedef AVL<string_t, Person *, Person *, Person::less, Person::byCity> Cities;
typedef AVL<string_t, Person *, Person *, Person::less, Person::byState> States;
typedef AVL<time_t, Person *, Person *, Person::less, Person::byDate> Dates;

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

typedef Table<PersonSet, Person> Persons;
