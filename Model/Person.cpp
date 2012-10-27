#include "Person.h"
#include "../Tile/Edit.h"
#include "../Tile/Combo.h"
#include "../Tile/Check.h"
#include <time.h>

using namespace Tiles;

Person::Person() : _date(0), _check(false), _rotor(100), _value(0), _iIPv4(0x100007f)
{
}

Person::Person(const Person &copy) :
	_first(copy._first), 
	_last(copy._last), 
	_address(copy._address),
	_city(copy._city), 
	_state(copy._state), 
	_postal(copy._postal), 
	_date(copy._date),
	_check(copy._check),
	_rotor(copy._rotor),
	_value(copy._value),
	_iIPv4(copy._iIPv4)
{
}

void Person::broadcast(const char *)
{
	// tell the world!
}

PersonSet::PersonSet(Theme &theme) : 
	SetT<Person>(NULL),
#pragma warning(disable:4355)
	First(*this, &Person::_first),
	Last(*this, &Person::_last),
	Address(*this, &Person::_address),
	City(*this, &Person::_city),
	State(*this, &Person::_state),
	Postal(*this, &Person::_postal),
	Check(*this, &Person::_check),
	Rotor(*this, &Person::_rotor),
	_value(*this, &Person::_value),
	Value(_value),
	_date(*this, &Person::_date),
	Date(_date, Time::eDate),
	Time(_date, Time::eTime),
	_ipv4(*this, &Person::_iIPv4),
#pragma warning(default:4355)
	IPv4(_ipv4)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};

	section = new Section(_T("Person"), _T("Person to be named"));
	prop = new Property(_T("First"), _T("First Name"), new Edit(0, theme, textFont, &First) );
	section->Items.push_back(prop);
	prop = new Property(_T("Last"), _T("Last Name"), new Edit(0, theme, textFont, &Last) );
	section->Items.push_back(prop);
	Add(section);

	section = new Section(_T("Address"), _T("Mailing address"));
	prop = new Property(_T("Street"), _T("Street number and name"), new Edit(0, theme, textFont, &Address) );
	section->Items.push_back(prop);
	prop = new Property(_T("City"), _T("City or town"), new Edit(0, theme, textFont, &City) );
	section->Items.push_back(prop);
	prop = new Property(_T("State"), _T("State or province"), new Edit(0, theme, textFont, &State) );
	section->Items.push_back(prop);
	prop = new Property(_T("Postal"), _T("Postal or Zip code"), new Edit(0, theme, textFont, &Postal) );
	section->Items.push_back(prop);
	Add(section);
	
	section = new Section(_T("Appointment"), _T("Appointment schedule") );
	prop = new Property( _T("Date"), _T("Appointment"), new Edit(0, theme, textFont, &Date) );
	section->Items.push_back(prop);
	prop = new Property( _T("Time"), _T("Appointment"), new Edit(0, theme, textFont, &Time) );
	section->Items.push_back(prop);
	prop = new Property( _T("Check"), _T("Selected for interview"), new Tiles::Check(0, theme, &Check) );
	section->Items.push_back(prop);

	static const Combo::Item list[] = { {_T("100"), 100}, {_T("200"), 200}, {_T("300"), 300} };
	std::vector<Combo::Item> items;
	for (size_t i = 0; i < _countof(list); i++)
		items.push_back(list[i]);
	prop = new Property( _T("Rotor"), _T("Thing 1, thing 2"), new Combo(0, theme, textFont, items, &Rotor) );
	section->Items.push_back(prop);
	prop = new Property( _T("Value"), _T("A numeric value"), new Edit(0, theme, textFont, &Value) );
	section->Items.push_back(prop);
	prop = new Property( _T("IP"), _T("Dotted IP Address"), new Edit(0, theme, textFont, &IPv4) );
	section->Items.push_back(prop);
	Add(section);
}
