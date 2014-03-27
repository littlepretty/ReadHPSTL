typedef bool _Rb_tree_Color_type;

struct _Rb_tree_node_base
{
	typedef _Rb_tree_Color_type _Color_type;
  	typedef _Rb_tree_node_base* _Base_ptr;

	_Color_type _M_color; 
	_Base_ptr _M_parent;
	_Base_ptr _M_left;
	_Base_ptr _M_right;

	static _Base_ptr _S_minimum(_Base_ptr __x)
	{
		while (__x->_M_left != 0) __x = __x->_M_left;
		return __x;
	}

	static _Base_ptr _S_maximum(_Base_ptr __x)
	{
		while (__x->_M_right != 0) __x = __x->_M_right;
		return __x;
	}

 };

 inline void 
_Rb_tree_rotate_left(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
{
  _Rb_tree_node_base* __y = __x->_M_right;
  __x->_M_right = __y->_M_left;
  if (__y->_M_left !=0)
    __y->_M_left->_M_parent = __x;
  __y->_M_parent = __x->_M_parent;

  if (__x == __root)
    __root = __y;
  else if (__x == __x->_M_parent->_M_left)
    __x->_M_parent->_M_left = __y;
  else
    __x->_M_parent->_M_right = __y;
  __y->_M_left = __x;
  __x->_M_parent = __y;
}

inline void 
_Rb_tree_rotate_right(_Rb_tree_node_base* __x, _Rb_tree_node_base*& __root)
{
  _Rb_tree_node_base* __y = __x->_M_left;
  __x->_M_left = __y->_M_right;
  if (__y->_M_right != 0)
    __y->_M_right->_M_parent = __x;
  __y->_M_parent = __x->_M_parent;

  if (__x == __root)
    __root = __y;
  else if (__x == __x->_M_parent->_M_right)
    __x->_M_parent->_M_right = __y;
  else
    __x->_M_parent->_M_left = __y;
  __y->_M_right = __x;
  __x->_M_parent = __y;
}

template <class _Value>
struct _Rb_tree_node : public _Rb_tree_node_base
{
	typedef _Rb_tree_node<_Value>* _Link_type;
	_Value _M_value_field;
};

struct _Rb_tree_base_iterator
{
	_Base_ptr _M_node;


};

template <class _Value, class _Ref, class _Ptr>
struct _Rb_tree_iterator : public _Rb_tree_base_iterator
{

};


template <class _Tp, class _Alloc, bool _S_instanceless>
class _Rb_tree_alloc_base 
{
	_Rb_tree_node<_Tp>* _M_header;
};


template <class _Tp, class _Alloc>
struct _Rb_tree_base: public _Rb_tree_alloc_base<_Tp, _Alloc,
                               _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{

};


template <class _Key, class _Value, class _KeyOfValue, class _Compare, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Value) >
class _Rb_tree : protected _Rb_tree_base<_Value, _Alloc> 
{
protected:
  size_type _M_node_count; // keeps track of size of tree
  _Compare _M_key_compare;
};

template <class _Key, class _Value, class _KeyOfValue, 
          class _Compare, class _Alloc>
typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
  ::_M_insert(_Base_ptr __x_, _Base_ptr __y_, const _Value& __v)
{
  _Link_type __x = (_Link_type) __x_;
  _Link_type __y = (_Link_type) __y_;
  _Link_type __z;

  if (__y == _M_header || __x != 0 || 
      _M_key_compare(_KeyOfValue()(__v), _S_key(__y))) {
    __z = _M_create_node(__v);
    _S_left(__y) = __z;               // also makes _M_leftmost() = __z 
                                      //    when __y == _M_header
    if (__y == _M_header) {
      _M_root() = __z;
      _M_rightmost() = __z;
    }
    else if (__y == _M_leftmost())
      _M_leftmost() = __z;   // maintain _M_leftmost() pointing to min node
  }
  else {
    __z = _M_create_node(__v);
    _S_right(__y) = __z;
    if (__y == _M_rightmost())
      _M_rightmost() = __z;  // maintain _M_rightmost() pointing to max node
  }
  _S_parent(__z) = __y;
  _S_left(__z) = 0;
  _S_right(__z) = 0;
  _Rb_tree_rebalance(__z, _M_header->_M_parent);
  ++_M_node_count;
  return iterator(__z);
}

template <class _Key, class _Value, class _KeyOfValue, 
          class _Compare, class _Alloc>
typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
  ::insert_equal(const _Value& __v)
{
  _Link_type __y = _M_header;
  _Link_type __x = _M_root();
  while (__x != 0) {
    __y = __x;
    __x = _M_key_compare(_KeyOfValue()(__v), _S_key(__x)) ? 
            _S_left(__x) : _S_right(__x);
  }
  return _M_insert(__x, __y, __v);
}













