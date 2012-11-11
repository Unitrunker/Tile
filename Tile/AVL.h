#pragma once
/*
Copyright © 2011 Rick Parrish
*/

#include <stdexcept>
#include "Follow.h"

// Notification interface.
// No type-awareness needed.
struct INotify
{
   // row "i" added.
   virtual void onAdded(size_t i) = 0;
   // row "i" changed.
   virtual void onChange(size_t i) = 0;
   // row "i" removed.
   virtual void onRemove(size_t i) = 0;
   // row "i" moved to row "j".
   virtual void onMoved(size_t i, size_t j) = 0;
};

// This one introduces a small amount of type-awareness.
template
<
	typename T
>
struct IRowSet
{
	virtual void follow(INotify *) = 0;
	virtual bool ignore(INotify *) = 0;
	virtual T operator [](size_t index) = 0;
	virtual size_t size() const = 0;
};

// Base class for tree.
// This expresses the limited amount of knowledge 
// a node must have about it's tree.
// The tree translates the node generated callbacks 
// below to INotify row events.
template
<
   // tree node
   typename node_t
>
struct _Base
{
	// Container's root node; null means tree is empty.
	node_t *_root;

	_Base() : _root(NULL) { };

	// node issued notifications.
	virtual void onChange(node_t *) = 0;
	virtual void onRemove(node_t *) = 0;
	virtual void onMove(node_t *) = 0;
};

// Tree node
// Important: assumes value type V inherits Follow<V>.
template
<
   // can be scalar, pointer, or object.
   typename K, 
   // can be scalar, pointer, or object.
   typename V, 
   // value reference - same as V if V is a pointer, otherwise V&.
   typename Vref,
   // returns true if left < right.
   bool less(const K &, const K &),
   // key value extractor
   K extract(const Vref)
>
struct node : public IFollow<Vref>
{
  typedef node node_t;
  // as you can see, this is a hefty node implementation - 24 bytes plus however many key bytes.
  // a node allocator would be nice.
  K _key;
  V _value;
  node_t *_kids[2];
  node_t *_parent;
  size_t _count;
  _Base<node> *_container;

  // Node holds a value. key explicit.
  node(_Base<node> *contain, const K &key, Vref value) : 
	 _container(contain),	// point to node's container.
	 _key(key),				// hold the key.
	 _value(value),			// hold the value.
	 _parent(NULL),			// no parent yet.
	 _count(1)				// lonely count of one.
  {
     // no kids (yet).
     _kids[0] = _kids[1] = NULL;
	 _value->follow(this);
  }

  // Node holds a value. Key inferred.
  node(_Base<node> *contain, Vref value) : 
	 _container(contain),	// point to node's container.
	 _key(extract(value)),	// hold the key.
	 _value(value),			// hold the value.
	 _parent(NULL),			// no parent yet.
	 _count(1)				// lonely count of one.
  {
     // no kids (yet).
     _kids[0] = _kids[1] = NULL;
	 _value->follow(this);
  }

  virtual ~node()
  {
	 // unlink/unbind from item.
	 _value->ignore(this);
  }

  // Compute index of this node relative to the left/right-most element.
  size_t index(bool bForward) const
  {
     size_t index = _kids[!bForward] ? _kids[!bForward]->_count : 0;
     const node_t *trail = this;
     const node_t *ancestor = _parent;
	 // walk up tree towards root.
     while (ancestor)
     {
		// did we climb up from left or right child?
        if (ancestor->_kids[bForward] == trail)
           index += ancestor->children(!bForward) + 1;
		// advance.
        trail = ancestor;
        ancestor = ancestor->_parent;
     }
	 // index is 0 based position in container.
     return index;
  }

  // How many left/right children?
  size_t children(bool child) const
  {
     return _kids[child] ? _kids[child]->_count : 0;
  }

  // How deep left/right child path?
  size_t depth(bool child) const
  {
     size_t depth = 0;
     size_t count = children(child);
     while (count)
     {
        count >>= 1;
        depth++;
     }
     return depth;
  }

  // How high are we?
  // No children equals height of 1.
  // Assumes tree is optimally balanced (which it should).
  size_t height() const
  {
     size_t height = 0;
     size_t size = _count;
     while (size)
     {
        height++;
        size >>= 1;
     }
     return height;
  }

