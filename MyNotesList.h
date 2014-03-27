
struct _List_node_base {
  _List_node_base* _M_next;
  _List_node_base* _M_prev;
};


template <class _Tp>
struct _List_node : public _List_node_base {
  _Tp _M_data;
};


struct _List_iterator_base 
{
	_List_node_base* _M_node;
};

template<class _Tp, class _Ref, class _Ptr>
struct _List_iterator : public _List_iterator_base 
{
	typedef _Ref reference;
	typedef _List_node<_Tp> _Node;
	reference operator*() const { return ((_Node*) _M_node)->_M_data; }
};


template <class _Tp, class _Allocator>
class _List_alloc_base<_Tp, _Allocator, bool _IsStatic> 
{
protected:
	_List_node<_Tp>* _M_node;
};

template <class _Tp, class _Alloc>
class _List_base: public _List_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
	_List_base(const allocator_type& __a) : _Base(__a) 
	{
	    _M_node = _M_get_node();
	    _M_node->_M_next = _M_node;
	    _M_node->_M_prev = _M_node;
	}

};

template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class list : protected _List_base<_Tp, _Alloc> 
{
public:

	iterator insert(iterator __position, const _Tp& __x) 
	{
	    _Node* __tmp = _M_create_node(__x);
	    __tmp->_M_next = __position._M_node;
	    __tmp->_M_prev = __position._M_node->_M_prev;
	    __position._M_node->_M_prev->_M_next = __tmp;
	    __position._M_node->_M_prev = __tmp;
	    return __tmp;
  	}

  	iterator erase(iterator __position) 
  	{
	    _List_node_base* __next_node = __position._M_node->_M_next;
	    _List_node_base* __prev_node = __position._M_node->_M_prev;
	    _Node* __n = (_Node*) __position._M_node;
	    __prev_node->_M_next = __next_node;
	    __next_node->_M_prev = __prev_node;
	    _Destroy(&__n->_M_data);
	    _M_put_node(__n);
	    return iterator((_Node*) __next_node);
	}

protected:
  void transfer(iterator __position, iterator __first, iterator __last) 
  {
    if (__position != __last) {
      // Remove [first, last) from its old position.
      __last._M_node->_M_prev->_M_next     = __position._M_node;
      __first._M_node->_M_prev->_M_next    = __last._M_node;
      __position._M_node->_M_prev->_M_next = __first._M_node; 

      // Splice [first, last) into its new position.
      _List_node_base* __tmp      = __position._M_node->_M_prev;
      __position._M_node->_M_prev = __last._M_node->_M_prev;
      __last._M_node->_M_prev     = __first._M_node->_M_prev; 
      __first._M_node->_M_prev    = __tmp;
    }
  }
};

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::remove(const _Tp& __value)
{
  iterator __first = begin();
  iterator __last = end();
  while (__first != __last) {
    iterator __next = __first;
    ++__next;
    if (*__first == __value) erase(__first);
    __first = __next;
  }
}

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::unique()
{
  iterator __first = begin();
  iterator __last = end();
  if (__first == __last) return;
  iterator __next = __first;
  while (++__next != __last) 
  {
    if (*__first == *__next)
      erase(__next);
    else
      __first = __next;

    __next = __first;
  }
}

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::merge(list<_Tp, _Alloc>& __x)
{
  iterator __first1 = begin();
  iterator __last1 = end();
  iterator __first2 = __x.begin();
  iterator __last2 = __x.end();

  while (__first1 != __last1 && __first2 != __last2)
    if (*__first2 < *__first1) {
      iterator __next = __first2;
      transfer(__first1, __first2, ++__next);
      __first2 = __next;
    }
    else
      ++__first1;

  if (__first2 != __last2) 
  	transfer(__last1, __first2, __last2);
}

template <class _Tp, class _Alloc>
void list<_Tp, _Alloc>::sort()
{
  // Do nothing if the list has length 0 or 1.
  if (_M_node->_M_next != _M_node && _M_node->_M_next->_M_next != _M_node) 
  {
    list<_Tp, _Alloc> __carry;
    list<_Tp, _Alloc> __counter[64];
    int __fill = 0;
    while (!empty()) {
      __carry.splice(__carry.begin(), *this, begin());
      int __i = 0;
      while(__i < __fill && !__counter[__i].empty()) {
        __counter[__i].merge(__carry);
        __carry.swap(__counter[__i++]);
      }
      __carry.swap(__counter[__i]);         
      if (__i == __fill) ++__fill;
    } 

    for (int __i = 1; __i < __fill; ++__i)
      __counter[__i].merge(__counter[__i-1]);
    swap(__counter[__fill-1]);
  }
}


