#define BOOST_TEST_MODULE __AFL_TESTS
#include <boost/test/included/unit_test.hpp>

// BOOST_AUTO_TEST_CASE(FailTest)
// {
//     BOOST_CHECK_EQUAL(5, 3 + 1);
// }

BOOST_AUTO_TEST_CASE(PassTest) {
    BOOST_CHECK_EQUAL(4, 2 * 2);
}