  // Reset node's key. 
  // Call this when key has changed.
  // Determines whether change impacts order in tree.
  // Uses iterators to find left/right neighbor to verify relative order.
  // Sends a moved event to container if order impacted.
  // Sends a changed event.
  void reset(const K &key)
  {
     node_t *before = advance(this, false);
     node_t *after = advance(this, true);

	 // moved down?
     bool bMoved = after && less(after->_key, key);
	 if (!bMoved)
     {
	    // no: moved up?
        bMoved = before && less(_key, before->_key);
     }
	 // update key.
	 _key = key;
	 // Needs to move?
     if (bMoved)
     {
		 _container->onMove(this);
		// don't fire value change event. 
		// Move event should be sufficient.
     }
	 else
	 {
		 // key change did not move node.
		 _container->onChange(this);
	 }
   }
   // Single rotation. Swaps parent node with it's left or right child.
   // adjusts all other parent/child links.
   // Primitive operation for balancing the tree.
   static void rotate(node_t* &parent, bool bChild)
   {
      node_t *drop = parent;
      parent = drop->_kids[bChild];
      parent->_parent = drop->_parent;
      drop->_parent = parent;
      drop->_kids[bChild] = parent->_kids[!bChild];
      if (drop->_kids[bChild]) drop->_kids[bChild]->_parent = drop;
      parent->_kids[!bChild] = drop;
      drop->_count = drop->children(false) + drop->children(true) + 1;
      parent->_count = parent->children(false) + parent->children(true) + 1;
   }

   // Balance left/right descendents of parent node.
   // Determines when to apply single vs. double rotation.
   static void balance(node_t *&parent, bool bChild)
   {
      node_t *child = parent->_kids[bChild];
      if ( child->depth(bChild) >= child->depth(!bChild) )
      {
         rotate(parent, bChild);
      }
      else
      {
         rotate(parent->_kids[bChild], !bChild);
         rotate(parent, bChild);
      }
   }

   // Insert node *somewhere* beneath parent node (recursive) - O(log n)
   static bool insert(node_t * &parent, node_t *child)
   {
      bool bKid = false;
      if (parent == NULL)
      {
         parent = child;
         return true;
      }
      bKid = less(parent->_key, child->_key);
	  if (!bKid && !less(child->_key, parent->_key))
	  {
		  // they're equal so child follows parent.
		  bKid = true;
		  // this gimmick gives chronological ordering on items with the same key.
	  }
      child->_parent = parent;
      if ( insert(parent->_kids[bKid], child) )
      {
         parent->_count++;
         bool bChild = false;
         if (parent->depth(bChild) > parent->depth(!bChild) + 1)
         {
            balance(parent, bChild);
         }
         bChild = true;
         if (parent->depth(bChild) > parent->depth(!bChild) + 1)
         {
            balance(parent, bChild);
         }
         return true;
      }
      return false;
   }

