// ------------------------------------------------------------
// Class _String_base.  

// _String_base is a helper class that makes it it easier to write an
// exception-safe version of basic_string.  The constructor allocates,
// but does not initialize, a block of memory.  The destructor
// deallocates, but does not destroy elements within, a block of
// memory.  The destructor assumes that _M_start either is null, or else
// points to a block of memory that was allocated using _String_base's 
// allocator and whose size is _M_end_of_storage - _M_start.

// Additionally, _String_base encapsulates the difference between
// old SGI-style allocators and standard-conforming allocators.


// General base class.
template <class _Tp, class _Alloc, bool _S_instanceless>
class _String_alloc_base 
{
  typedef typename _Alloc_traits<_Tp, _Alloc>::allocator_type allocator_type;

  protected:
  allocator_type _M_data_allocator;

  _Tp* _M_start;
  _Tp* _M_finish;
  _Tp* _M_end_of_storage;

public:

  _String_alloc_base(const allocator_type& __a)
  : _M_data_allocator(__a), _M_start(0), _M_finish(0), _M_end_of_storage(0)
    {}

protected:
  _Tp* _M_allocate(size_t __n)
    { return _M_data_allocator.allocate(__n); }
  void _M_deallocate(_Tp* __p, size_t __n) {
    if (__p)
      _M_data_allocator.deallocate(__p, __n); 
  }
};

template <class _Tp, class _Alloc>
class _String_base 
  : public _String_alloc_base<_Tp, _Alloc,
                              _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
protected:
  typedef _String_alloc_base<_Tp, _Alloc,
                             _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
          _Base;
  typedef typename _Base::allocator_type allocator_type;

  void _M_allocate_block(size_t __n) { 
    if (__n <= max_size()) {
      _M_start  = _M_allocate(__n);
      _M_finish = _M_start;
      _M_end_of_storage = _M_start + __n;
    }
    else
      _M_throw_length_error();
  }

  void _M_deallocate_block() 
    { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }
  
  size_t max_size() const { return (size_t(-1) / sizeof(_Tp)) - 1; }

  _String_base(const allocator_type& __a) : _Base(__a) { }
  
  _String_base(const allocator_type& __a, size_t __n) : _Base(__a)
    { _M_allocate_block(__n); }

  ~_String_base() { _M_deallocate_block(); }

  void _M_throw_length_error() const;
  void _M_throw_out_of_range() const;
};

// ------------------------------------------------------------
// Class basic_string.  

// Class invariants:
// (1) [start, finish) is a valid range.
// (2) Each iterator in [start, finish) points to a valid object
//     of type value_type.
// (3) *finish is a valid object of type value_type; in particular,
//     it is value_type().
// (4) [finish + 1, end_of_storage) is a valid range.
// (5) Each iterator in [finish + 1, end_of_storage) points to 
//     unininitialized memory.

// Note one important consequence: a string of length n must manage
// a block of memory whose size is at least n + 1.  


template <class _CharT, class _Traits, class _Alloc> 
class basic_string : private _String_base<_CharT,_Alloc> {
public:

  static const size_type npos;

  // Constructor, destructor, assignment.
  explicit basic_string(const allocator_type& __a = allocator_type())
    : _Base(__a, 8) { _M_terminate_string(); }

  basic_string(const basic_string& __s) : _Base(__s.get_allocator()) 
    { _M_range_initialize(__s.begin(), __s.end()); }

  basic_string(const basic_string& __s, size_type __pos, size_type __n = npos,
               const allocator_type& __a = allocator_type()) : _Base(__a) 
  {
    if (__pos > __s.size())
      _M_throw_out_of_range();
    else
      _M_range_initialize(__s.begin() + __pos,
                          __s.begin() + __pos + min(__n, __s.size() - __pos));
  }

  basic_string(const _CharT* __s, size_type __n,
               const allocator_type& __a = allocator_type()) 
    : _Base(__a) 
    { _M_range_initialize(__s, __s + __n); }

  basic_string(const _CharT* __s,
               const allocator_type& __a = allocator_type())
    : _Base(__a) 
    { _M_range_initialize(__s, __s + _Traits::length(__s)); }

  basic_string(size_type __n, _CharT __c,
               const allocator_type& __a = allocator_type())
    : _Base(__a, __n + 1)
  {
    _M_finish = uninitialized_fill_n(_M_start, __n, __c);
    _M_terminate_string();
  }

