#include <vector>

// Only valid when used in a single thread. No re-entrancy is permitted!

/*
Copyright © 2008 Rick Parrish
*/

#pragma once

// this class detects reentrancy conflicts - it does not prevent them.
struct Guard
{
	mutable long &_iEnter;

	Guard(long &iEnter) : _iEnter(iEnter)
	{
		_iEnter++;
		_ASSERT(_iEnter == 1);
	}

	~Guard()
	{
		_iEnter--;
		_ASSERT(_iEnter == 0);
	}

private:
	Guard();
	Guard(const Guard &);
	Guard &operator = (const Guard &);
};

template <class T, class Arg>
class Multicast
{
	//mutable CCriticalSection _critter;
	std::vector<T> _listeners;
	mutable long _iEnter;
public:
	Multicast() : _iEnter(0)
	{
		Guard g(_iEnter);
	};

	~Multicast()
	{
		Guard g(_iEnter);
	}

	Multicast &operator +=(T &add)
	{
		Guard g(_iEnter);
		_listeners.push_back(add);
		return *this;
	}
	Multicast &operator -=(T &remove)
	{
		Guard g(_iEnter);
		bool bFound = false;
		for (std::vector<T>::iterator it = _listeners.begin(); !bFound && it != _listeners.end(); ++it)
		{
			bFound = (*it) == remove;
			if (bFound)
			{
				_listeners.erase(it, it + 1);
				break;
			}
		}
		return *this;
	}

	void issueNotice(Arg *arg, const char *strLog) const
	{
		std::vector<T> listeners;
		{
			Guard g(_iEnter);
			listeners.assign(_listeners.begin(), _listeners.end());
		}
		for (std::vector<T>::iterator it = listeners.begin(); it != listeners.end(); ++it)
		{
			try
			{
				(*it)(arg);
			}
			catch (sophia::bad_function_call bad)
			{
				std::string strBad(bad.what());
				strBad += "\n";
				strBad += strLog;
				strBad += "\n";
#ifdef OutputDebugString
				OutputDebugStringA(strBad.c_str());
#endif
			}
		}
	}

	void clear()
	{
		Guard g(_iEnter);
		_listeners.clear();
	}
};

template <class T, typename Arg1, typename Arg2, typename Arg3>
class Multicast3
{
	std::vector<T> _listeners;
	mutable long _iEnter;
public:
	Multicast3() : _iEnter(0)
	{
		Guard g(_iEnter);
	};

	~Multicast3()
	{
		Guard g(_iEnter);
	}

	Multicast3 &operator +=(T &add)
	{
		Guard g(_iEnter);
		_listeners.push_back(add);
		return *this;
	}
	Multicast3 &operator -=(T &remove)
	{
		Guard g(_iEnter);
		for (std::vector<T>::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
		{
			if ( (*it) == remove )
			{
				_listeners.erase(it, it + 1);
				break;
			}
		}
		return *this;
	}

	void issueNotice(Arg1 arg1, Arg2 arg2, Arg3 arg3) const
	{
		std::vector<T> listeners;
		{
			Guard g(_iEnter);
			listeners.assign(_listeners.begin(), _listeners.end());
		}
		for (std::vector<T>::iterator it = listeners.begin(); it != listeners.end(); ++it)
		{
			try
			{
				(*it)(arg1, arg2, arg3);
			}
			catch (sophia::bad_function_call bad)
			{
				std::string strBad(bad.what());
				strBad += "\n";
#ifdef OutputDebugString
				OutputDebugStringA(strBad.c_str());
#endif
			}
		}
	}
	void clear()
	{
		Guard g(_iEnter);
		_listeners.clear();
	}
};