   // Internal node removal. 
   static bool removeInner(node_t *child, node_t * &root)
   {
      node_t *tidy = NULL;
      // leaf node
      if (child->_count == 1)
      {
         if (child->_parent)
         {
            tidy = child->_parent;
            bool bKid = tidy->_kids[true] == child;
            tidy->_kids[bKid] = NULL;
         }
      }
      // leaf node once removed.
      else if (child->_count == 2)
      {
         node_t *peel = child->_kids[true];
         if (!peel)
            peel = child->_kids[false];
         tidy = child->_parent;
         bool bKid = tidy->_kids[true] == child;
         if (peel)
         {
			link(tidy, peel, bKid);
            peel->_count = 1;
         }
		 else
			tidy->_kids[bKid] = peel;
      }
      // interior node
      else
      {
         node_t *parent = child->_parent;
         bool bChild = child->children(false) < child->children(true);
         node_t *peel = most(child->_kids[bChild], !bChild);
         tidy = peel->_parent;

         if (parent)
         {
            if (parent->_kids[false] == child)
			   link(parent, peel, false);
            else if (parent->_kids[true] == child)
			   link(parent, peel, true);
            else throw std::invalid_argument("should never happen");
         }
         else
		 {
            root = peel;
			peel->_parent = NULL;
		 }

		 if (tidy == child)
		 {
			 tidy = parent;
			 // peel is immediately below child
			 // nul so we don't link peel to itself
			 // when it assumes child's kids.
			 child->_kids[bChild] = NULL;
		 }
		 else
		 {
		    link(tidy, peel->_kids[bChild], !bChild);
		 }

         // replace child with peel.
         link(peel, child->_kids[false], false);
         link(peel, child->_kids[true], true);
         peel->_count = peel->children(false) + peel->children(true) + 1;
      }
      // rebalance tidy.
      while (tidy != NULL)
      {
         tidy->_count--;
         size_t left = tidy->depth(false);
         size_t right = tidy->depth(true);
         if (left > right + 1)
         {
            if (tidy == root)
               balance(root, false);
            else if (tidy->_parent->_kids[false] == tidy)
               balance(tidy->_parent->_kids[false], false);
            else if (tidy->_parent->_kids[true] == tidy)
               balance(tidy->_parent->_kids[true], false);
            else
               throw std::invalid_argument("should never occur");
         }
         else if (right > left + 1)
         {
            if (tidy == root)
               balance(root, true);
            else if (tidy->_parent->_kids[false] == tidy)
               balance(tidy->_parent->_kids[false], true);
            else if (tidy->_parent->_kids[true] == tidy)
               balance(tidy->_parent->_kids[true], true);
            else
               throw std::invalid_argument("should never occur");
         }
         tidy = tidy->_parent;
      }
      child->_parent = NULL;
      child->_kids[false] = child->_kids[true] = NULL;
      return true;
   }
   // link two nodes - parent and left/right child.
   static void link(node_t *parent, node_t *child, bool bFwd)
   {
      parent->_kids[bFwd] = child;
	  if (child)
	     child->_parent = parent;
   }
   // Seek left-most or right-most child
   static node_t *most(node_t *cursor, bool bFwd)
   {
      while (cursor->_kids[bFwd])
         cursor = cursor->_kids[bFwd];
      return cursor;
   }
   // Seek left-most or right-most child
   static const node_t *most(const node_t *cursor, bool bFwd)
   {
      while (cursor->_kids[bFwd])
         cursor = cursor->_kids[bFwd];
      return cursor;
   }
   // Retrieve indexed node, relative to left-most(true) or right-most(false) node.
   static node_t *index(size_t i, node_t *root, bool bFwd)
   {
      node_t *cursor = root;

      while (cursor != NULL && i < cursor->_count)
      {
         size_t children = cursor->children(!bFwd);
         if (i == 0 || i < children)
         {
            if (cursor->_kids[!bFwd] != NULL)
               cursor = cursor->_kids[!bFwd];
            else
               break;
         }
         else
         {
            i -= children;
            if (i == 0)
               break;
            i--;
            cursor = cursor->_kids[bFwd];
         }
      }
      return cursor;
   }

   // Retrieve indexed node, relative to left-most(true) or right-most(false) node.
   static const node_t *index(size_t i, const node_t *root, bool bFwd)
   {
      const node_t *cursor = root;

      while (cursor != NULL && i < cursor->_count)
      {
         size_t children = cursor->children(!bFwd);
         if (i == 0 || i < children)
         {
            if (cursor->_kids[!bFwd] != NULL)
               cursor = cursor->_kids[!bFwd];
            else
               break;
         }
         else
         {
            i -= children;
            if (i == 0)
               break;
            i--;
            cursor = cursor->_kids[bFwd];
         }
      }
      return cursor;
   }
   // Find first matching key or nearest node to key.
   // TODO: add boolean bForward parameter to do fordward/reverse.
   // const version follows.

   static node_t *find(node_t *root, const K &key)
   {
      node_t *cursor = root;
      while (cursor != NULL)
      {
         bool bFwd = less(cursor->_key, key);
         if (cursor->_kids[bFwd])
            cursor = cursor->_kids[bFwd];
         else break;
      }
	  return cursor;
   }