  // Check to see if _InputIterator is an integer type.  If so, then
  // it can't be an iterator.
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIterator>
  basic_string(_InputIterator __f, _InputIterator __l,
               const allocator_type& __a = allocator_type())
    : _Base(__a)
  {
    typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
    _M_initialize_dispatch(__f, __l, _Integral());
  }
#else /* __STL_MEMBER_TEMPLATES */
  basic_string(const _CharT* __f, const _CharT* __l,
               const allocator_type& __a = allocator_type())
    : _Base(__a)
  {
    _M_range_initialize(__f, __l);
  }
#endif

  ~basic_string() { destroy(_M_start, _M_finish + 1); }
    
  basic_string& operator=(const basic_string& __s) {
    if (&__s != this) 
      assign(__s.begin(), __s.end());
    return *this;
  }

  basic_string& operator=(const _CharT* __s) 
    { return assign(__s, __s + _Traits::length(__s)); }

  basic_string& operator=(_CharT __c)
    { return assign(static_cast<size_type>(1), __c); }

protected:                      // Protected members inherited from base.
#ifdef __STL_HAS_NAMESPACES
  using _Base::_M_allocate;
  using _Base::_M_deallocate;
  using _Base::_M_allocate_block;
  using _Base::_M_deallocate_block;
  using _Base::_M_throw_length_error;
  using _Base::_M_throw_out_of_range;

  using _Base::_M_start;
  using _Base::_M_finish;
  using _Base::_M_end_of_storage;
#endif /* __STL_HAS_NAMESPACES */

private:                        // Helper functions used by constructors
                                // and elsewhere.
  void _M_construct_null(_CharT* __p) {
    construct(__p);
#   ifdef __STL_DEFAULT_CONSTRUCTOR_BUG
    __STL_TRY {
      *__p = (_CharT) 0;
    }
    __STL_UNWIND(destroy(__p));
#   endif
  }

  static _CharT _M_null() {
#   ifndef __STL_DEFAULT_CONSTRUCTOR_BUG
    return _CharT();
#   else
    return (_CharT) 0;
#   endif
  }

private:                        
  // Helper functions used by constructors.  It is a severe error for
  // any of them to be called anywhere except from within constructors.

  void _M_terminate_string() {
    __STL_TRY {
      _M_construct_null(_M_finish);
    }
    __STL_UNWIND(destroy(_M_start, _M_finish));
  }

#ifdef __STL_MEMBER_TEMPLATES
    
  template <class _InputIter>
  void _M_range_initialize(_InputIter __f, _InputIter __l,
                           input_iterator_tag) {
    _M_allocate_block(8);
    _M_construct_null(_M_finish);
    __STL_TRY {
      append(__f, __l);
    }
    __STL_UNWIND(destroy(_M_start, _M_finish + 1));
  }

  template <class _ForwardIter>
  void _M_range_initialize(_ForwardIter __f, _ForwardIter __l, 
                           forward_iterator_tag) {
    difference_type __n = 0;
    distance(__f, __l, __n);
    _M_allocate_block(__n + 1);
    _M_finish = uninitialized_copy(__f, __l, _M_start);
    _M_terminate_string();
  }

  template <class _InputIter>
  void _M_range_initialize(_InputIter __f, _InputIter __l) {
    typedef typename iterator_traits<_InputIter>::iterator_category _Category;
    _M_range_initialize(__f, __l, _Category());
  }

  template <class _Integer>
  void _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type) {
    _M_allocate_block(__n + 1);
    _M_finish = uninitialized_fill_n(_M_start, __n, __x);
    _M_terminate_string();
  }

  template <class _InputIter>
  void _M_initialize_dispatch(_InputIter __f, _InputIter __l, __false_type) {
     _M_range_initialize(__f, __l);
  }
    
#else /* __STL_MEMBER_TEMPLATES */

  void _M_range_initialize(const _CharT* __f, const _CharT* __l) {
    ptrdiff_t __n = __l - __f;
    _M_allocate_block(__n + 1);
    _M_finish = uninitialized_copy(__f, __l, _M_start);
    _M_terminate_string();
  }

#endif /* __STL_MEMBER_TEMPLATES */

