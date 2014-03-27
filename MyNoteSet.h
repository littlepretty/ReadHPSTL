template <class _Key, class _Compare, class _Alloc>
class set 
{

private:
  typedef _Rb_tree<key_type, value_type, 
                  _Identity<value_type>, key_compare, _Alloc> _Rep_type;
  _Rep_type _M_t;  // red-black tree representing set

  pair<iterator,bool> insert(const value_type& __x) 
  { 
    pair<typename _Rep_type::iterator, bool> __p = _M_t.insert_unique(__x); 
    return pair<iterator, bool>(__p.first, __p.second);
  }

  size_type erase(const key_type& __x) 
  { 
    return _M_t.erase(__x); 
  }

};