   // Find first matching key or nearest node to key.
   static const node_t *find(const node_t *root, const K &key)
   {
      const node_t *cursor = root;
      while (cursor != NULL)
      {
         bool bFwd = less(cursor->_key, key);
         if (cursor->_kids[bFwd])
            cursor = cursor->_kids[bFwd];
         else break;
      }
	  return cursor;
   }

   // advance left/right one place
   static node_t *advance(node_t *cursor, bool bFwd)
   {
      node_t *tail = cursor;
      while ( cursor != NULL && (cursor->_kids[bFwd] == NULL || cursor->_kids[bFwd] == tail) )
      {
         tail = cursor;
         cursor = cursor->_parent;
         if (cursor != NULL && cursor->_kids[!bFwd] == tail)
            return cursor;
      }
      if (cursor != NULL && cursor->_kids[bFwd] != NULL)
      {
         cursor = cursor->_kids[bFwd];
         cursor = node_t::most(cursor, !bFwd);
      }
	  return cursor;
   }

private:
	// IFollow implementation.

	// Node value changed event.
	// Call this when item's contents have changed.
	// Sends a changed event.
	virtual void onChange(Vref)
	{
		K key = extract(_value);
		// has key changed?
		if ( less(key, _key) || less(_key, key) )
		{
			// yes: reset key and move.
			reset(key);
		}
		else
		{
			// announce change.
			_container->onChange(this);
		}
	}
	// Node removed event.
	// Call this to tell the tree the node is going away.
	// Node will inform the container and self-delete.
	virtual void onRemove(Vref)
	{
		_container->onRemove(this);
		delete this;
	}
};

// iterator with increment / decrement operators
// "advancement" is relative to the forward flag.
// a forward iterator advances ++ right and -- left.
// a reverse iterator advances ++ left and -- right.
// a const version of this iterator follows.
template
<
   // can be scalar, pointer, or object.
   typename K, 
   // can be scalar, pointer, or object.
   typename V, 
   // value reference - same as V if V is a pointer, otherwise V&.
   typename Vref,
   // tree node
   typename node_t,
   // AVL tree as friend
   typename F
>
struct iteratorT
{
   iteratorT() : _cursor(NULL), _forward(false) { }

   iteratorT(const iteratorT &copy) : _cursor(copy._cursor), _forward(copy._forward) { }

   // iterate from arbitrary item in tree.
   iteratorT(node_t *start, bool fwd) : _cursor(start), _forward(fwd)
   {
   }

   // postincrement
   V * operator ++(int)
   {
      node_t *post = _cursor;
      advance(_forward);
      return &post->_value;
   }

   // preincrement
   V * operator ++()
   {
      advance(_forward);
      return _cursor ? &_cursor->_value : NULL;
   }

   // postdecrement
   V * operator --(int)
   {
      node_t *post = _cursor;
      advance(!_forward);
      return &post->_value;
   }

   // predecrement
   V * operator --()
   {
      advance(!_forward);
      return cursor ? &_cursor->_value : NULL;
   }

   Vref operator *() const
   {
      if (_cursor == NULL)
         throw std::length_error("iterator points nowhere");
      return _cursor->_value;
   }

   // true if iterator past end.
   bool end() const { return _cursor == NULL; }

   // Iterators that refer to the same node are equal.
   bool operator == (const iteratorT &right)
   {
      return _cursor == right._cursor;
   }

   // reassign key through iterator.
   // iterator follows node to its new locations.
   bool move(const K &key)
   {
      if (end())
         return false;

      _cursor->reset(key);
      return true;
   }

private:
   // advance left/right one place
   void advance(bool bFwd)
   {
      _cursor = node_t::advance(_cursor, bFwd);
   }

   // container class needs direct access for removal by iterator.
   friend F;
   node_t *_cursor;
   bool _forward;
};

// iterator with increment / decrement operators
// "advancement" is relative to the forward flag.
// a forward iterator advances ++ right and -- left.
// a reverse iterator advances ++ left and -- right.
// a const version of this iterator follows.
template
<
   // can be scalar, pointer, or object.
   typename K, 
   // can be scalar, pointer, or object.
   typename V, 
   // value reference - same as V if V is a pointer, otherwise V&.
   typename Vref,
   // tree node
   typename node_t,
   // AVL tree as friend
   typename F