public:                         // Iterators.
  iterator begin()             { return _M_start; }
  iterator end()               { return _M_finish; }
  const_iterator begin() const { return _M_start; }
  const_iterator end()   const { return _M_finish; }  

  reverse_iterator rbegin()             
    { return reverse_iterator(_M_finish); }
  reverse_iterator rend()               
    { return reverse_iterator(_M_start); }
  const_reverse_iterator rbegin() const 
    { return const_reverse_iterator(_M_finish); }
  const_reverse_iterator rend()   const 
    { return const_reverse_iterator(_M_start); }

public:                         // Size, capacity, etc.
  size_type size() const { return _M_finish - _M_start; }
  size_type length() const { return size(); }

  size_t max_size() const { return _Base::max_size(); }


  void resize(size_type __n, _CharT __c) {
    if (__n <= size())
      erase(begin() + __n, end());
    else
      append(__n - size(), __c);
  }

  void resize(size_type __n) { resize(__n, _M_null()); }

  void reserve(size_type = 0);

  size_type capacity() const { return (_M_end_of_storage - _M_start) - 1; }

  void clear() {
    if (!empty()) {
      _Traits::assign(*_M_start, _M_null());
      destroy(_M_start+1, _M_finish+1);
      _M_finish = _M_start;
    }
  } 

  bool empty() const { return _M_start == _M_finish; }    

public:                         // Element access.

  const_reference operator[](size_type __n) const
    { return *(_M_start + __n); }
  reference operator[](size_type __n)
    { return *(_M_start + __n); }

  const_reference at(size_type __n) const {
    if (__n >= size())
      _M_throw_out_of_range();
    return *(_M_start + __n);
  }

  reference at(size_type __n) {
    if (__n >= size())
      _M_throw_out_of_range();
    return *(_M_start + __n);
  }

public:                         // Append, operator+=, push_back.

  basic_string& operator+=(const basic_string& __s) { return append(__s); }
  basic_string& operator+=(const _CharT* __s) { return append(__s); }
  basic_string& operator+=(_CharT __c) { push_back(__c); return *this; }

  basic_string& append(const basic_string& __s) 
    { return append(__s.begin(), __s.end()); }

  basic_string& append(const basic_string& __s,
                       size_type __pos, size_type __n)
  {
    if (__pos > __s.size())
      _M_throw_out_of_range();
    return append(__s.begin() + __pos,
                  __s.begin() + __pos + min(__n, __s.size() - __pos));
  }

  basic_string& append(const _CharT* __s, size_type __n) 
    { return append(__s, __s+__n); }

  basic_string& append(const _CharT* __s) 
    { return append(__s, __s + _Traits::length(__s)); }

  basic_string& append(size_type __n, _CharT __c);

#ifdef __STL_MEMBER_TEMPLATES

  // Check to see if _InputIterator is an integer type.  If so, then
  // it can't be an iterator.
  template <class _InputIter>
  basic_string& append(_InputIter __first, _InputIter __last) {
    typedef typename _Is_integer<_InputIter>::_Integral _Integral;
    return _M_append_dispatch(__first, __last, _Integral());
  }

#else /* __STL_MEMBER_TEMPLATES */

  basic_string& append(const _CharT* __first, const _CharT* __last);

#endif /* __STL_MEMBER_TEMPLATES */

  void push_back(_CharT __c) {
    if (_M_finish + 1 == _M_end_of_storage)
      reserve(size() + max(size(), static_cast<size_type>(1)));
    _M_construct_null(_M_finish + 1);
    _Traits::assign(*_M_finish, __c);
    ++_M_finish;
  }

  void pop_back() {
    _Traits::assign(*(_M_finish - 1), _M_null());
    destroy(_M_finish);
    --_M_finish;
  }

private:                        // Helper functions for append.

#ifdef __STL_MEMBER_TEMPLATES

  template <class _InputIter>
  basic_string& append(_InputIter __f, _InputIter __l, input_iterator_tag);

  template <class _ForwardIter>
  basic_string& append(_ForwardIter __f, _ForwardIter __l, 
                       forward_iterator_tag);

  template <class _Integer>
  basic_string& _M_append_dispatch(_Integer __n, _Integer __x, __true_type) {
    return append((size_type) __n, (_CharT) __x);
  }

  template <class _InputIter>
  basic_string& _M_append_dispatch(_InputIter __f, _InputIter __l,
                                   __false_type) {
    typedef typename iterator_traits<_InputIter>::iterator_category _Category;
    return append(__f, __l, _Category());
  }

