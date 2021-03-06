#include "Types.h"

/*
Copyright � 2011 Rick Parrish
*/

#pragma once

namespace Tiles
{

template <typename T>
struct IAccessor
{
	virtual const T& getValue() const = 0;
	virtual bool setValue(const T &value) = 0;
};

template <typename T>
struct Reference : public IAccessor<T>
{
	Reference(T &value) : _value(value) { };

	virtual const T& getValue() const
	{
		return _value;
	}

	virtual bool setValue(const T &value)
	{
		_value = value;
		return true;
	}

private:
	Reference & operator = (const Reference &never); // never used.
	T &_value;
};

template <typename T>
struct Value : public IAccessor<T>
{
	Value(T value) : _value(value) { };
	Value() { };

	virtual const T& getValue() const
	{
		return _value;
	}
	virtual bool setValue(const T &value)
	{
		_value = value;
		return true;
	}

private:
	T _value;
};

template <typename B, typename T>
struct MemberPtr : public IAccessor<T>
{
	MemberPtr(B*object, T B::*member) : _object(object), _member(member) { };

	virtual const T &getValue() const
	{
		return _object->*_member;
	}

	virtual bool setValue(const T &value)
	{
		_object->*_member = value;
		return true;
	}

private:
	B* _object;
	T B::*_member;
};

template <typename B, typename T>
struct MemberRef : public IAccessor<T>
{
	MemberRef(B& object, T B::*member) : _object(object), _member(member) { };

	virtual const T &getValue() const
	{
		return _object.*_member;
	}

	virtual bool setValue(const T &value)
	{
		_object.*_member = value;
		return true;
	}

private:
	B& _object;
	T B::*_member;
};

// Works just like MemberPtr except an accessor is used to retrieve the 
// object pointer prior to getting or setting a member of that object.
template <typename B, typename T>
struct MemberAccessPtr : public IAccessor<T>
{
	MemberAccessPtr(IAccessor<B*>& access, T B::*member) : 
		_access(access), _member(member)
	{
	}

	virtual const T &getValue() const
	{
		return _access.getValue()->*_member;
	}

	virtual bool setValue(const T &value)
	{
		_access.getValue()->*_member = value;
		return true;
	}

protected:
	IAccessor<B *>& _access;
	T B::*_member;
};

// Works much like MemberAccessPtr except:
// getValue operates on the first pointer in the vector and
// setValue operates on ALL pointers in the vector.

// Use this to create property sets where multiple objects are 
// to be edited en-mass, eg. a multiselect or "bulk edit".
template <typename B, typename T>
struct MemberVectorAccess : public IAccessor<T>
{
	MemberVectorAccess(std::vector<B*>& access, T B::*member) : 
		_access(access), _member(member)
	{
	}

	virtual const T &getValue() const
	{
		_ASSERT( _access.size() > 0 );
		return _access[0]->*_member;
	}

	virtual bool setValue(const T &value)
	{
		for (size_t i = 0; i < _access.size(); i++)
			_access[i]->*_member = value;
		return true;
	}

protected:
	std::vector<B *>& _access;
	T B::*_member;
};

// Injecting change notification when setValue called.
template <typename B, typename T>
struct Reflect : public IAccessor<T>
{
	Reflect(std::vector<B*>& access, T B::*member) : 
		_access(access), _member(member)
	{
	}

	virtual const T &getValue() const
	{
		_ASSERT(_access.size() > 0);
		return _access[0]->*_member;
	}

	virtual bool setValue(const T &value)
	{
		for (size_t i = 0; i < _access.size(); i++)
		{
			_access[i]->*_member = value;
			_access[i]->broadcast("Reflect");
		}
		return true;
	}

protected:
	std::vector<B *>& _access;
	T B::*_member;
};

// Injecting change notification when setValue called.
template <typename B, typename M, typename T>
struct ReflectSubtype : public IAccessor<T>
{
	ReflectSubtype(std::vector<B*>& access, M B::*middle, T M::*member) : 
		_access(access), _middle(middle), _member(member)
	{
	}

	virtual const T &getValue() const
	{
		_ASSERT(_access.size() > 0);
		return _access[0]->*_middle.*_member;
	}

	virtual bool setValue(const T &value)
	{
		for (size_t i = 0; i < _access.size(); i++)
		{
			_access[i]->*_middle.*_member = value;
			_access[i]->broadcast("Reflect");
		}
		return true;
	}

protected:
	std::vector<B *>& _access;
	T M::*_member;
	M B::*_middle;
};

// accessor adapter for string to color_t.
struct ColorReference : public IAccessor<string_t>
{
	ColorReference(IAccessor<color_t> &access);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<color_t>& _access;
	mutable string_t _text;
	ColorReference & operator = (const ColorReference &never);
};

// accessor adapter for string to long.
struct Integer : public IAccessor<string_t>
{
	Integer(IAccessor<long>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<long>& _wrap;
	mutable string_t _text;
	int _base;
	Integer & operator = (const Integer &never);
};

// accessor adapter for string to long.
struct UInteger : public IAccessor<string_t>
{
	UInteger(IAccessor<unsigned long>& wrap, int base = 10);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<unsigned long>& _wrap;
	mutable string_t _text;
	int _base;
	UInteger & operator = (const UInteger &never);
};

// Display date/time in compact format. Short format is display only.
struct Time : public IAccessor<string_t>
{
	Time(IAccessor<time_t>& wrap, char iFormat = eShort);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

	// If you want to edit BOTH date and time, create two separate 
	// accessors - one for the date part and another for the time part.

	enum {eShort, eDate, eTime};
	void setFormat(char iFormat);

private:
	IAccessor<time_t>& _wrap;
	mutable string_t _text;
	char _iFormat;
	Time & operator = (const Time &never);
};

// Display IPv4 dotted address.
// Address is presumed to be in network byte order.
struct IPv4 : public IAccessor<string_t>
{
	IPv4(IAccessor<long>& wrap);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);
private:
	IAccessor<long>& _wrap;
	mutable string_t _text;
	IPv4 & operator = (const IPv4 &never);
};

// static_cast adapter: converting integrals of different sizes.
template <typename O, typename I>
struct Adapter : public IAccessor<O>
{
	Adapter(IAccessor<I> *inside) : _inside(inside) { }

	virtual const O& getValue() const
	{
		const O copy = static_cast<const O>( _inside->getValue() );
		// returns a reference so we need to make a copy.
		_copy = static_cast<O>(copy); 
		return _copy;
	}
	virtual bool setValue(const O &value)
	{
		// local copy
		const I copy = static_cast<const I>(value);
		return _inside->setValue(copy);
	}
private:
	IAccessor<I> *_inside;
	mutable O _copy;
};

};