>
struct const_iteratorT
{
   const_iteratorT() : _cursor(NULL), _forward(false) { }

   const_iteratorT(const const_iteratorT &copy) : _cursor(copy._cursor), _forward(copy._forward) { }

   // iterate from arbitrary item in tree.
   const_iteratorT(const node_t *start, bool fwd) : _cursor(start), _forward(fwd)
   {
   }

   // postincrement
   const V * operator ++(int)
   {
      const node_t *post = _cursor;
      advance(_forward);
      return &post->_value;
   }

   // preincrement
   const V * operator ++()
   {
      advance(_forward);
      return _cursor ? &_cursor->_value : NULL;
   }

   // postdecrement
   const V * operator --(int)
   {
      const node_t *post = _cursor;
      advance(!_forward);
      return &post->_value;
   }

   // predecrement
   const V * operator --()
   {
      advance(!_forward);
      return cursor ? &_cursor->_value : NULL;
   }

   const Vref operator *() const
   {
      if (_cursor == NULL)
         throw std::length_error("iterator points nowhere");
      return _cursor->_value;
   }

   // true if iterator past end.
   bool end() const { return _cursor == NULL; }

   // Iterators that refer to the same node are equal.
   bool operator == (const const_iteratorT &right)
   {
      return _cursor == right._cursor;
   }

private:
   // advance left/right one place
   void advance(bool bFwd)
   {
       const node_t *tail = _cursor;
      while ( _cursor != NULL && (_cursor->_kids[bFwd] == NULL || _cursor->_kids[bFwd] == tail) )
      {
         tail = _cursor;
         _cursor = _cursor->_parent;
         if (_cursor != NULL && _cursor->_kids[!bFwd] == tail)
            return;
      }
      if (_cursor != NULL && _cursor->_kids[bFwd] != NULL)
      {
         _cursor = _cursor->_kids[bFwd];
         _cursor = node_t::most(_cursor, !bFwd);
      }
   }

   // container class needs direct access for removal by iterator.
   friend F;
   const node_t *_cursor;
   bool _forward;
};

// The AVL tree.
// The node implementation does not need full a-priori knowledge 
// of this class because it inherits from the _Base mini-class.
template
<
   // can be scalar, pointer, or object.
   typename K, 
   // can be scalar, pointer, or object.
   typename V, 
   // value reference - same as V if V is a pointer, otherwise V&.
   typename Vref,
   // returns true if left < right.
   bool less(const K &, const K &),
   // key extractor
   K extract(const Vref),
   // tree node
   typename node_t = node<K, V, Vref, less, extract>
>
struct AVL : public _Base<node_t>, public IRowSet<Vref>
{
   typedef AVL<K, V, Vref, less, extract, node_t> tree_t;
   typedef iteratorT<K, V, Vref, node_t, AVL> iterator;
   typedef const_iteratorT<K, V, Vref, node_t, AVL> const_iterator;

   // Construct an empty tree.
   // Note: deleting nodes from a tree does nothing to the item (if the value type is a pointer).
   AVL()
   {
   }

   // Discard tree contents.
   ~AVL()
   {
      clear();
   }

   // Apply action to each item; stops early if action returns false.
   // Designed for application use.
   bool foreach( bool action(Vref) )
   {
      bool bOK = true;
      iterator it = begin();
      while (bOK && !it.end())
      {
         bOK = action(*it);
		 it++;
      }
      return bOK;
   }

   // Apply action to each item; stops early if action returns false.
   // Designed for application use.
   bool foreach( bool action(const Vref) ) const
   {
      bool bOK = true;
      const_iterator it = begin();
      while (bOK && !it.end())
      {
         bOK = action(*it);
		 it++;
      }
      return bOK;
   }

   // Variation of foreach with extra typesafe context parameter.
   // Designed for application use.
   template<typename C>
   bool foreach( C context, bool action(C context, Vref) )
   {
      bool bOK = true;
      tree_t::iterator it = begin();
      while (bOK && !it.end())
      {
		 bOK = action(context, *it);
		 it++;
      }
      return bOK;
   }