#endif /* __STL_MEMBER_TEMPLATES */

public:                         // Assign
  
  basic_string& assign(const basic_string& __s) 
    { return assign(__s.begin(), __s.end()); }

  basic_string& assign(const basic_string& __s, 
                       size_type __pos, size_type __n) {
    if (__pos > __s.size())
      _M_throw_out_of_range();
    return assign(__s.begin() + __pos, 
                  __s.begin() + __pos + min(__n, __s.size() - __pos));
  }

  basic_string& assign(const _CharT* __s, size_type __n)
    { return assign(__s, __s + __n); }

  basic_string& assign(const _CharT* __s)
    { return assign(__s, __s + _Traits::length(__s)); }

  basic_string& assign(size_type __n, _CharT __c);

#ifdef __STL_MEMBER_TEMPLATES

  // Check to see if _InputIterator is an integer type.  If so, then
  // it can't be an iterator.
  template <class _InputIter>
  basic_string& assign(_InputIter __first, _InputIter __last) {
    typedef typename _Is_integer<_InputIter>::_Integral _Integral;
    return _M_assign_dispatch(__first, __last, _Integral());
  }

#endif  /* __STL_MEMBER_TEMPLATES */

  basic_string& assign(const _CharT* __f, const _CharT* __l);

private:                        // Helper functions for assign.

#ifdef __STL_MEMBER_TEMPLATES

  template <class _Integer>
  basic_string& _M_assign_dispatch(_Integer __n, _Integer __x, __true_type) {
    return assign((size_type) __n, (_CharT) __x);
  }

  template <class _InputIter>
  basic_string& _M_assign_dispatch(_InputIter __f, _InputIter __l,
                                   __false_type);

#endif  /* __STL_MEMBER_TEMPLATES */

public:                         // Insert

  basic_string& insert(size_type __pos, const basic_string& __s) {
    if (__pos > size())
      _M_throw_out_of_range();
    if (size() > max_size() - __s.size())
      _M_throw_length_error();
    insert(_M_start + __pos, __s.begin(), __s.end());
    return *this;
  }

  basic_string& insert(size_type __pos, const basic_string& __s,
                       size_type __beg, size_type __n) {
    if (__pos > size() || __beg > __s.size())
      _M_throw_out_of_range();
    size_type __len = min(__n, __s.size() - __beg);
    if (size() > max_size() - __len)
      _M_throw_length_error();
    insert(_M_start + __pos,
           __s.begin() + __beg, __s.begin() + __beg + __len);
    return *this;
  }

  basic_string& insert(size_type __pos, const _CharT* __s, size_type __n) {
    if (__pos > size())
      _M_throw_out_of_range();
    if (size() > max_size() - __n)
      _M_throw_length_error();
    insert(_M_start + __pos, __s, __s + __n);
    return *this;
  }

  basic_string& insert(size_type __pos, const _CharT* __s) {
    if (__pos > size())
      _M_throw_out_of_range();
    size_type __len = _Traits::length(__s);
    if (size() > max_size() - __len)
      _M_throw_length_error();
    insert(_M_start + __pos, __s, __s + __len);
    return *this;
  }
    
  basic_string& insert(size_type __pos, size_type __n, _CharT __c) {
    if (__pos > size())
      _M_throw_out_of_range();
    if (size() > max_size() - __n)
      _M_throw_length_error();
    insert(_M_start + __pos, __n, __c);
    return *this;
  }

  iterator insert(iterator __p, _CharT __c) {
    if (__p == _M_finish) {
      push_back(__c);
      return _M_finish - 1;
    }
    else
      return _M_insert_aux(__p, __c);
  }

  void insert(iterator __p, size_t __n, _CharT __c);

#ifdef __STL_MEMBER_TEMPLATES

  // Check to see if _InputIterator is an integer type.  If so, then
  // it can't be an iterator.
  template <class _InputIter>
  void insert(iterator __p, _InputIter __first, _InputIter __last) {
    typedef typename _Is_integer<_InputIter>::_Integral _Integral;
    _M_insert_dispatch(__p, __first, __last, _Integral());
  }

#else /* __STL_MEMBER_TEMPLATES */

  void insert(iterator __p, const _CharT* __first, const _CharT* __last);

#endif /* __STL_MEMBER_TEMPLATES */

private:                        // Helper functions for insert.

