#include "Types.h"

/*
Copyright © 2011 Rick Parrish
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
		if ( _access.size() )
			return _access[0]->*_member;
		_ASSERT(false);
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

// accessor adapter for string to color_t.
struct ColorReference : public IAccessor<string_t>
{
	ColorReference(IAccessor<color_t> &access);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<color_t>& _access;
	mutable string_t _text;
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
};

// Display date/time in compact format. Display only.
struct Time : public IAccessor<string_t>
{
	Time(IAccessor<time_t>& wrap);
	virtual const string_t &getValue() const;
	virtual bool setValue(const string_t &value);

private:
	IAccessor<time_t>& _wrap;
	mutable string_t _text;
};

// This template modifies MemberAccessPtr behavior by 
// injecting change notification when setValue called.
template <typename B, typename T>
struct Reflect : public MemberAccessPtr<B, T>
{
	Reflect(IAccessor<B*> &base, T B::*member) : MemberAccessPtr<B, T>(base, member) { };

	using MemberAccessPtr<B, T>::getValue;

	virtual bool setValue(const T &value)
	{
		bool bOK = MemberAccessPtr<B, T>::setValue(value);
		_access.getValue()->broadcast("Reflect");
		return bOK;
	}
};

};