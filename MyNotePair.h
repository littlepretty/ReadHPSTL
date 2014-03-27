template <class _T1, class _T2>
struct pair 
{
  typedef _T1 first_type;
  typedef _T2 second_type;

  _T1 first;
  _T2 second;

};

template <class _T1, class _T2>
inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y)
{
  return pair<_T1, _T2>(__x, __y);
}