#ifdef __STL_MEMBER_TEMPLATES

  template <class _InputIter>
  void insert(iterator __p, _InputIter, _InputIter, input_iterator_tag);

  template <class _ForwardIter>
  void insert(iterator __p, _ForwardIter, _ForwardIter, forward_iterator_tag);


  template <class _Integer>
  void _M_insert_dispatch(iterator __p, _Integer __n, _Integer __x,
                          __true_type) {
    insert(__p, (size_type) __n, (_CharT) __x);
  }

  template <class _InputIter>
  void _M_insert_dispatch(iterator __p, _InputIter __first, _InputIter __last,
                          __false_type) {
    typedef typename iterator_traits<_InputIter>::iterator_category _Category;
    insert(__p, __first, __last, _Category());
  }

  template <class _InputIterator>
  void 
  _M_copy(_InputIterator __first, _InputIterator __last, iterator __result) {
    for ( ; __first != __last; ++__first, ++__result)
      _Traits::assign(*__result, *__first);
  }

#endif /* __STL_MEMBER_TEMPLATES */

  iterator _M_insert_aux(iterator, _CharT);

  void 
  _M_copy(const _CharT* __first, const _CharT* __last, _CharT* __result) {
    _Traits::copy(__result, __first, __last - __first);
  }

public:                         // Erase.

  basic_string& erase(size_type __pos = 0, size_type __n = npos) {
    if (__pos > size())
      _M_throw_out_of_range();
    erase(_M_start + __pos, _M_start + __pos + min(__n, size() - __pos));
    return *this;
  }  

  iterator erase(iterator __position) {
                                // The move includes the terminating null.
    _Traits::move(__position, __position + 1, _M_finish - __position);
    destroy(_M_finish);
    --_M_finish;
    return __position;
  }

  iterator erase(iterator __first, iterator __last) {
    if (__first != __last) {
                                // The move includes the terminating null.
      _Traits::move(__first, __last, (_M_finish - __last) + 1);
      const iterator __new_finish = _M_finish - (__last - __first);
      destroy(__new_finish + 1, _M_finish + 1);
      _M_finish = __new_finish;
    }
    return __first;
  }

public:                         // Replace.  (Conceptually equivalent
                                // to erase followed by insert.)
  basic_string& replace(size_type __pos, size_type __n, 
                        const basic_string& __s) {
    if (__pos > size())
      _M_throw_out_of_range();
    const size_type __len = min(__n, size() - __pos);
    if (size() - __len >= max_size() - __s.size())
      _M_throw_length_error();
    return replace(_M_start + __pos, _M_start + __pos + __len, 
                   __s.begin(), __s.end());
  }

  basic_string& replace(size_type __pos1, size_type __n1,
                        const basic_string& __s,
                        size_type __pos2, size_type __n2) {
    if (__pos1 > size() || __pos2 > __s.size())
      _M_throw_out_of_range();
    const size_type __len1 = min(__n1, size() - __pos1);
    const size_type __len2 = min(__n2, __s.size() - __pos2);
    if (size() - __len1 >= max_size() - __len2)
      _M_throw_length_error();
    return replace(_M_start + __pos1, _M_start + __pos1 + __len1,
                   __s._M_start + __pos2, __s._M_start + __pos2 + __len2);
  }

  basic_string& replace(size_type __pos, size_type __n1,
                        const _CharT* __s, size_type __n2) {
    if (__pos > size())
      _M_throw_out_of_range();
    const size_type __len = min(__n1, size() - __pos);
    if (__n2 > max_size() || size() - __len >= max_size() - __n2)
      _M_throw_length_error();
    return replace(_M_start + __pos, _M_start + __pos + __len,
                   __s, __s + __n2);
  }

  basic_string& replace(size_type __pos, size_type __n1,
                        const _CharT* __s) {
    if (__pos > size())
      _M_throw_out_of_range();
    const size_type __len = min(__n1, size() - __pos);
    const size_type __n2 = _Traits::length(__s);
    if (__n2 > max_size() || size() - __len >= max_size() - __n2)
      _M_throw_length_error();
    return replace(_M_start + __pos, _M_start + __pos + __len,
                   __s, __s + _Traits::length(__s));
  }

  basic_string& replace(size_type __pos, size_type __n1,
                        size_type __n2, _CharT __c) {
    if (__pos > size())
      _M_throw_out_of_range();
    const size_type __len = min(__n1, size() - __pos);
    if (__n2 > max_size() || size() - __len >= max_size() - __n2)
      _M_throw_length_error();
    return replace(_M_start + __pos, _M_start + __pos + __len, __n2, __c);
  }

  basic_string& replace(iterator __first, iterator __last, 
                        const basic_string& __s) 
    { return replace(__first, __last, __s.begin(), __s.end()); }

  basic_string& replace(iterator __first, iterator __last,
                        const _CharT* __s, size_type __n) 
    { return replace(__first, __last, __s, __s + __n); }

  basic_string& replace(iterator __first, iterator __last,
                        const _CharT* __s) {
    return replace(__first, __last, __s, __s + _Traits::length(__s));
  }

  basic_string& replace(iterator __first, iterator __last, 
                        size_type __n, _CharT __c);

  // Check to see if _InputIterator is an integer type.  If so, then
  // it can't be an iterator.
