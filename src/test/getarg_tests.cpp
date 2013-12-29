#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-OPC");
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", false));
    BOOST_CHECK(GetBoolArg("-OPC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-OPCo"));
    BOOST_CHECK(!GetBoolArg("-OPCo", false));
    BOOST_CHECK(GetBoolArg("-OPCo", true));

    ResetArgs("-OPC=0");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", false));
    BOOST_CHECK(!GetBoolArg("-OPC", true));

    ResetArgs("-OPC=1");
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", false));
    BOOST_CHECK(GetBoolArg("-OPC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noOPC");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", false));
    BOOST_CHECK(!GetBoolArg("-OPC", true));

    ResetArgs("-noOPC=1");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", false));
    BOOST_CHECK(!GetBoolArg("-OPC", true));

    ResetArgs("-OPC -noOPC");  // -OPC should win
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", false));
    BOOST_CHECK(GetBoolArg("-OPC", true));

    ResetArgs("-OPC=1 -noOPC=1");  // -OPC should win
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", false));
    BOOST_CHECK(GetBoolArg("-OPC", true));

    ResetArgs("-OPC=0 -noOPC=0");  // -OPC should win
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", false));
    BOOST_CHECK(!GetBoolArg("-OPC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--OPC=1");
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", false));
    BOOST_CHECK(GetBoolArg("-OPC", true));

    ResetArgs("--noOPC=1");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", false));
    BOOST_CHECK(!GetBoolArg("-OPC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OPC", "eleven"), "eleven");

    ResetArgs("-OPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OPC", "eleven"), "");

    ResetArgs("-OPC=");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-OPC", "eleven"), "");

    ResetArgs("-OPC=11");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-OPC", "eleven"), "11");

    ResetArgs("-OPC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-OPC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-OPC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-OPC", 0), 0);

    ResetArgs("-OPC -bar");
    BOOST_CHECK_EQUAL(GetArg("-OPC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-OPC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-OPC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-OPC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-OPC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--OPC");
    BOOST_CHECK_EQUAL(GetBoolArg("-OPC"), true);

    ResetArgs("--OPC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-OPC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noOPC");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", true));
    BOOST_CHECK(!GetBoolArg("-OPC", false));

    ResetArgs("-noOPC=1");
    BOOST_CHECK(!GetBoolArg("-OPC"));
    BOOST_CHECK(!GetBoolArg("-OPC", true));
    BOOST_CHECK(!GetBoolArg("-OPC", false));

    ResetArgs("-noOPC=0");
    BOOST_CHECK(GetBoolArg("-OPC"));
    BOOST_CHECK(GetBoolArg("-OPC", true));
    BOOST_CHECK(GetBoolArg("-OPC", false));

    ResetArgs("-OPC --noOPC");
    BOOST_CHECK(GetBoolArg("-OPC"));

    ResetArgs("-noOPC -OPC"); // OPC always wins:
    BOOST_CHECK(GetBoolArg("-OPC"));
}

BOOST_AUTO_TEST_SUITE_END()
