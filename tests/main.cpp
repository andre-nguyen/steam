#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
///
/// catch.hpp Usage Note
///
/// REQUIRE -- terminates on failure
/// CHECK   -- continues test on failure
/// INFO    -- buffers a string that is printed on failure (cleared after scope ends)
///
/// TEST_CASE -- a unit test
/// SECTION -- resets initialization in TEST_CASE for various tests
///
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
/// Sample Test
/////////////////////////////////////////////////////////////////////////////////////////////
//TEST_CASE("example of a test case", "[example]" ) {

//    int a = 5;
//    REQUIRE( a == 5 );

//    SECTION("if we change a to 10" ) {
//        a = 10;
//        REQUIRE( a == 10 );
//    }

//    SECTION("in a later section it is still 5" ) {
//        REQUIRE( a == 5 );
//    }
//} // TEST_CASE


// Katarina
// Creating both positive and negative tests separately?
// Create tests to try out some edge cases or some of the cases that cause the code fail.
// The code looks good to handle these cases.
// Just mentioning this if you want to add more tests to see if you got all the possible
// combinations for each function.
// If the tests already do that then ignore this comment (the pattern_tests does checks, example:
// "if (solver.info() != Eigen::Success)". Do you want to add more tests that purposely fail?)
