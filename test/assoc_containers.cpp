#include <boost/core/lightweight_test.hpp>
#include <boost/hof/pipable.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/stl_interfaces/iterator_interface.hpp>

#include <QJsonObject>


void do_something(QString, QJsonValue) {}


// tag::iter_begin[]
template <class Iterator>
class KeyValuePairIterator
  : public boost::stl_interfaces::iterator_interface<
      KeyValuePairIterator<Iterator>,
      std::random_access_iterator_tag,
      std::pair<QString, QJsonValue>,
      std::pair<QString, QJsonValueRef>>
{
// end::iter_begin[]
public:
// tag::iter_star[]
  std::pair<QString, QJsonValueRef>
  operator*() { return std::make_pair(it_.key(), *it_); }

  std::pair<QString, QJsonValue>
  operator*() const { return std::make_pair(it_.key(), *it_); }
// end::iter_star[]

  KeyValuePairIterator() = default;

  KeyValuePairIterator(Iterator it)
    : it_(it)
  {}

private:
  friend boost::stl_interfaces::access;

  constexpr Iterator& base_reference() noexcept { return it_; }
  constexpr Iterator base_reference() const noexcept { return it_; }

  Iterator it_;
};


struct ToKeyValuePair {
  template <class Range>
  auto operator()(Range&& rng) const
  {
      using SrcIterator = decltype(rng.begin());
      using TgtIterator = KeyValuePairIterator<SrcIterator>;
      return boost::make_iterator_range(
        TgtIterator(rng.begin()),
        TgtIterator(rng.end()));
  }
};

constexpr auto toKeyValuePair = boost::hof::pipable(ToKeyValuePair());


int main()
{
  QJsonObject obj;
// tag::use[]
  for (auto [key, value]: obj | toKeyValuePair) {
    do_something(key, value);
  }
// end::use[]
  return boost::report_errors();
}
