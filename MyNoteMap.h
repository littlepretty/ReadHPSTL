
template <class _Key, class _Tp, class _Compare, class _Alloc>
class map 
{
	typedef _Key                  key_type;
	typedef _Tp                   data_type;
	typedef _Tp                   mapped_type;
	typedef pair<const _Key, _Tp> value_type;
	typedef _Compare              key_compare;

	/// function object for pair's key comparing
	class value_compare: public binary_function<value_type, value_type, bool> 
	{
		friend class map<_Key,_Tp,_Compare,_Alloc>;

		protected:
			_Compare comp;
			value_compare(_Compare __c) : comp(__c) {}
		public:
			bool operator()(const value_type& __x, const value_type& __y) const 
			{
				return comp(__x.first, __y.first);
			}
	};

	private:
	typedef _Rb_tree<key_type, value_type, _Select1st<value_type>, key_compare, _Alloc> _Rep_type;
	_Rep_type _M_t;  // red-black tree representing map

	key_compare key_comp() const { return _M_t.key_comp(); }
  	value_compare value_comp() const { return value_compare(_M_t.key_comp()); }

	_Tp& operator[](const key_type& __k) 
	{
	    iterator __i = lower_bound(__k);
	    // __i->first is greater than or equivalent to __k.
	    if (__i == end() || key_comp()(__k, (*__i).first))
	      __i = insert(__i, value_type(__k, _Tp()));
	    return (*__i).second;
  	}

	iterator insert(iterator position, const value_type& __x)
    { 
    	return _M_t.insert_unique(position, __x); 
    }

    size_type erase(const key_type& __x) 
    { 
    	return _M_t.erase(__x); 
    }


};
