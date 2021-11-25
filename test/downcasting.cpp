#include <boost/core/lightweight_test.hpp>
#include <boost/hof/pipable.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <QObject>


class MyClass : public QObject {};


constexpr
struct SomethingDoer {
  void operator()(MyClass& ) const
  {
  }

  void operator()(MyClass* obj) const
  {
    BOOST_TEST(obj);
    (*this)(*obj);
  }
} do_something;


#if defined(CAST_ITER_POINTER)
// tag::cast_iter_begin[]
template <class ForwardIterator, class T>
class DynamicFilterIterator
  : public boost::stl_interfaces::iterator_interface<
      DynamicFilterIterator<ForwardIterator, T>, // <1>
      std::forward_iterator_tag, // <2>
      T*, // <3>
      T*> // <4>
{
// end::cast_iter_begin[]
#else
// tag::cast_iter_trans2[]
template <class ForwardIterator, class T>
class DynamicFilterIterator
  : public boost::stl_interfaces::iterator_interface<
      DynamicFilterIterator<ForwardIterator, T>,
      std::forward_iterator_tag,
      T> // <1>
{
// end::cast_iter_trans2[]
#endif

public:
  using DynamicFilterIterator::iterator_interface::operator++;

#if defined(CAST_ITER_POINTER)
// tag::cast_iter_trans[]
  T* operator*() const { return val_; }
// end::cast_iter_trans[]
#else
// tag::cast_iter_trans2[]
  T& operator*()       { return *val_; }
  T& operator*() const { return *val_; }
// end::cast_iter_trans2[]
#endif

  DynamicFilterIterator() = default;

// tag::cast_iter_filter[]
  DynamicFilterIterator(ForwardIterator first, ForwardIterator last)
    : last_(last)
    , it_(advance(first))
  {}

  DynamicFilterIterator& operator++()
  {
      it_ = advance(std::next(it_));
      return *this;
  }
// end::cast_iter_filter[]

private:
// tag::cast_iter_advance[]
  ForwardIterator advance(ForwardIterator it)
  {
      return std::find_if(
        it, last_, [this](QObject* object) -> bool {
          return val_ = dynamic_cast<T*>(object); // <1>
        });
  }
// end::cast_iter_advance[]

  friend boost::stl_interfaces::access;

  constexpr ForwardIterator& base_reference() noexcept { return it_; }
  constexpr ForwardIterator base_reference() const noexcept { return it_; }

// tag::cast_iter_end[]
private:
  ForwardIterator last_;
  ForwardIterator it_;
  T* val_ = nullptr;
};
// end::cast_iter_end[]


template <class T> struct DynamicFilter {
  template <class Range>
  auto operator()(Range&& rng) const
  {
      using SrcIterator = decltype(rng.begin());
      using TgtIterator = DynamicFilterIterator<SrcIterator, T>;
      return boost::make_iterator_range(
        TgtIterator(rng.begin(), rng.end()),
        TgtIterator(rng.end(), rng.end()));
  }
};

template <class T>
constexpr auto dynamic_filter = boost::hof::pipable(DynamicFilter<T>());


// tag::caster[]
template <class To> struct DynamicCaster
{
  template <class From>
  To operator()(From&& from) const
  {
    return dynamic_cast<To>(std::forward<From>(from));
  }
};
template <class To> constexpr DynamicCaster<To> dynamic_caster;
// end::caster[]


void iterate0()
{
  using boost::adaptors::transformed;

  QObject object;

// tag::iterate0[]
auto kids = object.children()
          | transformed(dynamic_caster<MyClass*>);
for (MyClass* kid: kids)
{
  if (!kid) { continue; }

  do_something(kid);
}
// end::iterate0[]
}


void iterate1()
{
  using boost::adaptors::transformed;
  using boost::adaptors::filtered;

  QObject object;

// tag::iterate1[]
auto kids = object.children()
  | transformed(dynamic_caster<MyClass*>)
  | transformed(std::identity())
  ;
for (MyClass* kid: kids)
{
  do_something(kid);
}
// end::iterate1[]
}


void iterate2()
{
  using boost::adaptors::transformed;
  using boost::adaptors::filtered;

  QObject object;

// tag::iterate2[]
auto kids = object.children()
  | transformed(dynamic_caster<MyClass*>)
  | transformed(std::identity())
  ;
boost::for_each(kids, do_something);
// end::iterate2[]
}


void iterate3()
{
  using boost::adaptors::transformed;
  using boost::adaptors::filtered;

  QObject object;

// tag::iterate3[]
boost::for_each(
  object.children()
    | transformed(dynamic_caster<MyClass*>)
    | transformed(std::identity()),
  do_something);
// end::iterate3[]
}


void iterate4()
{
  QObject object;

// tag::iterate4[]
  boost::for_each(
    object.children() | dynamic_filter<MyClass>,
    do_something);
// end::iterate4[]
}


int main()
{
  iterate0();
  iterate1();
  iterate2();
  iterate3();
  iterate4();
  return boost::report_errors();
}