   // Variation of foreach with extra typesafe context parameter.
   // Designed for application use.
   template<typename C>
   bool foreach( C context, bool action(C context, const Vref) ) const
   {
      bool bOK = true;
      const_iterator it = begin();
      while (bOK && !it.end())
      {
		 bOK = action(context, *it);
		 it++;
      }
      return bOK;
   }

   // Apply action to each tree node; 
   // Stops early if action returns false.
   // Gives access to the actual AVL tree node for unit testing.
   // Not meant for application use.
   bool foreach( bool action(node_t *) )
   {
      bool bOK = true;
      tree_t::iterator it = begin();
      while (bOK && !it.end())
      {
         bOK = action(it.cursor);
		 it++;
      }
      return bOK;
   }

   // Variation of foreach with extra typesafe context parameter.
   // Gives access to the actual AVL tree node for unit testing.
   // Not meant for application use.
   template<typename C>
   bool foreach( C context, bool action(C context, node_t *) )
   {
      bool bOK = true;
      tree_t::iterator it = begin();
      while (bOK && !it.end())
      {
		 bOK = action(context, it.cursor);
		 it++;
      }
      return bOK;
   }

   // should this be private?
   bool insert(node_t *child)
   {
	  return node_t::insert(_root, child);
   }

   // insert value at key.
   bool insert(const K &key, Vref value)
   {
      node_t *child = new node_t(this, key, value);
	  bool bOK = insert(child);
      if (bOK)
	  {
		  onAdded(child);
	  }
      return bOK;
   }

   // insert value at key.
   bool insert(Vref value)
   {
      node_t *child = new node_t(this, value);
	  bool bOK = insert(child);
      if (bOK)
	  {
		  onAdded(child);
	  }
      return bOK;
   }

   // remove by value: be careful if there are multiple nodes with the same value.
   bool remove(const K &key)
   {
      iterator it = find(key);
	  return remove(it);
   }

   // remove node under the iterator
   bool remove(const iterator &it)
   {
	  node_t *cursor = it.cursor;
	  return remove(cursor);
   }

   // remove node under the iterator
   bool remove(node_t *cursor)
   {
      if (cursor == NULL || cursor->_container != this)
         return false;

	  return node_t::removeInner(cursor, _root);
   }

   // Find first matching key or nearest node to key.
   // TODO: add boolean bForward parameter to do forward/reverse.
   // const version follows.

   iterator find(const K &key)
   {
      node_t *cursor = node_t::find(_root, key);
      iterator here = iterator(cursor, true);
      if ( cursor != NULL && less(here._cursor->_key, key) )
         here++;
      return here;
   }

   // Find first matching key or nearest node to key.
   const_iterator find(const K &key) const
   {
      const node_t *cursor = node_t::find(_root, key);
      const_iterator here = const_iterator(cursor, true);
      if ( less(here._cursor->_key, key) )
         here++;
      return here;
   }

   // Clear tree contents
   // postconditions: size() == 0, height() == 0.
   void clear() 
   {
	  // delete all nodes.
      dfs(delete_node);
	  // clear root node pointer.
      _root = NULL;
   }

   // IRowSet
   // How many items in this tree?
   virtual size_t size() const
   {
      return _root ? _root->_count : 0;
   }

   // How tall is this tree?
   // An empty tree has height zero.
   size_t height() const
   {
      return _root ? _root->height() : 0;
   }

   // Depth First Search - with context.
   template <typename C>
   void dfs(C context, void action(C, node_t *) )
   {
      dfs<C>(context, _root, action);
   }

   // Depth First Search - no context.
   void dfs(bool action(node_t *) )
   {
      dfs(_root, action);
   }

   // Delete node - does not fire events - TODO: right?
   static bool delete_node(node_t *node)
   {
      delete node;
      return true;
   }

   // Get forward iterator points to indexed node.
   iterator index(size_t i)
   {
      node_t *here = index(i, true);
      return iterator(here, true);
   }

   // Get reverse iterator points to indexed node.
   iterator rindex(size_t i)
   {
      node_t *here = index(i, false);
      return iterator(here, false);
   }