#ifdef __STL_MEMBER_TEMPLATES
  template <class _InputIter>
  basic_string& replace(iterator __first, iterator __last,
                        _InputIter __f, _InputIter __l) {
    typedef typename _Is_integer<_InputIter>::_Integral _Integral;
    return _M_replace_dispatch(__first, __last, __f, __l,  _Integral());
  }
#else /* __STL_MEMBER_TEMPLATES */
  basic_string& replace(iterator __first, iterator __last,
                        const _CharT* __f, const _CharT* __l);
#endif /* __STL_MEMBER_TEMPLATES */

private:                        // Helper functions for replace.

#ifdef __STL_MEMBER_TEMPLATES

  template <class _Integer>
  basic_string& _M_replace_dispatch(iterator __first, iterator __last,
                                    _Integer __n, _Integer __x,
                                    __true_type) {
    return replace(__first, __last, (size_type) __n, (_CharT) __x);
  }

  template <class _InputIter>
  basic_string& _M_replace_dispatch(iterator __first, iterator __last,
                                    _InputIter __f, _InputIter __l,
                                    __false_type) {
    typedef typename iterator_traits<_InputIter>::iterator_category _Category;
    return replace(__first, __last, __f, __l, _Category());
  }

  template <class _InputIter>
  basic_string& replace(iterator __first, iterator __last,
                        _InputIter __f, _InputIter __l, input_iterator_tag);

  template <class _ForwardIter>
  basic_string& replace(iterator __first, iterator __last,
                        _ForwardIter __f, _ForwardIter __l, 
                        forward_iterator_tag);

#endif /* __STL_MEMBER_TEMPLATES */

public:                         // Other modifier member functions.

  size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const {
    if (__pos > size())
      _M_throw_out_of_range();
    const size_type __len = min(__n, size() - __pos);
    _Traits::copy(__s, _M_start + __pos, __len);
    return __len;
  }

  void swap(basic_string& __s) {
    __STD::swap(_M_start, __s._M_start);
    __STD::swap(_M_finish, __s._M_finish);
    __STD::swap(_M_end_of_storage, __s._M_end_of_storage);
  }

public:                         // Conversion to C string.

  const _CharT* c_str() const { return _M_start; }
  const _CharT* data()  const { return _M_start; }

public:                         // find.

  size_type find(const basic_string& __s, size_type __pos = 0) const 
    { return find(__s.begin(), __pos, __s.size()); }

  size_type find(const _CharT* __s, size_type __pos = 0) const 
    { return find(__s, __pos, _Traits::length(__s)); }

  size_type find(const _CharT* __s, size_type __pos, size_type __n) const;
  size_type find(_CharT __c, size_type __pos = 0) const;

public:                         // rfind.

  size_type rfind(const basic_string& __s, size_type __pos = npos) const 
    { return rfind(__s.begin(), __pos, __s.size()); }

  size_type rfind(const _CharT* __s, size_type __pos = npos) const 
    { return rfind(__s, __pos, _Traits::length(__s)); }

  size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const;
  size_type rfind(_CharT __c, size_type __pos = npos) const;

public:                         // find_first_of
  
  size_type find_first_of(const basic_string& __s, size_type __pos = 0) const 
    { return find_first_of(__s.begin(), __pos, __s.size()); }

  size_type find_first_of(const _CharT* __s, size_type __pos = 0) const 
    { return find_first_of(__s, __pos, _Traits::length(__s)); }

  size_type find_first_of(const _CharT* __s, size_type __pos, 
                          size_type __n) const;

  size_type find_first_of(_CharT __c, size_type __pos = 0) const 
    { return find(__c, __pos); }

