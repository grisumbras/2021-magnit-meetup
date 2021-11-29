#include <boost/core/lightweight_test.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/numeric.hpp>

#include <utility>
#include <vector>


struct Position {
  int x = 0;
  int y = 0;
};


struct Size {
  int width = 0;
  int height = 0;
};


// tag::shelf_def[]
struct Shelf
{
  Position position = {0, 0};
  Size size = {0, 0};
};

using Shelves = std::vector<Shelf>;

int runningMeter(Shelves const& shelves)
{
  int result = 0;
  for (Shelf shelf: shelves)
  {
    result += shelf.size.width;
  }
  return result;
}
// end::shelf_def[]


// tag::meter_bounded[]
int runningMeter(Shelves const& shelves, int l, int r)
{
  int result = 0;
  for (Shelf shelf: shelves)
  {
    int x1 = std::max(l, shelf.position.x);
    int x2 = std::clamp(
      shelf.position.x + shelf.size.width, x1, r);
    int width = x2 - x1;
    result += width;
  }
  return result;
}
// end::meter_bounded[]


namespace alternative1 {


// tag::alt1[]
int runningMeter(Shelves const& shelves)
{
  int result = 0;
  for (Shelf shelf: shelves)
  {



    int width = shelf.size.width;
    result += width;
  }
  return result;
}
// end::alt1[]


// tag::alt1_bounded[]
int runningMeter(Shelves const& shelves, int l, int r)
{
  int result = 0;
  for (Shelf shelf: shelves)
  {
    int x1 = std::max(l, shelf.position.x);
    int x2 = std::clamp(
      shelf.position.x + shelf.size.width, x1, r);
    int width = x2 - x1;
    result += width;
  }
  return result;
}
// end::alt1_bounded[]


} // namespace alternative1


namespace alternative2 {


// tag::alt2[]
int runningMeter(Shelves const& shelves)
{
  return std::accumulate(
    shelves.begin(),
    shelves.end(),
    0,
    [=](int acc, Shelf shelf) {




      return acc + shelf.size.width;
    });
}
// end::alt2[]


// tag::alt2_bounded[]
int runningMeter(Shelves const& shelves, int l, int r)
{
  return std::accumulate(
    shelves.begin(),
    shelves.end(),
    0,
    [=](int acc, Shelf shelf) {
      int x1 = std::max(l, shelf.position.x);
      int x2 = std::clamp(
        shelf.position.x + shelf.size.width, x1, r);
      int width = x2 - x1;
      return acc + width;
    });
}
// end::alt2_bounded[]


} // namespace alternative2


namespace alternative3 {


// tag::alt3[]
int runningMeter(Shelves const& shelves)
{
  return boost::accumulate(
    shelves,
    0,
    [=](int acc, Shelf shelf) {




      return acc + shelf.size.width;
    });
}
// end::alt3[]


// tag::alt3_bounded[]
int runningMeter(Shelves const& shelves, int l, int r)
{
  return boost::accumulate(
    shelves,
    0,
    [=](int acc, Shelf shelf) {
      int x1 = std::max(l, shelf.position.x);
      int x2 = std::clamp(
        shelf.position.x + shelf.size.width, x1, r);
      int width = x2 - x1;
      return acc + width;
    });
}
// end::alt3_bounded[]


} // namespace alternative3


namespace alternative4 {


using boost::adaptors::transformed;


// tag::alt4[]
int shelfWidth(Shelf const& shelf) {
  return shelf.size.width;
}






int runningMeter(Shelves const& shelves)
{
  return boost::accumulate(
    shelves | transformed(shelfWidth),
    0);
}
// end::alt4[]


// tag::alt4_bounded[]
auto clippedWidth(int l, int r) {
  return [=](Shelf shelf) {
    int x1 = std::max(l, shelf.position.x);
    int x2 = std::clamp(
      shelf.position.x + shelf.size.width, x1, r);
    return x2 - x1;
  };
}

int runningMeter(Shelves const& shelves, int l, int r)
{
  return boost::accumulate(
    shelves | transformed(clippedWidth(l, r)),
    0);
}
// end::alt4_bounded[]


} // namespace alternative3


Shelves shelves()
{
  return {
    {{50, 150}, {150, 30}},
    {{50, 120}, {150, 30}},
    {{0, 90},   {200, 30}},
    {{0, 60},   {200, 30}},
    {{0, 30},   {100, 30}},
    {{0, 0},    {100, 30}},
  };
}


int main()
{
  BOOST_TEST_EQ(runningMeter(shelves()), 150 * 2 + 200 * 2 + 100 * 2);
  BOOST_TEST_EQ(runningMeter(shelves(), 0, 200), 150 * 2 + 200 * 2 + 100 * 2);
  BOOST_TEST_EQ(runningMeter(shelves(), 0, 100), 2 * 50 + 4 * 100);
  BOOST_TEST_EQ(runningMeter(shelves(), 50, 200), 4 * 150 + 2 * 50);
  BOOST_TEST_EQ(runningMeter(shelves(), 20, 120), 2 * 70 + 2 * 100 + 2 * 80);

  BOOST_TEST_EQ(
    runningMeter(shelves()),
    alternative1::runningMeter(shelves()));
  BOOST_TEST_EQ(
    runningMeter(shelves(), 20, 180),
    alternative1::runningMeter(shelves(), 20, 180));

  BOOST_TEST_EQ(
    runningMeter(shelves()),
    alternative2::runningMeter(shelves()));
  BOOST_TEST_EQ(
    runningMeter(shelves(), 20, 180),
    alternative2::runningMeter(shelves(), 20, 180));

  BOOST_TEST_EQ(
    runningMeter(shelves()),
    alternative3::runningMeter(shelves()));
  BOOST_TEST_EQ(
    runningMeter(shelves(), 20, 180),
    alternative3::runningMeter(shelves(), 20, 180));

  BOOST_TEST_EQ(
    runningMeter(shelves()),
    alternative4::runningMeter(shelves()));
  BOOST_TEST_EQ(
    runningMeter(shelves(), 20, 180),
    alternative4::runningMeter(shelves(), 20, 180));

  return boost::report_errors();
}
