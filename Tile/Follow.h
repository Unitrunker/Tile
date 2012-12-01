#ifndef _FOLLOW_H
#define _FOLLOW_H

/*
Copyright © 2011 Rick Parrish
*/

#pragma once

#include <list>

// Change / remove following.
// implement this to follow an object.
template <typename T>
struct IFollow
{
   // changed.
   virtual void onChange(T t) = 0;
   // removed.
   virtual void onRemove(T t) = 0;
};

// Object inherits this to provide following.
// Example: struct X : public Follow<X> { /* blah */ };
// x.follow(sink);	// subscribe
// x.ignore(sink);	// unsubscribe
template <typename T>
struct Follow
{
   typedef IFollow<T> follow_t;
   typedef std::list<follow_t *> followers_t;

   //
   void follow(follow_t *who)
   {
      _followers.push_back(who);
   }
   //
   void ignore(follow_t *who)
   {
      followers_t::iterator it = _followers.begin();
      while (it != _followers.end())
      {
         if (*it == who)
	     {
            _followers.erase(it);
            break;
         }
         it++;
      }
   }

protected:
   // changed.
   void onChange(T self)
   {
      // Seems expensive. Is there a cheaper way?
      followers_t snap(_followers);
      followers_t::iterator it = snap.begin();
      while (it != snap.end())
      {
		  IFollow<T> *follow = *it++;
		  follow->onChange(self);
      }
   }

   // removed.
   void onRemove(T self)
   {
      // Seems expensive. Is there a cheaper way?
      followers_t snap(_followers);
      followers_t::iterator it = snap.begin();
      while (it != snap.end())
      {
		  IFollow<T> *follow = *it++;
		  follow->onRemove(self);
      }
   }

private:
   followers_t _followers;
};

#endif