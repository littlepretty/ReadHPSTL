/// auto pointer, reference counting always = 1
template <class _Tp> class auto_ptr 
{
private:
	_Tp* _M_ptr;
public:
	auto_ptr(auto_ptr& __a) __STL_NOTHROW : _M_ptr(__a.release()) {}

	auto_ptr& operator=(auto_ptr& __a) __STL_NOTHROW 
	{
		if (&__a != this) 
		{
      		delete _M_ptr;
      		_M_ptr = __a.release();
    	}
    	return *this;
	}

	~auto_ptr() { delete _M_ptr; }

	_Tp& operator*() const __STL_NOTHROW 
	{
		return *_M_ptr;
	}

	_Tp* operator->() const __STL_NOTHROW 
	{
		return _M_ptr;
	}

	_Tp* get() const __STL_NOTHROW 
	{
		return _M_ptr;
	}

	/// release: 
	/// 1. let the pointer no longer point to the object, 
	/// 2. return the pointed object to other auto pointer 
	_Tp* release() __STL_NOTHROW 
	{
		_Tp* __tmp = _M_ptr;
		_M_ptr = 0;
		return __tmp;
	}

	void reset(_Tp* __p = 0) __STL_NOTHROW 
	{
		if (__p != _M_ptr) {
			delete _M_ptr;
			_M_ptr = __p;
		}
	}

};


class sp_counted_base {
public:
    sp_counted_base(): use_count_(1), weak_count_(1)
    {
    }

    void add_ref_copy() { ++use_count_; }

    void release() // nothrow
	{
    	{
			long new_use_count = --use_count_;
			if(new_use_count != 0) return;
    	}
    	dispose();	/// will trigger a delete operation
    	weak_release();
    }
};

private:
    long use_count_;        // #shared
    long weak_count_;       // #weak + (#shared != 0)
};


class shared_count
{
private:
    sp_counted_base * pi_;

#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)

    int id_;

public:
	shared_count(shared_count const & r): pi_(r.pi_) // nothrow
	{
    	if(pi_!= 0) pi_->add_ref_copy();
	}

	~shared_count() // nothrow
	{
    	if(pi_ != 0) pi_->release();
	}

}

template<class T> 
class shared_ptr
{

	typedef shared_ptr<T> this_type;

private:

	T * px;                     // contained pointer
    boost::detail::shared_count pn;    // reference counter

public:

	shared_ptr & operator=( shared_ptr && r ) // never throws
    {
        this_type( static_cast< shared_ptr && >( r ) ).swap( *this );
        return *this;
    }

    template<class Y>
    shared_ptr & operator=(shared_ptr<Y> const & r) // never throws
    {
        this_type(r).swap(*this);
        return *this;
    }

    typedef typename boost::detail::shared_ptr_traits<T>::reference reference;

    reference operator* () const // never throws
    {
        BOOST_ASSERT(px != 0);
        return *px;
    }

    T * operator-> () const // never throws
    {
        BOOST_ASSERT(px != 0);
        return px;
    }

    shared_ptr(): px(0), pn() // never throws in 1.30+
    {
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this); 	/// this_type() trigger shared_ptr() constructor
    }

    void swap(shared_ptr<T> & other) // never throws
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

};  // shared_ptr




