#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <powertabdocument/complexsymbolarray.h>
#include <powertabdocument/position.h>

struct ComplexSymbolFixture
{
    ComplexSymbolFixture()
    {
        array[0] = MAKELONG(MAKEWORD(0, 0),
                            MAKEWORD(0, Position::volumeSwell));
        array[1] = ComplexSymbols::notUsed;
        array[2] = MAKELONG(MAKEWORD(0, 0),
                            MAKEWORD(0, Position::tremoloBar));
    }

    std::array<uint32_t, 3> array;
};

BOOST_AUTO_TEST_SUITE(ComplexSymbolTest)

    BOOST_FIXTURE_TEST_CASE(FindComplexSymbol, ComplexSymbolFixture)
    {
        BOOST_CHECK_EQUAL(ComplexSymbols::findComplexSymbol(array, Position::multibarRest), -1);

        BOOST_CHECK_EQUAL(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell), 0);

        BOOST_CHECK_EQUAL(ComplexSymbols::findComplexSymbol(array, Position::tremoloBar), 2);
    }

    BOOST_FIXTURE_TEST_CASE(GetComplexSymbolCount, ComplexSymbolFixture)
    {
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(array), 2);

        std::array<uint32_t, 3> emptyArray;
        std::fill(emptyArray.begin(), emptyArray.end(), 0);
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(emptyArray), 0);
    }

    BOOST_FIXTURE_TEST_CASE(ClearComplexSymbols, ComplexSymbolFixture)
    {
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(array), 2);

        ComplexSymbols::clearComplexSymbols(array);
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(array), 0);
    }

    BOOST_FIXTURE_TEST_CASE(RemoveComplexSymbol, ComplexSymbolFixture)
    {
        ComplexSymbols::removeComplexSymbol(array, Position::volumeSwell);
        BOOST_CHECK_EQUAL(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell), -1);
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(array), 1);
    }

    BOOST_AUTO_TEST_CASE(AddComplexSymbol)
    {
        std::array<uint32_t, 3> array;
        ComplexSymbols::clearComplexSymbols(array);

        // add a volume swell
        ComplexSymbols::addComplexSymbol(array, MAKELONG(MAKEWORD(0, 0), MAKEWORD(0, Position::volumeSwell)));
        BOOST_CHECK_EQUAL(ComplexSymbols::getComplexSymbolCount(array), 1);
        BOOST_CHECK_EQUAL(ComplexSymbols::findComplexSymbol(array, Position::volumeSwell), 0);
    }

BOOST_AUTO_TEST_SUITE_END()
