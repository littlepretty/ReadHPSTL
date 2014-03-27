template <class _Tp, class _Allocator, bool _IsStatic>
class _Vector_alloc_base
{
protected:
  allocator_type _M_data_allocator;
  _Tp* _M_start;
  _Tp* _M_finish;
  _Tp* _M_end_of_storage;
};


template <class _Tp, class _Alloc>
struct _Vector_base: public _Vector_alloc_base<_Tp, _Alloc, _Alloc_traits<_Tp, _Alloc>::_S_instanceless>
{
public:
	_Vector_base(size_t __n, const allocator_type& __a) : _Base(__a) 
	{
    	_M_start = _M_allocate(__n);
    	_M_finish = _M_start;
    	_M_end_of_storage = _M_start + __n;
	}
};

template <class _Tp, class _Alloc = __STL_DEFAULT_ALLOCATOR(_Tp) >
class vector : protected _Vector_base<_Tp, _Alloc> 
{
public:
	void reserve(size_type __n) 
	{
    	if (capacity() < __n) 
    	{
      		const size_type __old_size = size();
      		iterator __tmp = _M_allocate_and_copy(__n, _M_start, _M_finish);
      		destroy(_M_start, _M_finish);
      		_M_deallocate(_M_start, _M_end_of_storage - _M_start);
      		_M_start = __tmp;
      		_M_finish = __tmp + __old_size;
      		_M_end_of_storage = _M_start + __n;
    	}
  	}

  	void push_back(const _Tp& __x) 
  	{
    	if (_M_finish != _M_end_of_storage) 
    	{
      		construct(_M_finish, __x);
      		++_M_finish;
    	}
    	else
      		_M_insert_aux(end(), __x);
  	}

  	iterator insert(iterator __position, const _Tp& __x) 
  	{
    	size_type __n = __position - begin();
    	if (_M_finish != _M_end_of_storage && __position == end()) {
      		construct(_M_finish, __x);
      		++_M_finish;
    	}
    	else
      		_M_insert_aux(__position, __x);
    	return begin() + __n;
  	}

  	template <class _Tp, class _Alloc>
	void 
	vector<_Tp, _Alloc>::_M_insert_aux(iterator __position, const _Tp& __x)
	{
	  if (_M_finish != _M_end_of_storage) 
	  {
	    construct(_M_finish, *(_M_finish - 1));
	    ++_M_finish;
	    _Tp __x_copy = __x;
	    copy_backward(__position, _M_finish - 2, _M_finish - 1);
	    *__position = __x_copy;
	  }
	  else {
	    const size_type __old_size = size();
	    const size_type __len = __old_size != 0 ? 2 * __old_size : 1;
	    iterator __new_start = _M_allocate(__len);
	    iterator __new_finish = __new_start;
	    __STL_TRY {
	      __new_finish = uninitialized_copy(_M_start, __position, __new_start);
	      construct(__new_finish, __x);
	      ++__new_finish;
	      __new_finish = uninitialized_copy(__position, _M_finish, __new_finish);
	    }
	    __STL_UNWIND((destroy(__new_start,__new_finish), 
	                  _M_deallocate(__new_start,__len)));
	    destroy(begin(), end());
	    _M_deallocate(_M_start, _M_end_of_storage - _M_start);
	    _M_start = __new_start;
	    _M_finish = __new_finish;
	    _M_end_of_storage = __new_start + __len;
	  }
	}
};

