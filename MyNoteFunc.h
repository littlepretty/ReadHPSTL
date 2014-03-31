/// base struct for 
	///negate, logical_not, unary_negate
template <class _Arg, class _Result>
struct unary_function {
  typedef _Arg argument_type;
  typedef _Result result_type;
};

/// base struct for 
	/// plus, minus, multiplies, divides
	/// modules, equal_to, not_equal_to, greater, less, greater_equal, less_equal, 
	/// logic_and, logic_or, binary_negate
template <class _Arg1, class _Arg2, class _Result>
struct binary_function {
  typedef _Arg1 first_argument_type;
  typedef _Arg2 second_argument_type;
  typedef _Result result_type;
}; 


template <class _Operation> 
class binder1st
  : public unary_function<typename _Operation::second_argument_type,
                          typename _Operation::result_type> {
protected:
  _Operation op;
  typename _Operation::first_argument_type value;
public:
  binder1st(const _Operation& __x,
            const typename _Operation::first_argument_type& __y)
      : op(__x), value(__y) {}
  typename _Operation::result_type
  operator()(const typename _Operation::second_argument_type& __x) const {
    return op(value, __x); 
  }
};

/// base class for not1
template <class _Predicate>
class unary_negate
  : public unary_function<typename _Predicate::argument_type, bool> {
protected:
  _Predicate _M_pred;
public:
  explicit unary_negate(const _Predicate& __x) : _M_pred(__x) {}
  bool operator()(const typename _Predicate::argument_type& __x) const {
    return !_M_pred(__x);
  }
};

template <class _Predicate>
inline unary_negate<_Predicate> 
not1(const _Predicate& __pred)
{
  return unary_negate<_Predicate>(__pred);
}

/// base class for not2
template <class _Predicate> 
class binary_negate 
  : public binary_function<typename _Predicate::first_argument_type,
                           typename _Predicate::second_argument_type,
                           bool> {
protected:
  _Predicate _M_pred;
public:
  explicit binary_negate(const _Predicate& __x) : _M_pred(__x) {}
  bool operator()(const typename _Predicate::first_argument_type& __x, 
                  const typename _Predicate::second_argument_type& __y) const
  {
    return !_M_pred(__x, __y); 
  }
};

template <class _Predicate>
inline binary_negate<_Predicate> 
not2(const _Predicate& __pred)
{
  return binary_negate<_Predicate>(__pred);
}

/// function object with first parameter bound, similar to binder2nd
template <class _Operation> 
class binder1st
  : public unary_function<typename _Operation::second_argument_type,
                          typename _Operation::result_type> {
protected:
	_Operation op;
	typename _Operation::first_argument_type value;
public:
	binder1st(const _Operation& __x, const typename _Operation::first_argument_type& __y): op(__x), value(__y) {}
	
	/// pass second parameter __x and call op
	typename _Operation::result_type operator()(const typename _Operation::second_argument_type& __x) const 
	{
		return op(value, __x); 
	}
};

/// create bind1st class by calling this function. 
/// following classes like bind2nd, unary_compose etc also use similar functions
template <class _Operation, class _Tp>
inline binder1st<_Operation> 
bind1st(const _Operation& __fn, const _Tp& __x) 
{
	typedef typename _Operation::first_argument_type _Arg1_type;
	return binder1st<_Operation>(__fn, _Arg1_type(__x));
}

template <class _Operation> 
class binder2nd
  : public unary_function<typename _Operation::first_argument_type,
                          typename _Operation::result_type> {
protected:
  _Operation op;
  typename _Operation::second_argument_type value;
public:
  binder2nd(const _Operation& __x,
            const typename _Operation::second_argument_type& __y) 
      : op(__x), value(__y) {}
  typename _Operation::result_type
  operator()(const typename _Operation::first_argument_type& __x) const {
    return op(__x, value); 
  }
};