public:                         // find_last_of

  size_type find_last_of(const basic_string& __s,
                         size_type __pos = npos) const
    { return find_last_of(__s.begin(), __pos, __s.size()); }

  size_type find_last_of(const _CharT* __s, size_type __pos = npos) const 
    { return find_last_of(__s, __pos, _Traits::length(__s)); }

  size_type find_last_of(const _CharT* __s, size_type __pos, 
                         size_type __n) const;

  size_type find_last_of(_CharT __c, size_type __pos = npos) const {
    return rfind(__c, __pos);
  }

public:                         // find_first_not_of

  size_type find_first_not_of(const basic_string& __s, 
                              size_type __pos = 0) const 
    { return find_first_not_of(__s.begin(), __pos, __s.size()); }

  size_type find_first_not_of(const _CharT* __s, size_type __pos = 0) const 
    { return find_first_not_of(__s, __pos, _Traits::length(__s)); }

  size_type find_first_not_of(const _CharT* __s, size_type __pos,
                              size_type __n) const;

  size_type find_first_not_of(_CharT __c, size_type __pos = 0) const;

public:                         // find_last_not_of

  size_type find_last_not_of(const basic_string& __s, 
                             size_type __pos = npos) const
    { return find_last_not_of(__s.begin(), __pos, __s.size()); }

  size_type find_last_not_of(const _CharT* __s, size_type __pos = npos) const
    { return find_last_not_of(__s, __pos, _Traits::length(__s)); }

  size_type find_last_not_of(const _CharT* __s, size_type __pos,
                             size_type __n) const;

  size_type find_last_not_of(_CharT __c, size_type __pos = npos) const;

public:                         // Substring.

  basic_string substr(size_type __pos = 0, size_type __n = npos) const {
    if (__pos > size())
      _M_throw_out_of_range();
    return basic_string(_M_start + __pos, 
                        _M_start + __pos + min(__n, size() - __pos));
  }

public:                         // Compare

  int compare(const basic_string& __s) const 
    { return _M_compare(_M_start, _M_finish, __s._M_start, __s._M_finish); }

  int compare(size_type __pos1, size_type __n1,
              const basic_string& __s) const {
    if (__pos1 > size())
      _M_throw_out_of_range();
    return _M_compare(_M_start + __pos1, 
                      _M_start + __pos1 + min(__n1, size() - __pos1),
                      __s._M_start, __s._M_finish);
  }
    
  int compare(size_type __pos1, size_type __n1,
              const basic_string& __s,
              size_type __pos2, size_type __n2) const {
    if (__pos1 > size() || __pos2 > __s.size())
      _M_throw_out_of_range();
    return _M_compare(_M_start + __pos1, 
                      _M_start + __pos1 + min(__n1, size() - __pos1),
                      __s._M_start + __pos2, 
                      __s._M_start + __pos2 + min(__n2, size() - __pos2));
  }

  int compare(const _CharT* __s) const {
    return _M_compare(_M_start, _M_finish, __s, __s + _Traits::length(__s));
  }

  int compare(size_type __pos1, size_type __n1, const _CharT* __s) const {
    if (__pos1 > size())
      _M_throw_out_of_range();
    return _M_compare(_M_start + __pos1, 
                      _M_start + __pos1 + min(__n1, size() - __pos1),
                      __s, __s + _Traits::length(__s));
  }

  int compare(size_type __pos1, size_type __n1, const _CharT* __s,
              size_type __n2) const {
    if (__pos1 > size())
      _M_throw_out_of_range();
    return _M_compare(_M_start + __pos1, 
                      _M_start + __pos1 + min(__n1, size() - __pos1),
                      __s, __s + __n2);
  }

public:                        // Helper function for compare.
  static int _M_compare(const _CharT* __f1, const _CharT* __l1,
                        const _CharT* __f2, const _CharT* __l2) {
    const ptrdiff_t __n1 = __l1 - __f1;
    const ptrdiff_t __n2 = __l2 - __f2;
    const int cmp = _Traits::compare(__f1, __f2, min(__n1, __n2));
    return cmp != 0 ? cmp : (__n1 < __n2 ? -1 : (__n1 > __n2 ? 1 : 0));
  }
};

