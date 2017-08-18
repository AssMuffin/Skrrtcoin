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
    ResetArgs("-SKR");
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", false));
    BOOST_CHECK(GetBoolArg("-SKR", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-IPCo"));
    BOOST_CHECK(!GetBoolArg("-IPCo", false));
    BOOST_CHECK(GetBoolArg("-IPCo", true));

    ResetArgs("-SKR=0");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", false));
    BOOST_CHECK(!GetBoolArg("-SKR", true));

    ResetArgs("-SKR=1");
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", false));
    BOOST_CHECK(GetBoolArg("-SKR", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noIPC");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", false));
    BOOST_CHECK(!GetBoolArg("-SKR", true));

    ResetArgs("-noIPC=1");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", false));
    BOOST_CHECK(!GetBoolArg("-SKR", true));

    ResetArgs("-SKR -noIPC");  // -SKR should win
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", false));
    BOOST_CHECK(GetBoolArg("-SKR", true));

    ResetArgs("-SKR=1 -noIPC=1");  // -SKR should win
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", false));
    BOOST_CHECK(GetBoolArg("-SKR", true));

    ResetArgs("-SKR=0 -noIPC=0");  // -SKR should win
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", false));
    BOOST_CHECK(!GetBoolArg("-SKR", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--SKR=1");
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", false));
    BOOST_CHECK(GetBoolArg("-SKR", true));

    ResetArgs("--noIPC=1");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", false));
    BOOST_CHECK(!GetBoolArg("-SKR", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SKR", "eleven"), "eleven");

    ResetArgs("-SKR -bar");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SKR", "eleven"), "");

    ResetArgs("-SKR=");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-SKR", "eleven"), "");

    ResetArgs("-SKR=11");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-SKR", "eleven"), "11");

    ResetArgs("-SKR=eleven");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-SKR", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-SKR", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-SKR", 0), 0);

    ResetArgs("-SKR -bar");
    BOOST_CHECK_EQUAL(GetArg("-SKR", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-SKR=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-SKR", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-SKR=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-SKR", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--SKR");
    BOOST_CHECK_EQUAL(GetBoolArg("-SKR"), true);

    ResetArgs("--SKR=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-SKR", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noIPC");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", true));
    BOOST_CHECK(!GetBoolArg("-SKR", false));

    ResetArgs("-noIPC=1");
    BOOST_CHECK(!GetBoolArg("-SKR"));
    BOOST_CHECK(!GetBoolArg("-SKR", true));
    BOOST_CHECK(!GetBoolArg("-SKR", false));

    ResetArgs("-noIPC=0");
    BOOST_CHECK(GetBoolArg("-SKR"));
    BOOST_CHECK(GetBoolArg("-SKR", true));
    BOOST_CHECK(GetBoolArg("-SKR", false));

    ResetArgs("-SKR --noIPC");
    BOOST_CHECK(GetBoolArg("-SKR"));

    ResetArgs("-noIPC -SKR"); // SKR always wins:
    BOOST_CHECK(GetBoolArg("-SKR"));
}

BOOST_AUTO_TEST_SUITE_END()