   // Returns an iterator pointing to the left (forward=true) most or right (forward=false) most item.
   iterator begin(bool bForward = true)
   {
      node_t *cursor = _root;
      if (cursor != NULL)
         cursor = node_t::most(cursor, !bForward);
      return iterator(cursor, bForward);
   }

   // Get forward iterator points to indexed node.
   const_iterator index(size_t i) const
   {
      const node_t *here = index(i, true);
      return const_iterator(here, true);
   }

   // Get reverse iterator points to indexed node.
   const_iterator rindex(size_t i) const
   {
      const node_t *here = index(i, false);
      return const_iterator(here, false);
   }
 
   // Returns an iterator pointing to the left (forward=true) most or right (forward=false) most item.
   const_iterator begin(bool bForward = true) const
   {
      node_t *cursor = _root;
      if (cursor != NULL)
         cursor = node_t::most(cursor, !bForward);
      return const_iterator(cursor, bForward);
   }

   // IRowSet
   virtual Vref operator [](size_t i)
   {
      node_t *p = index(i, true);
      if (p != NULL)
      {
         return p->_value;
      }
      throw std::out_of_range("index too large or negative");
   }

   const Vref operator [](size_t i) const
   {
      const node_t *p = index(i, true);
      if (p != NULL)
      {
         return p->_value;
      }
      throw std::out_of_range("index too large or negative");
   }
   // callback cluster for adds/removals/updates of items.
   // By design, only one watcher is supported.
   virtual void follow(INotify *tell)
   {
      _watch.push_back(tell);
   }
   // callback cluster for adds/removals/updates of items.
   // By design, only one watcher is supported.
   virtual bool ignore(INotify *tell)
   {
      std::list<INotify *>::iterator it = _watch.begin();
      while (it != _watch.end())
      {
         if ( (*it) == tell )
         {
            _watch.erase(it);
            return true;
         }
      }
      return false;
   }

protected:

   // Retrieve indexed node, relative to left-most(true) or right-most(false) node.
   node_t *index(size_t i, bool bFwd)
   {
      return node_t::index(i, _root, bFwd);
   }

   // Retrieve indexed node, relative to left-most(true) or right-most(false) node.
   const node_t *index(size_t i, bool bFwd) const
   {
      return node_t::index(i, _root, bFwd);
   }

   // depth first search with typesafe context.
   // internal use.
   template <typename C>
   static bool dfs(C context, node_t *here, bool action(C, node_t *) )
   {
      if (here != NULL)
      {
         return dfs(context, here->_kids[0], action) &&
            dfs(context, here->_kids[1], action) &&
            action(context, here);
      }
      return true;
   }

   // depth first search with no context.
   // internal use.
   static bool dfs(node_t *here, bool action(node_t *) )
   {
      if (here != NULL)
      {
         return dfs(here->_kids[0], action) &&
            dfs(here->_kids[1], action) &&
            action(here);
      }
      return true;
   }

private:

	void onAdded(node_t *child)
	{
		size_t iIndex = child->index(true);
		std::list<INotify *> snap(_watch);
		std::list<INotify *>::iterator it = snap.begin();
		while (it != snap.end())
		{
			(*it++)->onAdded(iIndex);
		}
	}

	virtual void onChange(node_t *node)
	{
		size_t iIndex = node->index(true);
	    std::list<INotify *> snap(_watch);
	    std::list<INotify *>::iterator it = snap.begin();
		while (it != snap.end())
		{
			(*it++)->onChange(iIndex);
		}
	}

	virtual void onRemove(node_t *node)
	{
		std::list<INotify *> snap(_watch);
		size_t iIndex = node->index(true);
		std::list<INotify*>::iterator it = snap.begin();
		while (it != snap.end())
		{
			(*it++)->onRemove(iIndex);
		}
	}

	virtual void onMove(node_t *node)
	{
		std::list<INotify *> snap(_watch);

		size_t iFrom = node->index(true);
		node_t::removeInner(_root, node);
		node_t::insert(_root, node);
		size_t iTo = node->index(true);

		std::list<INotify *>::iterator it = snap.begin();
		while (it != snap.end())
		{
			(*it++)->onMoved(iFrom, iTo);
		}
	}

	// the tree's watchers.
	std::list<INotify*> _watch;
};