/// compose a function with another function as input parameter
template <class _Operation1, class _Operation2>
class unary_compose: public unary_function<typename _Operation2::argument_type, typename _Operation1::result_type> 
{
protected:
  _Operation1 _M_fn1;
  _Operation2 _M_fn2;
public:
  unary_compose(const _Operation1& __x, const _Operation2& __y) 
    : _M_fn1(__x), _M_fn2(__y) {}
  typename _Operation1::result_type
  operator()(const typename _Operation2::argument_type& __x) const 
  {
    return _M_fn1(_M_fn2(__x));
  }
};

/// compose 2 function with other 2 functions as input parameters
template <class _Operation1, class _Operation2, class _Operation3>
class binary_compose: public unary_function<typename _Operation2::argument_type,
                          					typename _Operation1::result_type> {
protected:
  _Operation1 _M_fn1;
  _Operation2 _M_fn2;
  _Operation3 _M_fn3;
public:
  binary_compose(const _Operation1& __x, const _Operation2& __y, 
                 const _Operation3& __z) 
    : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) { }
  typename _Operation1::result_type
  operator()(const typename _Operation2::argument_type& __x) const 
  {
    return _M_fn1(_M_fn2(__x), _M_fn3(__x));
  }
};


template <class _Arg, class _Result>
class pointer_to_unary_function : public unary_function<_Arg, _Result> {
protected:
  _Result (*_M_ptr)(_Arg);
public:
  pointer_to_unary_function() {}
  explicit pointer_to_unary_function(_Result (*__x)(_Arg)) : _M_ptr(__x) {}
  _Result operator()(_Arg __x) const { return _M_ptr(__x); }
};

template <class _Arg1, class _Arg2, class _Result>
class pointer_to_binary_function: public binary_function<_Arg1,_Arg2,_Result> {
protected:
    _Result (*_M_ptr)(_Arg1, _Arg2);
public:
    pointer_to_binary_function() {}
    explicit pointer_to_binary_function(_Result (*__x)(_Arg1, _Arg2)) 
      : _M_ptr(__x) {}
    _Result operator()(_Arg1 __x, _Arg2 __y) const {
      return _M_ptr(__x, __y);
    }
};

template <class _Pair>
struct _Select1st : public unary_function<_Pair, typename _Pair::first_type> 
{
  const typename _Pair::first_type& operator()(const _Pair& __x) const {
    return __x.first;
  }
};

template <class _Pair>
struct _Select2nd : public unary_function<_Pair, typename _Pair::second_type>
{
  const typename _Pair::second_type& operator()(const _Pair& __x) const {
    return __x.second;
  }
};

template <class _Arg1, class _Arg2>
struct _Project1st : public binary_function<_Arg1, _Arg2, _Arg1> 
{
  _Arg1 operator()(const _Arg1& __x, const _Arg2&) const { return __x; }
};

template <class _Arg1, class _Arg2>
struct _Project2nd : public binary_function<_Arg1, _Arg2, _Arg2> 
{
  _Arg2 operator()(const _Arg1&, const _Arg2& __y) const { return __y; }
};

/// functions return constant value with 0, 1 or 2 parameters
template <class _Result>
struct _Constant_void_fun 
{
  typedef _Result result_type;
  result_type _M_val;

  _Constant_void_fun(const result_type& __v) : _M_val(__v) {}
  const result_type& operator()() const { return _M_val; }
};  

template <class _Result, class _Argument>
struct _Constant_unary_fun 
{
  typedef _Argument argument_type;
  typedef  _Result  result_type;
  result_type _M_val;

  _Constant_unary_fun(const result_type& __v) : _M_val(__v) {}
  const result_type& operator()(const _Argument&) const { return _M_val; }
};

template <class _Result, class _Arg1, class _Arg2>
struct _Constant_binary_fun 
{
  typedef  _Arg1   first_argument_type;
  typedef  _Arg2   second_argument_type;
  typedef  _Result result_type;
  _Result _M_val;

  _Constant_binary_fun(const _Result& __v) : _M_val(__v) {}
  const result_type& operator()(const _Arg1&, const _Arg2&) const {
    return _M_val;
  }
};




















