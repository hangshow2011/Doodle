#include "doodle_core/logger/logger.h"
#include <doodle_core/doodle_core.h>
#include <doodle_core/pin_yin/convert.h>

#include <boost/test/tools/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <range/v3/view/filter.hpp>
#include <range/v3/view/iota.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <vector>

// #include <boost/lambda/bind.hpp>

using namespace doodle;

BOOST_AUTO_TEST_SUITE(renges)

BOOST_AUTO_TEST_CASE(ranges_ints) {
  auto l_view = ranges::views::ints(0, 10);

  BOOST_TEST(l_view.size() == 10);
  BOOST_TEST_MESSAGE(l_view);
}

BOOST_AUTO_TEST_CASE(ranges_ints_phoenix) {
  std::vector<std::size_t> l_test{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  auto l_view = ranges::views::ints(0, 10);

  // boost::phoenix::placeholders::

  BOOST_TEST(l_view.size() == 10);
  BOOST_TEST_MESSAGE(l_view);
}

BOOST_AUTO_TEST_SUITE_END()